#! /bin/bash

# Call this script from the repository root:
# ./test/replays.sh <no_fail_fast: optional> <thoroughness: default 1>

# examples:
# ./test/replays.sh no_fail_fast 1
# ./test/replays.sh 0

# thoroughness values (these can add huge wait times):
#  0: short
#  1: short + long

# Use Git Bash on Windows.

# Only scons builds are supported

fail_fast="true"
thoroughness=0
if [ "$1" = "no_fail_fast" ]; then 
    fail_fast="false"
    thoroughness="${2:-1}"
else
    thoroughness="${1:-1}"
fi

status=$(git diff-index --quiet HEAD -- && echo "clean" || echo "modified")
# TODO don't update success if the repo is modified

EXE_DIR="build/Blades of Exile"
EXE="Blades of Exile"
PLATFORM=""

if [ "$(uname)" = "Linux" ]; then
    PLATFORM="linux"
elif [ "$(uname)" = "Darwin" ]; then
    PLATFORM="mac"
    EXE_DIR="${EXE_DIR}/Blades of Exile.app/Contents/MacOS"
# Git Bash on Windows gives a more complicated, probably unreliable answer for $(uname)
else
    PLATFORM="windows"
    EXE="${EXE}.exe"
fi

if [ ! -f "$EXE_DIR/$EXE" ]; then
    echo "Build Blades of Exile with Scons first"
    exit 1
fi

function test_folder() {
    folder="$(pwd)/test/replays/$1"
    replays="$folder"/*.xml
    for replay in $replays; do
        (cd "$EXE_DIR" && "./$EXE" --replay "$replay") || echo "Replay failed: $replay" && [ "$fail_fast" = "true" ] && exit 1
    done
}

case $thoroughness in
    0)
        test_folder short
        ;;
    1)
        test_folder short
        test_folder long
        ;;
esac