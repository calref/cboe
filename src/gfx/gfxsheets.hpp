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
#include "location.hpp"

static const pic_num_t NO_PIC = -1;
using graf_pos = std::pair<sf::Texture*,rectangle>;
using graf_pos_ref = std::pair<sf::Texture*&,rectangle&>;

struct m_pic_index_t {
	unsigned char i, x, y;
};

struct cCustomGraphics {
	size_t numSheets;
	sf::Texture* sheets = nullptr;
	std::shared_ptr<sf::Texture> party_sheet;
	bool is_old = false;
	void clear() {
		if(sheets != nullptr) delete[] sheets;
		sheets = nullptr;
	}
	~cCustomGraphics() {
		clear();
	}
	explicit operator bool() {
		return sheets;
	}
	void convert_sheets();
	void copy_graphic(pic_num_t dest, pic_num_t src, size_t numSlots);
	graf_pos find_graphic(pic_num_t pic, bool party = false);
	size_t count(bool party = false);
	void replace_sheet(size_t num, sf::Image& newSheet);
	void init_sheet(size_t num);
};

rectangle calc_rect(short i, short j);
extern const std::vector<m_pic_index_t> m_pic_index;

#endif
