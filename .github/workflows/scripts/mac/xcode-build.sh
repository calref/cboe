#!/bin/sh -v

set -o pipefail

xcodebuild ARCHS=x86_64 -project proj/xc$1/BoE.xcodeproj -alltargets -configuration Release |
    tee build.log |
    xcpretty --color

EXIT=$?

if [ ! $EXIT ]; then
    echo 'FULL BUILD LOG:'
    echo
    echo build.log
fi

exit $EXIT