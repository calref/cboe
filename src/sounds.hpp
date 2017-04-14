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

typedef signed int snd_num_t;
void init_snd_tool();
bool sound_going(snd_num_t which_s);
void play_sound(short which, sf::Time delay = sf::Time());
void one_sound(short which);

void clear_sound_memory();

#endif
