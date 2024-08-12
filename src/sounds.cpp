/*
 *  soundtool.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */
#include "sounds.hpp"

#include <cmath>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <memory>
#include <unordered_set>
#include <unordered_map>

#include "fileio/resmgr/res_sound.hpp"
#include "mathutil.hpp"
#include "tools/prefs.hpp"
#include "utility.hpp"

struct SoundChannel {
	std::shared_ptr<sf::Sound> player;
	std::shared_ptr<const sf::SoundBuffer> sound;
	void play(const std::shared_ptr<const sf::SoundBuffer> what) {
		sound = what;
		player->setBuffer(*what);
		player->play();
	}
	bool isPlaying() const {
		return player->getStatus() == sf::Sound::Playing;
	}
	void reset() {
		player.reset();
		sound.reset();
	}
	void init() {
		player.reset(new sf::Sound);
	}
};

SoundChannel chan[4];
const int numchannel = 4;
int channel;
short snd_played[4];

short last_played;
std::unordered_set<int> always_async = {
	6,24,25,34,37,
	39,41,42,43,44,
	45,46,47,48,49,
	50,55,61,76,77,
	78,79,80,81,82,
	83,85,91
};
std::unordered_map<int,int> sound_delay = {
	{24,25},{25,25},{34,8},{37,8},{43,10},
	{44,20},{61,13}
};
short store_last_sound_played;

bool sound_going(snd_num_t which_s) {
	for(short i = 0; i < 4; i++)
		if(snd_played[i] == which_s)
			return chan[i].isPlaying();
	return false;
}

std::string sound_to_fname(snd_num_t snd_num) {
	std::ostringstream sout;
	sout << "SND" << snd_num;
	return sout.str();
}

static void exit_snd_tool() {
	for(auto& ch : chan) ch.reset();
	ResMgr::sounds.drain();
}

void init_snd_tool(){
	for(auto& ch : chan) ch.init();
	atexit(exit_snd_tool);
}

void play_sound(snd_num_t which, sf::Time delay) { // if < 0, play asynch
	std::shared_ptr<const sf::SoundBuffer> sndhandle;
	if(!get_bool_pref("PlaySounds", true)) {
		if(which >= 0)
			event_sleep(delay);
		return;
	}
	
	std::string sndname = sound_to_fname(abs(which));
	
	if(abs(which) >= 100 && !ResMgr::sounds.have(sndname)) {
		std::cerr << "Error: Sound #" << abs(which) << " does not exist." << std::endl;
		return;
	}
	
	channel++;
	
	if(channel >= numchannel) channel = 0;
	
	if(!sound_going(abs(which)))
		sndhandle = &ResMgr::sounds.get(sndname);
	
	if(which > 0)
 		if(always_async.find(which) != always_async.end())
			which *= -1;
	
 	if(sndhandle) {
		chan[channel].play(sndhandle);
		
		if(which > 0) {
			while(chan[channel].isPlaying());
		}
		snd_played[channel] = abs(which);
	}
	if(which < 0)
		event_sleep(time_in_ticks(sound_delay[-1 * which]));
}

void one_sound(snd_num_t which) {
	if(which == last_played)
		return;
	play_sound(which);
	last_played = which;
}

void clear_sound_memory(){
	last_played = 100;
}

