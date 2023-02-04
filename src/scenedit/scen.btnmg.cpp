
#include <cstring>
#include <cstdio>

#include "scen.global.hpp"
#include <array>
#include <string>
#include <memory>
#include "scen.graphics.hpp"
#include <cmath>
#include "scen.btnmg.hpp"
#include "dialogxml/widgets/scrollbar.hpp"
#include "fileio/resmgr/res_image.hpp"
#include "gfx/render_image.hpp"
#include "gfx/render_shapes.hpp"
#include "gfx/render_text.hpp"
#include "gfx/tiling.hpp"
#include "tools/drawable_manager.hpp"

cScenButtonsBars scen_controls;

extern sf::RenderWindow mainPtr;
extern cDrawableManager drawable_mgr;

ePalBtn cScenButtonsBars::town_buttons[6][10] = {
	{PAL_PENCIL, PAL_BRUSH_LG, PAL_BRUSH_SM, PAL_SPRAY_LG, PAL_SPRAY_SM, PAL_ERASER, PAL_DROPPER, PAL_RECT_HOLLOW, PAL_RECT_FILLED, PAL_BUCKET},
	{PAL_ENTER_N, PAL_ENTER_W, PAL_ENTER_S, PAL_ENTER_E, PAL_EDIT_SIGN, PAL_TEXT_AREA, PAL_WANDER, PAL_CHANGE, PAL_ZOOM, PAL_TERRAIN},
	{PAL_SPEC, PAL_COPY_SPEC, PAL_PASTE_SPEC, PAL_ERASE_SPEC, PAL_EDIT_SPEC, PAL_SPEC_SPOT, PAL_EDIT_ITEM, PAL_SAME_ITEM, PAL_ERASE_ITEM, PAL_ITEM},
	{PAL_BOAT, PAL_HORSE, PAL_ROAD, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_EDIT_MONST, PAL_SAME_MONST, PAL_ERASE_MONST, PAL_MONST},
	{PAL_WEB, PAL_CRATE, PAL_BARREL, PAL_BLOCK, PAL_FIRE_BARR, PAL_FORCE_BARR, PAL_QUICKFIRE, PAL_FORCECAGE, PAL_ERASE_FIELD, PAL_BLANK},
	{PAL_SFX_SB, PAL_SFX_MB, PAL_SFX_LB, PAL_SFX_SS, PAL_SFX_LS, PAL_SFX_ASH, PAL_SFX_BONE, PAL_SFX_ROCK, PAL_BLANK, PAL_BLANK},
};
ePalBtn cScenButtonsBars::out_buttons[6][10] = {
	{PAL_PENCIL, PAL_BRUSH_LG, PAL_BRUSH_SM, PAL_SPRAY_LG, PAL_SPRAY_SM, PAL_ERASER, PAL_DROPPER, PAL_RECT_HOLLOW, PAL_RECT_FILLED, PAL_BUCKET},
	{PAL_EDIT_TOWN, PAL_ERASE_TOWN, PAL_BLANK, PAL_BLANK, PAL_EDIT_SIGN, PAL_TEXT_AREA, PAL_WANDER, PAL_CHANGE, PAL_ZOOM, PAL_BLANK},
	{PAL_SPEC, PAL_COPY_SPEC, PAL_PASTE_SPEC, PAL_ERASE_SPEC, PAL_EDIT_SPEC, PAL_SPEC_SPOT, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK},
	{PAL_BOAT, PAL_HORSE, PAL_ROAD, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK},
	{PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK},
	{PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK},
};

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
	terrain_buttons_rect = {0,0,410,294};
	terrain_buttons_rect.offset(RIGHT_AREA_UL_X, RIGHT_AREA_UL_Y);

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

	// palette buttons
	palette_button_base = {0,0,18,26};
	palette_button_base.offset(RIGHT_AREA_UL_X, RIGHT_AREA_UL_Y);

	for(short i = 0; i < 10; i++)
		for(short j = 0; j < 6; j++) {
			palette_buttons[i][j] = palette_button_base;
			palette_buttons[i][j].offset(i * 25, j * 17);
		}
	for(short i = 0; i < 10; i++)
		for(short j = /*2*/0; j < 6; j++)
			palette_buttons[i][j].offset(0, 3);
	for(short i = 0; i < 10; i++)
		for(short j = /*3*/0; j < 6; j++)
			palette_buttons[i][j].offset(0, 3);
	for(short i = 0; i < 10; i++)
		for(short j = /*4*/0; j < 6; j++)
			palette_buttons[i][j].offset(0, 3);

	// scrollbars
	rectangle right_bar_rect;
	right_bar_rect.top = RIGHT_AREA_UL_Y - 1;
	right_bar_rect.left = RIGHT_AREA_UL_X + RIGHT_AREA_WIDTH - 1 - 16;
	right_bar_rect.bottom = RIGHT_AREA_UL_Y + RIGHT_AREA_HEIGHT + 1;
	right_bar_rect.right = RIGHT_AREA_UL_X + RIGHT_AREA_WIDTH - 1;
	init_bar(right_bar, "right_sbar", right_bar_rect, { 5, 287, 405, 577 }, NRSONPAGE - 1);
	right_bar->setPosition(0);
	
	rectangle pal_bar_rect = terrain_buttons_rect;
	pal_bar_rect.left = pal_bar_rect.right - 16;
	pal_bar_rect.height() = 17 * 16;
	init_bar(palette_bar, "pal_sbar", pal_bar_rect, { 5, 287, 279, 581 }, 16);
}

static std::string version() {
	static std::string version;
	if(version.empty()) {
		std::ostringstream sout;
		sout << "Version " << oboeVersionString();
#if defined(GIT_REVISION) && defined(GIT_TAG_REVISION)
		if(strcmp(GIT_REVISION, GIT_TAG_REVISION) != 0) {
			sout << " [" << GIT_REVISION << "]";
		}
#endif
		version = sout.str();
	}
	return version;
}

void cScenButtonsBars::set_startup_screen()
{
	reset_left_buttons();
	reset_right_bar_and_buttons();
	set_left_button(0,LB_TITLE,LB_NO_ACTION,"Blades of Exile");
	set_left_button(1,LB_TITLE,LB_NO_ACTION,"Scenario Editor");
	set_left_button(3,LB_TEXT,LB_NEW_SCEN,"Make New Scenario");
	set_left_button(4,LB_TEXT,LB_LOAD_SCEN,"Load Scenario");
	set_left_button(7,LB_TEXT,LB_NO_ACTION,"To find out how to use the");
	set_left_button(8,LB_TEXT,LB_NO_ACTION,"editor, select Getting Started ");
	set_left_button(9,LB_TEXT,LB_NO_ACTION,"from the Help menu.");
	set_left_button(NLS - 6,LB_TEXT,LB_NO_ACTION,"Be sure to read the file Blades");
	set_left_button(NLS - 5,LB_TEXT,LB_NO_ACTION,"of Exile License. Using this");
	set_left_button(NLS - 4,LB_TEXT,LB_NO_ACTION,"program implies that you agree ");
	set_left_button(NLS - 3,LB_TEXT,LB_NO_ACTION,"with the terms of the license.");
	set_left_button(NLS - 2,LB_TEXT,LB_NO_ACTION,"Copyright 1997, All rights reserved.");
	set_left_button(NLS - 1,LB_TEXT,LB_NO_ACTION,version());
}

void cScenButtonsBars::set_main_screen(location const &cur_out, int cur_town, std::string const &town_name)
{
	reset_left_buttons();
	reset_right_bar_and_buttons();
	set_left_button(-1,LB_TITLE,LB_NO_ACTION,"Blades of Exile");
	set_left_button(-1,LB_TEXT,LB_NO_ACTION,"Scenario Options");
	set_left_button(-1,LB_TEXT,LB_EDIT_TER,"Edit Terrain Types");
	set_left_button(-1,LB_TEXT,LB_EDIT_MONST,"Edit Monsters");
	set_left_button(-1,LB_TEXT,LB_EDIT_ITEM,"Edit Items");
	set_left_button(-1,LB_TEXT,LB_NEW_TOWN,"Create New Town");
	set_left_button(-1,LB_TEXT,LB_EDIT_TEXT,"Edit Scenario Text");
	set_left_button(-1,LB_TEXT,LB_EDIT_SPECITEM,"Edit Special Items");
	set_left_button(-1,LB_TEXT,LB_EDIT_QUEST,"Edit Quests");
	set_left_button(-1,LB_TEXT,LB_EDIT_SHOPS,"Edit Shops");
	set_left_button(-1,LB_TEXT,LB_NO_ACTION,"");
	set_left_button(-1,LB_TEXT,LB_NO_ACTION,"Outdoors Options");
	std::ostringstream strb;

	strb << "  Section x = " << cur_out.x << ", y = " << cur_out.y;
	set_left_button(-1,LB_TEXT,LB_NO_ACTION, strb.str());
	set_left_button(-1,LB_TEXT,LB_LOAD_OUT,"Load New Section");
	set_left_button(-1,LB_TEXT,LB_EDIT_OUT,"Edit Outdoor Terrain");
	set_left_button(-1,LB_TEXT,LB_NO_ACTION,"",0);
	set_left_button(-1,LB_TEXT,LB_NO_ACTION,"Town/Dungeon Options");
	strb.str("");
	strb << "  Town " << cur_town << ": " << town_name;
	set_left_button(-1,LB_TEXT,LB_NO_ACTION, strb.str());
	set_left_button(-1,LB_TEXT,LB_LOAD_TOWN,"Load Another Town");
	set_left_button(-1,LB_TEXT,LB_EDIT_TOWN,"Edit Town Terrain");
	set_left_button(-1,LB_TEXT,LB_EDIT_TALK,"Edit Town Dialogue");
	set_left_button(NLS - 2,LB_TEXT,LB_NO_ACTION,"Copyright 1997, All rights reserved.");
	set_left_button(NLS - 1,LB_TEXT,LB_NO_ACTION,version());
	show_right_bar();
	show_palette_bar(false);
}

void cScenButtonsBars::reset_right(long newMaximum)
{
	show_right_bar();
	show_palette_bar(false);
	
	right_bar->setPosition(0);;
	reset_right_bar_and_buttons();
	right_bar->setMaximum(newMaximum);
}

bool cScenButtonsBars::handle_one_event(const sf::Event& event)
{
	for (auto& listener : scen_controls.event_listeners) {
		if(listener.second->handle_event(event)) return true;
	}
	return false;
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
		auto const &edbuttons=*ResMgr::graphics.get("edbuttons");
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

void cScenButtonsBars::draw_palette(bool editing_town) const
{
	rectangle palette_to = palette_button_base;
	palette_to.offset(5,terrain_rects[255].bottom + 14);
	auto const &editor_mixed = *ResMgr::graphics.get("edbuttons");
	auto const &cur_palette_buttons = editing_town ? town_buttons : out_buttons;
	for(short i = 0; i < 10; i++){
		for(short j = 0; j < 6; j++){
			if(cur_palette_buttons[j][i] != PAL_BLANK) {
				rectangle palette_from = palette_button_base;
				palette_from.offset(-RIGHT_AREA_UL_X, -RIGHT_AREA_UL_Y);
				int n = cur_palette_buttons[j][i];
				palette_from.offset((n%10) * 25, (n/10) * 17);
				rect_draw_some_item(editor_mixed, palette_from, mainPtr, palette_to, sf::BlendAlpha);
			}
			palette_to.offset(0,17);
		}
		palette_to.offset(25,-6*17);
	}
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
