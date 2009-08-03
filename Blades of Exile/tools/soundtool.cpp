/*
 *  soundtool.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */
#include <cmath>

#if defined(__APPLE__)
#include <Carbon/Carbon.h>
#elif defined(WIN32)
#include <windows.h>
#include <mmsystem.h>
#endif

#include <cstdio>

#include "soundtool.h"

bool sound_going(short which_s) {
	short i;
	
	for (i = 0; i < 4; i++)
		if (snd_played[i] == which_s)
			return true;
	return false;
}

#ifdef __APPLE__
pascal void snd_channel_callback(SndChannelPtr theChannel,SndCommand* theCommand) {
	long theA5;
	short channel = -1,i,which_sound;
	
#ifndef EXILE_BIG_GUNS
	theA5 = SetA5(theCommand->param2);
#endif
	
	for (i = 0; i < 4; i++)
		if (chan[i] == theChannel)
			channel = i;
	which_sound = snd_played[channel];
	snd_played[channel] = -1;
	//if (in_startup_mode == false)
	//	print_num(0,snd_played[channel],channel);
  	if ((sound_going(which_sound) == false) && (load_when_play[which_sound] == true)) {
	  	HUnlock(sound_handles[which_sound]);
		//if (in_startup_mode == false)
		//	print_num(99,snd_played[channel],channel);
	}
	
#ifndef EXILE_BIG_GUNS
	theA5 = SetA5(theA5);
#endif
}
#endif

void init_snd_tool(){
#ifdef __APPLE__
	short i,t;
	SndCallBackUPP callback;
	
	for (i = 0; i < NUM_SOUNDS; i++) {
		if (!load_when_play[i]) {
			sound_handles[i] = GetResource('snd ', 20000 + i);
		}	
	}
	
	callback = NewSndCallBackUPP(snd_channel_callback);
	
	for(t=0;t<4;t++){ //  set up 4 sound channels
		SndNewChannel(&chan[t], sampledSynth, initMono + initNoDrop, callback);
		chan[t]->qLength = 128;
	}
#endif
}

void play_sound(short which, short how_many_times) { // if < 0, play asynch
#if defined(__APPLE__)
	Handle sndhandle;
	unsigned long dummy;
	OSErr err;
	SndCommand theCommand;
	if (!play_sounds || how_many_times == 0) return;
	
	if (abs(which) > NUM_SOUNDS) {
		//char msg[50];
		/*s*/printf(/*msg,*/"Error: Sound #%i does not exist.\n",abs(which));
		//give_error(msg,"",0);
		return;
	}
	
	channel++;
	
	if (channel > numchannel) channel = 0;
	
	if (!sound_going(abs(which)) && load_when_play[abs(which)]) 
		sndhandle = GetResource('snd ',20000 + abs(which));
	else sndhandle = sound_handles[abs(which)];
	
	if (which > 0)
 		if (always_asynch[which])
			which *= -1;
	
 	if (sndhandle != NULL)
	{
		HLock(sndhandle);
		
		if (which < 0) err = SndPlay(chan[channel],(SndListHandle) sndhandle,true); // Normal SndPlay
		else {
			err = SndPlay(chan[channel],(SndListHandle) sndhandle,false);
		}
		if (err != 0) {
			printf("Sound error.\n");
			//add_string_to_buf("Sound Error. Error codes:");
			//print_nums(channel,which,err);
			//add_string_to_buf("Your system could not play a sound.");
			//add_string_to_buf("Make sure editor isn't running.");
			//add_string_to_buf("Turn off sounds if necessary.");
		}
		HUnlock(sndhandle);
		snd_played[channel] = abs(which);
		theCommand.cmd = callBackCmd;
		theCommand.param1 = 0;
#ifndef EXILE_BIG_GUNS
		theCommand.param2 = SetCurrentA5();
#endif
#ifdef EXILE_BIG_GUNS
		theCommand.param2 = 0;
#endif
		SndDoCommand(chan[channel],&theCommand,true);
	}
	else SysBeep(20);
	if (which < 0)
		Delay(sound_delay[-1 * which],&dummy);
	if(how_many_times > 1)
		play_sound(which, how_many_times - 1);
#elif defined(WIN32)
	short i,num_fails = 0;
	char snd_name[30];
	bool asyn = false,a_sound_did_get_played = false;
	bool not_asyn = false,check_sound;
	HRSRC h;
	if ((sounds_missing) || (!play_sounds) || (how_many_times == 0))
		return;
	
	if (which < 0) {
		asyn = true;
		which = which * -1;
		}
	if (which >= 1000) {
		which -= 1000;
		not_asyn = true;
		}

	if (which >= 100)
		return;

	if ((always_async[which] == true) &&
	((can_ignore[which] == 1) || (can_ignore[which] >= 3)))
		asyn = true;
	if ((can_ignore[which] > 0) && (can_ignore[which] < 5) && (party.stuff_done[305][5] == 1))
		return;
	if ((can_ignore[which] != 1) && (can_ignore[which] < 3))
		asyn = false;
	if ((party.stuff_done[305][5] == 1) && (can_ignore[which] < 5))
		asyn = false;
	if (not_asyn == true)
		asyn = false;

	if ((load_when_play[which] == true) && (sound_handles[which] == NULL)) {
			asyn = false;
		sprintf((char *)snd_name,"#%d",which + 1);
		h = FindResource(hModule,snd_name,"#100");

		sound_handles[which] = LoadResource(hModule,h);
		snds[which] = (char *) LockResource(sound_handles[which]);

		}

	if (store_last_sound_played == 6)
		sndPlaySound(NULL,0);

	if (asyn == true) {
		if (can_ignore[which] >= 4)
			check_sound = sndPlaySound(snds[which],SND_ASYNC | SND_MEMORY | SND_NOSTOP);
			else check_sound = sndPlaySound(snds[which],SND_ASYNC | SND_MEMORY);

		while (check_sound == false) {

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
					sounds_missing = true;
					return;
					}
			sndPlaySound(NULL,0);

			if (can_ignore[which] >= 4)
				check_sound = sndPlaySound(snds[which],SND_ASYNC | SND_MEMORY | SND_NOSTOP);
				else check_sound = sndPlaySound(snds[which],SND_ASYNC | SND_MEMORY);
			}
	  a_sound_did_get_played = true;
	  }
		else {
		if (can_ignore[which] >= 4)
			check_sound = sndPlaySound(snds[which],SND_SYNC | SND_MEMORY | SND_NOSTOP);
			else check_sound = sndPlaySound(snds[which],SND_SYNC | SND_MEMORY);
		while (check_sound == false) {
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
					sounds_missing = true;
					return;
					}
			sndPlaySound(NULL,0);

			if (can_ignore[which] >= 4)
				check_sound = sndPlaySound(snds[which],SND_SYNC | SND_MEMORY | SND_NOSTOP);
				else check_sound = sndPlaySound(snds[which],SND_SYNC | SND_MEMORY);
			}
		a_sound_did_get_played = true;
	  }

	store_last_sound_played = which;

	if ((load_when_play[which] == true) && (asyn == false)) 
		sound_handles[which] = NULL;
		
	for (i = 0; i < NUM_SOUNDS; i++)
		if ((load_when_play[which] == true) && (sound_handles[which] != NULL)
			&& (a_sound_did_get_played == true) && (i != which))
		{
			sound_handles[i] = NULL;
		}
#endif
}

#ifdef WIN32
void kill_sound()
{
	sndPlaySound(NULL,0);
}
#endif

void one_sound(short which)
{
	if (which == last_played)
		return;
	play_sound(which);
	last_played = which;
}

void clear_sound_memory(){
	last_played = 100;
}

void flip_sound()
{
	play_sounds = (play_sounds == true) ? false : true;
}


#ifdef WIN32
void sound_pause(long len) {
	long t1,t2;

	t1 = (long) GetCurrentTime();
	t2 = t1;
	while (t2 - t1 < len) {
		t2 = (long)GetCurrentTime();
		}
}

void load_sounds(HMODULE handle)
{
	short i,t,err;
	HRSRC h;
	char snd_name[20];
	WAVEOUTCAPS wavecaps;
	
	hModule = handle;

	t = waveOutGetNumDevs();
	if (t == 0) {
		sounds_missing = true;
		return;
	}
	err = waveOutGetDevCaps(0,&wavecaps,sizeof(WAVEOUTCAPS));
	if (err != 0) {
		sounds_missing = true;
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
		if (load_when_play[i] == false) {
			sprintf((char *)snd_name,"#%d",i + 1);
			h = FindResource(handle,snd_name,"#100");

			sound_handles[i] = LoadResource(handle,h);
			snds[i] = (char*) LockResource(sound_handles[i]);
		}
	}
}
#endif