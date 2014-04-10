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
#include <memory>

const int NUM_SOUNDS = 100;

typedef unsigned short snd_num_t;
void init_snd_tool();
void play_sound(short which, short how_many_times = 1);
void one_sound(short which);

void clear_sound_memory();
void flip_sound();

#endif
