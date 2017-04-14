
#include <iomanip>
#include "universe.hpp"
#include "pc.graphics.hpp"
#include "pc.editors.hpp"
#include "pc.action.hpp"
#include "sounds.hpp"
#include "gfxsheets.hpp"
#include "render_shapes.hpp"
#include "render_image.hpp"
#include "res_image.hpp"
#include "message.hpp"
#include "mathutil.hpp"
#include "gitrev.hpp"
#include "tiling.hpp" // for bg
#include "utility.hpp"

extern cUniverse univ;

extern sf::RenderWindow mainPtr;
extern bool party_in_scen,scen_items_loaded;
extern fs::path file_in_mem;

extern short store_flags[3];
extern short current_active_pc;

rectangle whole_win_rect = {0,0,440,590};
extern rectangle pc_area_buttons[6][4] ; // 0 - whole 1 - pic 2 - name 3 - stat strs 4,5 - later
extern rectangle item_string_rects[24][4]; // 0 - name 1 - drop  2 - id  3 -
extern rectangle pc_info_rect; // Frame that holds a pc's basic info and items
extern rectangle name_rect;
extern rectangle pc_race_rect;
extern rectangle info_area_rect;
extern rectangle hp_sp_rect;
extern rectangle skill_rect;
extern rectangle pc_skills_rect[19];
extern rectangle status_rect;
extern rectangle pc_status_rect[15];
extern rectangle traits_rect;
extern rectangle pc_traits_rect[17];
extern rectangle edit_rect[5];

short store_str1a;
short store_str1b;
short store_str2a;
short store_str2b;
short store_which_string_dlog;
short store_page_on,store_num_i;
rectangle ed_buttons_from[2] = {{0,0,57,57},{0,57,57,114}};
short current_pressed_button = -1;
cCustomGraphics spec_scen_g; // not actually needed; just here to silence compiler because it's referenced in fileio.h
// (actually, it WILL be needed eventually)

void init_main_buttons() {
	rectangle base_rect;
	
	// Initialize pc_info_rect in center
	pc_info_rect= whole_win_rect;
	pc_info_rect.inset(100,100);
	pc_info_rect.bottom+=52;
	pc_info_rect.top-=25;
	pc_info_rect.right+=5;
	name_rect.left = pc_info_rect.left;
	name_rect.right = pc_info_rect.left + 100;
	name_rect.bottom = pc_info_rect.top  + 15;
	name_rect.top = pc_info_rect.top;
	
	// Initialize pc_area_buttons
	pc_area_buttons[0][0].top=pc_info_rect.top;
	pc_area_buttons[0][0].bottom=pc_area_buttons[0][0].top + 56;
	
	for(short i = 0; i < 6; i++) {
		pc_area_buttons[i][0].left = 20;
		pc_area_buttons[i][0].right = pc_area_buttons[0][0].left + 56;
		pc_area_buttons[i][2].left = 20;
		pc_area_buttons[i][2].right = pc_area_buttons[i][2].left + 56;
		pc_area_buttons[i][3].left = 20;
		pc_area_buttons[i][3].right = pc_area_buttons[i][3].left + 56;
		pc_area_buttons[i][1].left = 34;
		pc_area_buttons[i][1].right = pc_area_buttons[i][1].left + 28;
		
		pc_area_buttons[i][0].top = pc_area_buttons[0][0].top + 60*i;
		pc_area_buttons[i][0].bottom = pc_area_buttons[0][0].bottom + 60*i;
		
		pc_area_buttons[i][1].top = pc_area_buttons[i][0].top+2;
		pc_area_buttons[i][1].bottom = pc_area_buttons[i][2].top = pc_area_buttons[i][0].bottom - 18;
		pc_area_buttons[i][2].bottom = pc_area_buttons[i][3].top = pc_area_buttons[i][0].bottom - 9;
		pc_area_buttons[i][3].bottom = pc_area_buttons[i][0].bottom;
		pc_area_buttons[i][2].offset(0,-1);
		pc_area_buttons[i][3].offset(0,-2);
	}
	// Initialize the edit_rect buttons
	edit_rect[0].top = pc_info_rect.top;
	for(short i = 0; i < 5; i++) {
		edit_rect[i].top = edit_rect[0].top + 66*i;
		edit_rect[i].bottom = edit_rect[i].top + 53;
	 	edit_rect[i].left = 510;
		edit_rect[i].right = edit_rect[i].left + 53;
	}
	
	// Initialize pc_race_rect
	pc_race_rect.top = pc_info_rect.top;
	pc_race_rect.bottom = name_rect.bottom;
	pc_race_rect.left = name_rect.right;
	pc_race_rect.right = pc_info_rect.left + (pc_info_rect.right - pc_info_rect.left)/2;
	
	// initialize info_area_rect
	info_area_rect.top = pc_info_rect.top;
	info_area_rect.left = pc_info_rect.left;
	info_area_rect.right = pc_race_rect.right;
	info_area_rect.bottom = pc_info_rect.bottom;
	
	// Initialize hp_sp_rect
	hp_sp_rect.top = name_rect.bottom + 3;
	hp_sp_rect.left = pc_info_rect.left + 1;
	hp_sp_rect.right = pc_race_rect.right;
	hp_sp_rect.bottom = hp_sp_rect.top + 12;
	// Initialize skill_rect
	skill_rect.top = hp_sp_rect.bottom + 2;
	skill_rect.left = pc_info_rect.left + 1;
	skill_rect.right = pc_race_rect.right;
	skill_rect.bottom = skill_rect.top + 5;
	
	// Initialize skills_rect
	base_rect.top = skill_rect.bottom + 1;
	base_rect.left = skill_rect.left + 1;
	base_rect.right = name_rect.right - 1;
	base_rect.bottom = base_rect.top + (pc_info_rect.bottom - skill_rect.bottom)/30;
	
	for(short i = 0; i < 19; i++) {
		pc_skills_rect[i] = base_rect;
		pc_skills_rect[i].offset((i / 10) * ((name_rect.right)-(name_rect.left)), (i % 10) * (pc_info_rect.bottom - name_rect.bottom)/30);
	}
	
	// Initialize status_rect
	status_rect.top = pc_skills_rect[9].bottom + 5;
	status_rect.left = pc_info_rect.left + 1;
	status_rect.right = pc_race_rect.right;
	status_rect.bottom = status_rect.top + 10;
	// Initialize pc_status_rect
	base_rect.top = status_rect.bottom - 1;
	base_rect.left = status_rect.left + 1;
	base_rect.right = name_rect.right - 1;
	base_rect.bottom = base_rect.top + (pc_info_rect.bottom - status_rect.bottom)/15;
	for(short i = 0; i < 14; i++) {
		pc_status_rect[i] = base_rect;
		pc_status_rect[i].offset((i / 7) * ((name_rect.right)-(name_rect.left)), (i % 7) * 9);
	}
	pc_status_rect[14] = base_rect;
	pc_status_rect[14].offset(name_rect.right - name_rect.left, -9);
	
	// Initialize traits_rect
	traits_rect.top = pc_status_rect[6].bottom + 5;
	traits_rect.left = pc_info_rect.left + 1;
	traits_rect.right = pc_race_rect.right;
	traits_rect.bottom = traits_rect.top + 10;
	// Initialize pc_traits_rect
	base_rect.top = traits_rect.bottom - 1;
	base_rect.left = traits_rect.left + 1;
	base_rect.right = name_rect.right - 1;
	base_rect.bottom = base_rect.top + 10;
	for(short i = 0; i < 16; i++) {
		pc_traits_rect[i] = base_rect;
		pc_traits_rect[i].offset((i / 8) * ((name_rect.right)-(name_rect.left)), (i % 8) * 9);
	}
	pc_traits_rect[16] = base_rect;
	pc_traits_rect[16].offset(name_rect.right - name_rect.left, -9);
	
	item_string_rects[0][0].top = pc_info_rect.top + 3;
	item_string_rects[0][0].left = pc_info_rect.left + (pc_info_rect.right - pc_info_rect.left)/2 + 2;
	item_string_rects[0][0].right = pc_info_rect.right;
	item_string_rects[0][0].bottom = item_string_rects[0][0].top + 12;
	for(short i = 1; i < 24; i++) {
		item_string_rects[i][0] = item_string_rects[0][0];
		item_string_rects[i][0].offset(0,13 * i);
	}
	for(short i = 0; i < 24; i++) {
		item_string_rects[i][1] = item_string_rects[i][0];
		item_string_rects[i][1].right -= 14;
		item_string_rects[i][1].left = item_string_rects[i][1].right - 14;
		item_string_rects[i][2] = item_string_rects[i][0];
		item_string_rects[i][2].left = item_string_rects[i][2].right - 14;
	}
	
}

void Set_up_win () {
	// Preload the main PC editor interface images
	ResMgr::get<ImageRsrc>("pcedtitle");
	ResMgr::get<ImageRsrc>("icon");
	ResMgr::get<ImageRsrc>("invenbtns");
	ResMgr::get<ImageRsrc>("staticons");
	ResMgr::get<ImageRsrc>("dlogpics");
	ResMgr::get<ImageRsrc>("pcedbuttons");
	ResMgr::get<ImageRsrc>("pcs");
}

static void draw_main_screen();
static void display_party();
static void draw_items();

void redraw_screen() {
	draw_main_screen();
	display_party();
	draw_items();
	mainPtr.display();
}

static void frame_dlog_rect(sf::RenderWindow& target, rectangle rect) {
	// dk_gray had a 0..65535 component of 12287, and med_gray had a 0..65535 component of 24574
	static sf::Color dk_gray = {48,48,48},med_gray = {96,96,96};
	rectangle ul_rect = rect;
	
	rect.inset(-2,-2);
	ul_rect = rect;
	ul_rect.right -= 1;
	ul_rect.bottom -= 1;
	
	frame_rect(target, rect, med_gray);
	clip_rect(target, ul_rect);
	frame_rect(target, rect, dk_gray);
	undo_clip(target);
}

void draw_main_screen() {
	rectangle source_rect,dest_rec,dest_rect;
	rectangle reg_rect;
	
	tileImage(mainPtr,whole_win_rect,bg[12]); // fill whole window with background texture
	
	sf::Texture& icon_gworld = *ResMgr::get<ImageRsrc>("icon");
	dest_rec = source_rect = rectangle(icon_gworld);
	dest_rec.offset(23, 16);
	rect_draw_some_item(icon_gworld,source_rect,mainPtr,dest_rec);
	
	sf::Texture& title_gworld = *ResMgr::get<ImageRsrc>("pcedtitle");
	dest_rec = source_rect = rectangle(title_gworld);
	dest_rec.offset(66, 0);
	rect_draw_some_item(title_gworld,source_rect,mainPtr,dest_rec,sf::BlendAlpha);
	
	TextStyle style;
	style.lineHeight = 10;
	if(!file_in_mem.empty()) {
		dest_rect = dest_rec;
		dest_rect.left = 20;
		dest_rect.top = dest_rect.bottom - 10;
		dest_rect.bottom = dest_rect.top + 12;
		style.pointSize = 12;
		style.underline = true;
		win_draw_string(mainPtr,dest_rect,"Characters",eTextMode::WRAP,style);
		style.underline = false;
		style.pointSize = 10;
	}
	
	frame_dlog_rect(mainPtr,pc_info_rect); // draw the frame
	
	style.colour = sf::Color::Black;
	dest_rect = pc_area_buttons[5][0];
	dest_rect.right = whole_win_rect.right - 30; //What is this for? Commenting it out has no effect.
	dest_rect.left += 60;
	dest_rect.offset(0,21);
	if(!file_in_mem.empty())
		win_draw_string(mainPtr,dest_rect,"Click on character to edit it.",eTextMode::WRAP,style);
	else
		win_draw_string(mainPtr,dest_rect,"Select Open from File menu.",eTextMode::WRAP,style);
	if(!file_in_mem.empty() && party_in_scen && !scen_items_loaded){
		dest_rect.offset(200,0);
		win_draw_string(mainPtr,dest_rect,"Warning: Scenario item data could not be loaded.",eTextMode::WRAP,style);
		dest_rect.offset(-200,0);
	}
	dest_rect.offset(0,14);
	if(!file_in_mem.empty())
		win_draw_string(mainPtr,dest_rect,"Press 'I' button to identify item, and 'D' button to drop item.",eTextMode::WRAP,style);
	style.pointSize = 12;
	dest_rect.offset(0,16);
	if(!file_in_mem.empty())
		win_draw_string(mainPtr,dest_rect,"Back up save file before editing it!",eTextMode::WRAP,style);
	style.pointSize = 10;
	style.font = FONT_PLAIN;
	std::ostringstream sout;
	sout << "Created in 1997 by Spiderweb Software, Inc. v" << oboeVersionString();
#if defined(GIT_REVISION) && defined(GIT_TAG_REVISION)
	if(strcmp(GIT_REVISION, GIT_TAG_REVISION) != 0) {
		sout << " [" << GIT_REVISION << "]";
	}
#endif
	std::string copyright = sout.str();
	//dest_rect.offset(270,0);
	dest_rect.right = mainPtr.getSize().x - 5;
	dest_rect.left = dest_rect.right - string_length(copyright, style) - 5;
	win_draw_string(mainPtr,dest_rect,copyright,eTextMode::WRAP,style);
	
	
	reg_rect = whole_win_rect;
	reg_rect.left = reg_rect.right - 170;
	reg_rect.top += 8;
	reg_rect.right -= 3;
	
}

// TODO: Not quite sure what the first parameter is for
void do_button_action(short /*which_pc*/,short which_button) {
	
	current_pressed_button = which_button;
	redraw_screen();
	play_sound(34);
	sf::sleep(time_in_ticks(10));
	current_pressed_button = -1;
	redraw_screen();
}

//short clear_first; // 0 - redraw over, 1 - don't redraw over
void draw_items() {
	rectangle d_from = {12,28,24,42},i_from = {12,42,24,56},dest_rect;
	
	if(file_in_mem.empty())  // save file loaded
		return;
	
	dest_rect = item_string_rects[0][0];
	dest_rect.bottom += 3;
	dest_rect.offset(0,-15);
	
	TextStyle style;
	style.pointSize = 12;
	win_draw_string(mainPtr,dest_rect,univ.party[current_active_pc].name + "'s Items:",eTextMode::LEFT_TOP,style);
	
	if(univ.party[current_active_pc].main_status != eMainStatus::ALIVE){
		frame_dlog_rect(mainPtr,pc_info_rect); // re draw entire frame
		frame_dlog_rect(mainPtr,info_area_rect); // draw the frame
		frame_dlog_rect(mainPtr,pc_race_rect); // draw the frame
		frame_dlog_rect(mainPtr,name_rect); // draw the frame
		return; // If PC is dead, it has no items
	}
	sf::Texture& invenbtn_gworld = *ResMgr::get<ImageRsrc>("invenbtns");
	for(short i = 0; i < univ.party[current_active_pc].items.size(); i++) // Loop through items and draw each
		if(univ.party[current_active_pc].items[i].variety != eItemType::NO_ITEM) { // i.e. does item exist
			std::string to_draw = std::to_string(i + 1) + ". ";
			if(!univ.party[current_active_pc].items[i].ident)
				to_draw += univ.party[current_active_pc].items[i].name;
			else {
				to_draw += univ.party[current_active_pc].items[i].full_name;
				if(univ.party[current_active_pc].items[i].charges > 0)
					to_draw += " (" + std::to_string(univ.party[current_active_pc].items[i].charges) + ")";
			}
			
			TextStyle style;
			style.lineHeight = 10;
			win_draw_string(mainPtr,item_string_rects[i][0],to_draw,eTextMode::LEFT_TOP,style);
			
			// Draw id/drop buttons
			rect_draw_some_item(invenbtn_gworld,d_from,mainPtr,item_string_rects[i][1],sf::BlendAlpha);
			rect_draw_some_item(invenbtn_gworld,i_from,mainPtr,item_string_rects[i][2],sf::BlendAlpha);
		}
	frame_dlog_rect(mainPtr,pc_info_rect); // re draw entire frame
	frame_dlog_rect(mainPtr,name_rect); // draw the frame
	frame_dlog_rect(mainPtr,pc_race_rect); // draw the frame
	frame_dlog_rect(mainPtr,info_area_rect); // draw the frame
	
}

//short mode; // 0 - 5 this pc, 6 - all
//short clear_first; // 1 - redraw over what's already there, 0 - don't redraw over
void display_party() {
	short string_num, cur_rect=0;
	rectangle from_rect,no_party_rect,temp_rect;
	
	TextStyle style;
	style.lineHeight = 10;
	if(file_in_mem.empty()) { // what if no party loaded?
		no_party_rect=pc_info_rect;
		no_party_rect.top+=5;
		no_party_rect.left+=5;
		win_draw_string(mainPtr,no_party_rect,"No party loaded.",eTextMode::WRAP,style);
	}
	else {
		sf::Texture& buttons_gworld = *ResMgr::get<ImageRsrc>("pcedbuttons");
		from_rect = pc_info_rect;
		from_rect.top = from_rect.bottom - 11;
		if(!party_in_scen)
			win_draw_string(mainPtr,from_rect,"Party not in a scenario.",eTextMode::WRAP,style);
		else
			win_draw_string(mainPtr,from_rect,"Party is in a scenario (day " + std::to_string(1 + univ.party.age / 3700) + ").",eTextMode::WRAP,style);
		for(short i = 0; i < 6; i++) {
			if(i == current_active_pc) // active pc is drawn in blue
				fill_rect(mainPtr, pc_area_buttons[i][0], sf::Color::Blue);
			else fill_rect(mainPtr, pc_area_buttons[i][0], sf::Color::Black);
			
			from_rect = (current_pressed_button == i) ? ed_buttons_from[1] : ed_buttons_from[0];
			
			rect_draw_some_item(buttons_gworld,from_rect,mainPtr,pc_area_buttons[i][0], sf::BlendAdd);
			
			if(univ.party[i].main_status != eMainStatus::ABSENT) { // PC exists?
				// draw PC graphic
				pic_num_t pic = univ.party[i].which_graphic;
				sf::Texture* from_gw;
				if(pic >= 1000) {
					bool isParty = pic >= 10000;
					pic_num_t need_pic = pic % 1000;
					graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(need_pic, isParty);
				} else if(pic >= 100) {
					// Note that we assume it's a 1x1 graphic.
					// PCs can't be larger than that, but we leave it to the scenario designer to avoid assigning larger graphics.
					pic_num_t need_pic = pic - 100;
					pic_num_t picture_wanted = m_pic_index[need_pic].i % 20;
					from_rect = calc_rect(2 * (picture_wanted / 10), picture_wanted % 10);
					int which_sheet = m_pic_index[need_pic].i / 20;
					from_gw = ResMgr::get<ImageRsrc>("monst" + std::to_string(1 + which_sheet)).get();
				} else {
					from_rect = calc_rect(2 * (pic / 8), pic % 8);
					from_gw = ResMgr::get<ImageRsrc>("pcs").get();
				}
				rect_draw_some_item(*from_gw,from_rect,mainPtr,pc_area_buttons[i][1],sf::BlendAlpha);
				
				// draw name
				style.pointSize = 9;
				if( (univ.party[i].name.length()) >= 10) {
					style.font = FONT_PLAIN;
					style.pointSize = 6;
				}
				
				style.colour = sf::Color::White;
				win_draw_string(mainPtr,pc_area_buttons[i][2],univ.party[i].name,eTextMode::CENTRE,style);
				style.font = FONT_BOLD;
				style.pointSize = 10;
				
				if(i == current_active_pc){
					if( (univ.party[i].name.length()) > 12)
						style.pointSize = 8;
					style.colour = sf::Color::Black;
					win_draw_string(mainPtr,name_rect,univ.party[i].name,eTextMode::CENTRE,style);
					style.pointSize = 10;
					
				}
				switch(univ.party[i].main_status) {
						// draw statistics
					case eMainStatus::ALIVE:
						if(i == current_active_pc) {
							// Draw in race
							std::string race_str = "Unknown";
							if(univ.party[i].race != eRace::UNKNOWN)
								race_str = get_str("traits", int(univ.party[i].race) * 2 + 35);
							win_draw_string(mainPtr,pc_race_rect,race_str,eTextMode::CENTRE,style);
							// Draw in skills
							
							win_draw_string(mainPtr,skill_rect,"Skills:",eTextMode::WRAP,style);
							std::ostringstream to_draw;
							to_draw << "Hp: " << univ.party[i].cur_health << '/' << univ.party[i].max_health;
							to_draw << "  Sp: " << univ.party[i].cur_sp << '/' << univ.party[i].max_sp;
							win_draw_string(mainPtr,hp_sp_rect,to_draw.str(),eTextMode::WRAP,style);
							
							
							style.pointSize = 9;
							style.font = FONT_PLAIN;
							style.lineHeight = 9;
							string_num=1;
							for(short k = 0; k < 19 ; ++k) {
								temp_rect = pc_skills_rect[k];
								temp_rect.left = pc_skills_rect[k].left + 80;
								if(k < 10) temp_rect.left += 4;
								else temp_rect.left -= 2;
								
								win_draw_string(mainPtr,pc_skills_rect[k],get_str("skills",string_num),eTextMode::WRAP,style);
								
								eSkill skill = eSkill(k);
								win_draw_string(mainPtr,temp_rect,std::to_string(univ.party[i].skills[skill]),eTextMode::WRAP,style);
								string_num+=2;
							}
							style.lineHeight = 10;
							//end skills
							
							// Write in pc Status
							style.pointSize = 10;
							style.font = FONT_BOLD;
							win_draw_string(mainPtr,status_rect,"Status:",eTextMode::WRAP,style);
							
							style.pointSize = 9;
							style.font = FONT_PLAIN;
							style.lineHeight = 9;
							
							if(univ.party[i].status[eStatus::POISONED_WEAPON] > 0)
								if(cur_rect <= 14) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Poisoned Weap.",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::BLESS_CURSE] > 0)
								if(cur_rect <= 14) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Blessed",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::BLESS_CURSE] < 0)
								if(cur_rect <= 14) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Cursed",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::POISON] > 0)
								if(cur_rect <= 14) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Poisoned",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::HASTE_SLOW] > 0)
								if(cur_rect <= 14) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Hasted",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::HASTE_SLOW] < 0)
								if(cur_rect <= 14) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Slowed",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::INVULNERABLE] > 0)
								if(cur_rect <= 14) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Invulnerable",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::MAGIC_RESISTANCE] > 0)
								if(cur_rect <= 14) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Magic Resistant",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::MAGIC_RESISTANCE] < 0)
								if(cur_rect <= 14) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Magic Vulnerable",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::WEBS] > 0)
								if(cur_rect <= 14) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Webbed",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::DISEASE] > 0)
								if(cur_rect <= 14) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Diseased",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::INVISIBLE] > 0)
								if(cur_rect <= 14) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Sanctuary",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::DUMB] > 0)
								if(cur_rect <= 14) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Dumbfounded",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::DUMB] < 0)
								if(cur_rect <= 14) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Enlightened",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::MARTYRS_SHIELD] > 0)
								if(cur_rect <= 14) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Martyr's Shield",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::ASLEEP] > 0)
								if(cur_rect <= 14) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Asleep",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::ASLEEP] < 0)
								if(cur_rect <= 14) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Hyperactive",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::PARALYZED] > 0)
								if(cur_rect <= 14) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Paralyzed",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::ACID] > 0)
								if(cur_rect <= 14) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Acid",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::FORCECAGE] > 0)
								if(cur_rect <= 14) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Forcecage",eTextMode::WRAP,style);
									cur_rect++;
								}
							style.lineHeight = 10;
							//end pc status section
							
							// Write in Traits
							style.pointSize = 10;
							style.font = FONT_BOLD;
							win_draw_string(mainPtr,traits_rect,"Traits:",eTextMode::WRAP,style);
							//for(short k = 0 ; k < 16; k++)
							//frame_dlog_rect(GetWindowPort(mainPtr),pc_traits_rect[k],0);
							style.pointSize = 9;
							style.font = FONT_PLAIN;
							style.lineHeight = 9;
							
							cur_rect=0;
							if(univ.party[i].traits[eTrait::TOUGHNESS])
								if(cur_rect <= 16) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Toughness",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::MAGICALLY_APT])
								if(cur_rect <= 16) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Magically Apt",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::AMBIDEXTROUS])
								if(cur_rect <= 16) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Ambidextrous",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::NIMBLE])
								if(cur_rect <= 16) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Nimble Fingers",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::CAVE_LORE])
								if(cur_rect <= 16) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Cave Lore",eTextMode::WRAP,style);
									cur_rect++;
								}
							
							if(univ.party[i].traits[eTrait::WOODSMAN])
								if(cur_rect <= 16) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Woodsman",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::GOOD_CONST])
								if(cur_rect <= 16) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Good Constitution",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::HIGHLY_ALERT])
								if(cur_rect <= 16) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Highly Alert",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::STRENGTH])
								if(cur_rect <= 16) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Exceptional Str.",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::RECUPERATION])
								if(cur_rect <= 16) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Recuperation",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::SLUGGISH])
								if(cur_rect <= 16) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Sluggish",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::MAGICALLY_INEPT])
								if(cur_rect <= 16) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Magically Inept",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::FRAIL])
								if(cur_rect <= 16) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Frail",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::CHRONIC_DISEASE])
								if(cur_rect <= 16) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Chronic Disease",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::BAD_BACK])
								if(cur_rect <= 16) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Bad Back",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::PACIFIST])
								if(cur_rect <= 16) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Pacifist",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::ANAMA])
								if(cur_rect <= 16) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Anama Member",eTextMode::WRAP,style);
									cur_rect++;
								}
							style.lineHeight = 10;
							//end traits
						}
						
						style.colour = sf::Color::White;
						style.pointSize = 9;
						style.font = FONT_PLAIN;
						win_draw_string(mainPtr,pc_area_buttons[i][3],"Alive ",eTextMode::CENTRE,style);
						style.font = FONT_BOLD;
						style.pointSize = 10;
						break;
					case eMainStatus::DEAD:
						style.colour = sf::Color::White;
						style.pointSize = 9;
						style.font = FONT_PLAIN;
						win_draw_string(mainPtr,pc_area_buttons[i][3],"Dead ",eTextMode::CENTRE,style);
						style.font = FONT_BOLD;
						style.pointSize = 10;
						break;
					case eMainStatus::DUST:
						style.colour = sf::Color::White;
						style.pointSize = 9;
						style.font = FONT_PLAIN;
						win_draw_string(mainPtr,pc_area_buttons[i][3],"Dust ",eTextMode::CENTRE,style);
						style.font = FONT_BOLD;
						style.pointSize = 10;
						break;
					case eMainStatus::STONE:
						style.colour = sf::Color::White;
						style.pointSize = 9;
						style.font = FONT_PLAIN;
						win_draw_string(mainPtr,pc_area_buttons[i][3],"Stone ",eTextMode::CENTRE,style);
						style.font = FONT_BOLD;
						style.pointSize = 10;
						break;
					case eMainStatus::FLED:
						style.colour = sf::Color::White;
						style.pointSize = 9;
						style.font = FONT_PLAIN;
						win_draw_string(mainPtr,pc_area_buttons[i][3],"Fled ",eTextMode::CENTRE,style);
						style.font = FONT_BOLD;
						style.pointSize = 10;
						break;
					case eMainStatus::SURFACE:
						style.colour = sf::Color::White;
						style.pointSize = 9;
						style.font = FONT_PLAIN;
						win_draw_string(mainPtr,pc_area_buttons[i][3],"Surface ",eTextMode::CENTRE,style);
						style.font = FONT_BOLD;
						style.pointSize = 10;
						break;
					default:
						style.colour = sf::Color::White;
						style.pointSize = 9;
						style.font = FONT_PLAIN;
						win_draw_string(mainPtr,pc_area_buttons[i][3],"Absent ",eTextMode::CENTRE,style);
						style.font = FONT_BOLD;
						style.pointSize = 10;
						break;
				}
			}
			
		} // Closes the for i=6 loop
		
		style.lineHeight = 16;
		for(short i = 0; i < 5; i++) {
			from_rect = (current_pressed_button == i + 10) ? ed_buttons_from[1] : ed_buttons_from[0];
			rect_draw_some_item(buttons_gworld,from_rect,mainPtr,edit_rect[i]);
			style.colour = sf::Color::White;
			rectangle dest_rect = edit_rect[i];
			switch(i) {
				case 0:
					dest_rect.top += dest_rect.height() / 2 - style.lineHeight;
					win_draw_string(mainPtr,dest_rect,"Mage",eTextMode::CENTRE,style);
					break;
				case 1:
					dest_rect.top += dest_rect.height() / 2 - style.lineHeight;
					win_draw_string(mainPtr,dest_rect,"Priest",eTextMode::CENTRE,style);
					break;
				case 2:
					dest_rect.top += dest_rect.height() / 2 - style.lineHeight / 2;
					win_draw_string(mainPtr,dest_rect,"Traits",eTextMode::CENTRE,style);
					break;
				case 3:
					dest_rect.top += dest_rect.height() / 2 - style.lineHeight / 2;
					win_draw_string(mainPtr,dest_rect,"Skills",eTextMode::CENTRE,style);
					break;
				case 4:
					dest_rect.top += dest_rect.height() / 2 - style.lineHeight / 2;
					win_draw_string(mainPtr,dest_rect,"XP",eTextMode::CENTRE,style);
					break;
				default:
					break;
			}
			dest_rect.top -= style.lineHeight;
			win_draw_string(mainPtr,dest_rect,"Edit",eTextMode::CENTRE,style);
			if(i < 2) {
				dest_rect.top += style.lineHeight * 2;
				win_draw_string(mainPtr,dest_rect,"Spells",eTextMode::CENTRE,style);
			}
		}
		style.colour = sf::Color::Black;
		
		rectangle dest_rect = name_rect;
		dest_rect.offset(0,-14);
		std::ostringstream to_draw;
		to_draw << " Gold: " << std::setw(0) << univ.party.gold;
		win_draw_string(mainPtr,dest_rect,to_draw.str(),eTextMode::WRAP,style);
		dest_rect = pc_race_rect;
		dest_rect.offset(0,-14);
		to_draw.str("");
		to_draw << " Food: " << univ.party.food;
		win_draw_string(mainPtr,dest_rect,to_draw.str(),eTextMode::WRAP,style);
	}
}
