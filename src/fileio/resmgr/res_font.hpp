/*
 *  restypes.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 10-08-25.
 *
 */

#ifndef BOE_RES_FONT_HPP
#define BOE_RES_FONT_HPP

#include "resmgr.hpp"
#include <boost/filesystem/path.hpp>
#include <SFML/Graphics.hpp>

using FontRsrc = sf::Font;

// Redeclare this instead of including "fileio.h"
extern std::ostream& std_fmterr(std::ostream& out);

namespace ResMgr {
	/// Load a font from a TTF file.
	template<> inline FontRsrc* resLoader<FontRsrc>::operator() (fs::path fpath) {
		FontRsrc* theFont = new FontRsrc;
		if(theFont->loadFromFile(fpath.string())) return theFont;
		delete theFont;
		throw xResMgrErr("Failed to find font: " + fpath.string());
	}
}

#endif
