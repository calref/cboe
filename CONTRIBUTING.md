
This is a collection of general guidelines to consider when contributing to
Blades of Exile. It is currently a work in progress. If you have questions or
need clarifications, visit the IRC channel or post on the Blades of Exile forum.

Filing Tickets
==============

When opening bug reports on the GitHub issue tracker, make sure to explicitly state the
platform you are running on (Windows, OSX, or WINE). Give a concise description of the
behaviour you expected and contrast it with the behaviour you observed. If the bug
involves a crash, post the error message if one was shown before crashing. If there was
no error message, try to find and post a detailed crash report. These are often generated
automatically when a program crashes and frequently contain valuable information for
locating the cause. (Feel free to erase your name from the crash report before posting it.)

When opening a feature request, give a concise description of what you want to see. There
is no need to give suggestions on how it could be implemented, especially if you are
not familiar with the source, but a feature won't be rejected just because you gave
ill-informed suggestions on how to implement it.

In both cases, give as much additional information as you can.

Contributing Code
=================

The preferred method of contributing code to Blades of Exile is to fork the repository
on GitHub, implement your changes, and make a pull request. However, if you can't
or won't obtain a GitHub account, email-style patches may also be acceptable.

When submitting patches, there are some things to keep in mind:

* Please be careful of what you check into the repository. Please do not commit any
binaries, junk files, or anything that doesn't compile.
* If your patch affects the dialog engine, be sure to update any relevant documentation
or add new documentation as needed, using the Doxygen format. This also applies to
any other components currently covered by Doxygen documentation, such as the resource
manager.
* Be sure to update the game/editor documentation in doc/ if the patch affects any
player-visible or designer-visible interface. For example, if you add a new special
node type, you need to document it in doc/editor/appendix/Specials.html.
* Do not arbitrarily change indentation, formatting, or names of functions, variables,
or classes. If for some reason you must do this, make it a separate commit that contains
no functional changes (but you should probably get it approved beforehand to avoid
wasting work, unless it's merely spelling fixes).
* Make sure that all three programs still compile and successfully launch. If applicable,
make sure that certain basic actions still work correctly, such as starting a new game,
loading a saved game, creating a new scenario, or opening an existing scenario for
editing.
* Make sure the unit tests still compile and run with no failures. In some cases,
this may require changing the tests.
* If your patch touches file I/O, make sure there's an assertion in the unit tests
that can prevent later regression. In fact, if it touches any area that already has
unit tests, make sure that the fix is covered by the unit tests.
* If your patch alters game mechanics in a way that would change the behaviour of old
scenarios, make sure to add a check so that old scenarios retain the original behaviour.
You can check if a scenario is an old one by testing `univ.scenario.is_legacy`.
