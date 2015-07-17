/*
 *  soundtool.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */
#include "soundtool.hpp"

#include <cmath>
#include <cstdio>
#include <sstream>
#include <memory>
#include <unordered_set>
#include <unordered_map>

#include "restypes.hpp"
#include "mathutil.hpp"

sf::Sound chan[4];
const int numchannel = 4;
int channel;
short snd_played[4];

bool play_sounds = true;
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

static bool sound_going(snd_num_t which_s) {
	short i;
	
	for(i = 0; i < 4; i++)
		if(snd_played[i] == which_s)
			return chan[i].getStatus() == sf::Sound::Playing;
	return false;
}

static std::string sound_to_fname_map(snd_num_t snd_num) {
	std::ostringstream sout;
	sout << "SND" << snd_num;
	return sout.str();
}

void init_snd_tool(){
	ResMgr::setIdMapFn<SoundRsrc>(sound_to_fname_map);
}

void play_sound(short which, short how_many_times) { // if < 0, play asynch
	static bool inited = false;
	if(!inited) {
		inited = true;
		ResMgr::setIdMapFn<SoundRsrc>(sound_to_fname_map);
	}
	
	std::shared_ptr<sf::SoundBuffer> sndhandle;
	if(!play_sounds || how_many_times == 0) return;
	
	if(abs(which) > NUM_SOUNDS) {
		std::cerr << "Error: Sound #" << abs(which) << " does not exist." << std::endl;
		return;
	}
	
	channel++;
	
	if(channel >= numchannel) channel = 0;
	
	if(!sound_going(abs(which)))
		sndhandle = ResMgr::get<SoundRsrc>(abs(which));
	
	if(which > 0)
 		if(always_async.find(which) != always_async.end())
			which *= -1;
	
 	if(sndhandle) {
		chan[channel] = sf::Sound(*sndhandle);
		chan[channel].play();
		
		if(which > 0) {
			while(chan[channel].getStatus() == sf::Sound::Playing);
		}
		snd_played[channel] = abs(which);
	}
	if(which < 0)
		sf::sleep(time_in_ticks(sound_delay[-1 * which]));
	if(how_many_times > 1)
		play_sound(which, how_many_times - 1);
}

void one_sound(short which) {
	if(which == last_played)
		return;
	play_sound(which);
	last_played = which;
}

void clear_sound_memory(){
	last_played = 100;
}

void flip_sound() {
	play_sounds = (play_sounds) ? false : true;
}

