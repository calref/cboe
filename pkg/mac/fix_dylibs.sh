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
OLD_BOOST_MT_PATH=@loader_path"/libboost_system-mt.dylib"
NEW_BOOST_PATH=@loader_path"/../Frameworks/libboost_system.dylib"
NEW_BOOST_FS_PATH=@loader_path"/../Frameworks/libboost_filesystem.dylib"
NEW_BOOST_THREAD_PATH=@loader_path"/../Frameworks/libboost_thread-mt.dylib"

# Set up variables - SFML
OLD_SFML_PATH="/usr/local/opt/sfml/lib/libsfml-system.2.4.dylib"
OLD_SFML_GRAPHICS_PATH="/usr/local/opt/sfml/lib/libsfml-graphics.2.4.dylib"
OLD_SFML_AUDIO_PATH="/usr/local/opt/sfml/lib/libsfml-audio.2.4.dylib"
OLD_SFML_WINDOW_PATH="/usr/local/opt/sfml/lib/libsfml-window.2.4.dylib"
OLD_INTERNAL_SFML_PATH="@rpath/libsfml-system.2.4.dylib"
OLD_INTERNAL_SFML_WINDOW_PATH="@rpath/libsfml-window.2.4.dylib"
OLD_FREETYPE_PATH="/usr/local/opt/freetype/lib/libfreetype.6.dylib"
OLD_JPEG_PATH="/usr/local/opt/jpeg/lib/libfjpeg.9.dylib"
OLD_VORBISENC_PATH="/usr/local/opt/libvorbis/lib/libvorbisenc.2.dylib"
OLD_VORBISFILE_PATH="/usr/local/opt/libvorbis/lib/libvorbisfile.3.dylib"
OLD_VORBIS_PATH="/usr/local/opt/libvorbis/lib/libvorbis.0.dylib"
OLD_INTERNAL_VORBIS_PATH="/usr/local/Cellar/libvorbis/1.3.6/lib/libvorbis.0.dylib"
OLD_OGG_PATH="/usr/local/opt/libogg/lib/libogg.0.dylib"
OLD_FLAC_PATH="/usr/local/opt/flac/lib/libFLAC.8.dylib"
OLD_PNG_PATH="/usr/local/opt/libpng/lib/libpng16.16.dylib"
NEW_ABSOLUTE_SFML_PATH="$1/Contents/Frameworks/libsfml-system.2.4.2.dylib"
NEW_ABSOLUTE_SFML_GRAPHICS_PATH="$1/Contents/Frameworks/libsfml-graphics.2.4.2.dylib"
NEW_ABSOLUTE_SFML_AUDIO_PATH="$1/Contents/Frameworks/libsfml-audio.2.4.2.dylib"
NEW_ABSOLUTE_SFML_WINDOW_PATH="$1/Contents/Frameworks/libsfml-window.2.4.2.dylib"
NEW_ABSOLUTE_FLAC_PATH="$1/Contents/Frameworks/libFLAC.8.dylib"
NEW_ABSOLUTE_VORBIS_PATH="$1/Contents/Frameworks/libvorbis.0.dylib"
NEW_ABSOLUTE_VORBISENC_PATH="$1/Contents/Frameworks/libvorbisenc.2.dylib"
NEW_ABSOLUTE_VORBISFILE_PATH="$1/Contents/Frameworks/libvorbisfile.3.dylib"
NEW_ABSOLUTE_FREETYPE_PATH="$1/Contents/Frameworks/libfreetype.6.dylib"
NEW_SFML_PATH=@loader_path"/../Frameworks/libsfml-system.2.4.2.dylib"
NEW_SFML_GRAPHICS_PATH=@loader_path"/../Frameworks/libsfml-graphics.2.4.2.dylib"
NEW_SFML_AUDIO_PATH=@loader_path"/../Frameworks/libsfml-audio.2.4.2.dylib"
NEW_SFML_WINDOW_PATH=@loader_path"/../Frameworks/libsfml-window.2.4.2.dylib"
NEW_FREETYPE_PATH=@loader_path"/../Frameworks/libfreetype.6.dylib"
NEW_JPEG_PATH=@loader_path"/../Frameworks/libfjpeg.9.dylib"
NEW_VORBISENC_PATH=@loader_path"/../Frameworks/libvorbisenc.2.dylib"
NEW_VORBISFILE_PATH=@loader_path"/../Frameworks/libvorbisfile.3.dylib"
NEW_VORBIS_PATH=@loader_path"/../Frameworks/libvorbis.0.dylib"
NEW_OGG_PATH=@loader_path"/../Frameworks/libogg.0.dylib"
NEW_FLAC_PATH=@loader_path"/../Frameworks/libFLAC.8.dylib"
NEW_PNG_PATH=@loader_path"/../Frameworks/libpng16.16.dylib"

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
chmod +wx "$NEW_ABSOLUTE_FLAC_PATH"
chmod +wx "$NEW_ABSOLUTE_VORBIS_PATH"
chmod +wx "$NEW_ABSOLUTE_VORBISENC_PATH"
chmod +wx "$NEW_ABSOLUTE_VORBISFILE_PATH"
chmod +wx "$NEW_ABSOLUTE_FREETYPE_PATH"

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
install_name_tool -change "$OLD_BOOST_MT_PATH" "$NEW_BOOST_PATH" "$NEW_ABSOLUTE_BOOST_THREAD_PATH"

# Update references within SFML
install_name_tool -change "$OLD_INTERNAL_SFML_PATH" "$NEW_SFML_PATH" "$NEW_ABSOLUTE_SFML_GRAPHICS_PATH"
install_name_tool -change "$OLD_INTERNAL_SFML_WINDOW_PATH" "$NEW_SFML_WINDOW_PATH" "$NEW_ABSOLUTE_SFML_GRAPHICS_PATH"
install_name_tool -change "$OLD_FREETYPE_PATH" "$NEW_FREETYPE_PATH" "$NEW_ABSOLUTE_SFML_GRAPHICS_PATH"
install_name_tool -change "$OLD_JPEG_PATH" "$NEW_JPEG_PATH" "$NEW_ABSOLUTE_SFML_GRAPHICS_PATH"
install_name_tool -change "$OLD_INTERNAL_SFML_PATH" "$NEW_SFML_PATH" "$NEW_ABSOLUTE_SFML_AUDIO_PATH"
install_name_tool -change "$OLD_VORBISENC_PATH" "$NEW_VORBISENC_PATH" "$NEW_ABSOLUTE_SFML_AUDIO_PATH"
install_name_tool -change "$OLD_VORBISFILE_PATH" "$NEW_VORBISFILE_PATH" "$NEW_ABSOLUTE_SFML_AUDIO_PATH"
install_name_tool -change "$OLD_VORBIS_PATH" "$NEW_VORBIS_PATH" "$NEW_ABSOLUTE_SFML_AUDIO_PATH"
install_name_tool -change "$OLD_OGG_PATH" "$NEW_OGG_PATH" "$NEW_ABSOLUTE_SFML_AUDIO_PATH"
install_name_tool -change "$OLD_FLAC_PATH" "$NEW_FLAC_PATH" "$NEW_ABSOLUTE_SFML_AUDIO_PATH"
install_name_tool -change "$OLD_INTERNAL_SFML_PATH" "$NEW_SFML_PATH" "$NEW_ABSOLUTE_SFML_WINDOW_PATH"

# Update references within references
install_name_tool -change "$OLD_OGG_PATH" "$NEW_OGG_PATH" "$NEW_ABSOLUTE_FLAC_PATH"
install_name_tool -change "$OLD_OGG_PATH" "$NEW_OGG_PATH" "$NEW_ABSOLUTE_VORBIS_PATH"
install_name_tool -change "$OLD_OGG_PATH" "$NEW_OGG_PATH" "$NEW_ABSOLUTE_VORBISENC_PATH"
install_name_tool -change "$OLD_INTERNAL_VORBIS_PATH" "$NEW_VORBIS_PATH" "$NEW_ABSOLUTE_VORBISENC_PATH"
install_name_tool -change "$OLD_OGG_PATH" "$NEW_OGG_PATH" "$NEW_ABSOLUTE_VORBISFILE_PATH"
install_name_tool -change "$OLD_INTERNAL_VORBIS_PATH" "$NEW_VORBIS_PATH" "$NEW_ABSOLUTE_VORBISFILE_PATH"
install_name_tool -change "$OLD_PNG_PATH" "$NEW_PNG_PATH" "$NEW_ABSOLUTE_FREETYPE_PATH"

# Remove temporary write permissions.
chmod -w "$EXEPATH"
chmod -w "$NEW_ABSOLUTE_BOOST_PATH"
chmod -w "$NEW_ABSOLUTE_BOOST_FS_PATH"
chmod -w "$NEW_ABSOLUTE_BOOST_THREAD_PATH"
chmod -w "$NEW_ABSOLUTE_SFML_PATH"
chmod -w "$NEW_ABSOLUTE_SFML_GRAPHICS_PATH"
chmod -w "$NEW_ABSOLUTE_SFML_AUDIO_PATH"
chmod -w "$NEW_ABSOLUTE_SFML_WINDOW_PATH"
chmod -w "$NEW_ABSOLUTE_FLAC_PATH"
chmod -w "$NEW_ABSOLUTE_VORBIS_PATH"
chmod -w "$NEW_ABSOLUTE_VORBISENC_PATH"
chmod -w "$NEW_ABSOLUTE_VORBISFILE_PATH"
chmod -w "$NEW_ABSOLUTE_FREETYPE_PATH"