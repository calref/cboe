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

The Win32/ directory should be considered "frozen" from this point on.
Apart from minor bugfixes, it should not be altered. The src/ directory
is to be the official codebase now. As of the time of this writing, it
has a few major issues on Windows which are being worked on.

CBOE relies on a few libraries, and XCode needs to be told where they are. The graphics library we're using is SFML, which is a Framework that can be found [here](http://www.sfml-dev.org/). They need to be linked in the Project Build Phases menu. Then, it needs to be included in the Project Build Settings, under Apple LLVM Custom Compiler Flags -> Other C++ Flags, and should have this added to the end:

    -framework SFML

The other dependency is Boost, a C++ library, which can be found [here](http://www.boost.org/), however the most straightforward way to install it is via homebrew or some similar package manager. Wherever you put it, XCode needs to be linked to it via the Project Build Settings, under Linking->Other Linking flags, which should look similar to this:

    -lboost_filesystem -lboost_system -L/usr/local/Cellar/boost/1.57.0/lib

With the path in the third include being wherever you put the boost lib folder. In the same place we put the framework flag for SFML, the boost includes should be included, as such:

    -isystem/user/local/Cellar/boost/1.57.0/include

Again, the filepath just needs to point to Boost's includes.

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
most, if not all, of the Win32 work.
* [Experimental Builds](http://celmin.pwcsite.com/oboe/) - will be put
up whenever Celtic Minstrel feels like it.
* [Blades File Dump](http://blades.calref.net/) - A spot to quickly add
project files; mostly contains older binaries from the Google Code page.
