/*
 *  restypes.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 10-08-25.
 *
 */

#ifndef BOE_RES_SOUND_HPP
#define BOE_RES_SOUND_HPP

#include <SFML/Audio.hpp>
#include "resmgr.hpp"

using SoundRsrc = ResMgr::cPointer<sf::SoundBuffer>;

namespace ResMgr {
	extern cPool<sf::SoundBuffer> sounds;
}

#endif
