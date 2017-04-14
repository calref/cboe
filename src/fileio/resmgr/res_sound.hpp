/*
 *  restypes.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 10-08-25.
 *
 */

#ifndef BOE_RES_SOUND_HPP
#define BOE_RES_SOUND_HPP

#include <boost/filesystem/path.hpp>
#include <SFML/Audio.hpp>
#include "resmgr.hpp"

using SoundRsrc = sf::SoundBuffer;

// Redeclare this instead of including "fileio.h"
extern std::ostream& std_fmterr(std::ostream& out);

namespace ResMgr {
	/// Load a sound from a WAV file.
	template<> inline SoundRsrc* resLoader<SoundRsrc>::operator() (fs::path fpath) {
		SoundRsrc* snd = new SoundRsrc;
		if(snd->loadFromFile(fpath.string())) return snd;
		delete snd;
		throw xResMgrErr("Failed to load WAV sound: " + fpath.string());
	}
}

#endif
