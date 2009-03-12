#include "math.h"

#include <Carbon/Carbon.h>

#include "sound.h"
#include "global.h"

#define	NUM_SOUNDS	99

Handle sound_handles[NUM_SOUNDS];

extern Boolean play_sounds,in_startup_mode;
extern WindowPtr mainPtr;
extern Boolean gInBackground;
extern party_record_type	party;

short last_played = 10000;

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

SndChannelPtr chan[4];
char numchannel = 3;
char channel;

void load_sounds ()
{
	short i,t;
	
	for (i = 0; i < NUM_SOUNDS; i++) {
		load_when_play[i] = TRUE;
		always_asynch[i] = FALSE;
		if (load_when_play[i] == FALSE) {
		sound_handles[i] = GetResource('snd ', 20000 + i);
		}	
		}

 for(t=0;t<4;t++)/****** setup 4 sound channels **********/
  { 
   SndNewChannel(&chan[t], sampledSynth, initMono + initNoDrop, NIL);
   chan[t]->qLength = 128;
  }

}

void play_sound(short which)  // if < 0, play asynch
{
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
	unsigned long dummy;
	OSErr err;

//	if (play_sounds == TRUE)
//		SndPlay(NIL, sound_handles[which], TRUE);

//	if (play_sounds == TRUE) {
		 channel++;
	
 		if (channel > numchannel) channel = 0;
		
		if (load_when_play[((which < 0) ? -1 * which : which)] == TRUE) 
			sndhandle = GetResource('snd ',20000 + ((which < 0) ? -1 * which : which));
			else sndhandle = sound_handles[((which < 0) ? -1 * which : which)];
 
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
  			 	}
  			 HUnlock(sndhandle);
   			}
   			else SysBeep(2);
  		if (which < 0)
  			Delay(sound_delay[-1 * which],&dummy);
//   		}
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

