#!/bin/sh

#  fix_dylibs.sh
#  BoE
#
#  Created by Celtic Minstrel on 14-04-17.
#

echo "Fixing boost dylib install names..."

# Get in the right path.
pushd "$1"

# Set up variables - Boost
OLD_BOOST_PATH="/usr/local/opt/boost/lib/libboost_system.dylib"
OLD_BOOST_FS_PATH="/usr/local/opt/boost/lib/libboost_filesystem.dylib"
OLD_BOOST_THREAD_PATH="/usr/local/opt/boost/lib/libboost_thread-mt.dylib"
NEW_RELATIVE_BOOST_PATH="Contents/Frameworks/libboost_system.dylib"
NEW_RELATIVE_BOOST_FS_PATH="Contents/Frameworks/libboost_filesystem.dylib"
NEW_RELATIVE_BOOST_THREAD_PATH="Contents/Frameworks/libboost_thread-mt.dylib"
NEW_BOOST_PATH="@executable_path/../Contents/Frameworks/libboost_system.dylib"
NEW_BOOST_FS_PATH="@executable_path/../Contents/Frameworks/libboost_filesystem.dylib"
NEW_BOOST_THREAD_PATH="@executable_path/../Contents/Frameworks/libboost_thread-mt.dylib"

# Set up variables - SFML
OLD_SFML_PATH="/usr/local/opt/sfml/lib/libsfml-system.2.4.2.dylib"
OLD_SFML_GRAPHICS_PATH="/usr/local/opt/sfml/lib/libsfml-graphics.2.4.2.dylib"
OLD_SFML_AUDIO_PATH="/usr/local/opt/sfml/lib/libsfml-audio.2.4.2.dylib"
OLD_SFML_WINDOW_PATH="/usr/local/opt/sfml/lib/libsfml-window.2.4.2.dylib"
NEW_RELATIVE_SFML_PATH="Contents/Frameworks/libsfml-system.2.4.2.dylib"
NEW_RELATIVE_SFML_GRAPHICS_PATH="Contents/Frameworks/libsfml-graphics.2.4.2.dylib"
NEW_RELATIVE_SFML_AUDIO_PATH="Contents/Frameworks/libsfml-audio.2.4.2.dylib"
NEW_RELATIVE_SFML_WINDOW_PATH="Contents/Frameworks/libsfml-window.2.4.2.dylib"
NEW_SFML_PATH="@executable_path/../Contents/Frameworks/libsfml-system.2.4.2.dylib"
NEW_SFML_GRAPHICS_PATH="@executable_path/../Contents/Frameworks/libsfml-graphics.2.4.2.dylib"
NEW_SFML_AUDIO_PATH="@executable_path/../Contents/Frameworks/libsfml-audio.2.4.2.dylib"
NEW_SFML_WINDOW_PATH="@executable_path/../Contents/Frameworks/libsfml-window.2.4.2.dylib"

# Set up variables - General
EXEPATH="Contents/MacOS/$2"

# Fix permissions so we can change stuff.
chmod +wx "$EXEPATH"
chmod +w "$NEW_RELATIVE_BOOST_FS_PATH"
chmod +w "$NEW_RELATIVE_BOOST_THREAD_PATH"
chmod +w "$NEW_RELATIVE_SFML_GRAPHICS_PATH"
chmod +w "$NEW_RELATIVE_SFML_AUDIO_PATH"
chmod +w "$NEW_RELATIVE_SFML_WINDOW_PATH"

# Update references in the executable file
install_name_tool -change "$OLD_BOOST_PATH" "$NEW_BOOST_PATH" "$EXEPATH"
install_name_tool -change "$OLD_BOOST_FS_PATH" "$NEW_BOOST_FS_PATH" "$EXEPATH"
install_name_tool -change "$OLD_BOOST_THREAD_PATH" "$NEW_BOOST_THREAD_PATH" "$EXEPATH"
install_name_tool -change "$OLD_SFML_PATH" "$NEW_SFML_PATH" "$EXEPATH"
install_name_tool -change "$OLD_SFML_GRAPHICS_PATH" "$NEW_SFML_GRAPHICS_PATH" "$EXEPATH"
install_name_tool -change "$OLD_SFML_AUDIO_PATH" "$NEW_SFML_AUDIO_PATH" "$EXEPATH"
install_name_tool -change "$OLD_SFML_WINDOW_PATH" "$NEW_SFML_WINDOW_PATH" "$EXEPATH"

# Update references within Boost
install_name_tool -change "$OLD_BOOST_FS_PATH" "$NEW_BOOST_FS_PATH" "$NEW_RELATIVE_BOOST_PATH"
install_name_tool -change "$OLD_BOOST_THREAD_PATH" "$NEW_BOOST_THREAD_PATH" "$NEW_RELATIVE_BOOST_PATH"

# Update references within SFML
install_name_tool -change "$OLD_SFML_GRAPHICS_PATH" "$NEW_SFML_GRAPHICS_PATH" "$NEW_RELATIVE_SFML_PATH"
install_name_tool -change "$OLD_SFML_AUDIO_PATH" "$NEW_SFML_AUDIO_PATH" "$NEW_RELATIVE_SFML_PATH"
install_name_tool -change "$OLD_SFML_WINDOW_PATH" "$NEW_SFML_WINDOW_PATH" "$NEW_RELATIVE_SFML_PATH"

# Remove temporary write permissions.
chmod -w "$EXEPATH"
chmod -w "$NEW_RELATIVE_BOOST_FS_PATH"
chmod -w "$NEW_RELATIVE_BOOST_THREAD_PATH"
chmod -w "$NEW_RELATIVE_SFML_GRAPHICS_PATH"
chmod -w "$NEW_RELATIVE_SFML_AUDIO_PATH"
chmod -w "$NEW_RELATIVE_SFML_WINDOW_PATH"

# Restore path.
popd