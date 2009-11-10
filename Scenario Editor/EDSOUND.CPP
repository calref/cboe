#include "math.h"
#include <windows.h>
#include <mmsystem.h>

#include "stdio.h"

#include "global.h"
#include "edsound.h"

#define	NUM_SOUNDS	100

HGLOBAL sound_handles[NUM_SOUNDS];
char *snds[NUM_SOUNDS];

extern HINSTANCE store_hInstance;

extern Boolean play_sounds,in_startup_mode;
extern HWND mainPtr;
extern Boolean gInBackground;

extern short num_chirps_played,overall_mode;

short last_played = 10000;
short error_beeps = 0;
short store_last_sound_played = 0;

Boolean always_asynch[100] = {FALSE,FALSE,FALSE,FALSE,FALSE,
							TRUE,TRUE,FALSE,FALSE,FALSE,
							TRUE,FALSE,FALSE,FALSE,FALSE, // 10
							FALSE,FALSE,FALSE,FALSE,FALSE,
							FALSE,FALSE,TRUE,FALSE,TRUE, // 20
							TRUE,FALSE,FALSE,FALSE,FALSE,
							FALSE,FALSE,FALSE,FALSE,TRUE,  // 30
							FALSE,FALSE,TRUE,FALSE,TRUE,
							FALSE,TRUE,TRUE,TRUE,TRUE, // 40
							TRUE,TRUE,TRUE,TRUE,TRUE,
							TRUE,FALSE,FALSE,TRUE,FALSE, // 50
							TRUE,FALSE,FALSE,FALSE,FALSE,
							FALSE,TRUE,FALSE,FALSE,FALSE, // 60
							FALSE,FALSE,FALSE,FALSE,FALSE,
							FALSE,FALSE,FALSE,FALSE,FALSE, // 70
							FALSE,TRUE,TRUE,TRUE,TRUE,
							TRUE,TRUE,TRUE,TRUE,FALSE, // 80
							TRUE,FALSE,FALSE,FALSE,FALSE,
							FALSE,TRUE,FALSE,FALSE,FALSE, // 90
							FALSE,FALSE,FALSE,FALSE,FALSE};
Boolean load_when_play[100] = {
0,0,1,1,1,0,0,1,1,1,
0,0,0,1,0,1,1,1,1,1,
1,1,0,1,1,1,1,0,1,1,
1,1,1,1,0,1,1,0,1,1,
1,1,1,1,1,1,1,0,0,0,
0,1,1,0,1,0,1,1,1,1, // 50
1,0,1,1,1,1,1,1,1,0,
0,0,0,0,0,0,0,0,0,0, // 70
1,1,1,1,1,0,0,0,0,0,
1,0,1,1,1,1,1,1,1,0
};

short can_ignore[100] = {
0,0,0,0,0,5,3,0,0,0,
5,5,5,0,5,0,0,0,0,0,
0,0,5,0,0,0,0,0,0,2,
2,2,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,1,1,1,
1,0,0,5,0,1,0,0,0,0, // 50
0,0,0,0,5,0,0,0,0,0,
0,0,0,0,0, 0,5,5,5,5, // 70
4,4,4,4,0,0,0,0,0,0,
0,5,5,0,0,0,0,0,0,0};
// 1 - polite asych, 1 or 2 or 3- lose easily when pref is set for fewer snds
// 3 can be async
// 4 - nostop asynch ... when 4, this sound is NOSTOP, i.e. when played, is played
//     asynch, and refuses all other sounds. Sounds that come in are ignored, and
//     disappear into the ether
// 5 - finally, bold asynch ... when 5, this sound is NOSTOP, i.e. when played, is played
//     asynch, and refuses all other sounds. When a sound is played on top of this, game
//     hangs on until sound is done, and then and only then plays later sound.


short num_devs;
Boolean sounds_fucked = FALSE;
long intro_music_start_time = -1;

void load_sounds ()
{
	short i,t,err;
	HRSRC h;
	char snd_name[20];
	WAVEOUTCAPS wavecaps;

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
		load_when_play[i] = TRUE;
		if (load_when_play[i] == FALSE) {
		sprintf((char *)snd_name,"#%d",i + 1);
		h = FindResource(store_hInstance,snd_name,"#100");

		sound_handles[i] = LoadResource(store_hInstance,h);
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
	Boolean check_sound;
	HRSRC h;

	if ((sounds_fucked == TRUE) || (play_sounds == FALSE))
		return;
	if (which < 0) {
		asyn = TRUE;
		which = which * -1;
		}

	if (which >= 100)
		return;

	if ((always_asynch[which] == TRUE) &&
	((can_ignore[which] == 1) || (can_ignore[which] >= 3)))
		asyn = TRUE;
	if ((can_ignore[which] != 1) && (can_ignore[which] < 3))
		asyn = FALSE;

	if ((load_when_play[which] == TRUE) && (sound_handles[which] == NULL)) {
	  //	if (can_ignore[which] != 4)
			asyn = FALSE;
		sprintf((char *) snd_name,"#%d",which + 1);
		h = FindResource(store_hInstance,snd_name,"#100");

		sound_handles[which] = LoadResource(store_hInstance,h);
		snds[which] = (char*) LockResource(sound_handles[which]);
		}

	if (store_last_sound_played == 6) {
		//ASB("Interrupted snd.");
		sndPlaySound(NULL,0);
		}

	if (asyn == TRUE) {
		if (can_ignore[which] >= 4)
			check_sound = sndPlaySound(snds[which],SND_ASYNC | SND_MEMORY | SND_NOSTOP);
			else check_sound = sndPlaySound(snds[which],SND_ASYNC | SND_MEMORY);

		while (check_sound == FALSE) {

			if (can_ignore[store_last_sound_played] == 4) {// then sound goes away
				//ASB("Sound overruled by asynch sound.");
				return;
				}


			num_fails++;
			if (num_fails < 40)
				sound_pause(25);
				else {
					MessageBox(mainPtr,"Cannot play sounds - Sounds stuck error a. Game can still be played, but quietly.",
					  "Sound Error",MB_OK | MB_ICONEXCLAMATION);
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
					MessageBox(mainPtr,"Cannot play sounds - Sounds stuck error b. Game can still be played, but quietly.",
					 "Sound Error",MB_OK | MB_ICONEXCLAMATION);
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

	if ((load_when_play[which] == TRUE) && (asyn == FALSE)) {
		// deleted a seemingly extraneous LoadResource here
		//ASB("Dumped a sound.");
		sound_handles[which] = NULL;
		}
	for (i = 0; i < NUM_SOUNDS; i++)
		if ((load_when_play[which] == TRUE) && (sound_handles[which] != NULL)
			&& (a_sound_did_get_played == TRUE) && (i != which)){
		// deleted a seemingly extraneous LoadResource here
		sound_handles[i] = NULL;
		}

}

void sound_pause(long len) {
	long t1,t2;

	t1 = (long) GetCurrentTime();
	t2 = t1;
	while (t2 - t1 < len) {
		t2 = (long)GetCurrentTime();
		}
}
