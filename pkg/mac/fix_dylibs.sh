#!/bin/sh

#  fix_dylibs.sh
#  BoE
#
#  Created by Celtic Minstrel on 14-04-17.
#

echo "Fixing boost dylib install names..."

# Get in the right path.
pushd "$1"

# Set up variables
OLD_BOOST_PATH="/usr/local/opt/boost/lib/libboost_system.dylib"
OLD_BOOST_FS_PATH="/usr/local/opt/boost/lib/libboost_filesystem.dylib"
OLD_BOOST_THREAD_PATH="/usr/local/opt/boost/lib/libboost_thread-mt.dylib"
NEW_BOOST_PATH="@executable_path/../Contents/Frameworks/libboost_system.dylib"
NEW_BOOST_FS_PATH="@executable_path/../Contents/Frameworks/libboost_filesystem.dylib"
NEW_BOOST_THREAD_PATH="@executable_path/../Contents/Frameworks/libboost_thread-mt.dylib"
EXEPATH="Contents/MacOS/$2"

# Fix permissions so we can change stuff.
chmod +wx "$EXEPATH"
chmod +wx "$NEW_BOOST_FS_PATH"
chmod +wx "$NEW_BOOST_THREAD_PATH"

# Update references in the executable file
install_name_tool -change "$OLD_BOOST_PATH" "$NEW_BOOST_PATH" "$EXEPATH"
install_name_tool -change "$OLD_BOOST_FS_PATH" "$NEW_BOOST_FS_PATH" "$EXEPATH"
install_name_tool -change "$OLD_BOOST_THREAD_PATH" "$NEW_BOOST_THREAD_PATH" "$EXEPATH"

# Update references within Boost
install_name_tool -change "$OLD_BOOST_FS_PATH" "$NEW_BOOST_FS_PATH" "$NEW_BOOST_PATH"
install_name_tool -change "$OLD_BOOST_THREAD_PATH" "$NEW_BOOST_THREAD_PATH" "$NEW_BOOST_PATH"

# Remove temporary write permissions.
chmod -w "$EXEPATH"
chmod -w "$NEW_BOOST_FS_PATH"
chmod -w "$NEW_BOOST_THREAD_PATH"

# Restore path.
popd