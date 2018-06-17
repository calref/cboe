#!/bin/sh

#  fix_dylibs.sh
#  BoE
#
#  Created by Celtic Minstrel on 14-04-17.
#

echo "Fixing Boost and SFML dylib install names..."

# Set up variables - Boost
OLD_BOOST_PATH="/usr/local/opt/boost/lib/libboost_system.dylib"
OLD_BOOST_FS_PATH="/usr/local/opt/boost/lib/libboost_filesystem.dylib"
OLD_BOOST_THREAD_PATH="/usr/local/opt/boost/lib/libboost_thread-mt.dylib"
NEW_ABSOLUTE_BOOST_PATH="$1/Contents/Frameworks/libboost_system.dylib"
NEW_ABSOLUTE_BOOST_FS_PATH="$1/Contents/Frameworks/libboost_filesystem.dylib"
NEW_ABSOLUTE_BOOST_THREAD_PATH="$1/Contents/Frameworks/libboost_thread-mt.dylib"
NEW_BOOST_PATH=@rpath"/libboost_system.dylib"
NEW_BOOST_FS_PATH=@rpath"/libboost_filesystem.dylib"
NEW_BOOST_THREAD_PATH=@rpath"/libboost_thread-mt.dylib"

# Set up variables - SFML
OLD_SFML_PATH="/usr/local/opt/sfml/lib/libsfml-system.2.4.2.dylib"
OLD_SFML_GRAPHICS_PATH="/usr/local/opt/sfml/lib/libsfml-graphics.2.4.2.dylib"
OLD_SFML_AUDIO_PATH="/usr/local/opt/sfml/lib/libsfml-audio.2.4.2.dylib"
OLD_SFML_WINDOW_PATH="/usr/local/opt/sfml/lib/libsfml-window.2.4.2.dylib"
NEW_ABSOLUTE_SFML_PATH="$1/Contents/Frameworks/libsfml-system.2.4.2.dylib"
NEW_ABSOLUTE_SFML_GRAPHICS_PATH="$1/Contents/Frameworks/libsfml-graphics.2.4.2.dylib"
NEW_ABSOLUTE_SFML_AUDIO_PATH="$1/Contents/Frameworks/libsfml-audio.2.4.2.dylib"
NEW_ABSOLUTE_SFML_WINDOW_PATH="$1/Contents/Frameworks/libsfml-window.2.4.2.dylib"
NEW_SFML_PATH=@rpath"/libsfml-system.2.4.2.dylib"
NEW_SFML_GRAPHICS_PATH=@rpath"/libsfml-graphics.2.4.2.dylib"
NEW_SFML_AUDIO_PATH=@rpath"/libsfml-audio.2.4.2.dylib"
NEW_SFML_WINDOW_PATH=@rpath"/libsfml-window.2.4.2.dylib"

# Set up variables - General
EXEPATH="$1/Contents/MacOS/$2"

# Fix permissions so we can change stuff.
chmod +wx "$EXEPATH"
chmod +wx "$NEW_ABSOLUTE_BOOST_PATH"
chmod +wx "$NEW_ABSOLUTE_BOOST_FS_PATH"
chmod +wx "$NEW_ABSOLUTE_BOOST_THREAD_PATH"
chmod +wx "$NEW_ABSOLUTE_SFML_PATH"
chmod +wx "$NEW_ABSOLUTE_SFML_GRAPHICS_PATH"
chmod +wx "$NEW_ABSOLUTE_SFML_AUDIO_PATH"
chmod +wx "$NEW_ABSOLUTE_SFML_WINDOW_PATH"

# Add new paths
install_name_tool -add_rpath "$NEW_BOOST_PATH" "$EXEPATH"
install_name_tool -add_rpath "$NEW_BOOST_FS_PATH" "$EXEPATH"
install_name_tool -add_rpath "$NEW_BOOST_THREAD_PATH" "$EXEPATH"
install_name_tool -add_rpath "$NEW_SFML_PATH" "$EXEPATH"
install_name_tool -add_rpath "$NEW_SFML_GRAPHICS_PATH" "$EXEPATH"
install_name_tool -add_rpath "$NEW_SFML_AUDIO_PATH" "$EXEPATH"
install_name_tool -add_rpath "$NEW_SFML_WINDOW_PATH" "$EXEPATH"
install_name_tool -add_rpath "$NEW_BOOST_FS_PATH" "$NEW_ABSOLUTE_BOOST_PATH"
install_name_tool -add_rpath "$NEW_BOOST_THREAD_PATH" "$NEW_ABSOLUTE_BOOST_PATH"
install_name_tool -add_rpath "$NEW_SFML_GRAPHICS_PATH" "$NEW_ABSOLUTE_SFML_PATH"
install_name_tool -add_rpath "$NEW_SFML_AUDIO_PATH" "$NEW_ABSOLUTE_SFML_PATH"
install_name_tool -add_rpath "$NEW_SFML_WINDOW_PATH" "$NEW_ABSOLUTE_SFML_PATH"

# Update references in the executable file
install_name_tool -change "$OLD_BOOST_PATH" "$NEW_BOOST_PATH" "$EXEPATH"
install_name_tool -change "$OLD_BOOST_FS_PATH" "$NEW_BOOST_FS_PATH" "$EXEPATH"
install_name_tool -change "$OLD_BOOST_THREAD_PATH" "$NEW_BOOST_THREAD_PATH" "$EXEPATH"
install_name_tool -change "$OLD_SFML_PATH" "$NEW_SFML_PATH" "$EXEPATH"
install_name_tool -change "$OLD_SFML_GRAPHICS_PATH" "$NEW_SFML_GRAPHICS_PATH" "$EXEPATH"
install_name_tool -change "$OLD_SFML_AUDIO_PATH" "$NEW_SFML_AUDIO_PATH" "$EXEPATH"
install_name_tool -change "$OLD_SFML_WINDOW_PATH" "$NEW_SFML_WINDOW_PATH" "$EXEPATH"

# Update references within Boost
install_name_tool -change "$OLD_BOOST_FS_PATH" "$NEW_BOOST_FS_PATH" "$NEW_ABSOLUTE_BOOST_PATH"
install_name_tool -change "$OLD_BOOST_THREAD_PATH" "$NEW_BOOST_THREAD_PATH" "$NEW_ABSOLUTE_BOOST_PATH"

# Update references within SFML
install_name_tool -change "$OLD_SFML_GRAPHICS_PATH" "$NEW_SFML_GRAPHICS_PATH" "$NEW_ABSOLUTE_SFML_PATH"
install_name_tool -change "$OLD_SFML_AUDIO_PATH" "$NEW_SFML_AUDIO_PATH" "$NEW_ABSOLUTE_SFML_PATH"
install_name_tool -change "$OLD_SFML_WINDOW_PATH" "$NEW_SFML_WINDOW_PATH" "$NEW_ABSOLUTE_SFML_PATH"

# Remove temporary write permissions.
chmod -w "$EXEPATH"
chmod -w "$NEW_ABSOLUTE_BOOST_PATH"
chmod -w "$NEW_ABSOLUTE_BOOST_FS_PATH"
chmod -w "$NEW_ABSOLUTE_BOOST_THREAD_PATH"
chmod -w "$NEW_ABSOLUTE_SFML_PATH"
chmod -w "$NEW_ABSOLUTE_SFML_GRAPHICS_PATH"
chmod -w "$NEW_ABSOLUTE_SFML_AUDIO_PATH"
chmod -w "$NEW_ABSOLUTE_SFML_WINDOW_PATH"