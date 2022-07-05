#!/bin/sh -v

sed -E -i '' -e 's| "?/Library/Frameworks/([a-zA-Z-]+)\.framework"?| "/usr/local/Frameworks/\1.framework"|' -e 's|/opt/local/libexec/boost/[0-9.]+/lib/|/usr/local/lib/|' proj/xc12/BoE.xcodeproj/project.pbxproj
