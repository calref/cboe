//
//  tiling.cpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-14.
//
//

#include "tiling.hpp"

#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "res_image.hpp"
#include "render_image.hpp"
#include "render_shapes.hpp"

rectangle bg_rects[21];
tessel_ref_t bg[21];
tessel_ref_t bw_pats[6];

struct tessel_t {
	sf::RenderTexture* tessel;
	sf::Texture* img;
	rectangle srcRect;
};

bool operator==(const tessel_ref_t& a, const tessel_ref_t& b) {
	return a.key == b.key;
}

namespace std {
template<> struct hash<tessel_ref_t> {
	size_t operator()(tessel_ref_t key) const {
		return key.key;
	}
};
}

std::unordered_map<tessel_ref_t, tessel_t> tiling_reservoir;
static int tessel_index = 0;

tessel_ref_t prepareForTiling(sf::Texture& srcImg, rectangle srcRect) {
	tessel_ref_t ref = {tessel_index++};
	tiling_reservoir[ref].img = &srcImg;
	tiling_reservoir[ref].srcRect = srcRect;
	tiling_reservoir[ref].tessel = new sf::RenderTexture;
	tiling_reservoir[ref].tessel->create(srcRect.width(), srcRect.height());
	rectangle tesselRect(*tiling_reservoir[ref].tessel);
	rect_draw_some_item(srcImg, srcRect, *tiling_reservoir[ref].tessel, tesselRect);
	tiling_reservoir[ref].tessel->display();
	tiling_reservoir[ref].tessel->setRepeated(true);
	return ref;
}

void tileImage(sf::RenderTarget& target, rectangle area, tessel_ref_t tessel, sf::BlendMode mode) {
	// First, set up a dictionary of all textures ever tiled.
	// The key type is a pair<Texture*,rectangle>.
	// The value type is a Texture.
	tessel_t& tesselInfo = tiling_reservoir[tessel];
	rectangle clipArea = area;
	area.left -= area.left % tesselInfo.srcRect.width();
	area.top -= area.top % tesselInfo.srcRect.height();
	area &= rectangle(target); // Make sure we don't draw out of bounds
	
	sf::RectangleShape tesselShape(sf::Vector2f(area.width(),area.height()));
	tesselShape.setTexture(&tesselInfo.tessel->getTexture());
	tesselShape.setTextureRect(area);
	tesselShape.setPosition(area.left, area.top);
	sf::RenderStates renderMode(mode);
	setActiveRenderTarget(target);
	clip_rect(target, clipArea);
	target.draw(tesselShape, renderMode);
	undo_clip(target);
}

void init_tiling() {
	static const location pat_offs[17] = {
		{0,3}, {1,1}, {2,1}, {2,0},
		{3,0}, {3,1}, {1,3}, {0,0},
		{0,2}, {1,2}, {0,1}, {2,2},
		{2,3}, {3,2}, {1,0}, {4,0}, {3,3}
	};
	static const int pat_i[17] = {
		2, 3, 4, 5, 6, 8, 9, 10,
		11,12,13,14,15,16,17,19,20
	};
	for(short i = 0; i < 17; i++){
		bg_rects[pat_i[i]] = {0,0,64,64};
		bg_rects[pat_i[i]].offset(64 * pat_offs[i].x,64 * pat_offs[i].y);
	}
	rectangle tmp_rect = bg_rects[19];
	tmp_rect.offset(0, 64);
	bg_rects[0] = bg_rects[1] = bg_rects[18] = bg_rects[7] = tmp_rect;
	bg_rects[0].right -= 32;
	bg_rects[0].bottom -= 32;
	bg_rects[1].left += 32;
	bg_rects[1].bottom -= 32;
	bg_rects[18].right -= 32;
	bg_rects[18].top += 32;
	bg_rects[7].left += 32;
	bg_rects[7].top += 32;
	
	rectangle bw_rect = {0,0,8,8};
	sf::Texture& bg_gworld = *ResMgr::get<ImageRsrc>("pixpats");
	sf::Texture& bw_gworld = *ResMgr::get<ImageRsrc>("bwpats");
	for(int i = 0; i < 21; i++) {
		if(i < 6) {
			bw_pats[i] = prepareForTiling(bw_gworld, bw_rect);
			bw_rect.offset(8,0);
		}
		bg[i] = prepareForTiling(bg_gworld, bg_rects[i]);
	}
}
