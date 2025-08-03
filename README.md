[![CI status](https://github.com/calref/cboe/actions/workflows/ci.yml/badge.svg)](https://github.com/calref/cboe/actions/workflows/ci.yml)

Open Blades of Exile
====================

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

Contributions are accepted primarily via pull request. See
[CONTRIBUTING.md](./CONTRIBUTING.md) for more guidelines.

Before Building
---------------

The most common error when building Open Blades of Exile is to try
building without initializing the submodules first. When you are obtaining
the source code, don't download the zip file directly from GitHub. The best way
to get started is: `git clone --recursive https://github.com/calref/cboe`
(replacing the URL with your fork's address if you forked the repo to make
contributions).

If you already cloned this repo without the `--recursive` flag, use this command:
`git submodule update --init`. This will also fix the problem.

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

Some dependencies are supplied automatically as submodules. You can obtain them by cloning
the repository with `git clone --recursive` or by calling `git submodule update --init`
after making your clone. (**This notice is repeated because it is the MOST COMMON problem
when building Open Blades of Exile.**)

The following dependencies are required:

- [SFML 2](http://www.sfml-dev.org/) - all components except sfml-net.
- [Boost 1.85](http://www.boost.org/) - Filesystem and System, plus several header-only
  libraries; if you're picky, you can run scons and see it enumerate exactly which
  libraries are needed
- ZLib - This is included with the system on the Mac.

For Linux builds, the following additional dependencies are required:
- [TGUI](https://tgui.eu/) - version 0.9, **built with C++14** as shown [here](./.github/workflows/scripts/linux/install-tgui.sh)
  - or, if cmake is available when you call `scons`, TGUI will be built from source automatically
- zenity command-line tools

For Windows builds, [7-zip](https://7-zip.org/download.html) is required.

If you are using the Visual Studio toolset, we recommend installing
[vcpkg](https://github.com/Microsoft/vcpkg) to manage these dependencies.

Use the following command to install the required dependencies. Note: You must call it from one of
the Visual Studio project directories, `cboe/proj/vs2013` or `cboe/proj/vs2017`.

For 32-bit builds:

    vcpkg install --triplet=x86-windows

For 64-bit builds:

    vcpkg install --triplet=x64-windows

If this is the first time installing packages with vcpkg since install you will need to run the `integrate` command:

    vcpkg integrate install

If you're using XCode, you may need to adjust the project file settings to specify the
correct place to search. For example, if you installed Boost via Homebrew, you may need to
add something like the following to the Project Build Settings, under Linking->Other
Linking Flags:

    -lboost_filesystem -lboost_system -lboost_locale -L/usr/local/Cellar/boost/1.57.0/lib

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
