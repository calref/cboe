/*
 *  graphtool.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#ifndef GRAPHTOOL_H
#define GRAPHTOOL_H

#include <string>
#include <boost/filesystem/path.hpp>
#include <SFML/Graphics.hpp>
#include "location.h"

namespace fs = boost::filesystem;

#define LINES_IN_TEXT_WIN	11
#define NUM_TER_SHEETS		7
#define NUM_MONST_SHEETS	11

struct m_pic_index_t {
	unsigned char i, x, y;
};

enum eFont {
	FONT_PLAIN,
	FONT_BOLD,
	FONT_DUNGEON,
	FONT_MAIDWORD
};

struct TextStyle {
	bool italic = false, underline = false;
	eFont font = FONT_BOLD;
	int pointSize = 10, lineHeight;
	sf::Color colour;
	TextStyle() : colour(sf::Color::Black) {}
	void applyTo(sf::Text& text);
};

class Region {
	std::vector<std::shared_ptr<sf::Shape>> shapes;
	void setStencil(sf::RenderWindow& where);
	friend void clip_region(sf::RenderWindow& where, Region& region);
public:
	void addEllipse(RECT frame);
	void addRect(RECT rect);
	void clear();
	void offset(int x, int y);
	void offset(location off);
	Region& operator-=(Region& other);
};

typedef unsigned short pic_num_t;
static const pic_num_t NO_PIC = std::numeric_limits<pic_num_t>::max();
using graf_pos = std::pair<sf::Texture*,RECT>;
using graf_pos_ref = std::pair<sf::Texture*&,RECT&>;
using hilite_t = std::pair<size_t,size_t>;

struct cCustomGraphics {
	size_t numSheets;
	sf::Texture* sheets = NULL;
	sf::Texture* party = NULL;
	bool is_old = false;
	void clear() {
		if(sheets != NULL) delete[] sheets;
		sheets = NULL;
	}
	~cCustomGraphics() {
		if(party != NULL) delete party;
	}
	explicit operator bool() {
		return sheets;
	}
	graf_pos find_graphic(pic_num_t pic, bool party = false);
	size_t count();
};

struct snippet_t {
	std::string text;
	location at;
	bool hilited;
};

enum class eTextMode {
	WRAP,
	CENTRE,
	LEFT_TOP,
	LEFT_BOTTOM,
};

void init_graph_tool();
void rect_draw_some_item(sf::RenderTarget& targ_gworld,RECT targ_rect);
void rect_draw_some_item(const sf::Texture& src_gworld,RECT src_rect,sf::RenderTarget& targ_gworld,RECT targ_rect,sf::BlendMode mode = sf::BlendNone);
void rect_draw_some_item(const sf::Texture& src_gworld,RECT src_rect,RECT targ_rect,location offset,sf::BlendMode mode = sf::BlendNone);
void rect_draw_some_item(const sf::Texture& src_gworld,RECT src_rect,const sf::Texture& mask_gworld,RECT mask_rect,sf::RenderTarget& targ_gworld,RECT targ_rect);

std::vector<RECT> draw_string_hilite(sf::RenderTarget& dest_window,RECT dest_rect,std::string str,TextStyle style,std::vector<hilite_t> hilites,sf::Color hiliteClr);
std::vector<snippet_t> draw_string_sel(sf::RenderTarget& dest_window,RECT dest_rect,std::string str,TextStyle style,std::vector<hilite_t> hilites,sf::Color hiliteClr);
void win_draw_string(sf::RenderTarget& dest_window,RECT dest_rect,std::string str,eTextMode mode,TextStyle style, location offset = {0,0});
short string_length(std::string str, TextStyle style);
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
void fill_region(sf::RenderWindow& target, Region& region, sf::Color colour);
void frame_region(sf::RenderWindow& target, Region& region, sf::Color colour);
void draw_line(sf::RenderTarget& target, location from, location to, int thickness, sf::Color colour, sf::BlendMode mode = sf::BlendNone);

void clip_rect(sf::RenderTarget& where, RECT rect);
void clip_region(sf::RenderWindow& where, Region& region);
void undo_clip(sf::RenderTarget& where);

#ifndef GRAPHTOOL_CPP
extern m_pic_index_t m_pic_index[200];
extern RECT bg[];
#endif

#endif
