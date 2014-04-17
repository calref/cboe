#!/bin/sh

#  fix_dylibs.sh
#  BoE
#
#  Created by Celtic Minstrel on 14-04-17.
#

echo Fixing boost dylib install names...

EXEPATH=@executable_path/../Frameworks
BOEPATH="$BUILT_PRODUCTS_DIR/$EXECUTABLE_PATH"
BFSPATH="$BUILT_PRODUCTS_DIR/$FRAMEWORKS_FOLDER_PATH/libboost_filesystem.dylib"

# Update references in the executable file
install_name_tool -change libboost_system.dylib "$EXEPATH/libboost_system.dylib" "$BOEPATH"
install_name_tool -change libboost_filesystem.dylib "$EXEPATH/libboost_filesystem.dylib" "$BOEPATH"

# Update references within Boost
install_name_tool -change libboost_system.dylib "$EXEPATH/libboost_system.dylib" "$BFSPATH"

