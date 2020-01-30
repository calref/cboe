//
//  boe.ui.hpp
//  BoE
//
//  Created by Celtic Minstrel on 20-01-27.
//
//

#ifndef BOE_GAME_UI_H
#define BOE_GAME_UI_H

#include <vector>
#include "location.hpp"

// These correspond to the icons in buttons.png
enum eToolbarButton {
	TOOLBAR_MAGE, TOOLBAR_PRIEST, TOOLBAR_LOOK, TOOLBAR_CAMP, TOOLBAR_SCROLL, TOOLBAR_SAVE,
	TOOLBAR_SHIELD, TOOLBAR_BAG, TOOLBAR_TALK, TOOLBAR_HAND, TOOLBAR_SWORD, TOOLBAR_LOAD,
	TOOLBAR_WAIT, TOOLBAR_END, TOOLBAR_SHOOT, TOOLBAR_ACT, TOOLBAR_USE, TOOLBAR_MAP,
	TOOLBAR_NONE = -1 // Keep this last
};

class cToolbar {
	// These correspond to the button frames in buttons.png (top row)
	enum eButtonType {BTN_LG, BTN_SM_HI, BTN_SM_LO};
	
	struct cButton {
		eToolbarButton btn;
		eButtonType type;
		rectangle bounds;
	};
	
	sf::RenderTexture cache;
	std::vector<cButton> toolbar;
	rectangle total_rect;
	enum eMode {UNKNOWN, OUTDOORS, TOWN, COMBAT};
	eMode cur_mode = UNKNOWN, get_mode();
	int active = -1;
	void place_buttons(const std::vector<eToolbarButton>& src);
	void draw_buttons();
	void init();
public:
	void draw(sf::RenderTarget& targ);
	eToolbarButton button_hit(sf::RenderWindow& win, location click);
};

namespace UI {
	extern cToolbar toolbar;
}

#endif /* defined(BOE_GAME_UI_H) */
