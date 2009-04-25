/*
 *  universe.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

namespace legacy {
	struct setup_save_type;
	struct out_info_type;
	struct current_town_type;
	struct town_item_list;
	struct stored_town_maps_type;
	struct stored_outdoor_maps_type;
	struct big_tr_type;
};

class cCurTown {
public:
	// formerly current_town_type
	short num; // 200 if outdoors (my addition)
	short difficulty;
	cTown* town;
	char explored[64][64];
	bool hostile;
	cPopulation monst;
	bool in_boat; // is this really needed?
	location p_loc;
	cSpeech* cur_talk; // my addition
	short cur_talk_loaded; // my addition
	
	unsigned char setup[4][64][64]; // formerly setup_save_type
	cItemRec items[115]; // formerly town_item_list type
	char maps[200][8][64]; // formerly stored_town_maps_type
	
	unsigned char template_terrain[64][64];
	
	void append(legacy::current_town_type& old,short which_size);
	void append(legacy::town_item_list& old);
	void append(legacy::setup_save_type& old);
	void append(legacy::stored_town_maps_type& old);
	void append(unsigned char(& old_sfx)[64][64], unsigned char(& old_misc_i)[64][64]);
	void cCurTown::append(legacy::big_tr_type& old);
};

class cCurOut {
public:
	char expl[96][96]; // formerly out_info_type
	char maps[100][6][48]; // formerly stored_outdoor_maps_type
	unsigned char out[96][96],out_e[96][96];
	cOutdoors outdoors[2][2];
	
	unsigned char sfx[64][64];
	unsigned char misc_i[64][64];
	
	void append(legacy::out_info_type& old);
	void append(legacy::stored_outdoor_maps_type& old);
	void append(unsigned char(& old_sfx)[64][64], unsigned char(& old_misc_i)[64][64]);
};

class cUniverse{
public:
	cParty party;
	cCurTown town;
	cCurOut out;
	FSSpec file;
};