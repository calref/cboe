//
//  gfxsheets.hpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-14.
//
//

#ifndef BoE_CUSTOM_SHEETS_HPP
#define BoE_CUSTOM_SHEETS_HPP

#include <utility>
#include <vector>
#include <memory>
#include <SFML/Graphics/Texture.hpp>
#include "global.hpp"
#include "location.hpp"
#include "texture.hpp"

static const pic_num_t NO_PIC = -1;

struct m_pic_index_t {
	unsigned char i, x, y;
};

struct cCustomGraphics {
	size_t numSheets;
	std::vector<Texture> sheets;
	Texture party_sheet;
	bool is_old = false;
	void clear() {
		sheets.clear();
		party_sheet=Texture();
	}
	explicit operator bool() {
		return !sheets.empty() && bool(sheets[0]);
	}
	bool operator!() {
		return !bool(*this);
	}
	void convert_sheets();
	void copy_graphic(pic_num_t dest, pic_num_t src, size_t numSlots);
	Texture_pos find_graphic(pic_num_t pic, bool party = false);
	size_t count(bool party = false);
	void replace_sheet(size_t num, sf::Image& newSheet);
	void init_sheet(size_t num);
};

rectangle calc_rect(short i, short j);
extern const std::vector<m_pic_index_t> m_pic_index;

#endif
