//
//  boe.minimap.cpp
//  Blades of Exile
//
//  Created by alonso on 10/10/2021.
//

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>

#include "cursors.hpp"
#include "dialog.hpp"
#include "gfxsheets.hpp"
#include "pict.hpp"
#include "prefs.hpp"
#include "render_image.hpp"
#include "render_shapes.hpp"
#include "render_text.hpp"
#include "res_image.hpp"
#include "sounds.hpp"
#include "tiling.hpp"
#include "universe.hpp"
#include "winutil.hpp"

#include "boe.minimap.hpp"

#include "boe.consts.hpp"
#include "boe.infodlg.hpp"
#include "boe.locutils.hpp"

extern eGameMode overall_mode, store_pre_shop_mode, store_pre_talk_mode;
extern short which_combat_type;
extern cUniverse univ;

extern sf::RenderWindow mainPtr;
extern cCustomGraphics spec_scen_g;

namespace minimap {
bool visible=false;
bool changed=false;
sf::RenderWindow windows;
sf::RenderTexture gworld;

void init() {
	float ui_scale = get_float_pref("UIMapScale", 1.0);
	if (ui_scale < 0.1) ui_scale = 1.0;
	if (windows.isOpen()) windows.close();
	windows.create(sf::VideoMode(ui_scale*296,ui_scale*277), "Map", sf::Style::Titlebar | sf::Style::Close);
	windows.setPosition(sf::Vector2i(52,62));
	sf::View view;
	view.reset(sf::FloatRect(0, 0, ui_scale*296,ui_scale*277));
	view.setViewport(sf::FloatRect(0, 0, ui_scale, ui_scale));
	windows.setView(view);
	windows.setVisible(false);
	visible=false;
	setWindowFloating(windows, true);
	makeFrontWindow(mainPtr);
	
	// Create and initialize map gworld
	if(!gworld.create(384, 384)) {
		play_sound(2);
		throw std::string("Failed to initialized automap!");
	} else {
		gworld.clear(sf::Color::White);
	}
}

bool need_redraw()
{
	return visible && changed;
}

void add_pending_redraw()
{
	if (visible)
		changed = true;
}

bool is_visible()
{
	return visible;
}

void set_visible(bool vis)
{
	if (visible==vis)
		return;
	if (vis) {
		give_help(62,0);
		
		rectangle the_rect;
		rectangle dlogpicrect = {6,6,42,42};
		
		windows.setVisible(true);
		visible = true;
		draw(true);
		// when a game is loaded, the windows is not adapted correctly
		// try to force a new refresh
		changed = true;
		makeFrontWindow(mainPtr);
		
		set_cursor(sword_curs);
	}
	else {
		visible=false;
		windows.setVisible(false);
	}
}

void draw(bool need_refresh) {
#ifdef __APPLE__
	// can not be updated when a dialog is running
	if (cDialog::checkIfDialogIsRunning())
		return;
#endif
	if (changed) {
		need_refresh=true;
		changed = false;
	}
	if(!visible) return;
	pic_num_t pic;
	rectangle the_rect;
	location where;
	location kludge;
	rectangle draw_rect,orig_draw_rect = {0,0,6,6},ter_temp_from,base_source_rect = {0,0,12,12};
	rectangle	dlogpicrect = {6,6,42,42};
	bool draw_pcs = true,out_mode;
	rectangle view_rect= {0,0,48,48},tiny_rect = {0,0,32,32},
	redraw_rect = {0,0,48,48},big_rect = {0,0,64,64}; // Rectangle visible in view screen
	
	rectangle area_to_draw_from,area_to_draw_on = {29,47,269,287};
	ter_num_t what_ter;
	bool expl;
	short total_size = 48; // if full redraw, use this to figure out everything
	rectangle custom_from;
	
	// view rect is rect that is visible, redraw rect is area to redraw now
	// area_to_draw_from is final draw from rect
	// area_to_draw_on is final draw to rect
	// extern short store_pre_shop_mode,store_pre_talk_mode;
	if(is_out() || (is_combat() && which_combat_type == 0) ||
		(overall_mode == MODE_TALKING && store_pre_talk_mode == MODE_OUTDOORS) ||
		(overall_mode == MODE_SHOPPING && store_pre_shop_mode == MODE_OUTDOORS)) {
		view_rect.left = minmax(0,8,univ.party.loc_in_sec.x - 20);
		view_rect.right = view_rect.left + 40;
		view_rect.top = minmax(0,8,univ.party.loc_in_sec.y - 20);
		view_rect.bottom = view_rect.top + 40;
		redraw_rect = view_rect;
	}
	else {
		total_size = univ.town->max_dim;
		switch(total_size) {
			case 64:
				view_rect.left = minmax(0,24,univ.party.town_loc.x - 20);
				view_rect.right = view_rect.left + 40;
				view_rect.top = minmax(0,24,univ.party.town_loc.y - 20);
				view_rect.bottom = view_rect.top + 40;
				redraw_rect = big_rect;
				break;
			case 48:
				view_rect.left = minmax(0,8,univ.party.town_loc.x - 20);
				view_rect.right = view_rect.left + 40;
				view_rect.top = minmax(0,8,univ.party.town_loc.y - 20);
				view_rect.bottom = view_rect.top + 40;
				redraw_rect = view_rect;
				break;
			case 32:
				view_rect = tiny_rect;
				redraw_rect = view_rect;
				break;
		}
	}
	if((is_out()) || ((is_combat()) && (which_combat_type == 0)) ||
		((overall_mode == MODE_TALKING) && (store_pre_talk_mode == MODE_OUTDOORS)) ||
		((overall_mode == MODE_SHOPPING) && (store_pre_shop_mode == MODE_OUTDOORS)) ||
		is_town() || is_combat()) {
		area_to_draw_from = view_rect;
		area_to_draw_from.width() = 40;
		area_to_draw_from.height() = 40;
		area_to_draw_from.left *= 6;
		area_to_draw_from.right *= 6;
		area_to_draw_from.top *= 6;
		area_to_draw_from.bottom *= 6;
	}
	
	if(is_combat())
		draw_pcs = false;
	
	const char* title_string = "Your map:";
	bool canMap = true;
	
	static rectangle last_redraw_rect;
	if (redraw_rect!=last_redraw_rect) {
		need_refresh=true;
		last_redraw_rect=redraw_rect;
	}
	if((is_combat()) && (which_combat_type == 0)) {
		title_string = "No map in combat.";
		canMap = false;
	}else if((is_town() && univ.town->defy_mapping)) {
		title_string = "This place defies mapping.";
		canMap = false;
	}
	else if(need_refresh) {
		// CHECKME: unsure, but on osx, if mainPtr.setActive() is not called,
		// the following code does not update the gworld texture if we are
		// called just after closing a dialog
		mainPtr.setActive();
		
		rectangle map_world_rect = rectangle(gworld);
		gworld.setActive();
		
		fill_rect(gworld, map_world_rect, sf::Color::Black);
		
		// Now, if shopping or talking, just don't touch anything.
		if((overall_mode == MODE_SHOPPING) || (overall_mode == MODE_TALKING))
			redraw_rect.right = -1;
		
		if((is_out()) ||
			((is_combat()) && (which_combat_type == 0)) ||
			((overall_mode == MODE_TALKING) && (store_pre_talk_mode == MODE_OUTDOORS)) ||
			((overall_mode == MODE_SHOPPING) && (store_pre_shop_mode == MODE_OUTDOORS)))
			out_mode = true;
		else out_mode = false;
		
		// TODO: It could be possible to draw the entire map here and then only refresh if a spot actually changes terrain type
		for(where.x = redraw_rect.left; where.x < redraw_rect.right; where.x++)
			for(where.y = redraw_rect.top; where.y < redraw_rect.bottom; where.y++) {
				draw_rect = orig_draw_rect;
				draw_rect.offset(6 * where.x, 6 * where.y);
				
				if(out_mode)
					what_ter = univ.out[where.x + 48 * univ.party.i_w_c.x][where.y + 48 * univ.party.i_w_c.y];
				else what_ter = univ.town->terrain(where.x,where.y);
				
				ter_temp_from = base_source_rect;
				
				if(out_mode)
					expl = univ.out.out_e[where.x + 48 * univ.party.i_w_c.x][where.y + 48 * univ.party.i_w_c.y];
				else expl = is_explored(where.x,where.y);
				
				if(expl == 0)
					continue;
				Texture src_gw;
				if (!cPict::get_picture(univ.get_terrain(what_ter).get_map_picture_num(), src_gw,custom_from))
					fill_rect(gworld, draw_rect, sf::Color::Yellow);  // FIXME: show an error here
				else
					rect_draw_some_item(src_gw, custom_from, gworld, draw_rect);
					
				if(is_out() ? univ.out->is_road(where.x,where.y) : univ.town.is_road(where.x,where.y)) {
					draw_rect.inset(1,1);
					rect_draw_some_item(*ResMgr::textures.get("trim"),{8,112,12,116},gworld,draw_rect);
				}
			}
		
		gworld.display();
#ifdef __APPLE__
		// this stops flickering if the display time is too long
		glFlush();
#endif
	}
	
	windows.setActive(false);
	
	// Now place terrain map gworld
	TextStyle style;
	style.font = FONT_BOLD;
	style.pointSize = 10;;
	
	the_rect = rectangle(windows);
	tileImage(windows, the_rect,bg[4]);
	cPict theGraphic(windows);
	theGraphic.setBounds(dlogpicrect);
	theGraphic.setPict(21, PIC_DLOG);
	theGraphic.setFormat(TXT_FRAME, FRM_NONE);
	theGraphic.draw();
	style.colour = sf::Color::White;
	style.lineHeight = 12;
	rectangle const map_title_rect = {3,50,15,300};
	rectangle const map_bar_rect = {15,50,27,300};
	win_draw_string(windows, map_title_rect,title_string,eTextMode::WRAP,style);
	win_draw_string(windows, map_bar_rect,"(Hit Escape to close.)",eTextMode::WRAP,style);
	
	if(canMap) {
		rect_draw_some_item(Texture(gworld.getTexture()),area_to_draw_from,windows,area_to_draw_on);
		
		// Now place PCs and monsters
		if(draw_pcs) {
			if((is_town()) && (univ.party.status[ePartyStatus::DETECT_LIFE] > 0))
				for(short i = 0; i < univ.town.monst.size(); i++)
					if(univ.town.monst[i].active > 0) {
						where = univ.town.monst[i].cur_loc;
						if((is_explored(where.x,where.y)) &&
							((where.x >= view_rect.left) && (where.x < view_rect.right)
							 && where.y >= view_rect.top && where.y < view_rect.bottom)){
								
								draw_rect.left = area_to_draw_on.left + 6 * (where.x - view_rect.left);
								draw_rect.top = area_to_draw_on.top + 6 * (where.y - view_rect.top);
								draw_rect.right = draw_rect.left + 6;
								draw_rect.bottom = draw_rect.top + 6;
								
								fill_rect(windows, draw_rect, Colours::GREEN);
								frame_circle(windows, draw_rect, Colours::BLUE);
							}
					}
			if((overall_mode != MODE_SHOPPING) && (overall_mode != MODE_TALKING)) {
				where = (is_town()) ? univ.party.town_loc : global_to_local(univ.party.out_loc);
				
				draw_rect.left = area_to_draw_on.left + 6 * (where.x - view_rect.left);
				draw_rect.top = area_to_draw_on.top + 6 * (where.y - view_rect.top);
				draw_rect.right = draw_rect.left + 6;
				draw_rect.bottom = draw_rect.top + 6;
				fill_rect(windows, draw_rect, Colours::RED);
				frame_circle(windows, draw_rect, sf::Color::Black);
				
			}
		}
	}
	windows.setActive();
	windows.display();
	
	// Now exit gracefully
	mainPtr.setActive();
}

bool pollEvent()
{
	if (!visible)
		return false;

	sf::Event event;
	if (!windows.pollEvent(event))
		return false;
	if(event.type == sf::Event::Closed)
		set_visible(false);
	else if(event.type == sf::Event::GainedFocus)
		makeFrontWindow(mainPtr);
	else if(event.type == sf::Event::KeyPressed) {
		switch(event.key.code) {
			// checkme: the zone seems big enough
			//   but maybe we can check for arrows here to move
			//   in the zone
			case sf::Keyboard::Escape:
				set_visible(false);
				break;
			default: break;
		}
	}

	return true;
}

}
