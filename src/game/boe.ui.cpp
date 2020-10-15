//
//  boe.ui.cpp
//  BoE
//
//  Created by Celtic Minstrel on 20-01-27.
//
//

#include "boe.ui.hpp"

#include <numeric>
#include "enum_map.hpp"
#include "boe.consts.hpp"
#include "boe.locutils.hpp"
#include "boe.graphics.hpp"
#include "render_shapes.hpp"
#include "render_image.hpp"
#include "res_image.hpp"
#include "mathutil.hpp"
#include "sounds.hpp"

namespace UI {
	cToolbar toolbar;
}

// The location of each UI area
enum_map(eGuiArea, rectangle) win_to_rects = {
	{7,19,358,298},      // terrain view
	{385,19,423,285},    // action buttons
	{7,305,123,576},     // pc stats area
	{132,305,276,576},   // inventory area
	{360,19,381,298},    // info bar
	{285,305,423,561},   // text transcript
};

// Get the location of the button icon in buttons.png.
static location btn_pos(eToolbarButton icon) {
	location pos;
	pos.x = icon % 6;
	pos.y = icon / 6;
	return pos;
}

// The list of buttons for each mode
static const std::vector<eToolbarButton> out_buttons = {TOOLBAR_MAGE, TOOLBAR_PRIEST, TOOLBAR_LOOK, TOOLBAR_CAMP, TOOLBAR_SCROLL, TOOLBAR_SAVE, TOOLBAR_LOAD};
static const std::vector<eToolbarButton> town_buttons = {TOOLBAR_MAGE, TOOLBAR_PRIEST, TOOLBAR_LOOK, TOOLBAR_TALK, TOOLBAR_HAND, TOOLBAR_USE, TOOLBAR_MAP, TOOLBAR_SWORD};
static const std::vector<eToolbarButton> fight_buttons = {TOOLBAR_MAGE, TOOLBAR_PRIEST, TOOLBAR_LOOK, TOOLBAR_SHIELD, TOOLBAR_BAG, TOOLBAR_WAIT, TOOLBAR_SHOOT, TOOLBAR_END, TOOLBAR_ACT};

// Source rects for each button type in buttons.png.
static const rectangle btn_src_rects[] = {{0,0,38,38}, {0,38,19,76}, {19,38,38,76}};

eToolbarButton cToolbar::button_hit(sf::RenderWindow& win, location click) {
	rectangle dest_rect = win_to_rects[WINRECT_ACTBTNS];
	click.x -= dest_rect.left;
	click.y -= dest_rect.top;
	if(click.in(total_rect)) {
		for(int i = 0; i < toolbar.size(); i++) {
			if(click.in(toolbar[i].bounds)) {
				sf::Event e;
				bool done = false, clicked = false;
				win.setActive();
				active = i;
				while(!done){
					redraw_screen(REFRESH_NONE);
					while(win.pollEvent(e)) {
						if(e.type == sf::Event::MouseButtonReleased){
							done = true;
							location clickPos(e.mouseButton.x, e.mouseButton.y);
							clickPos = win.mapPixelToCoords(clickPos);
							clickPos.x -= dest_rect.left;
							clickPos.y -= dest_rect.top;
							clicked = toolbar[i].bounds.contains(clickPos);
							active = -1;
						} else if(e.type == sf::Event::MouseMoved){
							location toPos(e.mouseMove.x, e.mouseMove.y);
							toPos = win.mapPixelToCoords(toPos);
							toPos.x -= dest_rect.left;
							toPos.y -= dest_rect.top;
							active = toolbar[i].bounds.contains(toPos) ? i : -1;
						}
					}
				}
				play_sound(37, time_in_ticks(5));
				redraw_screen(REFRESH_NONE);
				if(clicked) return toolbar[i].btn;
			}
		}
	}
	return TOOLBAR_NONE;
}

cToolbar::eMode cToolbar::get_mode() {
	if(is_combat()) return COMBAT;
	if(is_town()) return TOWN;
	return OUTDOORS;
}

void cToolbar::init() {
	eMode mode = get_mode();
	if(mode == cur_mode) return;
	cur_mode = mode;
	switch(mode) {
		case UNKNOWN: case OUTDOORS:
			place_buttons(out_buttons);
			break;
		case TOWN:
			place_buttons(town_buttons);
			break;
		case COMBAT:
			place_buttons(fight_buttons);
			break;
	}
	draw_buttons();
	// Calculate total bounds
	if(toolbar.empty()) return;
	total_rect = std::accumulate(toolbar.begin() + 1, toolbar.end(), toolbar[0].bounds, [](const rectangle& accum, const cButton& next) {
		return rectunion(accum, next.bounds);
	});
}

void cToolbar::place_buttons(const std::vector<eToolbarButton>& src) {
	// Calculate the positions of the toolbar buttons
	toolbar.clear();
	int offset = 0;
	bool bottom_half = false;
	for(eToolbarButton btn : src) {
		location slot = btn_pos(btn);
		cButton btn_info;
		btn_info.btn = btn;
		if(slot.y == 2) {
			// Small button (3rd row)
			btn_info.type = bottom_half ? BTN_SM_LO : BTN_SM_HI;
			btn_info.bounds = {0,0,19,38};
			if(bottom_half) {
				btn_info.bounds.offset(0,19);
				bottom_half = false;
			} else bottom_half = true;
		} else {
			// Large button (1st & 2nd rows)
			btn_info.type = BTN_LG;
			btn_info.bounds = {0,0,38,38};
		}
		btn_info.bounds.offset(offset, 0);
		if(btn_info.type != BTN_SM_HI) offset += 38;
		toolbar.push_back(btn_info);
	}
}

void cToolbar::draw_buttons() {
	// Draw the buttons into the cache
	static bool inited = false;
	if(!inited) {
		inited = true;
		// TODO: Possibly this should be based on total_rect instead of hard-coded?
		cache.create(266,38);
		cache.clear(sf::Color::Black);
	}
	sf::Texture& buttons_gworld = *ResMgr::graphics.get("buttons");
	for(const auto& btn : toolbar) {
		rectangle icon_rect = {0, 0, 32, 32}, to_rect = btn.bounds;
		location slot = btn_pos(btn.btn);
		// Small buttons are half the height, so the icon is also half the height
		if(btn.type != BTN_LG) icon_rect.bottom /= 2;
		// buttons.png consists of 1 row of 38x38 buttons, two rows of 32x32 icons, and one row of 32x16 icons.
		icon_rect.offset(32 * slot.x, 38 + 32 * slot.y);
		rect_draw_some_item(buttons_gworld, btn_src_rects[btn.type], cache, to_rect);
		to_rect.inset(3,3);
		// Insetting the small rect overcompensates, so correct for that.
		if(btn.type != BTN_LG) to_rect.bottom += 3;
		rect_draw_some_item(buttons_gworld, icon_rect, cache, to_rect, sf::BlendAlpha);
	}
	cache.display();
}

void cToolbar::draw(sf::RenderTarget& targ) {
	init();
	
	// Determine actual rect
	rectangle to = win_to_rects[WINRECT_ACTBTNS];
	to.width() = total_rect.width();
	
	// Prepare background for additive blending
	fill_rect(targ, to, sf::Color::Black);
	if(active >= 0 && active < toolbar.size()) {
		rectangle press_rect = toolbar[active].bounds;
		press_rect.offset(win_to_rects[WINRECT_ACTBTNS].topLeft());
		fill_rect(targ, press_rect, sf::Color::Blue);
	}
	
	// Add the cached toolbar over the background
	rect_draw_some_item(cache.getTexture(), rectangle(cache), targ, to, sf::BlendAdd);
}
