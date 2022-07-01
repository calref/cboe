#!/bin/sh

#  fix_dylibs.sh
#  BoE
#
#  Created by Celtic Minstrel on 14-04-17.
#

echo Fixing boost dylib install names...

EXEPATH=@executable_path/../Frameworks
BOEPATH="$BUILT_PRODUCTS_DIR/$EXECUTABLE_PATH"

cd "$BUILT_PRODUCTS_DIR/$FRAMEWORKS_FOLDER_PATH"

FSPATH=libboost_filesystem*.dylib
SYSPATH=libboost_system*.dylib

echo $FSPATH $SYSPATH

# Update references in the executable file
install_name_tool -change "$SYSPATH" "$EXEPATH/libboost_system.dylib" "$BOEPATH"
install_name_tool -change "$FSPATH" "$EXEPATH/libboost_filesystem.dylib" "$BOEPATH"

# Update references within Boost
install_name_tool -id "$SYSPATH" $SYSPATH
install_name_tool -id "$FSPATH" $FSPATH
install_name_tool -change "$SYSPATH" "$EXEPATH/libboost_system-mt.dylib" $FSPATH

