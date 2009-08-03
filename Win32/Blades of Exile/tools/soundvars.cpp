#ifdef __APPLE__
#include <Carbon/Carbon.h>
Handle sound_handles[NUM_SOUNDS];
#else
#include <windows.h>
#endif

bool play_sounds = true;
short last_played = 10000;
short store_last_sound_played = 0;

#ifdef __APPLE__ // TODO: should go over these to decide how to merge them
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
#else
bool always_asynch[100] = {
	FALSE,FALSE,FALSE,FALSE,FALSE,
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
bool load_when_play[100] = {
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
#endif

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
bool sounds_missing = false;

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
	0,5,5,0,0,0,0,0,0,0
}; // currently windows-only?
// 1 - polite asych, 1 or 2 or 3- lose easily when pref is set for fewer snds
// 3 can be async
// 4 - nostop asynch ... when 4, this sound is NOSTOP, i.e. when played, is played
//     asynch, and refuses all other sounds. Sounds that come in are ignored, and
//     disappear into the ether
// 5 - finally, bold asynch ... when 5, this sound is NOSTOP, i.e. when played, is played
//     asynch, and refuses all other sounds. When a sound is played on top of this, game
//     hangs on until sound is done, and then and only then plays later sound.

//Allocate  SndChannelPtr chan[4]   as a global variable.
//Allocate  char numchannel = 3;   (This gives 4 channels,  = 2  gives 3  etc...
//Allocate  char channel;    This is just used as a counter so each sound is played by the next
//channel in line.  You can have up to 4 sounds playing at once.

#ifdef __APPLE__
SndChannelPtr chan[4];
char numchannel = 3;
char channel;
#else
HMODULE hModule;
#endif
short snd_played[4] = {-1,-1,-1,-1};
