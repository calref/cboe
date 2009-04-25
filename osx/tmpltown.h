/*
 *  tmpltown.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

class cTemplTown : cTown {
public:
	class cCityBlock { // formerly city_block_type
	public:
		short type;
		short destroy_time;
		short alignment;
		short key_time;
		location where;
	};
	class cTerRect { // formerly city_ter_rect_type
	public:
		rectangle rect;
		unsigned char ter_type;
		unsigned char hollow;
	};
private:
	cCreature _creatures[30];
	// unsigned char _terrain[?][?];
	rectangle _room_rect[16];
	unsigned char _lighting[4][32];
	cCityBlock blocks[15];
	cTerRect ter_rects[10];
public:
	unsigned char& terrain(size_t x, size_t y);
	rectangle& room_rect(size_t i);
	cCreature& creatures(size_t i);
	unsigned char& lighting(size_t i, size_t r);
	short max_dim();
	short max_monst();
	
	cTemplTown();
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