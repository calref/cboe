/*
 *  town.h
 *  BoE
 *
 *  Created by Celtic Minsrel on 22/04/09.
 *
 */

#include "talking.h"

namespace legacy {
	struct town_record_type;
	struct big_tr_type;
	struct ave_tr_type;
	struct tiny_tr_type;
};

class cTown { // formerly town_record_type
public:
	class cCreature { // formerly creature_start_type
	public:
		unsigned char number;
		unsigned char start_attitude;
		location start_loc;
		unsigned char  mobile;
		unsigned char  time_flag;
		unsigned char  extra1,extra2;
		short spec1, spec2;
		char spec_enc_code,time_code;
		short monster_time,personality;
		short special_on_kill,facial_pic;
	};
	class cWandering { // formerly wnadering_type
	public:
		unsigned char monst[4];
	};
	class cItem { // formerly preset_item_type
	public:
		location loc;
		short code,ability;
		unsigned char charges,always_there,property,contained;
	};
	class cField { // formerly preset_field_type
	public:
		location loc;
		short type;
	};
	short town_chop_time,town_chop_key;
	cWandering wandering[4];
	location wandering_locs[4];
	location special_locs[50];
	unsigned char spec_id[50];
	location sign_locs[15];
	short lighting_type;
	location start_locs[4];
	location exit_locs[4];
	short exit_specs[4];
	Rect in_town_rect;
	cItem preset_items[64];
	short max_num_monst;
	cField preset_fields[50];
	short spec_on_entry,spec_on_entry_if_dead;
	short timer_spec_times[8];
	short timer_specs[8];
	unsigned char strlens[180];
	cSpecial specials[100];
	unsigned char specials1,specials2,res1,res2;
	short difficulty;
	//char town_strs[180][256];
	char town_name[256];
	char rect_names[16][256];
	char comment[3][256];
	char spec_strs[100][256];
	char sign_strs[20][256];
	char(& town_strs(short i))[256];
	char talk_strs[170][256];
	cSpeech talking;
	
	virtual ~cTown(){}
	virtual void append(legacy::big_tr_type& old);
	virtual void append(legacy::ave_tr_type& old);
	virtual void append(legacy::tiny_tr_type& old);
	virtual unsigned char& terrain(size_t x, size_t y) = 0;
	virtual Rect& room_rect(size_t i) = 0;
	virtual cCreature& creatures(size_t i) = 0;
	virtual unsigned char& lighting(size_t i, size_t r) = 0;
	virtual short max_dim() = 0;
	
	cTown();
	cTown(short size);
	cTown& operator = (legacy::town_record_type& old);
};

class cBigTown : public cTown { // formerly big_tr_type
	unsigned char _terrain[64][64];
	Rect _room_rect[16];
	cCreature _creatures[60];
	unsigned char _lighting[8][64];
public:
	void append(legacy::big_tr_type& old);
	unsigned char& terrain(size_t x, size_t y);
	Rect& room_rect(size_t i);
	cCreature& creatures(size_t i);
	unsigned char& lighting(size_t i, size_t r);
	short max_dim();
	
	cBigTown();
};

class cMedTown : public cTown { // formerly ave_tr_type
	unsigned char _terrain[48][48];
	Rect _room_rect[16];
	cCreature _creatures[40];
	unsigned char _lighting[6][48];
public:
	void append(legacy::ave_tr_type& old);
	unsigned char& terrain(size_t x, size_t y);
	Rect& room_rect(size_t i);
	cCreature& creatures(size_t i);
	unsigned char& lighting(size_t i, size_t r);
	short max_dim();
	
	cMedTown();
};

class cTinyTown : public cTown { // formerly tiny_tr_type
	unsigned char _terrain[32][32];
	Rect _room_rect[16];
	cCreature _creatures[30];
	unsigned char _lighting[4][32];
public:
	void append(legacy::tiny_tr_type& old);
	unsigned char& terrain(size_t x, size_t y);
	Rect& room_rect(size_t i);
	cCreature& creatures(size_t i);
	unsigned char& lighting(size_t i, size_t r);
	short max_dim();
	
	cTinyTown();
};

// City blocks - I want to figure out how to use these sometime.

//struct city_block_type {
//	short block_type;
//	short block_destroy_time;
//	char block_alignment;
//	char block_key_time;
//	location block_loc;
//};
//
//struct city_ter_rect_type {
//	Rect what_rect;
//	unsigned char ter_type;
//	unsigned char hollow;
//};
//
//struct template_town_type {
//	creature_start_type creatures[30];
//	city_block_type city_block[15];
//	city_ter_rect_type city_ter_rect[10];
//};