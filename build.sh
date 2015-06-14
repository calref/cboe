#!/bin/sh
# cboe build - paul_erdos 2015-06-14

SRC=`ls src/*.{cpp,mm} | grep -v '.win' &&
	ls src/classes/*.cpp | grep -v '.win' &&
	ls src/dialogxml/*.cpp | grep -v '.win' &&
	ls src/dialogxml/xml-parser/*.cpp | grep -v '.win' &&
	ls src/tools/*.{cpp,mm} | grep -v '.win' &&
	ls src/tools/gzstream/*.cpp | grep -v '.win'`
SRC=`echo $SRC src/pcedit/pc.editors.cpp`

function usage {
	echo "usage: build [-chrw]"
}

function help {
	usage
	echo "options:
-b, --build    clean targets and compile
-c, --clean    clean obj/ directory without compiling
-h, --help     this help text
-r, --run      runs the build"
}

function clean {
	rm obj/
}

function build {
	make game "SRC=$SRC"
}

if [ $# -gt 0 ]; then
	while [ "$1" != "" ]; do
		case $1 in
			-b | --build )  build
							exit;;
			-c | --clean )	clean;;
			-h | --help )	help
							exit;;
			-r | --run )	run;;
			* )				usage
							exit 1
		esac
		shift
	done
else
	build
fi


