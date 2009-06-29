#include "math.h"
#include <windows.h>
#include <mmsystem.h>

#include "stdio.h"

#include "global.h"
#include "exlsound.h"
#include "text.h"
#include "fields.h"

#include "globvar.h"

#define	NUM_SOUNDS	100

void load_sounds(HMODULE handle)
{
	short i,t,err;
	HRSRC h;
	char snd_name[20];
	WAVEOUTCAPS wavecaps;
	
	hModule = handle;

	t = waveOutGetNumDevs();
	if (t == 0) {
		sounds_fucked = TRUE;
		return;
		}
	err = waveOutGetDevCaps(0,&wavecaps,sizeof(WAVEOUTCAPS));
if (err != 0) {
	sounds_fucked = TRUE;
	switch (err) {
		case MMSYSERR_BADDEVICEID:
		MessageBox(mainPtr,"Cannot initialize sounds - No sound device detected. Game can still be played, but quietly.",
	  "Sound Error",MB_OK | MB_ICONEXCLAMATION);
		return;
		case MMSYSERR_NODRIVER:
		MessageBox(mainPtr,"Cannot initialize sounds - No driver installed. Game can still be played, but quietly.",
	  "Sound Error",MB_OK | MB_ICONEXCLAMATION);
		return;
		case MMSYSERR_NOMEM :
		MessageBox(mainPtr,"Cannot initialize sounds - can't find enough memory. Game can still be played, but quietly.",
	  "Sound Error",MB_OK | MB_ICONEXCLAMATION);
		return;
		case MMSYSERR_ALLOCATED:
		MessageBox(mainPtr,"Cannot initialize sounds - sound card already allocated. Game can still be played, but quietly.",
	  "Sound Error",MB_OK | MB_ICONEXCLAMATION);
		return;
		case MMSYSERR_ERROR:
		MessageBox(mainPtr,"Cannot initialize sounds - internal error. Game can still be played, but quietly.",
	  "Sound Error",MB_OK | MB_ICONEXCLAMATION);
		return;
		default:
		MessageBox(mainPtr,"Cannot initialize sounds - unidentified error. Game can still be played, but quietly.",
	  "Sound Error",MB_OK | MB_ICONEXCLAMATION);
		return;

		}

	}

	for (i = 0; i < NUM_SOUNDS; i++) {
		sound_handles[i] = NULL;
		if (load_when_play[i] == FALSE) {
		sprintf((char *)snd_name,"#%d",i + 1);
		h = FindResource(handle,snd_name,"#100");

		sound_handles[i] = LoadResource(handle,h);
		snds[i] = (char*) LockResource(sound_handles[i]);
		}
		}

}

void play_sound(short which)  // if < 0, play asynch
{
	if (play_sounds == TRUE)
		force_play_sound(which);
}


void force_play_sound(short which)
{
	short i,num_fails = 0;
	char snd_name[30];
	Boolean asyn = FALSE,a_sound_did_get_played = FALSE;
	Boolean not_asyn = FALSE,check_sound;
	HRSRC h;

	if ((sounds_fucked == TRUE) || (play_sounds == FALSE))
		return;
	if (which < 0) {
		asyn = TRUE;
		which = which * -1;
		}
	if (which >= 1000) {
		which -= 1000;
		not_asyn = TRUE;
		}

	if (which >= 100)
		return;

	if ((always_asynch[which] == TRUE) &&
	((can_ignore[which] == 1) || (can_ignore[which] >= 3)))
		asyn = TRUE;
	if ((can_ignore[which] > 0) && (can_ignore[which] < 5) && (party.stuff_done[305][5] == 1))
		return;
	if ((can_ignore[which] != 1) && (can_ignore[which] < 3))
		asyn = FALSE;
	if ((party.stuff_done[305][5] == 1) && (can_ignore[which] < 5))
		asyn = FALSE;
	if (not_asyn == TRUE)
		asyn = FALSE;

	if ((load_when_play[which] == TRUE) && (sound_handles[which] == NULL)) {
			asyn = FALSE;
		sprintf((char *)snd_name,"#%d",which + 1);
		h = FindResource(hModule,snd_name,"#100");

		sound_handles[which] = LoadResource(hModule,h);
		snds[which] = (char *) LockResource(sound_handles[which]);

		}

	if (store_last_sound_played == 6)
		sndPlaySound(NULL,0);

	if (asyn == TRUE) {
		if (can_ignore[which] >= 4)
			check_sound = sndPlaySound(snds[which],SND_ASYNC | SND_MEMORY | SND_NOSTOP);
			else check_sound = sndPlaySound(snds[which],SND_ASYNC | SND_MEMORY);

		while (check_sound == FALSE) {

			if (can_ignore[store_last_sound_played] == 4) {// then sound goes away
				return;
				}


			num_fails++;
			if (num_fails < 40)
				sound_pause(25);
				else {
					MessageBox(mainPtr,"Cannot play sounds - Sounds stuck error a. Game can still be played, but quietly. Check to make sure your sound drivers are up to date and not corrupted.",
					  "Sound Error",MB_OK | MB_ICONEXCLAMATION);
					print_nums(111,which,num_fails);
					sounds_fucked = TRUE;
					return;
					}
			sndPlaySound(NULL,0);

			if (can_ignore[which] >= 4)
				check_sound = sndPlaySound(snds[which],SND_ASYNC | SND_MEMORY | SND_NOSTOP);
				else check_sound = sndPlaySound(snds[which],SND_ASYNC | SND_MEMORY);
			}
	  a_sound_did_get_played = TRUE;
	  }
		else {
		if (can_ignore[which] >= 4)
			check_sound = sndPlaySound(snds[which],SND_SYNC | SND_MEMORY | SND_NOSTOP);
			else check_sound = sndPlaySound(snds[which],SND_SYNC | SND_MEMORY);
		while (check_sound == FALSE) {
			if (can_ignore[store_last_sound_played] == 4) {// then sound goes away
				return;
				}


			num_fails++;
			if (num_fails < 40)
				sound_pause(25);
				else {
					MessageBox(mainPtr,"Cannot play sounds - Sounds stuck error b. Game can still be played, but quietly. Check to make sure your sound drivers are up to date and not corrupted.",
					 "Sound Error",MB_OK | MB_ICONEXCLAMATION);
					print_nums(222,which,num_fails);
					sounds_fucked = TRUE;
					return;
					}
			sndPlaySound(NULL,0);

			if (can_ignore[which] >= 4)
				check_sound = sndPlaySound(snds[which],SND_SYNC | SND_MEMORY | SND_NOSTOP);
				else check_sound = sndPlaySound(snds[which],SND_SYNC | SND_MEMORY);
			}
		a_sound_did_get_played = TRUE;
	  }

	store_last_sound_played = which;

	if ((load_when_play[which] == TRUE) && (asyn == FALSE)) 
		sound_handles[which] = NULL;
		
	for (i = 0; i < NUM_SOUNDS; i++)
		if ((load_when_play[which] == TRUE) && (sound_handles[which] != NULL)
			&& (a_sound_did_get_played == TRUE) && (i != which))
		{
			sound_handles[i] = NULL;
		}

}

void kill_sound()
{
	sndPlaySound(NULL,0);
}

void one_sound(short which)
{
	if (which == last_played)
		return;
	play_sound(which);
	last_played = which;
}

void sound_pause(long len) {
	long t1,t2;

	t1 = (long) GetCurrentTime();
	t2 = t1;
	while (t2 - t1 < len) {
		t2 = (long)GetCurrentTime();
		}
}
void move_sound(unsigned char ter,short step)
{
	short spec;
	
	spec = scenario.ter_types[ter].special;
	
	if ((monsters_going == FALSE) && (overall_mode < 10) && (party.in_boat >= 0)) {
		if (spec == 21)
			return;
		play_sound(48);
		}
	else if ((monsters_going == FALSE) && (overall_mode < 10) && (party.in_horse >= 0)) {////
		play_sound(85);
		}
	else if(spec == 5) //if poisoned land don't play squish sound : BoE legacy behavior, can be removed safely
		  return;
//  else if(spec == 6) //if diseased land do the same
//		  return;
		else switch(scenario.ter_types[ter].step_sound){
            case 0:
                if (step % 2 == 0)          //footsteps alternate sound
					play_sound(49);
				else play_sound(50);
                break;
            case 1:
                play_sound(55);         //squish
                break;
            case 2:
                play_sound(47);         //crunch
                break;
            case 3:
                break;                //silence : do nothing
            default:
                 if (step % 2 == 0)          //safety footsteps valve
					play_sound(49);
				else play_sound(50);
        }
}
