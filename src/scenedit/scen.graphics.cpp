
#include <cstdio>
#include <sstream>
#include <cstring>
#include "scen.global.hpp"
#include "scenario/scenario.hpp"
#include "gfx/gfxsheets.hpp"
#include "gfx/render_image.hpp"
#include "gfx/render_shapes.hpp"
#include "gfx/render_text.hpp"
#include "gfx/tiling.hpp" // for bg
#include "scen.graphics.hpp"
#include <cmath>
#include "scen.keydlgs.hpp"
#include "sounds.hpp"
#include "mathutil.hpp"
#include "tools/drawable_manager.hpp"
#include "tools/cursors.hpp"

#include "dialogxml/dialogs/dialog.hpp"

#include "scen.core.hpp"
#include "scen.menus.hpp"
#include "scen.townout.hpp"
#include "dialogxml/widgets/scrollbar.hpp"
#include "fileio/resmgr/res_image.hpp"

#include "scen.btnmg.hpp"

void undo_clip();

extern cOutdoors* current_terrain;
extern sf::RenderWindow mainPtr;
extern sf::View mainView;
extern cDrawableManager drawable_mgr;
extern cTown* current_town;
extern short cen_x, cen_y,current_terrain_type,cur_town;
extern cTown* town;
extern short cur_viewing_mode;
extern eScenMode overall_mode;
eDrawMode draw_mode = DRAW_TERRAIN;
extern bool editing_town;
extern cScenario scenario;

short mini_map_scales[3] = {12, 6, 4};
extern location cur_out, mouse_spot;
extern ter_num_t current_ground;

cCustomGraphics spec_scen_g;
const sf::Color hilite_colour = {0xff, 0x00, 0x80, 0x40};

// begin new stuff
std::string current_string[2];

static short get_small_icon(ter_num_t ter){
	short icon = -1;
	auto const &ter_type=scenario.get_terrain(ter);
	switch(ter_type.special){
		case eTerSpec::NONE:
			icon = ter_type.flag1;
			break;
		case eTerSpec::CHANGE_WHEN_STEP_ON:
			icon = 87;
			break;
		case eTerSpec::DAMAGING:
			switch(eDamageType(ter_type.flag3)) {
				case eDamageType::WEAPON:
					icon = 16;
					break;
				case eDamageType::FIRE:
					icon = 18;
					break;
				case eDamageType::POISON:
					icon = 17;
					break;
				case eDamageType::MAGIC:
					icon = 20;
					break;
				case eDamageType::SPECIAL:
					icon = 22;
					break;
				case eDamageType::UNBLOCKABLE:
					icon = 21;
					break;
				case eDamageType::COLD:
					icon = 19;
					break;
				case eDamageType::UNDEAD:
					icon = 25;
					break;
				case eDamageType::DEMON:
					icon = 23;
					break;
				case eDamageType::MARKED: // Invalid
					break;
			}
			break;
		case eTerSpec::BRIDGE:
			icon = 82;
			break;
		case eTerSpec::BED:
			icon = -1;
			break;
		case eTerSpec::DANGEROUS:
			icon = 12;
			switch((eStatus)ter_type.flag3){
				case eStatus::POISONED_WEAPON: // TODO: Do something here
					break;
				case eStatus::BLESS_CURSE:
					icon = ter_type.flag1 > 0 ? 4 : 5;
					break;
				case eStatus::POISON:
					if(ter_type.flag1 > 0)
						icon = 1;
					break;
				case eStatus::HASTE_SLOW:
					icon = ter_type.flag1 > 0 ? 6 : 7;
					break;
				case eStatus::INVULNERABLE: // TODO: Do something here
					break;
				case eStatus::MAGIC_RESISTANCE: // TODO: Do something here
					break;
				case eStatus::WEBS:
					if(ter_type.flag1 > 0)
						icon = 52;
					break;
				case eStatus::DISEASE:
					if(ter_type.flag1 > 0)
						icon = 0;
					break;
				case eStatus::INVISIBLE: // TODO: Do something here
					break;
				case eStatus::DUMB:
					icon = ter_type.flag1 > 0 ? 8 : 9;
					break;
				case eStatus::MARTYRS_SHIELD: // TODO: Do something here
					break;
				case eStatus::ASLEEP:
					if(ter_type.flag1 > 0)
						icon = 3;
					break;
				case eStatus::PARALYZED: // TODO: Do something here
					break;
				case eStatus::ACID:
					if(ter_type.flag1 > 0)
						icon = 2;
					break;
				case eStatus::FORCECAGE:
					if(ter_type.flag1 > 0)
						icon = 43;
					break;
				case eStatus::MAIN: case eStatus::CHARM:
					icon = -1;
					break; // Nothing to do here; these values are "magic" and should not be used
			}
			break;
		case eTerSpec::CRUMBLING:
			icon = 98;
			break;
		case eTerSpec::LOCKABLE:
			icon = 94;
			break;
		case eTerSpec::UNLOCKABLE:
			if(ter_type.flag2 >= 5)
				icon = (ter_type.flag2 == 10) ? 96 : 95;
			else icon = 94;
			break;
		case eTerSpec::IS_A_SIGN:
			icon = 92;
			break;
		case eTerSpec::CALL_SPECIAL:
		case eTerSpec::CALL_SPECIAL_WHEN_USED:
			icon = ter_type.flag3;
			break;
		case eTerSpec::IS_A_CONTAINER:
			icon = 93;
			break;
		case eTerSpec::WATERFALL_CAVE:
		case eTerSpec::WATERFALL_SURFACE:
			icon = 91;
			break;
		case eTerSpec::CONVEYOR:
			switch(ter_type.flag1){
				case DIR_N:
					icon = 78;
					break;
				case DIR_NE:
					icon = 79;
					break;
				case DIR_NW:
					icon = 77;
					break;
				case DIR_E:
					icon = 80;
					break;
				case DIR_S:
					icon = 74;
					break;
				case DIR_SE:
					icon = 81;
					break;
				case DIR_SW:
					icon = 75;
					break;
				case DIR_W:
					icon = 76;
					break;
			}
			break;
		case eTerSpec::BLOCKED_TO_MONSTERS:
			icon = 28;
			break;
		case eTerSpec::TOWN_ENTRANCE:
			icon = 84;
			break;
		case eTerSpec::CHANGE_WHEN_USED:
			icon = 97;
			break;
		case eTerSpec::WILDERNESS_CAVE:
		case eTerSpec::WILDERNESS_SURFACE:
			icon = 90;
			break;
		default:
			icon = -1;
	}
	if(icon == 255) icon = -1;
	return icon;
}

static std::vector<short> get_small_icons(location at, ter_num_t t_to_draw) {
	std::vector<short> icons;
	// draw start icon, if starting point
	if(editing_town && cur_town == scenario.which_town_start && scenario.where_start == at) {
		icons.push_back(-1);
	}
	if(!editing_town && scenario.out_sec_start == cur_out && scenario.out_start == at) {
		icons.push_back(-1);
	}
	short ter_small_i = get_small_icon(t_to_draw);
	// Special case for towns
	if(ter_small_i == 84 && !editing_town) {
		bool have_town = false;
		for(size_t i = 0; i < current_terrain->city_locs.size(); i++) {
			if(current_terrain->city_locs[i] == at)
				have_town = true;
		}
		if(!have_town) ter_small_i++;
	}
	if(ter_small_i >= 0)
		icons.push_back(ter_small_i);
	
	if(is_special(at.x, at.y)) {
		std::vector<spec_loc_t>& spec_list = editing_town ? town->special_locs : current_terrain->special_locs;
		int num_spec = std::count_if(spec_list.begin(), spec_list.end(), [at](spec_loc_t which) {
			return which.spec >= 0 && which.x == at.x && which.y == at.y;
		});
		if(num_spec > 1)
			icons.push_back(89);
		else icons.push_back(88);
	}
	if(editing_town) {
		if(scenario.get_terrain(t_to_draw).light_radius > 0)
			icons.push_back(83);
		for(size_t i = 0; i < town->start_locs.size(); i++)
			if(at == town->start_locs[i]) {
				icons.push_back(70 + i);
			}
		for(size_t i = 0; i < town->wandering_locs.size(); i++)
			if(at == town->wandering_locs[i]) {
				icons.push_back(86);
			}
		if(is_field_type(at.x, at.y, BARRIER_FIRE)) {
			icons.push_back(33);
		}
		if(is_field_type(at.x, at.y, BARRIER_FORCE)) {
			icons.push_back(34);
		}
	} else {
		for(size_t i = 0; i < current_terrain->wandering_locs.size(); i++)
			if(at == current_terrain->wandering_locs[i]) {
				icons.push_back(86);
			}
	}
	return icons;
}

void run_startup_g() {
	sf::Event event;
	auto & pict_to_draw = *ResMgr::textures.get("edsplash", true);
	rectangle dest_rect = rectangle(pict_to_draw);
	
	play_sound(-95);
	sf::Time delay = time_in_ticks(120);
	sf::Clock timer;
	while(sound_going(95) || timer.getElapsedTime() < delay) {
		draw_splash(pict_to_draw, mainPtr, dest_rect);
		if(!mainPtr.pollEvent(event)) continue;
		if(event.type == sf::Event::GainedFocus || event.type == sf::Event::MouseMoved)
			set_cursor(watch_curs);
		if(event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed)
			break;
	}
	
	// It's never needed again, so don't keep it in GPU memory
	ResMgr::textures.free("edsplash");
}

void load_graphics(){
	// Preload the main editor interface graphics
	ResMgr::textures.get("edbuttons");
	ResMgr::textures.get("teranim");
	ResMgr::textures.get("fields");
	ResMgr::textures.get("objects");
	ResMgr::textures.get("tinyobj");
	ResMgr::textures.get("termap");
}

void redraw_screen() {
	rectangle windRect(mainPtr);
	
	// Switch back to the default view while drawing the background tiles
	// so that they are not upscaled
	mainPtr.setView(mainPtr.getDefaultView());
	tileImage(mainPtr,windRect,bg[20]);
	mainPtr.setView(mainView);
	
	draw_main_screen();
	
	if(overall_mode < MODE_MAIN_SCREEN)
		draw_terrain();
	
	// DIRTY FIX to a problem that exist somewhere else. But where?
	undo_clip(mainPtr);
	
	drawable_mgr.draw_all();
	
	mainPtr.display();
}

void draw_main_screen() {
	scen_controls.draw_left_buttons();
	// draw right buttons (only when not editing terrain)
	if(overall_mode >= MODE_MAIN_SCREEN) {
		rectangle draw_rect;
		draw_rect.left = RIGHT_AREA_UL_X;
		draw_rect.top = RIGHT_AREA_UL_Y;
		draw_rect.right = RIGHT_AREA_UL_X + RIGHT_AREA_WIDTH - 16;
		draw_rect.bottom = RIGHT_AREA_UL_Y + RIGHT_AREA_HEIGHT;
		
		frame_rect(mainPtr, draw_rect, sf::Color::Black);
		draw_rect.inset(1,1);
		tileImage(mainPtr,draw_rect,bg[17]);
		
		scen_controls.draw_right_buttons();
	}
	
	// draw terrain palette
	if(overall_mode < MODE_MAIN_SCREEN || overall_mode == MODE_EDIT_TYPES) {
		place_location();
		set_up_terrain_buttons(false);
	}
	
}
	
void set_up_terrain_buttons(bool reset) {
	int max;
	switch(draw_mode) {
		case DRAW_TERRAIN: max = scenario.ter_types.size(); break;
		case DRAW_ITEM: max = scenario.scen_items.size(); break;
		case DRAW_MONST: max = scenario.scen_monsters.size(); max--; break;
		default: return;
	}
	if(overall_mode == MODE_EDIT_TYPES) max++;
	
	auto &controls=scen_controls;
	auto &palette_bar=scen_controls.palette_bar;
	if(reset) palette_bar->setPosition(0);
	palette_bar->setMaximum((max - 241) / 16);
	
	int first = palette_bar->getPosition() * 16;
	if(draw_mode == DRAW_MONST) first++, max++;
	int end = min(first + 256, max);
 	
	short pic,small_i;
	rectangle ter_from;
	rectangle tiny_from,tiny_to;
	
	// first make terrain buttons
	auto const &editor_mixed = *ResMgr::textures.get("edbuttons");
	for(short i = first; i < end; i++) {
		rectangle draw_rect = controls.terrain_rects[i - first];
		draw_rect.offset(RIGHT_AREA_UL_X, RIGHT_AREA_UL_Y);
		switch(draw_mode){
			case DRAW_TERRAIN: {
				if(i == scenario.ter_types.size()) {
					rectangle ter_plus_from = {148,235,164,251};
					rect_draw_some_item(editor_mixed, ter_plus_from, mainPtr, draw_rect);
					break;
				}
				Texture source_gworld;
				if (cPict::get_picture(scenario.get_terrain(i).get_picture_num(), source_gworld, ter_from))
					rect_draw_some_item(source_gworld,ter_from, mainPtr, draw_rect);
				small_i = get_small_icon(i);
				tiny_from = controls.terrain_base_small_button_from;
				tiny_from.offset(7 * (small_i % 30),7 * (small_i / 30));
				tiny_to = draw_rect;
				tiny_to.top = tiny_to.bottom - 7;
				tiny_to.left = tiny_to.right - 7;
				if(small_i >= 0 && small_i < 255)
					rect_draw_some_item(editor_mixed, tiny_from, mainPtr, tiny_to);
				break;
			}
			case DRAW_MONST:
				frame_rect(mainPtr, draw_rect, sf::Color::Black);
				cPict::draw_monster(mainPtr, draw_rect, scenario.get_monster(i).get_picture_num());
				break;
			case DRAW_ITEM:
				Texture source_gworld;
				tiny_to = draw_rect;
				frame_rect(mainPtr, tiny_to, sf::Color::Black);
				if (cPict::get_picture(scenario.get_item(i).get_picture_num(true), source_gworld, ter_from))
					rect_draw_some_item(source_gworld, ter_from, mainPtr, tiny_to, sf::BlendAlpha);
				break;
		}
	}
	
	if(overall_mode < MODE_MAIN_SCREEN)
		controls.draw_palette(editing_town);
}

void draw_terrain(){
	location which_pt,where_draw;
	rectangle draw_rect,clipping_rect = {8,8,332,260};
	ter_num_t t_to_draw;
	rectangle source_rect,tiny_to,tiny_to_base = {37,29,44,36},from_rect,to_rect;
	rectangle boat_rect = {0,0,36,28};
	tiny_to_base.offset(TER_RECT_UL_X,TER_RECT_UL_Y);
	clipping_rect.offset(TER_RECT_UL_X,TER_RECT_UL_Y);
	
	if(overall_mode >= MODE_MAIN_SCREEN)
		return;
	
	auto const &controls=scen_controls;
	if(cur_viewing_mode == 0) {
		tileImage(mainPtr, controls.terrain_rectangle,bg[17]);
		frame_rect(mainPtr, controls.terrain_rectangle, sf::Color::Black);
		// limits for town: only town and for outside terrain: sector+one outside bordering
		int const limits[]={editing_town ? 4 : 3, int(editing_town ? town->max_dim-1 : 48)-4};
		// if the position is outside, resets it to center of the map
		if (cen_x<limits[0] || cen_x>limits[1]) cen_x=(editing_town ? town->max_dim-1 : 48)/2;
		if (cen_y<limits[0] || cen_y>limits[1]) cen_y=(editing_town ? town->max_dim-1 : 48)/2;
		for(short q = 0; q < 9; q++)
			for(short r = 0; r < 9; r++) {
				where_draw.x = q;
				where_draw.y = r;
				if(editing_town) {
					t_to_draw = town->terrain(cen_x + q - 4,cen_y + r - 4);
				}
				else {
					short ter_x = cen_x + q - 4, ter_y = cen_y + r - 4;
					if(ter_x == -1 && ter_y == -1 && cur_out.x > 0 && cur_out.y > 0)
						t_to_draw = scenario.outdoors[cur_out.x - 1][cur_out.y - 1]->terrain[47][47];
					else if(ter_x == -1 && ter_y == 48 && cur_out.x > 0 && cur_out.y < scenario.outdoors.height() - 1)
						t_to_draw = scenario.outdoors[cur_out.x - 1][cur_out.y + 1]->terrain[47][0];
					else if(ter_x == 48 && ter_y == -1 && cur_out.x < scenario.outdoors.width() - 1 && cur_out.y > 0)
						t_to_draw = scenario.outdoors[cur_out.x + 1][cur_out.y - 1]->terrain[0][47];
					else if(ter_x == 48 && ter_y == 48 && cur_out.x < scenario.outdoors.width() - 1 && cur_out.y < scenario.outdoors.height() - 1)
						t_to_draw = scenario.outdoors[cur_out.x + 1][cur_out.y + 1]->terrain[0][0];
					else if(ter_x == -1 && ter_y >= 0 && ter_y < 48 && cur_out.x > 0)
						t_to_draw = scenario.outdoors[cur_out.x - 1][cur_out.y]->terrain[47][ter_y];
					else if(ter_y == -1 && ter_x >= 0 && ter_x < 48 && cur_out.y > 0)
						t_to_draw = scenario.outdoors[cur_out.x][cur_out.y - 1]->terrain[ter_x][47];
					else if(ter_x == 48 && ter_y >= 0 && ter_y < 48 && cur_out.x < scenario.outdoors.width() - 1)
						t_to_draw = scenario.outdoors[cur_out.x + 1][cur_out.y]->terrain[0][ter_y];
					else if(ter_y == 48 && ter_x >= 0 && ter_x < 48 && cur_out.y < scenario.outdoors.height() - 1)
						t_to_draw = scenario.outdoors[cur_out.x][cur_out.y + 1]->terrain[ter_x][0];
					else if(ter_x == -1 || ter_x == 48 || ter_y == -1 || ter_y == 48)
						t_to_draw = 90;
					else t_to_draw = current_terrain->terrain[ter_x][ter_y];
				}
				draw_one_terrain_spot(q,r,t_to_draw);
				
				rectangle destrec;
				
				destrec.left = 8 + BITMAP_WIDTH * where_draw.x;
				destrec.right = destrec.left + BITMAP_WIDTH;
				destrec.top = 8 + BITMAP_HEIGHT * where_draw.y;
				destrec.bottom = destrec.top + BITMAP_HEIGHT;
				destrec.offset(TER_RECT_UL_X,TER_RECT_UL_Y);
				
				auto const & fields_gworld = *ResMgr::textures.get("fields");
				auto const & vehicle_gworld = *ResMgr::textures.get("vehicle");
				
				if(is_road(cen_x + q - 4,cen_y + r - 4))
					rect_draw_some_item(fields_gworld, calc_rect(0, 2), mainPtr, destrec, sf::BlendAlpha);
				if(is_spot(cen_x + q - 4,cen_y + r - 4))
					rect_draw_some_item(fields_gworld, calc_rect(4, 0), mainPtr, destrec, sf::BlendAlpha);
				
				which_pt.x = cen_x + q - 4;
				which_pt.y =cen_y + r - 4;
				
				if(!editing_town) {
					for(short i = 0; i < scenario.boats.size(); i++) {
						if(scenario.boats[i].which_town == 200 &&
						   scenario.boats[i].sector == cur_out &&
						   scenario.boats[i].loc == which_pt)
							rect_draw_some_item(vehicle_gworld,boat_rect,mainPtr,destrec,sf::BlendAlpha);
						
					}
					for(short i = 0; i < scenario.horses.size(); i++) {
						source_rect = boat_rect;
						source_rect.offset(0,36);
						if(scenario.horses[i].which_town == 200 &&
						   scenario.horses[i].sector == cur_out &&
						   scenario.horses[i].loc == which_pt)
							rect_draw_some_item(vehicle_gworld,source_rect,mainPtr,destrec,sf::BlendAlpha);
						
					}
				}
				
				if(editing_town) {
					for(short i = 0; i < scenario.boats.size(); i++) {
						if(scenario.boats[i].which_town == cur_town &&
						   scenario.boats[i].loc == which_pt)
							rect_draw_some_item(vehicle_gworld,boat_rect,mainPtr,destrec,sf::BlendAlpha);
						
					}
					for(short i = 0; i < scenario.horses.size(); i++) {
						source_rect = boat_rect;
						source_rect.offset(0,36);
						if(scenario.horses[i].which_town == cur_town &&
						   scenario.horses[i].loc == which_pt)
							rect_draw_some_item(vehicle_gworld,source_rect,mainPtr,destrec,sf::BlendAlpha);
						
					}
					std::set<eFieldType> list_fields;
					for(auto const &field : town->preset_fields)
						if(field.loc.x == cen_x + q - 4 && field.loc.y == cen_y + r - 4)
							list_fields.insert(field.type);
					if(list_fields.count(FIELD_WEB)) {
						from_rect = calc_rect(5,0);
						rect_draw_some_item(fields_gworld,from_rect,mainPtr,destrec,sf::BlendAlpha);
					}
					if(list_fields.count(OBJECT_CRATE)) {
						from_rect = calc_rect(6,0);
						rect_draw_some_item(fields_gworld,from_rect,mainPtr,destrec,sf::BlendAlpha);
					}
					if(list_fields.count(OBJECT_BARREL)) {
						from_rect = calc_rect(7,0);
						rect_draw_some_item(fields_gworld,from_rect,mainPtr,destrec,sf::BlendAlpha);
					}
					if(list_fields.count(BARRIER_FIRE)) {
						from_rect = calc_rect(8,4);
						rect_draw_some_item(*ResMgr::textures.get("teranim"),from_rect,mainPtr,destrec,sf::BlendAlpha);
					}
					if(list_fields.count(FIELD_QUICKFIRE)) {
						from_rect = calc_rect(7,1);
						rect_draw_some_item(fields_gworld,from_rect,mainPtr,destrec,sf::BlendAlpha);
					}
					if(list_fields.count(BARRIER_FORCE)) {
						from_rect = calc_rect(10,4);
						rect_draw_some_item(*ResMgr::textures.get("teranim"),from_rect,mainPtr,destrec,sf::BlendAlpha);
					}
					if(list_fields.count(OBJECT_BLOCK)) {
						from_rect = calc_rect(3,0);
						rect_draw_some_item(fields_gworld,from_rect,mainPtr,destrec,sf::BlendAlpha);
					}
					if(list_fields.count(BARRIER_CAGE)) {
						from_rect = calc_rect(0,0);
						rect_draw_some_item(fields_gworld,from_rect,mainPtr,destrec,sf::BlendAlpha);
					}
					for(short i = 0; i < 8; i++) {
						if(list_fields.count(eFieldType(SFX_SMALL_BLOOD + i))) {
							from_rect = calc_rect(i,3);
							rect_draw_some_item(fields_gworld,from_rect,mainPtr,destrec,sf::BlendAlpha);
						}
					}
				}
				
				tiny_to = tiny_to_base;
				tiny_to.offset(28 * q, 36 * r);
				
				// Tiny icons
				std::vector<short> icons = get_small_icons(loc(cen_x + q - 4, cen_y + r - 4), t_to_draw);
				
				if(!icons.empty()) {
					bool has_start = icons[0] == -1;
					rectangle tiny_from_base = {120, 0, 127, 7};
					auto const &editor_mixed = *ResMgr::textures.get("edbuttons");
					for(short icon : icons) {
						rectangle tiny_from = tiny_from_base;
						if(icon == -1) {
							tiny_from.offset(30 * 7, 0);
							tiny_from.right += 14;
							tiny_to.left -= 14;
						} else {
							tiny_from.offset((icon % 30) * 7, (icon / 30) * 7);
						}
						rect_draw_some_item(editor_mixed, tiny_from, mainPtr, tiny_to);
						if(icon == -1) tiny_to.left += 14;
						tiny_to.offset(0, -7);
						// Now check to see if it's overflowing our space
						if(tiny_to.top < destrec.top) {
							tiny_to.offset(-7, 7 * (has_start ? 4 : 5));
						}
					}
				}
				
				if(mouse_spot.x >= 0 && mouse_spot.y >= 0) {
					bool need_hilite = false, large_hilite = false;
					int d = dist(where_draw, mouse_spot);
					if(overall_mode == MODE_SMALL_PAINTBRUSH && d <= 1)
						large_hilite = need_hilite = true;
					else if((overall_mode == MODE_ERASER || overall_mode == MODE_SMALL_SPRAYCAN) && d <= 2)
						large_hilite = need_hilite = true;
					else if((overall_mode == MODE_LARGE_PAINTBRUSH || overall_mode == MODE_LARGE_SPRAYCAN) && d <= 4)
						large_hilite = need_hilite = true;
					else if(where_draw == mouse_spot)
						need_hilite = true;
					else if(overall_mode == MODE_PLACE_CREATURE || overall_mode == MODE_PLACE_SAME_CREATURE) {
						extern short mode_count;
						cMonster& monst = scenario.get_monster(mode_count);
						for(int x = 0; x < monst.x_width; x++) {
							for(int y = 0; y < monst.y_width; y++) {
								location this_spot = {where_draw.x - x, where_draw.y - y};
								if(this_spot == mouse_spot)
									need_hilite = true;
							}
						}
					} else if(overall_mode == MODE_DRAWING) {
						cTerrain& ter = scenario.get_terrain(current_terrain_type);
						if(ter.obj_num > 0) {
							// TODO: Don't do this if auto-completion of large terrain objects is disabled
							for(int x = 0; x < ter.obj_size.x; x++) {
								for(int y = 0; y < ter.obj_size.y; y++) {
									location this_spot = {where_draw.x - x + ter.obj_pos.x, where_draw.y - y + ter.obj_pos.y};
									if(this_spot == mouse_spot)
										need_hilite = true;
								}
							}
						}
					}
					if(need_hilite) {
						fill_rect(mainPtr, destrec, hilite_colour);
						if(large_hilite && where_draw == mouse_spot)
							fill_rect(mainPtr, destrec, hilite_colour);
					}
				}
			}
		if(editing_town) {
			draw_monsts();
			draw_items();
		}
		
		clip_rect(mainPtr,clipping_rect);
		
		if(editing_town) {
			// draw info rects
			for(short i = 0; i < town->area_desc.size(); i++)
				if(town->area_desc[i].left > 0) {
					draw_rect.left = 22 + 28 * (town->area_desc[i].left - cen_x + 4);
					draw_rect.right = 22 + 28 * (town->area_desc[i].right - cen_x + 4);
					draw_rect.top = 24 + 36 * (town->area_desc[i].top - cen_y + 4);
					draw_rect.bottom = 24 + 36 * (town->area_desc[i].bottom - cen_y + 4);
					draw_rect.inset(-10, -13);
					draw_rect.offset(TER_RECT_UL_X,TER_RECT_UL_Y);
					frame_rect(mainPtr, draw_rect, Colours::RED);
				}
			// draw border rect
			draw_rect.left = 21 + 28 * (town->in_town_rect.left - cen_x + 4);
			draw_rect.right = 21 + 28 * (town->in_town_rect.right - cen_x + 4);
			draw_rect.top = 25 + 36 * (town->in_town_rect.top - cen_y + 4);
			draw_rect.bottom = 25 + 36 * (town->in_town_rect.bottom - cen_y + 4);
			draw_rect.inset(10, 13);
			draw_rect.offset(TER_RECT_UL_X, TER_RECT_UL_Y);
			frame_rect(mainPtr, draw_rect, sf::Color::White);
		}
		if(!editing_town) {
			// draw info rects
			for(short i = 0; i < current_terrain->area_desc.size(); i++)
				if(current_terrain->area_desc[i].left > 0) {
					draw_rect.left = 22 + 28 * (current_terrain->area_desc[i].left - cen_x + 4);
					draw_rect.right = 22 + 28 * (current_terrain->area_desc[i].right - cen_x + 4);
					draw_rect.top = 24 + 36 * (current_terrain->area_desc[i].top - cen_y + 4);
					draw_rect.bottom = 24 + 36 * (current_terrain->area_desc[i].bottom - cen_y + 4);
					draw_rect.inset(-10, -13);
					draw_rect.offset(TER_RECT_UL_X, TER_RECT_UL_Y);
					frame_rect(mainPtr, draw_rect, Colours::RED);
				}
		}
		clip_rect(mainPtr, controls.terrain_rectangle);
		
		//if(cur_viewing_mode == 0)
		//	draw_frames();
	}
	
	else {
		tileImage(mainPtr, controls.terrain_rectangle,bg[17]);
		frame_rect(mainPtr, controls.terrain_rectangle, sf::Color::Black);
		// Width available:  64 4x4 tiles, 42 6x6 tiles, or 21 12x12 tiles -- 256 pixels
		// Height available: 81 4x4 tiles, 54 6x6 tiles, or 27 12x12 tiles -- 324 pixels
		short const size = cur_viewing_mode<=3 ? mini_map_scales[cur_viewing_mode - 1] : 12;
		int const max_dim = (editing_town ? town->max_dim : 48);
		int const xNumTiles=(256 / size);
		int xMin = cen_x-(xNumTiles/2), xMax = xMin+xNumTiles;
		if (xMin<0) {
			xMin=0;
			xMax=std::min(xNumTiles, max_dim);
			cen_x=(xNumTiles/2);
		}
		else if (xMax>max_dim) {
			xMax=max_dim;
			xMin=std::max(0, xMax-xNumTiles);
			cen_x=xMin+(xNumTiles/2);
		}
		if (xMax-xMin!=xNumTiles)
			cen_x=(xNumTiles/2)-xMin;
		int const yNumTiles=(324 / size);
		int yMin = cen_y-(yNumTiles/2), yMax = yMin+yNumTiles;
		if (yMin<0) {
			yMin=0;
			yMax=std::min(yNumTiles, max_dim);
			cen_y=(yNumTiles/2);
		}
		else if (yMax>max_dim) {
			yMax=max_dim;
			yMin=std::max(0, yMax-yNumTiles);
			cen_y=yMin+(yNumTiles/2);
		}
		if (yMax-yMin!=yNumTiles)
			cen_y=(yNumTiles/2);
		for(short q = xMin; q < xMax; q++)
			for(short r = yMin; r < yMax; r++) {
				if(q < 0 || q >= max_dim || r < 0 || r >= max_dim)
					t_to_draw = 90;
				else t_to_draw = editing_town ? town->terrain(q,r) : current_terrain->terrain(q,r);
				draw_one_tiny_terrain_spot(q-xMin,r-yMin,t_to_draw,size,is_road(q,r));
			}
	}
}

void draw_monsts() {
	short width,height,m_start_pic;
	Texture from_gworld;
	rectangle source_rect;
	location where_draw,store_loc;
	
	for(auto const &creature : town->creatures) {
		if(creature.number == 0) continue;
		where_draw.x = creature.start_loc.x - cen_x + 4;
		where_draw.y = creature.start_loc.y - cen_y + 4;
		auto const &monster=scenario.get_monster(creature.number);
		width = monster.x_width;
		height = monster.y_width;
			
		for(short k = 0; k < width * height; k++) {
			if(where_draw.x == minmax(0,8,where_draw.x) && where_draw.y == minmax(0,8,where_draw.y) && monster.picture_num >= 1000)
				std::tie(from_gworld,source_rect) = spec_scen_g.find_graphic((monster.picture_num%1000) + k);
			else if(monster.picture_num>=0 && monster.picture_num < 1000 && monster.picture_num<m_pic_index.size())
				cPict::get_picture(cPictNum(monster.picture_num,PIC_MONST), from_gworld, source_rect, 0, k);
			else
				continue;
				
			store_loc = where_draw;
			store_loc.x += k % width;
			store_loc.y += k / width;
			if(store_loc.x < 0 || store_loc.x > 8 || store_loc.y < 0 || store_loc.y > 8)
				continue;
				
			rectangle destrec;
			destrec.left = 8 + BITMAP_WIDTH * store_loc.x;
			destrec.right = destrec.left + BITMAP_WIDTH;
			destrec.top = 8 + BITMAP_HEIGHT * store_loc.y;
			destrec.bottom = destrec.top + BITMAP_HEIGHT;
				
			destrec.left = destrec.right - (source_rect.right - source_rect.left);
			destrec.top = destrec.bottom - (source_rect.bottom - source_rect.top);
			destrec.offset(TER_RECT_UL_X,TER_RECT_UL_Y);
				
			rect_draw_some_item(from_gworld, source_rect, mainPtr, destrec, sf::BlendAlpha);
		}
	}
}

static void update_item_rectangle(cPictNum const &pict, rectangle &rect)
{
	if (pict.type==ePicType::PIC_CUSTOM_ITEM || pict.num<45)
		return;
	rect.top += 9;
	rect.bottom -= 9;
	rect.left += 5;
	rect.right -= 5;
}

void draw_items() {
	rectangle dest_rect;
	location where_draw;
	
	for(short i = 0; i < town->preset_items.size(); i++) {
		if(town->preset_items[i].code < 0)
			continue;
		where_draw.x = town->preset_items[i].loc.x - cen_x + 4;
		where_draw.y = town->preset_items[i].loc.y - cen_y + 4;
		if(where_draw.x < 0 || where_draw.x > 8 || where_draw.y < 0 || where_draw.y > 8)
			continue;
		auto const &pic_num = scenario.get_item(town->preset_items[i].code).get_picture_num(false);
		rectangle source_rect;
		Texture source_gworld;

		dest_rect = calc_rect(where_draw.x,where_draw.y);
		dest_rect.offset(8+TER_RECT_UL_X,8+TER_RECT_UL_Y);
		if (cPict::get_picture(pic_num, source_gworld, source_rect)) {
			update_item_rectangle(pic_num, dest_rect);
			rect_draw_some_item(source_gworld, source_rect, mainPtr, dest_rect, sf::BlendAlpha);
		}
	}
}


void force_tiny_redraw() {
//	for(short q = 0; q < 8; q++)
//		for(short r = 0; r < 64; r++)
//			ter_changed[q][r] = 255;
	
}

void draw_one_terrain_spot (short i,short j,ter_num_t terrain_to_draw) {
	if(i < 0 || i > 8 || j < 0 || j > 8)
		return;
	
	rectangle source_rect;
	Texture source_gworld;
	if (!cPict::get_picture(scenario.get_terrain(terrain_to_draw).get_picture_num(), source_gworld, source_rect))
		return;

	location where_draw;
	where_draw.x = (char) i;
	where_draw.y = (char) j;
	rectangle destrec;
	destrec.left = 8 + BITMAP_WIDTH * where_draw.x;
	destrec.right = destrec.left + BITMAP_WIDTH;
	destrec.top = 8 + BITMAP_HEIGHT * where_draw.y;
	destrec.bottom = destrec.top + BITMAP_HEIGHT;
	destrec.offset(TER_RECT_UL_X,TER_RECT_UL_Y);
	
	rect_draw_some_item(source_gworld, source_rect, mainPtr, destrec);
}

void draw_one_tiny_terrain_spot (short i,short j,ter_num_t terrain_to_draw,short size,bool road) {
	rectangle from_rect;
	Texture source_gworld;
	rectangle dest_rect = {0,0,size,size};
	dest_rect.offset(8 + TER_RECT_UL_X + size * i, 8 + TER_RECT_UL_Y + size * j);
	if (cPict::get_picture(scenario.get_terrain(terrain_to_draw).get_map_picture_num(), source_gworld, from_rect))
		rect_draw_some_item(source_gworld, from_rect, mainPtr, dest_rect);
	if(road) {
		rectangle road_rect = dest_rect;
		int border = (size - 4) / 2;
		road_rect.inset(border,border);
		auto const &edbuttons=*ResMgr::textures.get("edbuttons");
		rectangle const road_from={120, 231, 124, 235};
		rect_draw_some_item(edbuttons, road_from, mainPtr, road_rect);
	}
	if(mouse_spot.x >= 0 && mouse_spot.y >= 0) {
		location where_draw(i,j);
		bool need_hilite = false, large_hilite = false;
		int d = dist(where_draw, mouse_spot);
		if(overall_mode == MODE_SMALL_PAINTBRUSH && d <= 1)
			large_hilite = need_hilite = true;
		else if((overall_mode == MODE_ERASER || overall_mode == MODE_SMALL_SPRAYCAN) && d <= 2)
			large_hilite = need_hilite = true;
		else if((overall_mode == MODE_LARGE_PAINTBRUSH || overall_mode == MODE_LARGE_SPRAYCAN) && d <= 4)
			large_hilite = need_hilite = true;
		else if(where_draw == mouse_spot)
			need_hilite = true;
		if(need_hilite) {
			fill_rect(mainPtr, dest_rect, hilite_colour);
			if(large_hilite && where_draw == mouse_spot)
				fill_rect(mainPtr, dest_rect, hilite_colour);
		}
	}
}

/* Input terrain currently trying to draw. Get back rectangle in terrain template containing
 desired pixmap, or rectangle to darkness if desired map not present */
rectangle get_template_rect (unsigned short type_wanted) {
	rectangle store_rect;
	short picture_wanted;
	
	picture_wanted = scenario.get_terrain(type_wanted).picture;
	if(picture_wanted >= 1000)
		picture_wanted = 0;
	picture_wanted = picture_wanted % 50;
	store_rect.top = 0 + (picture_wanted / 10) * BITMAP_HEIGHT;
	store_rect.bottom = store_rect.top + BITMAP_HEIGHT;
	store_rect.left = 0 + (picture_wanted % 10) * BITMAP_WIDTH;
	store_rect.right = store_rect.left + BITMAP_WIDTH;
	
	return store_rect;
}

void draw_frames() {
	location which_pt;
	rectangle draw_rect;
	
	for(short q = 0; q < 9; q++) {
		for(short r = 0; r < 9; r++) {
			which_pt.x = cen_x + q - 4;
			which_pt.y = cen_y + r - 4;
			draw_rect.top = 23 + r * 36;
			draw_rect.bottom = 58 + r * 36;
			draw_rect.left = 23 + q * 28;
			draw_rect.right = 50 + q * 28;
			draw_rect.offset(TER_RECT_UL_X,TER_RECT_UL_Y);
			for(short i = 0; i < town->wandering_locs.size(); i++)
				if((which_pt.x == town->wandering_locs[i].x) &&
					(which_pt.y == town->wandering_locs[i].y)) {
					
					frame_rect(mainPtr, draw_rect, Colours::RED);
				}
			for(short i = 0; i < town->start_locs.size(); i++)
				if((which_pt.x == town->start_locs[i].x) &&
					(which_pt.y == town->start_locs[i].y)) {
					frame_rect(mainPtr, draw_rect, Colours::PINK);
				}
			
			
		}
	}
	
}

static void place_selected_terrain(ter_num_t ter, rectangle draw_rect) {
	rectangle source_rect;
	Texture source_gworld;
	if (cPict::get_picture(scenario.get_terrain(ter).get_picture_num(), source_gworld, source_rect))
		rect_draw_some_item(source_gworld,source_rect, mainPtr,draw_rect);

	short small_i = get_small_icon(ter);
	rectangle tiny_to = draw_rect;
	tiny_to.top = tiny_to.bottom - 7;
	tiny_to.left = tiny_to.right - 7;
	rectangle tiny_from = scen_controls.terrain_base_small_button_from;
	tiny_from.offset(7 * (small_i % 30),7 * (small_i / 30));
	if(small_i >= 0 && small_i < 255)
		rect_draw_some_item(*ResMgr::textures.get("edbuttons"),tiny_from,mainPtr,tiny_to);
}

void place_location() {
	std::ostringstream sout;
	auto &controls=scen_controls;
	rectangle draw_rect,source_rect;
	rectangle text_rect = {0,0,12,100};
	text_rect.offset(RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
	short picture_wanted;
	tileImage(mainPtr, controls.terrain_buttons_rect, bg[17]);
	frame_rect(mainPtr, controls.terrain_buttons_rect, sf::Color::Black);
	location mouse = translate_mouse_coordinates(sf::Mouse::getPosition(mainPtr));
	
	draw_rect = text_rect;
	draw_rect.offset({5, controls.terrain_rects[255].top + 18});
	if(overall_mode < MODE_MAIN_SCREEN) {
		// std::cout << "Mouse: " << mouse << " Buttons: " << terrain_buttons_rect << " Terrain: " << terrain_rect << std::endl;
		if(mouse.in(controls.terrain_buttons_rect)) {
			location rel_mouse = mouse;
			rel_mouse.x -= RIGHT_AREA_UL_X;
			rel_mouse.y -= RIGHT_AREA_UL_Y;
			int first = scen_controls.palette_bar->getPosition() * 16;
			for(int i = 0; i < 256; i++) {
				if(rel_mouse.in(controls.terrain_rects[i])) {
					switch(draw_mode) {
						case DRAW_TERRAIN:
							if(first+i >= 0 && first + i < scenario.ter_types.size())
								sout << "Terrain: " << scenario.ter_types[first + i].name;
							break;
						case DRAW_ITEM:
							if(first+i >= 0 && first + i < scenario.scen_items.size())
								sout << "Item: " << scenario.scen_items[first + i].full_name;
							break;
						case DRAW_MONST:
							if(first+i >= 0 && first + i + 1 < scenario.scen_monsters.size())
								sout << "Monster: " << scenario.scen_monsters[first + i + 1].m_name;
							break;
					}
					break;
				}
			}
		} else if(mouse.in(controls.terrain_rectangle) && mouse_spot.x >= 0) {
			if(cur_viewing_mode <= 0 || cur_viewing_mode>=4)
				sout << "Under mouse: x = " << (cen_x - 4 + mouse_spot.x)
					<< ", y = " << (cen_y - 4 + mouse_spot.y);
			else {
				short const &scale = mini_map_scales[cur_viewing_mode - 1];
				sout << "Under mouse: x = " << (cen_x-256/scale/2 + mouse_spot.x)
					<< ", y = " << (cen_y-324/scale/2 + mouse_spot.y);
			}
		}
		if(sout.str().empty())
			sout << "Center: x = " << cen_x << ", y = " << cen_y;
	} else {
		sout << "Click terrain to edit. ";
	}
	TextStyle style;
	style.lineHeight = 12;
	win_draw_string(mainPtr, draw_rect, sout.str(), eTextMode::LEFT_TOP, style);
	sout.str("");
	
	draw_rect = text_rect;
	draw_rect.offset({260 ,controls.terrain_rects[255].top + 18});
	sout << current_terrain_type;
	win_draw_string(mainPtr, draw_rect, sout.str(), eTextMode::LEFT_TOP, style);
	sout.str("");
	
	if(overall_mode < MODE_MAIN_SCREEN) {
		draw_rect = text_rect;
		draw_rect.offset({5,controls.terrain_rects[255].bottom + 121});
		win_draw_string(mainPtr, draw_rect, current_string[0], eTextMode::LEFT_TOP, style);
		draw_rect = text_rect;
		draw_rect.offset({RIGHT_AREA_WIDTH / 2,controls.terrain_rects[255].bottom + 121});
		win_draw_string(mainPtr, draw_rect, current_string[1], eTextMode::LEFT_TOP, style);
	}
	
	draw_rect.top = controls.palette_buttons[0][0].top + controls.terrain_rects[255].bottom + 5;
	draw_rect.left = controls.palette_buttons[9][0].right + 10; // + 17;
	draw_rect.bottom = draw_rect.top + 36;
	draw_rect.right = draw_rect.left + 28;
	
	if(overall_mode < MODE_MAIN_SCREEN) {
		place_selected_terrain(current_terrain_type, draw_rect);
		extern short mode_count;
		bool draw_field = false;
		if(overall_mode == MODE_PLACE_CREATURE || overall_mode == MODE_PLACE_SAME_CREATURE)
			cPict::draw_monster(mainPtr, draw_rect, scenario.get_monster(mode_count).get_picture_num());
		else if(overall_mode == MODE_PLACE_ITEM || overall_mode == MODE_PLACE_SAME_ITEM) {
			cPictNum pic=scenario.get_item(mode_count).get_picture_num(false);
			Texture source_gworld;
			if (cPict::get_picture(pic,source_gworld, source_rect)) {
				update_item_rectangle(pic, draw_rect);
				rect_draw_some_item(source_gworld,source_rect,mainPtr,draw_rect,sf::BlendAlpha);
			}
		} else if(overall_mode == MODE_TOGGLE_SPECIAL_DOT) {
			draw_field = true;
			source_rect = calc_rect(4, 0);
		} else if(overall_mode == MODE_PLACE_FORCECAGE) {
			draw_field = true;
			source_rect = calc_rect(0, 0);
		} else if(overall_mode == MODE_PLACE_WEB) {
			draw_field = true;
			source_rect = calc_rect(5, 0);
		} else if(overall_mode == MODE_PLACE_CRATE) {
			draw_field = true;
			source_rect = calc_rect(6, 0);
		} else if(overall_mode == MODE_PLACE_BARREL) {
			draw_field = true;
			source_rect = calc_rect(7, 0);
		} else if(overall_mode == MODE_PLACE_FIRE_BARRIER) {
			source_rect = calc_rect(8, 4);
			rect_draw_some_item(*ResMgr::textures.get("teranim"),source_rect,mainPtr,draw_rect,sf::BlendAlpha);
		} else if(overall_mode == MODE_PLACE_FORCE_BARRIER) {
			source_rect = calc_rect(8, 4);
			rect_draw_some_item(*ResMgr::textures.get("teranim"),source_rect,mainPtr,draw_rect,sf::BlendAlpha);
		} else if(overall_mode == MODE_PLACE_QUICKFIRE) {
			draw_field = true;
			source_rect = calc_rect(7, 1);
		} else if(overall_mode == MODE_PLACE_STONE_BLOCK) {
			draw_field = true;
			source_rect = calc_rect(3, 0);
		} else if(overall_mode == MODE_PLACE_SFX) {
			draw_field = true;
			source_rect = calc_rect(mode_count, 3);
		}
		if(draw_field) {
			auto const & fields_gworld = *ResMgr::textures.get("fields");
			rect_draw_some_item(fields_gworld,source_rect,mainPtr,draw_rect,sf::BlendAlpha);
		}
		draw_rect.offset(0,40);
		place_selected_terrain(current_ground, draw_rect);
	}
}

void set_string(std::string string,std::string string2) {
	current_string[0] = string;
	current_string[1] = string2;
}

bool is_special(short i,short j) {
	location check(i,j);
	
	if(editing_town) {
		for(auto const &spec : town->special_locs)
			if(spec == check && spec.spec >= 0)
				return true;
	}
	else {
		for(auto const &spec : current_terrain->special_locs)
			if(spec == check && spec.spec >= 0)
				return true;
	}
	return false;
}

bool is_spot(short i,short j){
	return editing_town ? is_field_type(i,j,SPECIAL_SPOT) : current_terrain->is_special_spot(i,j);
}

bool is_road(short i,short j){
	return editing_town ? is_field_type(i,j,SPECIAL_ROAD) : current_terrain->is_road(i,j);
}

bool is_field_type(short i,short j,eFieldType field_type) {
	for(auto const &field : town->preset_fields)
		if(field.type == field_type && field.loc.x == i && field.loc.y == j)
			return true;
	return false;
}

void make_field_type(short i,short j,eFieldType field_type) {
	if(is_field_type(i,j,field_type))
		return;
	cTown::cField the_field={{i,j}, field_type};
	for(auto &field : town->preset_fields)
		if(field.type == 0) {
			field=the_field;
			return;
		}
	town->preset_fields.push_back(the_field);
}


void take_field_type(short i,short j,eFieldType field_type) {
	for(auto &field : town->preset_fields)
		if(field.type == field_type && field.loc.x == i && field.loc.y == j) {
			field.type = FIELD_DISPEL;
			return;
		}
}

bool container_there(location l) {
	if(!editing_town)
		return false;
	if(scenario.get_terrain(town->terrain(l.x,l.y)).special == eTerSpec::IS_A_CONTAINER)
		return true;
	return is_field_type(l.x,l.y, OBJECT_BARREL) || is_field_type(l.x,l.y, OBJECT_CRATE);
}

// Translate mouse event coordinates based on the global view and viewport
sf::Vector2f translate_mouse_coordinates(sf::Vector2i const point) {
	return mainPtr.mapPixelToCoords(point, mainView);
}
