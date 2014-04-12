/*
 *  graphtool.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#ifndef GRAPHTOOL_H
#define GRAPHTOOL_H

#include <SFML/Graphics.hpp>
#include <boost/filesystem/path.hpp>
#include <string>
#include "location.h"

namespace fs = boost::filesystem;

#define LINES_IN_TEXT_WIN	11
#define NUM_TER_SHEETS		7
#define NUM_MONST_SHEETS	11

enum cursor_type {
	wand_curs = 0,
	eyedropper_curs = 1,
	brush_curs = 2,
	spray_curs = 3,
	eraser_curs = 4,
	topleft_curs = 5,
	bottomright_curs = 6,
	hand_curs = 7,
	NW_curs = 8,
	N_curs = 9,
	NE_curs = 10,
	W_curs = 11,
	wait_curs = 12,
	E_curs = 13,
	SW_curs = 14,
	S_curs = 15,
	SE_curs = 16,
	sword_curs = 17,
	boot_curs = 18,
	drop_curs = 19,
	target_curs = 20,
	talk_curs = 21,
	key_curs = 22,
 	look_curs = 23,
};

struct m_pic_index_t {
	unsigned char i, x, y;
};

struct TextStyle {
	int style;
	std::string font;
	int pointSize;
	sf::Color colour;
	void applyTo(sf::Text& text);
};

class Region {
	std::vector<std::shared_ptr<sf::Shape>> shapes;
	void setStencil(sf::RenderTarget& where);
	friend void clip_region(sf::RenderTarget& where, Region& region);
public:
	void addEllipse(RECT frame);
	void addRect(RECT rect);
	void clear();
	void offset(int x, int y);
	Region& operator-=(Region& other);
};

typedef unsigned short pic_num_t;
using graf_pos = std::pair<sf::Texture*,RECT>;
using graf_pos_ref = std::pair<sf::Texture*&,RECT&>;

struct cCustomGraphics {
	sf::Texture* sheets = NULL;
	sf::Texture* party = NULL;
	bool is_old = false;
	void clear() {
		if(sheets != NULL) delete[] sheets;
	}
	~cCustomGraphics() {
		if(party != NULL) delete party;
	}
	explicit operator bool() {
		return sheets;
	}
	graf_pos find_graphic(pic_num_t pic, bool party = false);
};



void init_graph_tool(void (*redraw_callback)());
void clean_up_graphtool();
void set_cursor(cursor_type which_curs);
void restore_cursor();
void rect_draw_some_item(sf::RenderTarget& targ_gworld,RECT targ_rect);
void rect_draw_some_item(const sf::Texture& src_gworld,RECT src_rect,sf::RenderTarget& targ_gworld,RECT targ_rect,sf::BlendMode mode = sf::BlendNone);
void rect_draw_some_item(const sf::Texture& src_gworld,RECT src_rect,RECT targ_rect,location offset,sf::BlendMode mode = sf::BlendNone);
void win_draw_string(sf::RenderTarget& dest_window,RECT dest_rect,const char* str,short mode,short line_height,location offset = {0,0});
short string_length(const char *str);
//OSStatus flip_pict(OSType domain, OSType type, short id, void *ptr, UInt32 size, bool isNative, void *refcon);
//void draw_terrain();
RECT calc_rect(short i, short j);
void setActiveRenderTarget(sf::RenderTarget& where);
void tileImage(sf::RenderTarget& target, RECT area, sf::Texture& img, sf::BlendMode mode = sf::BlendNone);
void tileImage(sf::RenderTarget& target, RECT area, sf::Texture& img, RECT srcRect, sf::BlendMode mode = sf::BlendNone);
void fill_rect(sf::RenderTarget& target, RECT rect, sf::Color colour);
void frame_rect(sf::RenderTarget& target, RECT rect, sf::Color colour);
void fill_circle(sf::RenderTarget& target, RECT rect, sf::Color colour);
void frame_circle(sf::RenderTarget& target, RECT rect, sf::Color colour);
void fill_roundrect(sf::RenderTarget& target, RECT rect, int rad, sf::Color colour);
void frame_roundrect(sf::RenderTarget& target, RECT rect, int rad, sf::Color colour);
void fill_region(sf::RenderTarget& target, Region& region, sf::Color colour);
void frame_region(sf::RenderTarget& target, Region& region, sf::Color colour);

void clip_rect(sf::RenderTarget& where, RECT rect);
void clip_region(sf::RenderTarget& where, Region& region);
void undo_clip(sf::RenderTarget& where);

#ifndef GRAPHTOOL_CPP
extern cursor_type arrow_curs[3][3];
extern cursor_type current_cursor;
extern m_pic_index_t m_pic_index[200];
extern RECT bg[];
extern TextStyle TEXT;
#endif

#endif
