/*
 *  soundtool.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#ifndef _SOUNDTOOL_H
#define _SOUNDTOOL_H

#define NUM_SOUNDS 100

typedef unsigned short snd_num_t;
void init_snd_tool();
void play_sound(short which, short how_many_times = 1);
void one_sound(short which);

#if defined(__APPLE__)
void clear_sound_memory();
void flip_sound();

extern Handle sound_handles[NUM_SOUNDS];
extern SndChannelPtr chan[4];
extern char numchannel;
extern char channel;
#elif defined(WIN32)
#include <windows.h>	// for HMODULE
void load_sounds (HMODULE handle);
void sound_pause(long len);
void kill_sound();

extern HMODULE hModule;
extern short can_ignore[100];
extern bool sounds_missing;
extern HGLOBAL sound_handles[NUM_SOUNDS];
extern char* snds[NUM_SOUNDS];
#else
#error Invalid platform
#endif

extern short snd_played[4];
extern bool play_sounds;
extern short last_played;
extern bool always_asynch[100];
extern bool load_when_play[100];
extern short sound_delay[100];
extern short store_last_sound_played;

void move_sound(unsigned char ter,short step);

#endif
