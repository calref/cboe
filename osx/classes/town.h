/*
 *  town.h
 *  BoE
 *
 *  Created by Celtic Minsrel on 22/04/09.
 *
 */

namespace legacy {
	struct town_record_type;
	struct big_tr_type;
	struct ave_tr_type;
	struct tiny_tr_type;
	struct creature_start_type;
	struct wandering_type;
	struct preset_item_type;
	struct preset_field_type;
};

class cTown { // formerly town_record_type
public:
//	class cCreature { // formerly creature_start_type
//	public:
//		unsigned short number;
//		unsigned char start_attitude;
//		location start_loc;
//		unsigned char mobile;
//		unsigned char time_flag;
//		unsigned char extra1,extra2;
//		short spec1, spec2;
//		char spec_enc_code,time_code;
//		short monster_time,personality;
//		short special_on_kill,facial_pic;
//		
//		cCreature& operator = (legacy::creature_start_type old);
//	};
	class cWandering { // formerly wandering_type
	public:
		unsigned char monst[4];
		
		cWandering& operator = (legacy::wandering_type old);
	};
	class cItem { // formerly preset_item_type
	public:
		location loc;
		short code,ability;
		unsigned char charges,always_there,property,contained;
		
		cItem& operator = (legacy::preset_item_type old);
	};
	class cField { // formerly preset_field_type
	public:
		location loc;
		short type;
		
		cField& operator = (legacy::preset_field_type old);
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
	rectangle in_town_rect;
	cItem preset_items[64];
	short max_num_monst;
	vector<cField> preset_fields;
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
	cSpeech talking;
	
	virtual ~cTown(){}
	virtual void append(legacy::big_tr_type& old);
	virtual void append(legacy::ave_tr_type& old);
	virtual void append(legacy::tiny_tr_type& old);
	virtual unsigned short& terrain(size_t x, size_t y) = 0;
	virtual rectangle& room_rect(size_t i) = 0;
	virtual cCreature& creatures(size_t i) = 0;
	virtual unsigned char& lighting(size_t i, size_t r) = 0;
	virtual short max_dim() = 0;
	virtual short max_monst() = 0;
	
	cTown();
	cTown(short size);
	cTown& operator = (legacy::town_record_type& old);
	void writeTo(ostream& file);
};
