
#include <array>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <memory>
#include <string>

#include "drawable_manager.hpp"
#include "event_listener.hpp"
#include "render_image.hpp"
#include "render_shapes.hpp"
#include "render_text.hpp"
#include "res_image.hpp"
#include "tiling.hpp"

#include "scen.global.hpp"
#include "scen.graphics.hpp"
#include "scen.btnmg.hpp"

cScenButtonsBars scen_controls;

extern sf::RenderWindow mainPtr;
extern cDrawableManager drawable_mgr;
extern rectangle terrain_buttons_rect;

void cScenButtonsBars::init()
{
	// left_buttons
	static rectangle left_button_base = {5,5,21,280};
	for(short i = 0; i < NLS; i++) {
		auto &button_rects=left_buttons_rectangles[i];
		button_rects[0] = left_button_base;
		button_rects[0].offset(0,i * 16);
		button_rects[1] = button_rects[0];
		button_rects[1].top += 1;
		button_rects[1].bottom -= 1;
		button_rects[1].left += 0;
		button_rects[1].right = button_rects[1].left + 16;
	}
	// right_button
	rectangle right_button_base;
	right_button_base.left = RIGHT_AREA_UL_X + 1;
	right_button_base.top = RIGHT_AREA_UL_Y + 1;
	right_button_base.bottom = right_button_base.top + 12;
	right_button_base.right = right_button_base.left + RIGHT_AREA_WIDTH - 20;
	for(short i = 0; i < NRSONPAGE; i++) {
		right_buttons_rectangles[i] = right_button_base;
		right_buttons_rectangles[i].offset(0,i * 12);
	}

	// terrains
	terrain_rectangle = {0,0,340,272};
	terrain_rectangle.offset(TER_RECT_UL_X, TER_RECT_UL_Y);
	for(auto &rect : terrain_border_rects)
		rect = scen_controls.terrain_rectangle;
	terrain_border_rects[0].bottom = terrain_border_rects[0].top + 8;
	terrain_border_rects[1].right = terrain_border_rects[1].left + 8;
	terrain_border_rects[2].top = terrain_border_rects[2].bottom - 8;
	terrain_border_rects[3].left = terrain_border_rects[3].right - 8;

	static rectangle terrain_rect_base = {0,0,16,16};
	for(short i = 0; i < 256; i++) {
		terrain_rects[i] = terrain_rect_base;
		terrain_rects[i].offset(3 + (i % 16) * (terrain_rect_base.right + 1),
			3 + (i / 16) * (terrain_rect_base.bottom + 1));
	}

	// scrollbars
	rectangle right_bar_rect;
	right_bar_rect.top = RIGHT_AREA_UL_Y - 1;
	right_bar_rect.left = RIGHT_AREA_UL_X + RIGHT_AREA_WIDTH - 1 - 16;
	right_bar_rect.bottom = RIGHT_AREA_UL_Y + RIGHT_AREA_HEIGHT + 1;
	right_bar_rect.right = RIGHT_AREA_UL_X + RIGHT_AREA_WIDTH - 1;
	init_bar(right_bar, "right_sbar", right_bar_rect, { 5, 287, 405, 577 }, NRSONPAGE - 1);
	right_bar->setPosition(0);
	
	rectangle pal_bar_rect = terrain_buttons_rect;
	pal_bar_rect.offset(RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
	pal_bar_rect.left = pal_bar_rect.right - 16;
	pal_bar_rect.height() = 17 * 16;
	init_bar(palette_bar, "pal_sbar", pal_bar_rect, { 5, 287, 279, 581 }, 16);
}

void cScenButtonsBars::draw_left_buttons() {
	for(short i = 0; i < NLS; i++)
		draw_left_slot(i,0);
}

// mode 0 normal 1 click
void cScenButtonsBars::draw_left_slot (short which,short mode)  {
	rectangle text_rect,from_rect;
	
	auto const &button=left_buttons[which];
	auto const &button_rects=left_buttons_rectangles[which];
	tileImage(mainPtr,button_rects[0],bg[20]);
	if(button.mode == LB_CLEAR)
		return;
	text_rect = button_rects[0];
	if(button.action != LB_NO_ACTION) {
		text_rect.left += 18;
		static rectangle const blue_button_from = {120,235,134,251};
		from_rect = blue_button_from;
		if(mode > 0)
			from_rect.offset(0,from_rect.height());
		auto const &edbuttons=*ResMgr::textures.get("edbuttons");
		rect_draw_some_item(edbuttons,from_rect,mainPtr,button_rects[1]);
	}
	if(button.mode == LB_INDENT)
		text_rect.left += 16;
	TextStyle style;
	if(button.mode == LB_TITLE) {
		style.pointSize = 14;
	}
	else text_rect.offset(0,2);
	if(mode > 0)
		style.colour = Colours::BLUE;
	style.lineHeight = 12;
	win_draw_string(mainPtr,text_rect,button.label,eTextMode::WRAP,style);
}


// is slot >= 0, force that slot
// if -1, use 1st free slot
void cScenButtonsBars::set_left_button(short slot, eLBMode mode, eLBAction action, std::string const &label, bool do_draw) {
	if(slot < 0) {
		auto iter = std::find_if(left_buttons.begin(), left_buttons.end(), [](const lb_t& btn) {
			return btn.mode == LB_CLEAR;
		});
		if(iter == left_buttons.end())
			return;
		slot = iter - left_buttons.begin();
	}
	left_buttons[slot]={mode, action, label};
	if(do_draw)
		draw_left_slot(slot,0);
}

void cScenButtonsBars::reset_left_buttons() {
	lb_t const defButtons;
	for (auto &button : left_buttons)
		button = defButtons;
}

void cScenButtonsBars::draw_right_buttons() const {
	long pos = right_bar->getPosition();
	for(long i = pos; i < pos + NRSONPAGE && i < right_buttons.size(); i++)
		draw_right_slot(i,0);
}

// mode 0 normal 1 pressed
void cScenButtonsBars::draw_right_slot(short which,short mode) const {
	long pos = right_bar->getPosition();
	if(which < pos || which >= pos + NRSONPAGE || which >= right_buttons.size())
		return;
	auto const &rect=right_buttons_rectangles[which - pos];
	tileImage(mainPtr,rect,bg[17]);
	if(right_buttons[which].action == RB_CLEAR)
		return;

	TextStyle style;
	if(mode > 0)
		style.colour = Colours::RED;
	style.lineHeight = 12;
	win_draw_string(mainPtr,rect,right_buttons[which].label,eTextMode::WRAP,style);
}

// is slot >= 0, force that slot
// if -1, use 1st free slot
void cScenButtonsBars::set_right_button(short slot, eRBAction action, int n, std::string const &label, bool do_draw) {
	if(slot < 0) {
		for(short i = 0; i < right_buttons.size(); i++)
			if(right_buttons[i].action == RB_CLEAR) {
				slot = i;
				break;
			}
	}
	if(slot >= right_buttons.size())
		right_buttons.resize(slot + 1);
	right_buttons[slot]={action, n, label};
	std::replace(right_buttons[slot].label.begin(), right_buttons[slot].label.end(), '|', ' ');
	if(do_draw)
		draw_right_slot(slot,0);
}

void cScenButtonsBars::reset_right_bar_and_buttons() {
	right_buttons.clear();
	draw_right_buttons();
	right_bar->setMaximum(0);
	right_bar->setPosition(0);
}

void cScenButtonsBars::init_bar(std::shared_ptr<cScrollbar>& sbar, const std::string& name, rectangle const &rect, rectangle const &events_rect, int pgSz) {
	sbar.reset(new cScrollbar(mainPtr));
	sbar->setBounds(rect);
	sbar->set_wheel_event_rect(events_rect);
	sbar->setPageSize(pgSz);
	sbar->hide();
	
	drawable_mgr.add_drawable(UI_LAYER_DEFAULT, name, sbar);
	event_listeners[name] = std::dynamic_pointer_cast<iEventListener>(sbar);
}


