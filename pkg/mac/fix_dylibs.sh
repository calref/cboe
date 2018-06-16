#!/bin/sh

#  fix_dylibs.sh
#  BoE
#
#  Created by Celtic Minstrel on 14-04-17.
#

echo "Fixing boost dylib install names..."

EXEPATH="Contents/Frameworks"
BOEPATH="Contents/MacOS/$2"
BFSPATH="$EXEPATH/libboost_filesystem.dylib"
BTHPATH="$EXEPATH/libboost_thread-mt.dylib"

# Fix permissions so we can change stuff.
chmod +wx "$EXEPATH/libboost_system.dylib"
chmod +wx "$EXEPATH/libboost_filesystem.dylib"
chmod +wx "$EXEPATH/libboost_thread-mt.dylib"

# Update references in the executable file
install_name_tool -rpath libboost_system.dylib "$EXEPATH/libboost_system.dylib" "$BOEPATH"
install_name_tool -rpath libboost_filesystem.dylib "$EXEPATH/libboost_filesystem.dylib" "$BOEPATH"
install_name_tool -rpath libboost_thread.dylib "$EXEPATH/libboost_thread-mt.dylib" "$BOEPATH"

# Update references within Boost
install_name_tool -rpath libboost_system.dylib "$EXEPATH/libboost_system.dylib" "$BFSPATH"
install_name_tool -rpath libboost_system.dylib "$EXEPATH/libboost_system.dylib" "$BTHPATH"

# Remove temporary write permissions.
chmod -w "$EXEPATH/libboost_system.dylib"
chmod -w "$EXEPATH/libboost_filesystem.dylib"
chmod -w "$EXEPATH/libboost_thread-mt.dylib"