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

There are currently four ways to build Blades of Exile:

- The build.sh shell-script, which builds and runs a Makefile.
- XCode 4 or later
- Microsoft Visual Studio 2013 or later
- scons

The officially supported method is scons. The script is currently a work-in-progress and
as such may be a little fragile, but we're working on that. With luck, just running
`scons` will simply do the right thing and build a working executable under _build/Blades
of Exile_. If not, though, you may have to adjust a few things.

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
- [Boost](http://www.boost.org/) - Filesystem, System, and Thread, plus several header-only
  libraries; if you're picky, you can run scons and see it enumerate exactly which
  libraries are needed
- ZLib - This is included with the system on the Mac, but you may need to install it if
  compiling on Windows.

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

Helpful Links
-------------
* [Jenkins Server](http://te.calref.net:8080/) - Runs a build after
  every push
* [BoE Forum at Spiderweb
  Software](http://spiderwebforums.ipbhost.com/index.php?/forum/12-blades-
  of-exile/)
* [Original Game
  Source](http://www.spiderwebsoftware.com/blades/opensource.html) - Where
  it all began. Warning: Terrifying code.
* [Ormus's Code](http://info.wsisiz.edu.pl/~kowalsg0/) - The basis for
  most, if not all, of the older Win32 work (prior to the SFML port).
  However, the current Win32 code is not based on it anymore.
* [Experimental Builds](http://celmin.pwcsite.com/oboe/?C=M;O=D) - will be put
  up whenever Celtic Minstrel feels like it.
* [Blades File Dump](http://blades.calref.net/) - A spot to quickly add
  project files; mostly contains older binaries from the Google Code page.
