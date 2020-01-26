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

using ImageRsrc = ResMgr::cPointer<sf::Texture>;

namespace ResMgr {
	extern cPool<sf::Texture> graphics;
}

#endif
