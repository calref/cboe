#!/bin/sh
# cboe build - paul_erdos 2015-06-14

COMMON_SRC=`
	ls src/classes/*.cpp | grep -v '\.win' |
		sed -E 's/es\/([^\/]*)\.cpp$/_\1.o/ ; s/src/obj/' &&
	ls src/dialogxml/*.cpp | grep -v '\.win' |
		sed -E 's/xml\/([^\/]*)\.cpp$/_\1.o/ ; s/src/obj/' &&
	ls src/dialogxml/xml-parser/*.cpp | grep -v '\.win' |
		sed 's/dialogxml\/xml-parser\/// ; s/\.cpp/.o/ ; s/src/obj/' &&
	ls src/tools/*.{cpp,mm} | grep -v '\.win' |
		sed -E 's/\/([^\/]*)\.(cpp|mm)$/_\1.o/ ; s/src/obj/'
`
BOE_SRC=`
	ls src/boe.*.{cpp,mm} | grep -v '.win' |
		sed -E 's/\.(cpp|mm)$/.o/ ; s/src/obj/'
`
ED_SRC=`
	ls src/scenedit/scen.*.{cpp,mm} | grep -v '\.win' |
		sed -E 's/scenedit\/// ; s/\.(cpp|mm)$/.o/ ; s/src/obj/'
`
PC_SRC=`
	ls src/pcedit/pc.*.{cpp,mm} | grep -v '\.win' |
		sed -E 's/pcedit\/// ; s/\.(cpp|mm)$/.o/ ; s/src/obj/'
`
COMMON_SRC=`echo $COMMON_SRC`
BOE_SRC=`echo $BOE_SRC src/pcedit/pc.editors.cpp`
ED_SRC=`echo $ED_SRC`
PC_SRC=`echo $PC_SRC`

PLATFORM=macosx

function usage {
	echo "usage: $0 [-chrw]"
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
	rm -rf obj/ exe/
}

function build {
	mkdir -p obj exe/bin 'exe/Blades of Exile/data'
	mkdir -p 'exe/Blades of Exile/Blades of Exile Scenarios/'
	mkdir -p 'exe/Blades of Exile/Scenario Editor/Blades of Exile Base'
	echo "$COMMON_SRC" > test.txt
	export COMMON_SRC BOE_SRC ED_SRC PC_SRC PLATFORM
	make all
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


