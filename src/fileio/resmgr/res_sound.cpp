/*
 *  restypes.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 10-08-25.
 *
 */

#include "res_sound.hpp"

class SoundLoader : public ResMgr::cLoader<sf::SoundBuffer> {
	/// Load a sound from a WAV file.
	sf::SoundBuffer* operator() (const fs::path& fpath) const override {
		sf::SoundBuffer* snd = new sf::SoundBuffer;
		if(snd->loadFromFile(fpath.string())) return snd;
		delete snd;
		throw ResMgr::xError(ResMgr::ERR_LOAD, "Failed to load WAV sound: " + fpath.string());
	}

	ResourceList expand(const std::string& name) const override {
		return {name + ".wav"};
	}

	std::string typeName() const override {
		return "sound";
	}
};

// TODO: What's a good max sound count?
static SoundLoader loader;
ResMgr::cPool<sf::SoundBuffer> ResMgr::sounds(loader, 50);
