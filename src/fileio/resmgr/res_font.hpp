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
#include <SFML/Graphics.hpp>

using FontRsrc = ResMgr::cPointer<sf::Font>;

namespace ResMgr {
	extern cPool<sf::Font> fonts;
}

#endif
