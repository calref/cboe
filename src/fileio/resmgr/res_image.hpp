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
#include "gfx/texture.hpp"

using TextureRsrc = ResMgr::cPointer<Texture>;

namespace ResMgr {
	extern cPool<Texture> graphics;
}

#endif
