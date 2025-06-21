/*
 *  soundtool.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#ifndef _SOUNDTOOL_H
#define _SOUNDTOOL_H

#include <SFML/Audio.hpp>
#include <string>

void init_snd_tool();
bool sound_going(snd_num_t which_s);
void play_sound(snd_num_t which, sf::Time delay = sf::Time(), bool force = false);
inline void force_play_sound(snd_num_t which, sf::Time delay = sf::Time()) { play_sound(which, delay, true); }
void one_sound(snd_num_t which);
std::string sound_to_fname(snd_num_t snd_num);

void clear_sound_memory();

#endif
