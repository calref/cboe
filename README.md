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

The current task at hand is to merge the Windows code into the current
Mac codebase, which no longer uses platform-specific APIs for most
things (a few files would need to be rewritten for Windows).

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