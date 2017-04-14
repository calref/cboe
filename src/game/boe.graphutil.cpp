
#include <SFML/Graphics.hpp>

#include <cstdio>
#include <queue>

#include "boe.global.hpp"

#include "universe.hpp"
#include "boe.graphutil.hpp"
#include "boe.text.hpp"
#include "boe.locutils.hpp"
#include "boe.graphics.hpp"
#include "boe.infodlg.hpp"
#include "boe.monster.hpp"
#include "boe.specials.hpp"
#include "sounds.hpp"
#include "render_image.hpp"
#include "render_shapes.hpp"
#include "mathutil.hpp"
#include "strdlog.hpp"
#include "res_image.hpp"

extern sf::RenderWindow mainPtr;
extern rectangle	windRect;
extern short stat_window;
extern bool cartoon_happening;
extern eGameMode overall_mode;
extern short current_spell_range;
extern bool anim_onscreen;
extern cUniverse univ;
extern effect_pat_type current_pat;
extern sf::RenderWindow mini_map;
extern short combat_posing_monster , current_working_monster ; // 0-5 PC 100 + x - monster x

extern sf::RenderTexture terrain_screen_gworld;
extern std::queue<pending_special_type> special_queue;

extern location ul;
extern location center;
extern short which_combat_type;
extern bool monsters_going,anim_onscreen;

extern short num_targets_left;
extern location spell_targets[8];

extern long anim_ticks;

extern char spot_seen[9][9];
extern short monster_index[21];

extern bool supressing_some_spaces;
extern location ok_space[4];
extern bool can_draw_pcs;
extern cCustomGraphics spec_scen_g;

rectangle boat_rects[4] = {{0,0,36,28}, {0,28,36,56},{0,56,36,84},{0,84,36,112}};
bool gave_no_g_error = false;

//short dest; // 0 - terrain gworld   1 - screen
// if terrain_to_draw is -1, do black
// if terrain_to_draw >= 10000, force to draw graphic which is terrain_to_draw - 10000
void draw_one_terrain_spot (short i,short j,short terrain_to_draw) {
	rectangle where_draw;
	rectangle source_rect;
	sf::Texture* source_gworld;
	short anim_type = 0;
	location l;
	
	l.x = i; l.y = j;
	if(supressing_some_spaces && (l != ok_space[0]) && (l != ok_space[1]) && (l != ok_space[2]) && (l != ok_space[3]))
		return;
	
	where_draw = calc_rect(i,j);
 	where_draw.offset(13,13);
 	if(terrain_to_draw == -1) {
		fill_rect(terrain_screen_gworld, where_draw, sf::Color::Black);
		return;
	}
 	
	if(terrain_to_draw >= 10000) { // force using a specific graphic
		terrain_to_draw -= 10000;
		int which_sheet = terrain_to_draw / 50;
		source_gworld = ResMgr::get<ImageRsrc>("ter" + std::to_string(1 + which_sheet)).get();
		terrain_to_draw %= 50;
		source_rect = calc_rect(terrain_to_draw % 10, terrain_to_draw / 10);
		anim_type = -1;
	}
	else if(univ.scenario.ter_types[terrain_to_draw].picture >= 2000) { // custom
		graf_pos_ref(source_gworld, source_rect) = spec_scen_g.find_graphic(univ.scenario.ter_types[terrain_to_draw].picture - 2000 + (anim_ticks % 4));
		anim_type = 0;
	}
	else if(univ.scenario.ter_types[terrain_to_draw].picture >= 1000) { // custom
		graf_pos_ref(source_gworld, source_rect) = spec_scen_g.find_graphic(univ.scenario.ter_types[terrain_to_draw].picture - 1000);
	}
	else if(univ.scenario.ter_types[terrain_to_draw].picture >= 960) { // animated
		source_gworld = ResMgr::get<ImageRsrc>("teranim").get();
		terrain_to_draw = univ.scenario.ter_types[terrain_to_draw].picture;
		source_rect = calc_rect(4 * ((terrain_to_draw - 960) / 5) + (anim_ticks % 4),(terrain_to_draw - 960) % 5);
		anim_type = 0;
	}
	else {
		terrain_to_draw = univ.scenario.ter_types[terrain_to_draw].picture;
		int which_sheet = terrain_to_draw / 50;
		source_gworld = ResMgr::get<ImageRsrc>("ter" + std::to_string(1 + which_sheet)).get();
		terrain_to_draw %= 50;
		source_rect = calc_rect(terrain_to_draw % 10, terrain_to_draw / 10);
		anim_type = -1;
	}
	
	if(anim_type >= 0) {
		if((is_town()) || (is_out()))
			anim_onscreen = true;
	}
	
	rect_draw_some_item(*source_gworld, source_rect, terrain_screen_gworld, where_draw);
}

void draw_monsters() {
	short width,height;
	rectangle source_rect,to_rect;
	location where_draw,store_loc;
	ter_num_t ter;
	rectangle monst_rects[4][4] = {
		{{0,0,36,28}},
		{{0,7,18,21},{18,7,36,21}},
		{{9,0,27,14},{9,14,27,28}},
		{{0,0,18,14},{0,14,18,28},{18,0,36,14},{18,14,36,28}}
	};
	
	if(is_out())
		for(auto& enc : univ.party.out_c)
			if(enc.exists) {
				if(point_onscreen(univ.party.out_loc,enc.m_loc) && can_see_light(univ.party.out_loc,enc.m_loc,sight_obscurity) < 5) {
					where_draw.x = enc.m_loc.x - univ.party.out_loc.x + 4;
					where_draw.y = enc.m_loc.y - univ.party.out_loc.y + 4;
					
					short picture_wanted = -1;
					for(mon_num_t i : enc.what_monst.monst) {
						if(i > 0) {
							picture_wanted = get_monst_picnum(i);
							std::tie(width, height) = get_monst_dims(i);
							break;
						}
					}
					
					if(picture_wanted >= 0) {
						if(picture_wanted >= 1000) {
							for(short k = 0; k < width * height; k++) {
								sf::Texture* src_gw;
								graf_pos_ref(src_gw, source_rect) = spec_scen_g.find_graphic(picture_wanted % 1000 +
																							 ((enc.direction < 4) ? 0 : (width * height)) + k);
								to_rect = monst_rects[(width - 1) * 2 + height - 1][k];
								to_rect.offset(13 + 28 * where_draw.x,13 + 36 * where_draw.y);
								rect_draw_some_item(*src_gw, source_rect, terrain_screen_gworld,to_rect, sf::BlendAlpha);
							}
						}
						if(picture_wanted < 1000) {
							for(short k = 0; k < width * height; k++) {
								source_rect = get_monster_template_rect(picture_wanted,(enc.direction < 4) ? 0 : 1,k);
								to_rect = monst_rects[(width - 1) * 2 + height - 1][k];
								to_rect.offset(13 + 28 * where_draw.x,13 + 36 * where_draw.y);
								int which_sheet = m_pic_index[picture_wanted].i / 20;
								sf::Texture& monst_gworld = *ResMgr::get<ImageRsrc>("monst" + std::to_string(1 + which_sheet));
								rect_draw_some_item(monst_gworld, source_rect, terrain_screen_gworld,to_rect, sf::BlendAlpha);
							}
						}
					} else enc.exists = false;
				}
			}
	if(is_town() || is_combat()) {
		for(short i = 0; i < univ.town.monst.size(); i++) {
			const cCreature& monst = univ.town.monst[i];
			if(monst.active != 0 && !monst.invisible && monst.status[eStatus::INVISIBLE] <= 0)
				if(point_onscreen(center,monst.cur_loc) && party_can_see_monst(i)) {
					where_draw.x = monst.cur_loc.x - center.x + 4;
					where_draw.y = monst.cur_loc.y - center.y + 4;
					std::tie(width, height) = get_monst_dims(monst.number);
					
					for(short k = 0; k < width * height; k++) {
						store_loc = where_draw;
						store_loc.x += k % width;
						store_loc.y += k / width;
						ter = univ.town->terrain(monst.cur_loc.x,monst.cur_loc.y);
						// in bed?
						if(store_loc.x >= 0 && store_loc.x < 9 && store_loc.y >= 0 && store_loc.y < 9 &&
						   (univ.scenario.ter_types[ter].special == eTerSpec::BED) && isHumanoid(monst.m_type)
						   && (monst.active == 1 || monst.target == 6) &&
						   width == 1 && height == 1)
							draw_one_terrain_spot((short) where_draw.x,(short) where_draw.y,10000 + univ.scenario.ter_types[ter].flag1);
						else if(monst.picture_num >= 1000) {
							bool isParty = monst.picture_num >= 10000;
							sf::Texture* src_gw;
							pic_num_t need_pic = (monst.picture_num % 1000) + k;
							if(monst.direction >= 4) need_pic += width * height;
							if(combat_posing_monster == i + 100) need_pic += (2 * width * height);
							graf_pos_ref(src_gw, source_rect) = spec_scen_g.find_graphic(need_pic, isParty);
							Draw_Some_Item(*src_gw, source_rect, terrain_screen_gworld, store_loc, 1, 0);
						} else {
							pic_num_t this_monst = monst.picture_num;
							int pic_mode = (monst.direction) < 4 ? 0 : 1;
							pic_mode += (combat_posing_monster == i + 100) ? 10 : 0;
							source_rect = get_monster_template_rect(this_monst, pic_mode, k);
							int which_sheet = m_pic_index[this_monst].i / 20;
							sf::Texture& monst_gworld = *ResMgr::get<ImageRsrc>("monst" + std::to_string(1 + which_sheet));
							Draw_Some_Item(monst_gworld, source_rect, terrain_screen_gworld, store_loc, 1, 0);
						}
					}
				}
		}
	}
}

void play_see_monster_str(unsigned short m, location monst_loc) {
	short pic, spec;
	ePicType type;
	pic = univ.scenario.scen_monsters[m].picture_num;
	type =  get_monst_pictype(m);
	spec = univ.scenario.scen_monsters[m].see_spec;
	// Then run the special, if any
	if(spec > -1){
		queue_special(eSpecCtx::SEE_MONST, 0, spec, monst_loc);
	}
}

void draw_combat_pc(cPlayer& who, location center, bool attacking) {
	rectangle active_pc_rect;
	if(who.main_status == eMainStatus::ALIVE)
		if(point_onscreen(center, who.combat_pos) && (cartoon_happening || party_can_see(who.combat_pos) < 6)) {
			location where_draw(who.combat_pos.x - center.x + 4, who.combat_pos.y - center.y + 4);
			rectangle source_rect;
			sf::Texture* from_gw;
			pic_num_t pic = who.which_graphic;
			if(pic >= 1000) {
				bool isParty = pic >= 10000;
				pic_num_t need_pic = pic % 1000;
				if(who.direction >= 4)
					need_pic++;
				if(attacking)
					need_pic += 2;
				graf_pos_ref(from_gw, source_rect) = spec_scen_g.find_graphic(need_pic, isParty);
			} else if(pic >= 100) {
				// Note that we assume it's a 1x1 graphic.
				// PCs can't be larger than that, but we leave it to the scenario designer to avoid assigning larger graphics.
				pic_num_t need_pic = pic - 100;
				int mode = 0;
				if(who.direction >= 4)
					mode++;
				if(attacking)
					mode += 10;
				source_rect = get_monster_template_rect(need_pic, mode, 0);
				int which_sheet = m_pic_index[need_pic].i / 20;
				from_gw = ResMgr::get<ImageRsrc>("monst" + std::to_string(1 + which_sheet)).get();
			} else {
				source_rect = calc_rect(2 * (pic / 8), pic % 8);
				if(who.direction >= 4)
					source_rect.offset(28,0);
				if(attacking)
					source_rect.offset(0,288);
				from_gw = ResMgr::get<ImageRsrc>("pcs").get();
			}
			
			Draw_Some_Item(*from_gw, source_rect, terrain_screen_gworld, where_draw, 1, 0);
			
		}
}

void frame_active_pc(location center) {
	if(monsters_going) return;
	location where_draw(univ.current_pc().combat_pos.x - center.x + 4, univ.current_pc().combat_pos.y - center.y + 4);
	rectangle active_pc_rect;
	active_pc_rect.top = 18 + where_draw.y * 36;
	active_pc_rect.left = 18 + where_draw.x * 28;
	active_pc_rect.bottom = 54 + where_draw.y * 36;
	active_pc_rect.right = 46 + where_draw.x * 28;
	active_pc_rect.offset(ul);
	
	frame_roundrect(mainPtr, active_pc_rect, 8, sf::Color::Magenta);
}

void draw_pcs(location center) {
	rectangle source_rect,active_pc_rect;
	location where_draw;
	
	if(!univ.party.is_alive())
		return;
	if(!can_draw_pcs)
		return;
	
	for(auto& pc : univ.party)
		draw_combat_pc(pc, center, combat_posing_monster == univ.get_target_i(pc));
	
	// This means the active PC is drawn twice, which is a bit inefficient. Oh well!
	draw_combat_pc(univ.current_pc(), center, combat_posing_monster == univ.cur_pc);
}

void draw_items(location where){
	if(!point_onscreen(center,where)) return;
	location where_draw(4 + where.x - center.x, 4 + where.y - center.y);
	rectangle from_rect, to_rect;
	if(supressing_some_spaces && (where != ok_space[0]) && (where != ok_space[1]) && (where != ok_space[2]) && (where != ok_space[3]))
		return;
	for(int i = 0; i < univ.town.items.size(); i++) {
		if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].item_loc == where) {
			if(univ.town.items[i].contained) continue;
			if(party_can_see(where) >= 6) continue;
			sf::Texture* src_gw;
			to_rect = coord_to_rect(where_draw.x,where_draw.y);
			if(univ.town.items[i].graphic_num >= 10000){
				graf_pos_ref(src_gw, from_rect) = spec_scen_g.find_graphic(univ.town.items[i].graphic_num - 10000, true);
			}else{
				graf_pos_ref(src_gw, from_rect) = calc_item_rect(univ.town.items[i].graphic_num, to_rect);
			}
			rect_draw_some_item(*src_gw, from_rect, terrain_screen_gworld, to_rect, sf::BlendAlpha);
		}
	}
}

void draw_outd_boats(location center) {
	location where_draw;
	rectangle source_rect;
	sf::Texture& vehicle_gworld = *ResMgr::get<ImageRsrc>("vehicle");
	cVehicle& in_boat = univ.party.boats[univ.party.in_boat];
	cVehicle& in_horse = univ.party.horses[univ.party.in_horse];
	
	for(auto& boat : univ.party.boats) {
		if(!boat.exists) continue;
		if(boat.which_town != 200) continue;
		if(boat == in_boat) continue;
		location loc = local_to_global(boat.loc);
		if(point_onscreen(center, loc) && can_see_light(center, loc, sight_obscurity) < 5) {
			where_draw.x = loc.x - center.x + 4;
			where_draw.y = loc.y - center.y + 4;
			Draw_Some_Item(vehicle_gworld, calc_rect(0,0), terrain_screen_gworld, where_draw, 1, 0);
		}
	}
	for(auto& horse : univ.party.horses) {
		if(!horse.exists) continue;
		if(horse.which_town != 200) continue;
		if(horse == in_horse) continue;
		location loc = local_to_global(horse.loc);
		if(point_onscreen(center, loc) && can_see_light(center, loc, sight_obscurity) < 5) {
			where_draw.x = loc.x - center.x + 4;
			where_draw.y = loc.y - center.y + 4;
			Draw_Some_Item(vehicle_gworld, calc_rect(0,1), terrain_screen_gworld, where_draw, 1, 0);
		}
	}
}

void draw_town_boat(location center) {
	location where_draw;
	rectangle source_rect;
	sf::Texture& vehicle_gworld = *ResMgr::get<ImageRsrc>("vehicle");
	cVehicle& in_boat = univ.party.boats[univ.party.in_boat];
	cVehicle& in_horse = univ.party.horses[univ.party.in_horse];
	
	for(auto& boat : univ.party.boats) {
		if(!boat.exists) continue;
		if(boat.which_town != univ.party.town_num) continue;
		if(boat == in_boat) continue;
		if(point_onscreen(center, boat.loc) && can_see_light(center, boat.loc, sight_obscurity) < 5 && pt_in_light(center, boat.loc)) {
			where_draw.x = boat.loc.x - center.x + 4;
			where_draw.y = boat.loc.y - center.y + 4;
			Draw_Some_Item(vehicle_gworld, calc_rect(1,0), terrain_screen_gworld, where_draw, 1, 0);
		}
	}
	for(auto& horse : univ.party.horses) {
		if(!horse.exists) continue;
		if(horse.which_town != univ.party.town_num) continue;
		if(horse == in_horse) continue;
		if(point_onscreen(center, horse.loc) && can_see_light(center, horse.loc, sight_obscurity) < 5 && pt_in_light(center, horse.loc)) {
			where_draw.x = horse.loc.x - center.x + 4;
			where_draw.y = horse.loc.y - center.y + 4;
			Draw_Some_Item(vehicle_gworld, calc_rect(1,1), terrain_screen_gworld, where_draw, 1, 0);
		}
	}
}

extern std::vector<location> forcecage_locs;

void draw_fields(location where){
	if(!point_onscreen(center,where)) return;
	if(party_can_see(where) >= 6) return;
	location where_draw(4 + where.x - center.x, 4 + where.y - center.y);
	sf::Texture& fields_gworld = *ResMgr::get<ImageRsrc>("fields");
	if(is_out()){
		if(univ.out.is_spot(where.x,where.y))
			Draw_Some_Item(fields_gworld,calc_rect(4,0),terrain_screen_gworld,where_draw,1,0);
		return;
	}
	if(univ.town.is_force_wall(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(0,1),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_fire_wall(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(1,1),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_antimagic(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(2,1),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_scloud(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(3,1),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_ice_wall(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(4,1),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_blade_wall(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(5,1),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_sleep_cloud(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(6,1),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_block(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(3,0),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_spot(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(4,0),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_web(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(5,0),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_crate(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(6,0),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_barrel(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(7,0),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_fire_barr(where.x,where.y) || univ.town.is_force_barr(where.x,where.y))
		Draw_Some_Item(*ResMgr::get<ImageRsrc>("teranim"),calc_rect(8+(anim_ticks%4),4),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_quickfire(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(7,1),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_sm_blood(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(0,3),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_med_blood(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(1,3),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_lg_blood(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(2,3),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_sm_slime(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(3,3),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_lg_slime(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(4,3),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_ash(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(5,3),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_bones(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(6,3),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_rubble(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(7,3),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_force_cage(where.x,where.y)) {
		Draw_Some_Item(fields_gworld,calc_rect(1,0),terrain_screen_gworld,where_draw,1,0);
		forcecage_locs.push_back(where_draw);
	}
}

void draw_party_symbol(location center) {
	rectangle source_rect;
	location target(4,4);
	short i = 0;
	
	if(!can_draw_pcs)
		return;
	if(!univ.party.is_alive())
		return;
	if((is_town()) && (univ.party.town_loc.x > 70))
		return;
	if(overall_mode == MODE_LOOK_TOWN || cartoon_happening) {
		target.x += univ.party.town_loc.x - center.x;
		target.y += univ.party.town_loc.y - center.y;
	}
	
	if((univ.party.in_boat < 0) && (univ.party.in_horse < 0)) {
		i = first_active_pc();
		sf::Texture* from_gw;
		pic_num_t pic = univ.party[i].which_graphic;
		if(pic >= 1000) {
			bool isParty = pic >= 10000;
			pic_num_t need_pic = pic % 1000;
			if(univ.party.direction >= 4)
				need_pic++;
			graf_pos_ref(from_gw, source_rect) = spec_scen_g.find_graphic(need_pic, isParty);
		} else if(pic >= 100) {
			// Note that we assume it's a 1x1 graphic.
			// PCs can't be larger than that, but we leave it to the scenario designer to avoid assigning larger graphics.
			pic_num_t need_pic = pic - 100;
			int mode = 0;
			if(univ.party.direction >= 4)
				mode++;
			source_rect = get_monster_template_rect(need_pic, mode, 0);
			int which_sheet = m_pic_index[need_pic].i / 20;
			from_gw = ResMgr::get<ImageRsrc>("monst" + std::to_string(1 + which_sheet)).get();
		} else {
			source_rect = calc_rect(2 * (pic / 8), pic % 8);
			if(univ.party.direction >= 4)
				source_rect.offset(28,0);
			from_gw = ResMgr::get<ImageRsrc>("pcs").get();
		}
		ter_num_t ter = 0;
		if(is_out())
			ter = univ.out[univ.party.out_loc.x][univ.party.out_loc.y];
		else if(is_town() || is_combat())
			ter = univ.town->terrain(univ.party.town_loc.x,univ.party.town_loc.y);
		// now wedge in bed graphic
		if(is_town() && univ.scenario.ter_types[ter].special == eTerSpec::BED)
			draw_one_terrain_spot((short) target.x,(short) target.y,10000 + univ.scenario.ter_types[ter].flag1);
		else Draw_Some_Item(*from_gw, source_rect, terrain_screen_gworld, target, 1, 0);
	}
	else if(univ.party.in_boat >= 0) {
		if(univ.party.direction == DIR_N) i = 2;
		else if(univ.party.direction == DIR_S) i = 3;
		else i = univ.party.direction > DIR_S;
		Draw_Some_Item(*ResMgr::get<ImageRsrc>("vehicle"), calc_rect(i,0), terrain_screen_gworld, target, 1, 0);
	}else {
		i = univ.party.direction > 3;
		Draw_Some_Item(*ResMgr::get<ImageRsrc>("vehicle"), calc_rect(i + 2, 1), terrain_screen_gworld, target, 1, 0);
	}
}

// Give the position of the monster graphic in the template in memory
//mode; // 0 - left  1 - right  +10 - combat mode
rectangle get_monster_template_rect (pic_num_t picture_wanted,short mode,short which_part) {
	short adj = 0;
	
	if(mode >= 10) {
		adj += 4;
		mode -= 10;
	}
	if(mode == 0) adj++;
	picture_wanted = (m_pic_index[picture_wanted].i + which_part) % 20;
	return calc_rect(2 * (picture_wanted / 10) + adj, picture_wanted % 10);
}

// Is this a fluid that gets shore plopped down on it?
bool is_fluid(ter_num_t ter_type) {
//	if(((ter_type >= 71) && (ter_type <= 76)) || (ter_type == 90))
//		return true;
//	return false;
	return univ.scenario.ter_types[ter_type].trim_type == eTrimType::FRILLS;
}

// Is this a beach that gets shore plopped down next to it?
bool is_shore(ter_num_t ter_type) {
	if(is_fluid(ter_type))
		return false;
	if(univ.scenario.ter_types[ter_type].trim_type == eTrimType::WATERFALL)
		return false;
//	if(ter_type == 77)
//		return false;
//	if(ter_type == 90)
//		return false;
/*	if(ter_type == 240)
		return false;
	if((ter_type >= 117) && (ter_type <= 131))
		return false;
	if((ter_type >= 193) && (ter_type <= 207))
		return false; */
	return true;
}

// These two functions used to determine wall round-cornering
bool is_wall(ter_num_t ter_type) {
	return univ.scenario.ter_types[ter_type].trim_type == eTrimType::WALL;
//	short pic;
//
//	pic = scenario.ter_types[ter_type].picture;
//
//	if((pic >= 88) && (pic <= 120))
//		return true;
//
//	return false;
}
bool is_ground(ter_num_t ter_type) {
	if(univ.scenario.ter_types[ter_type].trim_type == eTrimType::WALL)
		return false;
	if(univ.scenario.ter_types[ter_type].block_horse)
		return false;
//	if(scenario.ter_types[ter_type].trim_type == eTrimType::WALKWAY)
//		return false;
	return true;
}

char get_fluid_trim(location where,ter_num_t ter_type) {
	bool at_top = false,at_bot = false,at_left = false,at_right = false;
	ter_num_t store;
	char to_return = 0;
	
	if(where.x == 0)
		at_left = true;
	if(where.y == 0)
		at_top = true;
	if((overall_mode == MODE_OUTDOORS) || (overall_mode == MODE_LOOK_OUTDOORS)) {
		if(where.x == 95)
			at_right = true;
		if(where.y == 95)
			at_bot = true;
	}
	else {
		if(where.x == univ.town->max_dim - 1)
			at_right = true;
		if(where.y == univ.town->max_dim - 1)
			at_bot = true;
	}
	
	// Set up trim for fluids
	if(is_fluid(ter_type)) {
		if(!at_left) {
			store = coord_to_ter(where.x - 1,where.y);
			if(is_shore(store))
				to_return |= 64;
		}
		if(!at_right) {
			store = coord_to_ter(where.x + 1,where.y);
			if(is_shore(store))
				to_return |= 4;
		}
		if(!at_top) {
			store = coord_to_ter(where.x,where.y - 1);
			if(is_shore(store))
				to_return |= 1;
		}
		if(!at_bot) {
			store = coord_to_ter(where.x,where.y + 1);
			if(is_shore(store))
				to_return |= 16;
		}
		if((!at_left) && (!at_top)) {
			store = coord_to_ter(where.x - 1,where.y - 1);
			if(is_shore(store))
				to_return |= 128;
		}
		if((!at_right) && (!at_bot)) {
			store = coord_to_ter(where.x + 1,where.y + 1);
			if(is_shore(store))
				to_return |= 8;
		}
		if((!at_right) && (!at_top)) {
			store = coord_to_ter(where.x + 1,where.y - 1);
			if(is_shore(store))
				to_return |= 2;
		}
		if((!at_left) && (!at_bot)) {
			store = coord_to_ter(where.x - 1,where.y + 1);
			if(is_shore(store))
				to_return |= 32;
		}
	}
	if(to_return & 1)
		to_return &= 125;
	if(to_return & 4)
		to_return &= 245;
	if(to_return & 10)
		to_return &= 215;
	if(to_return & 64)
		to_return &= 95;
	
	return to_return;
}

// Sees if party has seen a monster of this sort, gives special messages as necessary
void check_if_monst_seen(unsigned short m_num, location at) {
	// Give special messages if necessary
	if(m_num < 10000 && !univ.party.m_seen.count(m_num)) {
		univ.party.m_seen.insert(m_num);
		play_see_monster_str(m_num, at);
	}
	// Make the monster vocalize if applicable
	snd_num_t sound = -1;
	if(m_num >= 10000)
		sound = univ.party.summons[m_num - 10000].ambient_sound;
	else sound = univ.scenario.scen_monsters[m_num].ambient_sound;
	if(sound > 0 && get_ran(1,1,100) < 10)
		play_sound(-sound);
}

void play_ambient_sound(){ // TODO: Maybe add a system for in-town ambient sounds
	if(overall_mode == MODE_TOWN) {
		for(int i = 0; i < univ.town.monst.size(); i++) {
			if(party_can_see_monst(i))
				check_if_monst_seen(univ.town.monst[i].number, univ.town.monst[i].cur_loc);
		}
		return;
	}
	static const short drip[2] = {78,79}, bird[3] = {76,77,91};
	if(overall_mode != MODE_OUTDOORS) return; // ambient sounds are outdoors only at the moment
	if(get_ran(1,1,100) > 10) return; // 10% chance per move of playing a sound
	short sound_to_play;
	switch(univ.out->ambient_sound){
		case AMBIENT_DRIP:
			sound_to_play = get_ran(1,0,1);
			play_sound(-drip[sound_to_play]);
			break;
		case AMBIENT_BIRD:
			sound_to_play = get_ran(1,0,2);
			play_sound(-bird[sound_to_play]);
			break;
		case AMBIENT_CUSTOM:
			sound_to_play = univ.out->out_sound;
			play_sound(-sound_to_play);
			break;
		case AMBIENT_NONE:
			break; // obviously, do nothing
	}
}


