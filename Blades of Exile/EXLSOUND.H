#ifndef _EXLSOUND_H
	#define _EXLSOUND_H
	
#include <windows.h>	// for HMODULE

void move_sound(unsigned char ter,short step);
void one_sound(short which);
void force_play_sound(short which);
void play_sound(short which) ;
void load_sounds (HMODULE handle);
void sound_pause(long len);
void kill_sound();

#endif
