
Blades of Exile Coding Conventions
==================================

This file lists some guidelines for submitting code to the Open Blades
of Exile project. Most of these can be taken as guidelines rather than
hard rules.

The most important rule is, no formatting-only changes mixed in a commit
with actual code changes. Feel free to reformat a line that has been
otherwise touched, or re-indent a section that you've edited, but don't
change formatting just for the sake of changing formatting. If you make
a change that requires re-indenting a large block of code (about 10
lines or more), such as wrapping it in a loop, if, or try, we prefer it
to be split into two commits - one for the re-indenting, then one for
the actual logic.

With that out of the way, here's the actual guidelines. The coding
guidelines are loosely based on Google's style guide; the formatting
guidelines, however, are my own.

Header Files
============

Include Guards
--------------

All headers should have an include guard. The preferred format of the
include guard `#define` is `BOE_COMPONENT_FILENAME`.

Possible components are:

* `game` - files specific to the game
* `data` - files defining the game's data (currently kept in the classes
folder)
* `pced` - files specific to the player character editor
* `scened` - files specific to the scenario editor
* `dlog` - files that are part of the DialogXML framework

The COMPONENT can be omitted if none of the above apply.

Any hyphens or periods in the FILENAME should be replaced with
underscores in the include guard.

Declarations
------------

Whenever feasible, try to forward declare required classes instead of
including the file that defines them. With templates, however, it's
generally preferred to include. Don't use pointers solely for the
purpose of avoiding an include.

Unless dependencies make it impossible, the order of declarations in a
header should be:

* preprocessor defines
* typedef or using declarations
* enums
* simple struct or union types (usually with no member functions)
* classes
* global functions

However, it's preferred to not include classes, enums, and global
functions in the same file. Try to keep enum declarations in a separate
header file, and don't include functions in the same file as a class
unless they're directly related to the class.

One class per header is preferred. More is tolerated, if they're related
classes.

Never use a using namespace declaration in a header file.

Order of Includes
-----------------

Include files at the top of any file (header or source) should appear in
the following order:

* the source file's counterpart header (only applicable for source
files)
* System headers (eg WinAPI, Cocoa, CoreFoundation, POSIX, X)
* C or C++ standard library headers (use the cxx form instead of xx.h
for C headers)
* Boost library headers
* Other library headers (eg SFML, OpenGL, Zlib, TinyXML++, etc)
* DialogXML headers
* General Blades of Exile headers
* Component-specific Blades of Exile headers

All except the last three and the first one should use the angle
brackets include style. Normally, there should be a blank line
between groups; however, if there are only one or two headers in
a group, it can be merged with an adjacent group. The counterpart
header should always be followed by a blank line, however.

Try to make all includes explicit in the header files, rather than
relying on indirect includes by including one file that includes others.

Inline template definition headers should be included at the bottom of
the header.

Include by filename only for Blades of Exile headers; assume all project
folders containing headers are in the include path.

Scoping
=======

Never use a using namespace declaration in a header file. It's
acceptable in a source file, but discouraged. Using declarations are
fine in source files, but not in header files unless they're in an
inline function or class declaration. Namespace aliases are encouraged
for long namespaces. When using `std::bind` within a function, always put
a `using namespace std::placeholders;` line somewhere in the function.

Don't use nested classes.

Only put absolutely necessary functions in header files; if a function
only needs to be used in one file, it shouldn't be in a header. In this
case, it should be declared `static`. (Don't use an anonymous namespace
for this purpose.)

Local variables should be declared as close to the point of use as
possible. Much of the legacy code placed all variable declarations at
the top of the function, since this was required in C at that time;
don't do this in new code.

Whenever possible, eliminate global variables. It's accepted that some
global variables can't be helped, such as the global `cUniverse` and the
global `cScenario`, but if a global is just being used to pass information
between disparate functions, find another way. There are such variables
in the code currently which we would like to eliminate.

Classes
=======

Use the explicit keyword for constructors that have only one
non-defaultable argument, unless you actually intend to define an
implicit conversion. An operator bool should usually be declared
explicit as well.

Generally, you should use the `class` keyword (rather than the `struct`
keyword) to declare anything with member functions or non-public
members.

Operator overloading is encouraged, provided it doesn't change the basic
meaning of the operator being overloaded.

Declarations within a class should be in the following order:

* friend class declarations
* friend function declarations
* public member functions
* public member variables
* protected member functions
* protected member variables
* private member functions
* private member variables

Be sparing with friend declarations.

Functions
=========

If a function takes a parameter by reference or pointer and doesn't
modify it, the parameter should be const. Don't declare parameters const
if they are passed by value. Don't use const pointers for parameters (eg
`int*const`).

Overloading functions is encouraged.

Default arguments are encouraged.

Don't use `throw()` specifications.

General Coding Style
====================

Use of exceptions is encouraged, provided it makes sense, though you
should ensure that they get caught somewhere before `main()`.

Prefer C++-style casts to C-style casts.

Do not use the C-style stream functions `printf`, `fprintf`, or `perror`
to print diagnostic output to stdout or stderr. Use either `cout` or
`cerr` for this purpose. Do not use `clog` or `cin`. For other uses,
C++ streams are included. Do not use `sprintf` for anything. Do not
even use `snprintf`.

Use const whenever it makes sense. In particular, declare string
constants as `const char*const`.

Never use simple macros for constants. Create an enum instead. Macros
that take arguments may be acceptable, depending.

Don't use 0 where a pointer is expected; use `NULL` or `nullptr` instead.

Feel free to use `auto` if it simplifies the code, but generally only for
local variables.

Use of lambda functions, `std::function`, and `std::bind` is encouraged.

Use of Boost libraries is encouraged.

Naming Rules
============

Header files should have a "_.hpp_" extension. C++ source files should have
a "_.cpp_" extension, and Objective-C++ files should have a "_.mm_" extension.

Files specific to one component should be named with one of the prefixes
"_boe._", "_pc._", "_scen._".

Sometimes, a header will define an interface that has to be implemented
differently depending on the platform. In this case, the naming should
be as follows:

* _file.hpp_
* _file.win.cpp_
* _file.mac.mm_ (or _file.mac.cpp_ if it doesn't include Objective-C code)
* _file.linux.cpp_

If the Mac and Linux implementation happen to be the same due to only
using POSIX headers, use "_file.posix.cpp_".

For files containing inline template definitions, an extension of
"_.tpl.hpp_" is preferred.

In general, functions and variables should always start with a lowercase
letter. Both `underscore_separated` and `camelCase` names are acceptable.

Enums should be named with a lowercase `e` followed by an uppercase
letter.

Classes should either start with an uppercase letter or a lowercase `c`
followed by an uppercase letter.

Typedef or using declarations should usually end with `_t`.

Do not use a trailing underscore or `m_` prefix for member variables.

Names of constants (both enum constants and other constants) should
either be all uppercase and separated by underscores, or camelCase
starting with a lowercase k followed by an uppercase letter.

Macros, when present, should be all uppercase and separated by
underscores.

Comments and Annotations
========================

Generally, C++-style comments are preferred, but feel free to use
C-style comments for longer comments.

If we ever introduce Doxygen comments, they will be in the Javadoc style
- block comments start with `/**`, line comments start with `///`, and
directives start with `@`.

Comment whenever something's purpose may be unclear. If you're uncertain
of something's purpose, feel free to add a `TODO:` comment. Use `TODO:` for
anything that is temporary or not perfect, or in places where there
needs to be code that hasn't been written yet.

For organizing the function popup menu, use `MARK:` comments rather than
`#pragma mark`.

To mark parameters as unused, omit the name or surround it in block
comment syntax. Other unused entities should be removed or commented out
if you want to avoid warnings, though if you think it might have a use
in the future, it's reasonable to leave it in with a comment. Do not use
any kind of attributes, including `__attribute__`, `__declspec`, and
C++11 attributes.

Formatting
==========

There's no line limit, and breaking a long line up by simply adding line
breaks is discouraged. However, if a line is longer than about 100
character, you should probably consider writing it in a way to shorten
it, such as using more intermediate local variables or writing a
function to compute the value.

If you need non-ASCII characters in strings, make sure the file is saved
as UTF-8.

All indentation should be tabs. Assume four spaces per tab.

Braces go on the same line as the control statement or function
declaration. The else keyword shares its line with the closing brace of
the preceding if statement.

Do not put space after an open parenthesis or before a closing
parenthesis. Do not put space between a keyword or function and the
following open parenthesis.

In declarations of pointers and references, the pointer or reference
operator should bind to the type, not the variable. Never declare
another type in the same declaration as a pointer or reference.

Do not put parentheses around return values. Do not put redundant
parentheses around comparisons mixed in with logical operators.

If a constructor initializer must be wrapped, put the colon on the first
line and one variable per line thereafter.

Never put whitespace at the end of a line (unless there's nothing else
on the line and the whitespace matches nearby indentation). Never have
more than two consecutive blank lines.