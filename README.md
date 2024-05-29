[![CI status](https://github.com/calref/cboe/actions/workflows/ci.yml/badge.svg)](https://github.com/calref/cboe/actions/workflows/ci.yml)

Classic Blades of Exile
=======================

This project hosts the source code to the classic RPG creator Blades of
Exile after it was released
by [Spiderweb Software](http://www.spiderwebsoftware.com/) under GPLv2.
It is a svn-git transfer from the
[Google Code Project](http://code.google.com/p/openexile/); all issues
and trunk code has been ported.

The primary purpose of this project is to provide bug fixes to Blades of
Exile
so that it can compile and run on modern operating systems. The
secondary purpose is
to generalize and extend its functionality while maintaining backwards
compatibility.

If you would like to be added to this project contact
[Sylae](https://github.com/sylae)
or one of the other people on the owners list. This is an open project,
with no strong prerequisites
for getting commit access. With that in mind, please be careful of what
you check into the repository.
Please do not commit any binaries, junk files, or anything that doesn't
compile.

Building
--------

There are currently three ways to build Blades of Exile:

- XCode 4 or later
- Microsoft Visual Studio 2013/2017
- scons

The officially supported method is scons. It's set up to work on any platform
with any toolset, but only the following have been tested:

- Mac with Xcode tools
- Windows with Microsoft Cl
- Linux with clang or gcc

The script is currently a work-in-progress and
as such may be a little fragile, but we're working on that.
With luck, just running `scons` will simply do the right thing and
build a working executable under _build/Blades of Exile_.
If not, though, you may have to adjust a few things.

You can pass the following parameters to scons:

- OS=windows|darwin - Override platform auto-detection (only partially implemented)
- LIBPATH=pathlist - Specify where to look for non-system dynamic libraries to link
  against.
- FRAMEWORKPATH=pathlist (Mac only) - Specify where to look for non-system frameworks.
- INCLUDEPATH=pathlist - Specify where to look for non-system header files.

If you can't get that to work, you can copy the required dependencies into _deps/lib_ (for
dynamic libraries and frameworks) or _deps/include_ (for headers). That means that
_deps/include_ would have two subfolders _boost/_ and _SFML/_.

The following dependencies are required:

- [SFML](http://www.sfml-dev.org/) - all components except sfml-net.
- [Boost](http://www.boost.org/) - Filesystem and System, plus several header-only
  libraries; if you're picky, you can run scons and see it enumerate exactly which
  libraries are needed
- ZLib - This is included with the system on the Mac.

For Linux builds, the following additional dependencies are required:
- [TGUI](https://tgui.eu/) - version 0.9 or later required
- zenity command-line tools

If you are using the Visual Studio toolset, we recommend installing
[vcpkg](https://github.com/Microsoft/vcpkg) to manage these dependencies.
Use the following commands to install the required dependencies:
For 32-bit builds:

    vcpkg install --triplet=x86-windows zlib sfml opengl boost-any boost-dynamic-bitset boost-ptr-container boost-core boost-filesystem boost-system boost-date-time boost-chrono boost-math boost-spirit

For 64-bit builds:

    vcpkg install --triplet=x64-windows zlib sfml opengl boost-any boost-dynamic-bitset boost-ptr-container boost-core boost-filesystem boost-system boost-date-time boost-chrono boost-math boost-spirit

If this is the first time installing packages with vcpkg since install you will need to run the `integrate` command:

    vcpkg integrate install

If you're using XCode, you may need to adjust the project file settings to specify the
correct place to search. For example, if you installed Boost via Homebrew, you may need to
add something like the following to the Project Build Settings, under Linking->Other
Linking Flags:

    -lboost_filesystem -lboost_system -L/usr/local/Cellar/boost/1.57.0/lib

Again with Homebrew, you may also need to add the following to Apple LLVM Custom
Compiler Flags -> Other C++ Flags:

    -isystem/usr/local/Cellar/boost/1.57.0/include

Notes and oddities:

- The XCode build uses SFML frameworks, while the scons build on the Mac uses
  SFML dynamic libraries.
- On Windows, scons assumes that Boost is statically linked and thus does not copy
  the dlls into the program directory. However, on the Mac, scons links dynamically with
  Boost.

Contributing
------------

If you want to contribute code to the project, please open a pull request.
Be sure to read the [style guide](StyleGuide.md) before coding.

This project targets the subset of C++11 supported by both Xcode 4 and Visual Studio 2013.
That means that generally you can use C++11, but the following features (at least) are not permitted:

* Ref-qualifiers
* Attributes
* Inheriting constructors (via `using`)
* Unicode string literals
* User-defined literals

Helpful Links
-------------
* [BoE Forum at Spiderweb Software](http://spiderwebforums.ipbhost.com/index.php?/forum/12-blades-of-exile/)
* [Official IRC channel](https://web.libera.chat/#openboe) (or join
#openboe on irc.libera.chat)
* [Original Game Source](http://www.spiderwebsoftware.com/blades/opensource.html) - Where
  it all began. Warning: Terrifying code.
* [Experimental Windows/Mac Builds](http://pentalithia.ca/oboe/?C=M;O=D) - will be put
  up whenever Celtic Minstrel feels like it.
* [Official Web Site](https://openboe.com/) - The hub for all things OpenBoE. The documentation is also in viewable format, and there is an archive of all known scenarios that could be located.
