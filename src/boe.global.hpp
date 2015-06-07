

#ifndef BOE_GAME_GLOBAL_H
#define BOE_GAME_GLOBAL_H

#include <vector>
#include <string>
#include <sstream>
#include "boe.consts.hpp"

#define DRAG_EDGE		15

#define	DISPLAY_LEFT	23
#define	DISPLAY_TOP		23
#define	BITMAP_WIDTH	28
#define	BITMAP_HEIGHT	36

#define	PC_WIN_UL_X	291
#define	PC_WIN_UL_Y	5
#define	ITEM_WIN_UL_X	291
#define	ITEM_WIN_UL_Y	130
#define	TEXT_WIN_UL_X	291
#define	TEXT_WIN_UL_Y	283
#define	NUM_BUTTONS		15
#define ASB	add_string_to_buf
#define	PSD	univ.party.stuff_done

#define	NUM_MONST_G	173
#define	NUM_TER_G	251
#define	NUM_ITEM_G	120
#define	NUM_FACE_G	80
#define	NUM_DLOG_G	28

#define	CDGT	cd_retrieve_text_edit_str
#define	CDGN	cd_retrieve_text_edit_num
#define	CDST	cd_set_text_edit_str
#define	CDSN	cd_set_text_edit_num

struct scen_header_type{
	int intro_pic, rating, difficulty, ver[3], prog_make_ver[3];
	std::string name, who1, who2, file;
};

struct effect_pat_type {
	unsigned short pattern[9][9];
};

#endif
