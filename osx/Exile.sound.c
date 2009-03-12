#include "math.h"


#include <Carbon/Carbon.h>

#include "global.h"
#include "Exile.sound.h"
#include "text.h"
#include "fields.h"

#define	NUM_SOUNDS	99

Handle sound_handles[NUM_SOUNDS];

extern Boolean play_sounds,in_startup_mode;
extern WindowPtr mainPtr;
extern Boolean gInBackground;
extern party_record_type	party;
extern Boolean monsters_going;
extern short num_chirps_played,overall_mode;
extern scenario_data_type scenario;

short last_played = 10000;
short error_beeps = 0;

Boolean always_asynch[100] = {FALSE,FALSE,FALSE,FALSE,FALSE,
							FALSE,TRUE,FALSE,FALSE,FALSE,
							FALSE,FALSE,FALSE,FALSE,FALSE, // 10
							FALSE,FALSE,FALSE,FALSE,FALSE,
							FALSE,FALSE,FALSE,FALSE,TRUE, // 20
							TRUE,FALSE,FALSE,FALSE,FALSE,
							FALSE,FALSE,FALSE,FALSE,TRUE,  // 30
							FALSE,FALSE,TRUE,FALSE,TRUE,
							FALSE,TRUE,TRUE,TRUE,TRUE, // 40
							TRUE,TRUE,TRUE,TRUE,TRUE,
							TRUE,FALSE,FALSE,FALSE,FALSE, // 50
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



short sound_delay[100] = {0,0,0,0,0,0,0,0,0,0,
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

short snd_played[4] = {-1,-1,-1,-1};
pascal void snd_channel_callback(SndChannelPtr theChannel,SndCommand theCommand);


SndChannelPtr chan[4];
char numchannel = 3;
char channel = 0;

Boolean sound_going(short which_s) {
	short i;
	
	for (i = 0; i < 4; i++)
		if (snd_played[i] == which_s)
			return TRUE;
	return FALSE;
	}

pascal void snd_channel_callback(SndChannelPtr theChannel,SndCommand theCommand) {
	long theA5;
	short channel = -1,i,which_sound;
	
#ifndef EXILE_BIG_GUNS
	theA5 = SetA5(theCommand.param2);
#endif
	
	for (i = 0; i < 4; i++)
		if (chan[i] == theChannel)
			channel = i;
	which_sound = snd_played[channel];
	snd_played[channel] = -1;
	//if (in_startup_mode == FALSE)
	//	print_num(0,snd_played[channel],channel);
  	if ((sound_going(which_sound) == FALSE) && (load_when_play[which_sound] == TRUE)) {
	  	HUnlock(sound_handles[which_sound]);
		//if (in_startup_mode == FALSE)
		//	print_num(99,snd_played[channel],channel);
		}
		
#ifndef EXILE_BIG_GUNS
	theA5 = SetA5(theA5);
#endif
	}

void load_sounds ()
{
	short i,t;
	SndCallBackUPP callback;
	
	for (i = 0; i < NUM_SOUNDS; i++) 
		if (load_when_play[i] == FALSE) {
		sound_handles[i] = GetResource('snd ', 20000 + i);
		}	

	callback = NewSndCallBackUPP(snd_channel_callback);
	
 for(t=0;t<4;t++)/****** setup 4 sound channels **********/
  { 
   SndNewChannel(&chan[t], sampledSynth, initMono + initNoDrop, callback);
   chan[t]->qLength = 128;
  }

}

void play_sound(short which)  // if < 0, play asynch
{							
	if (PSD[304][9] > 0)
		return;
	if (in_startup_mode == TRUE) {
		//if (FrontWindow() != mainPtr) {
		//	return;
		//	}
		if (gInBackground) {
			return;
			}
		}
	if (play_sounds == TRUE)
		force_play_sound(which);
}

/*
sound(id)
int id;
{
 Boolean secondtry = FALSE;
 
 channel++;

 if (channel > numchannel) channel = 0;
 
 sndhandle = NIL;
 sndhandle = GetResource('snd ',abs(id));
 
 if (sndhandle != NIL)
 {
  HLock(sndhandle);
  HNoPurge(sndhandle);
   
  if (id > 0) SndPlay(chan[channel],sndhandle,TRUE);/****** Normal SndPlay *****/
/*   else SndPlay(chan[channel],sndhandle,FALSE);/****** play ASYNC *****/
/* }
}
*/

void force_play_sound(short which)
{
	Handle sndhandle;
	OSErr err;
	short which2;
	SndCommand theCommand;
	
	if ((which > NUM_SOUNDS) || (-1 * which > NUM_SOUNDS)) {
		//SysBeep(2);
		return;
		}
//	if (play_sounds == TRUE)
//		SndPlay(NIL, sound_handles[which], TRUE);

	if (play_sounds == TRUE) {
		which2 = a_v(which);
		
		channel++;
	
 		if (channel > numchannel) channel = 0;
		
		if (snd_played[channel] >= 0)
			return;
		
		if ((sound_going(which2) == FALSE) && (load_when_play[((which < 0) ? -1 * which : which)] == TRUE)) {
			sound_handles[((which < 0) ? -1 * which : which)] 
			  = GetResource('snd ',20000 + ((which < 0) ? -1 * which : which));			
			}
		sndhandle = sound_handles[((which < 0) ? -1 * which : which)];
 
 		if (which > 0)
	 		if (always_asynch[which] == TRUE)
 				which = which * -1;
 
	 	if (sndhandle != NIL)
 			{
  			HLock(sndhandle);

  			if (which < 0) err = SndPlay(chan[channel],(SndListHandle) sndhandle,TRUE);/****** Normal SndPlay *****/
  			 	else {
  			 		err = SndPlay(chan[channel],(SndListHandle) sndhandle,FALSE);
  			 		}
  			 if (err != 0) {
  			 	if (error_beeps < 5) {
	  			 	SysBeep(50);
	  			 	error_beeps++;
	  			 	}
  			 	add_string_to_buf("Sound Error. Error codes:");
  			 	print_nums(channel,which,err);
  			 	add_string_to_buf("Your system could not play a sound.");
  			 	add_string_to_buf("Make sure editor isn't running.");
  			 	add_string_to_buf("Turn off sounds if necessary.");
  			 	HUnlock(sndhandle);
  			 	return;
  			 	}
  			 //HUnlock(sndhandle);
  			 
  			 //if (which < 0) {
  			 	snd_played[channel] = which2;
  			 	theCommand.cmd = callBackCmd;
  			 	theCommand.param1 = 0;
#ifndef EXILE_BIG_GUNS
  			 	theCommand.param2 = SetCurrentA5();
#endif
#ifdef EXILE_BIG_GUNS
  			 	theCommand.param2 = 0;
#endif
  			 	SndDoCommand(chan[channel],&theCommand,TRUE);
  			 //	}
  			 
   			}
   			else SysBeep(2);
  		if (which < 0)
  			FlushAndPause(sound_delay[-1 * which]);
   		}
}

void one_sound(short which)
{
	if (which == last_played)
		return;
	play_sound(which);
	last_played = which;
}

void clear_sound_memory()
{
	last_played = 100;
}

void flip_sound()
{
	play_sounds = (play_sounds == TRUE) ? FALSE : TRUE;
}

void move_sound(unsigned char ter,short step)
{
	short pic,spec;
	
	pic = scenario.ter_types[ter].picture;
	spec = scenario.ter_types[ter].special;
	
						if ((monsters_going == FALSE) && (overall_mode < 10) && (party.in_boat >= 0)) {
							if (spec == 21)
								return;
							play_sound(48);
							}
						else if ((monsters_going == FALSE) && (overall_mode < 10) && (party.in_horse >= 0)) {////
							play_sound(85);
							}
							else if ((ter >= 84) && (ter <= 89))
								play_sound(47);
								else if ((pic == 80) || (pic == 76)) // already played in special terrain check
									; 
									else if (pic == 180)
									play_sound(55);
									else if (step % 2 == 0)
										play_sound(49);
										else play_sound(50);
}

void incidental_noises()
{}