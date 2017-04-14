/*
 *  restypes.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 10-08-25.
 *
 */

#ifndef BOE_RES_IMAGE_HPP
#define BOE_RES_IMAGE_HPP

#include <boost/filesystem/path.hpp>
#include <SFML/Graphics.hpp>
#include "resmgr.hpp"

using ImageRsrc = sf::Texture;

// Redeclare this instead of including "fileio.h"
extern std::ostream& std_fmterr(std::ostream& out);

namespace ResMgr {
	/// Load an image from a PNG file.
	template<> inline ImageRsrc* resLoader<ImageRsrc>::operator() (fs::path fpath) {
		ImageRsrc* img = new ImageRsrc();
		if(img->loadFromFile(fpath.string())) return img;
		delete img;
		throw xResMgrErr("Failed to load PNG image: " + fpath.string());
	}
}

#endif
