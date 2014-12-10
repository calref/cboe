
#ifndef BOE_GLOBAL_H
#define BOE_GLOBAL_H

#include <vector>
#include <string>
#include <sstream>
#include "boe.consts.h"

#define DRAG_EDGE		15

#define NUM_TOWN_ITEMS	115


#define	DISPLAY_LEFT	23
#define	DISPLAY_TOP		23
#define	BITMAP_WIDTH	28
#define	BITMAP_HEIGHT	36

//#define	STORED_GRAPHICS	240

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

struct scen_header_type {
	unsigned char flag1, flag2, flag3, flag4;
	unsigned char ver[3],min_run_ver,prog_make_ver[3],num_towns;
	unsigned char out_width,out_height,difficulty,intro_pic,rating;
};

struct scen_header_str_type{
	std::string name, who1, who2, file;
};

class cScenarioList {
	std::vector<scen_header_type> d;
	std::vector<scen_header_str_type> s;
public:
	void clear() {d.clear(); s.clear();}
	size_t size() {return d.size();}
	void push_back(scen_header_type& head, scen_header_str_type& strs) {d.push_back(head); s.push_back(strs);}
	scen_header_type& data(size_t i) {return d.at(i);}
	scen_header_str_type& strs(size_t i) {return s.at(i);}
};

struct effect_pat_type {
	unsigned char pattern[9][9];
};

#endif
