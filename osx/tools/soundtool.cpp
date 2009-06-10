/*
 *  soundtool.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#include <Carbon/Carbon.h>
#include "soundtool.h"

Handle sound_handles[NUM_SOUNDS];
bool play_sounds = true;
short last_played = 10000;

bool always_asynch[100] = {
	false,false,false,false,false,
	false,true,false,false,false,
	false,false,false,false,false, // 10
	false,false,false,false,false,
	false,false,false,false,true, // 20
	true,false,false,false,false,
	false,false,false,false,true,  // 30
	false,false,true,false,true,
	false,true,true,true,true, // 40
	true,true,true,true,true,
	true,false,false,false,false, // 50
	true,false,false,false,false,
	false,true,false,false,false, // 60
	false,false,false,false,false,
	false,false,false,false,false, // 70
	false,true,true,true,true,
	true,true,true,true,false, // 80
	true,false,false,false,false,
	false,true,false,false,false, // 90
	false,false,false,false,false
};
bool load_when_play[100] = { 
	0,0,1,1,1,1,0,1,1,1,
	0,0,0,1,0,1,1,1,1,1,
	1,1,1,1,1,1,1,0,1,1,
	1,1,1,1,0,1,1,0,1,1,
	1,1,1,1,1,1,1,0,0,0,
	0,1,1,1,1,0,1,1,1,1,
	1,0,1,1,1,1,1,1,1,0,
	0,0,0,0,0,0,1,1,1,1,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,0
};

short sound_delay[100] = {
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,25,25,0,0,0,0,
	0,0,0,0,8,0,0,8,0,0,
	0,0,0,10,20,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,13,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0
};
//Allocate  SndChannelPtr chan[4]   as a global variable.
//Allocate  char numchannel = 3;   (This gives 4 channels,  = 2  gives 3  etc...
//Allocate  char channel;    This is just used as a counter so each sound is played by the next
//channel in line.  You can have up to 4 sounds playing at once.

SndChannelPtr chan[4];
char numchannel = 3;
char channel;
short snd_played[4] = {-1,-1,-1,-1};

bool sound_going(short which_s) {
	short i;
	
	for (i = 0; i < 4; i++)
		if (snd_played[i] == which_s)
			return true;
	return false;
}

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

void init_snd_tool(){
	short i,t;
	SndCallBackUPP callback;
	
	for (i = 0; i < NUM_SOUNDS; i++) {
		if (!load_when_play[i]) {
			sound_handles[i] = GetResource('snd ', 20000 + i);
		}	
	}
	
	callback = NewSndCallBackUPP(snd_channel_callback);
	
	for(t=0;t<4;t++){ //  setup 4 sound channels
		SndNewChannel(&chan[t], sampledSynth, initMono + initNoDrop, callback);
		chan[t]->qLength = 128;
	}
	
}

void play_sound(short which, short how_many_times){  // if < 0, play asynch
	if (!play_sounds) return;
	Handle sndhandle;
	unsigned long dummy;
	OSErr err;
	SndCommand theCommand;
	
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
}

void one_sound(short which){
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
