//
//  restypes.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-09-25.
//
//

#include "restypes.hpp"

namespace ResMgr {
	template<> const std::string resLoader<ImageRsrc>::file_ext = "png";
	template<> const std::string resLoader<CursorRsrc>::file_ext = "gif";
	template<> const std::string resLoader<FontRsrc>::file_ext = "ttf";
	template<> const std::string resLoader<StringRsrc>::file_ext = "txt";
	template<> const std::string resLoader<SoundRsrc>::file_ext = "wav";
}
