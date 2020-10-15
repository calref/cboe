/*
 *  restypes.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 10-08-25.
 *
 */

#ifndef BOE_RES_IMAGE_HPP
#define BOE_RES_IMAGE_HPP

#include <SFML/Graphics.hpp>
#include "resmgr.hpp"
#include "texture.hpp"

using ImageRsrc = ResMgr::cPointer<sf::Texture>;
using TextureRsrc = ResMgr::cPointer<Texture>;

namespace ResMgr {
	extern cPool<sf::Texture> graphics;
	// temporary while graphics is not suppressed
	class cPoolTexture : public cPool<Texture> {
	public:
		cPoolTexture(cLoader<Texture>& loader, size_t max, std::string dir = "")
			: cPool<Texture>(loader, max, dir)
		{}
		fs::path popPath() {
			graphics.popPath();
			return cPool<Texture>::popPath();
		}
		void pushPath(const fs::path& path) {
			cPool<Texture>::pushPath(path);
			graphics.pushPath(path);
		}
	};

	extern ResMgr::cPoolTexture textures;
}

#endif
