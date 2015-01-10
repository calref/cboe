
#include <iomanip>
#include "pc.global.h"
#include "classes.h"
#include "pc.graphics.h"
#include "pc.editors.h"
#include "pc.action.h"
#include "soundtool.hpp"
#include "graphtool.hpp"
#include "dlogutil.hpp"
#include "restypes.hpp"
#include "message.hpp"
#include "mathutil.hpp"

extern cUniverse univ;

extern sf::RenderWindow mainPtr;
extern bool play_sounds,party_in_scen,scen_items_loaded;
extern fs::path file_in_mem;

extern short store_flags[3];
extern sf::Texture button_num_gworld,bg_gworld;
extern short current_active_pc;
//extern bool ed_reg;

//extern long register_flag,stored_key;
//extern long ed_flag,ed_key;

sf::Texture title_gworld,pc_gworld,dlogpics_gworld;
sf::Texture buttons_gworld,invenbtn_gworld,status_gworld;
//GWorldPtr race_dark,train_dark,items_dark,spells_dark;
//GWorldPtr race_light,train_light,items_light,spells_light;
rectangle whole_win_rect = {0,0,440,590};
rectangle title_from = {0,0,70,380};
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
extern rectangle pc_status_rect[10];
extern rectangle traits_rect;
extern rectangle pc_traits_rect[16];
extern rectangle edit_rect[5][2];

short store_str1a;
short store_str1b;
short store_str2a;
short store_str2b;
short store_which_string_dlog;
short store_page_on,store_num_i;
// TODO: The duplication of rectangle here shouldn't be necessary...
rectangle ed_buttons_from[2] = {rectangle{0,0,57,57},rectangle{0,57,57,114}};
short current_pressed_button = -1;
cCustomGraphics spec_scen_g; // not actually needed; just here to silence compiler because it's referenced in fileio.h
// (actually, it WILL be needed eventually; the same is true about most of the rest of these.)
sf::Texture items_gworld,tiny_obj_gworld,fields_gworld,roads_gworld,boom_gworld,missiles_gworld;
sf::Texture monst_gworld[NUM_MONST_SHEETS],terrain_gworld[NUM_TER_SHEETS],anim_gworld,talkfaces_gworld;
sf::Texture vehicle_gworld, small_ter_gworld;

void init_main_buttons() {
	
	short i;
	int	indent = 0, indent2 = 0;
	
	rectangle base_rect;
	
	//whole_win_rect = mainPtr->portRect;
	//Initialize pc_info_rect in center
	pc_info_rect= whole_win_rect;
	pc_info_rect.inset(100,100);
	pc_info_rect.bottom+=52;
	pc_info_rect.top-=25;
	pc_info_rect.right+=5;
	name_rect.left = pc_info_rect.left;
	name_rect.right = pc_info_rect.left + 100;
	name_rect.bottom = pc_info_rect.top  + 15;
	name_rect.top = pc_info_rect.top;
	
	//Initialize pc_area_buttons
	pc_area_buttons[0][0].top=pc_info_rect.top;
	pc_area_buttons[0][0].bottom=pc_area_buttons[0][0].top + 56;
	
	for(i=0; i<6; i++) {
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
	//Initialize the edit_rect buttons
	edit_rect[0][0].top = pc_info_rect.top;
	for(i = 0; i < 5; i++) {
		if(i >= 2)
			indent = 5;
		else
			indent = 0;
		if( i == 4)
			indent2 = 1;
		edit_rect[i][0].top = edit_rect[0][0].top + 66*i;
		edit_rect[i][0].bottom = edit_rect[i][0].top + 53;
	 	edit_rect[i][0].left = 510;
		edit_rect[i][0].right = edit_rect[i][0].left + 53;
		edit_rect[i][1].top = edit_rect[i][0].top + 11 + indent;
		edit_rect[i][1].bottom = edit_rect[i][0].bottom - 11 - indent;
		edit_rect[i][1].right = edit_rect[i][0].right - 8 +indent2;
		edit_rect[i][1].left = edit_rect[i][0].left + 8 + indent2;
	}
	
	
	
//	name_rect.left = pc_info_rect.left;
//	name_rect.right = pc_info_rect.left + (pc_info_rect.right - pc_info_rect.left)/4;
//	name_rect.bottom = pc_info_rect.top  + 15;
//	name_rect.top = pc_info_rect.top;
	
//	if(name_rect.right == name_rect.left) {
//		SysBeep(50); SysBeep(50);
//		}
	//Initialize pc_race_rect
	pc_race_rect.top = pc_info_rect.top;
	pc_race_rect.bottom = name_rect.bottom;
	pc_race_rect.left = name_rect.right;
	pc_race_rect.right = pc_info_rect.left + (pc_info_rect.right - pc_info_rect.left)/2;
	
	//initialize info_area_rect
	info_area_rect.top = pc_info_rect.top;
	info_area_rect.left = pc_info_rect.left;
	info_area_rect.right = pc_race_rect.right;
	info_area_rect.bottom = pc_info_rect.bottom;
/*	if(name_rect.right == name_rect.left) {
		SysBeep(50);
	} */
	
	// Initialize hp_sp_rect
	hp_sp_rect.top = name_rect.bottom + 1;
	hp_sp_rect.left = pc_info_rect.left + 1;
	hp_sp_rect.right = pc_race_rect.right;
	hp_sp_rect.bottom = hp_sp_rect.top + 12;
	// Initialize skill_rect
	skill_rect.top = hp_sp_rect.bottom + 2;
	skill_rect.left = pc_info_rect.left + 1;
	skill_rect.right = pc_race_rect.right;
	skill_rect.bottom = skill_rect.top + 12;
	
	//if(name_rect.right == name_rect.left) {
	//	SysBeep(50); SysBeep(50);
	//}
	//Initialize skills_rect
	base_rect.top = skill_rect.bottom + 1;
	base_rect.left = skill_rect.left + 1;
	base_rect.right = name_rect.right - 1;
	base_rect.bottom = base_rect.top + (pc_info_rect.bottom - skill_rect.bottom)/30;
	
	for(i = 0; i < 19; i++) {
		pc_skills_rect[i] = base_rect;
		pc_skills_rect[i].offset((i / 10) * ((name_rect.right)-(name_rect.left)), (i % 10) * (pc_info_rect.bottom - name_rect.bottom)/30);
	}
	
	//Initialize status_rect
	status_rect.top = pc_skills_rect[9].bottom + 5;
	status_rect.left = pc_info_rect.left + 1;
	status_rect.right = pc_race_rect.right;
	status_rect.bottom = status_rect.top + 12;
	//Initialize pc_status_rect
	base_rect.top = status_rect.bottom + 1;
	base_rect.left = status_rect.left + 1;
	base_rect.right = name_rect.right - 1;
	base_rect.bottom = base_rect.top + (pc_info_rect.bottom - status_rect.bottom)/15;
	for(i = 0; i < 10; i++) {
		pc_status_rect[i] = base_rect;
		pc_status_rect[i].offset((i / 5) * ((name_rect.right)-(name_rect.left)), (i % 5) * (pc_info_rect.bottom - status_rect.bottom)/15);
	}
	//Initialize traits_rect
	traits_rect.top = pc_status_rect[4].bottom + 5;
	traits_rect.left = pc_info_rect.left + 1;
	traits_rect.right = pc_race_rect.right;
	traits_rect.bottom = traits_rect.top + 12;
	//Initialize pc_traits_rect
	base_rect.top = traits_rect.bottom - 1;
	base_rect.left = traits_rect.left + 1;
	base_rect.right = name_rect.right - 1;
	base_rect.bottom = base_rect.top + 10;
	for(i = 0; i < 16; i++) {
		pc_traits_rect[i] = base_rect;
		pc_traits_rect[i].offset((i / 8) * ((name_rect.right)-(name_rect.left)), (i % 8) * 9);
	}
	
	
	item_string_rects[0][0].top = pc_info_rect.top + 3;
	item_string_rects[0][0].left = pc_info_rect.left + (pc_info_rect.right - pc_info_rect.left)/2;
	item_string_rects[0][0].right = pc_info_rect.right;
	item_string_rects[0][0].bottom = item_string_rects[0][0].top + 12;
	for(i = 1; i < 24; i++) {
		item_string_rects[i][0] = item_string_rects[0][0];
		item_string_rects[i][0].offset(0,13 * i);
	}
	for(i = 0; i < 24; i++) {
		item_string_rects[i][1] = item_string_rects[i][0];
		item_string_rects[i][1].right -= 14;
		item_string_rects[i][1].left = item_string_rects[i][1].right - 14;
		item_string_rects[i][2] = item_string_rects[i][0];
		item_string_rects[i][2].left = item_string_rects[i][2].right - 14;
	}
	
}

void Set_up_win () {
	title_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("pcedtitle"));
	invenbtn_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("invenbtns"));
	status_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("staticons"));
	dlogpics_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("dlogpics"));
	buttons_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("pcedbuttons"));
	pc_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("pcs"));
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
	cTextMsg text(target);
	text.setFormat(TXT_FRAME, true);
	text.setFormat(TXT_FRAMESTYLE, 1);
	text.setBounds(rect);
	text.draw();
}

void draw_main_screen() {
	rectangle	source_rect, dest_rec,dest_rect;
	rectangle reg_rect;
	
	tileImage(mainPtr,whole_win_rect,bg[12]); // fill whole window with background texture
	dest_rec = source_rect = title_from; // initializes, to draw title
	// title_from is a rectangle constant
	dest_rec.offset(20,0);
	
	rect_draw_some_item(title_gworld,source_rect,mainPtr,dest_rec,sf::BlendAlpha);
	
	TextStyle style;
	style.lineHeight = 10;
	// TODO: Is this needed?
	if(!file_in_mem.empty()) {
		dest_rect = dest_rec;
		dest_rect.top = dest_rect.bottom - 10;
		dest_rect.bottom = dest_rect.top + 12;
		style.pointSize = 12;
		style.underline = true;
		win_draw_string(mainPtr,dest_rect,"Characters",eTextMode::WRAP,style);
		style.underline = false;
		style.pointSize = 10;
	}
	
	frame_dlog_rect(mainPtr,pc_info_rect); // draw the frame
	//i = pc_info_rect.left-pc_info_rect.right;
	//sprintf((char *)temp_str,"Width of pc_info_rect %d ",
	//	(short) i);
	//win_draw_string(mainPtr,pc_info_rect,temp_str,0,12);
	
	style.colour = sf::Color::Black;
	dest_rect = pc_area_buttons[5][0];
	dest_rect.right = whole_win_rect.right - 30; //What is this for? Commenting it out has no effect.
	dest_rect.left += 60;
	//Off0setRect(&dest_rect,0,45);
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
	dest_rect.offset(0,12);
	if(!file_in_mem.empty())
		win_draw_string(mainPtr,dest_rect,"Press 'I' button to identify item, and 'D' button to drop item.",eTextMode::WRAP,style);
	style.pointSize = 12;
	dest_rect.offset(0,16);
	if(!file_in_mem.empty())
		win_draw_string(mainPtr,dest_rect,"Back up save file before editing it!",eTextMode::WRAP,style);
	style.pointSize = 10;
	style.font = FONT_PLAIN;
	dest_rect.offset(280,0);
	win_draw_string(mainPtr,dest_rect,"Created in 1997 by Spiderweb Software, Inc.",eTextMode::WRAP,style);
	
	
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

//extern rectangle pc_area_buttons[6][6] ; // 0 - whole 1 - pic 2 - name 3 - stat strs 4,5 - later
//extern rectangle item_string_rects[24][4]; // 0 - name 1 - drop  2 - id  3 -
//short clear_first; // 0 - redraw over, 1 - don't redraw over
void draw_items() {
	short i;
	rectangle d_from = {12,28,24,42},i_from = {12,42,24,56},dest_rect;
	
	if(file_in_mem.empty())  // save file loaded
		return;
	
	dest_rect = item_string_rects[0][0];
	dest_rect.bottom += 3;
	dest_rect.offset(0,-14);
	
	// First, draw "Fred's Items:"
	//sprintf((char *)to_draw,"%s items:",univ.party[current_active_pc].name);
	//TextSize(12);
	//ClipRect(&dest_rect);
	//win_draw_string(mainPtr,item_string_rects[0][0],to_draw,0,12);
	//undo_clip();
	//TextSize(10);
	
	if(univ.party[current_active_pc].main_status != eMainStatus::ALIVE){
		frame_dlog_rect(mainPtr,pc_info_rect); // re draw entire frame
		frame_dlog_rect(mainPtr,info_area_rect); // draw the frame
		frame_dlog_rect(mainPtr,pc_race_rect); // draw the frame
		return; // If PC is dead, it has no items
	}
	for(i = 0; i < 24; i++) // Loop through items and draw each
		if(univ.party[current_active_pc].items[i].variety != eItemType::NO_ITEM) { // i.e. does item exist
			std::string to_draw = std::to_string(i + 1) + ". ";
			if(!univ.party[current_active_pc].items[i].ident)
				to_draw += univ.party[current_active_pc].items[i].name;
			else {
				to_draw += univ.party[current_active_pc].items[i].full_name;
				if(univ.party[current_active_pc].items[i].charges > 0)
					to_draw += " (" + std::to_string(univ.party[current_active_pc].items[i].charges) + ")";
			}
			
			//if(i % 2 == 0)
			//	sprintf((char *) to_draw, "%d %d %d %d",
			//		pc_info_rect.left,pc_info_rect.right,pc_info_rect.top,pc_info_rect.bottom);
			//else sprintf((char *) to_draw, "%d %d %d %d",
			//	name_rect.left,name_rect.right,name_rect.top,name_rect.bottom);
			
			TextStyle style;
			style.lineHeight = 10;
			win_draw_string(mainPtr,item_string_rects[i][0],to_draw,eTextMode::WRAP,style);
			
			//Draw id/drop buttons
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
	short i,k,string_num, cur_rect=0;
	rectangle from_base = {0,0,36,28},from_rect,no_party_rect,temp_rect;
	
	TextStyle style;
	style.lineHeight = 10;
	if(file_in_mem.empty()) { // what if no party loaded?
		no_party_rect=pc_info_rect;
		no_party_rect.top+=5;
		no_party_rect.left+=5;
		win_draw_string(mainPtr,no_party_rect,"No party loaded.",eTextMode::WRAP,style);
	}
	else {
		from_rect = pc_info_rect;
		from_rect.top = from_rect.bottom - 14;
		if(!party_in_scen)
			win_draw_string(mainPtr,from_rect,"Party not in a scenario.",eTextMode::WRAP,style);
		else
			win_draw_string(mainPtr,from_rect,"Party is in a scenario (day " + std::to_string(1 + univ.party.age / 3700) + ").",eTextMode::WRAP,style);
		for(i = 0; i < 6; i++) {
			if(i == current_active_pc) // active pc is drawn in blue
				fill_rect(mainPtr, pc_area_buttons[i][0], sf::Color::Blue);
			else fill_rect(mainPtr, pc_area_buttons[i][0], sf::Color::Black);
			
			from_rect = (current_pressed_button == i) ? ed_buttons_from[1] : ed_buttons_from[0];
			
			rect_draw_some_item(buttons_gworld,from_rect,mainPtr,pc_area_buttons[i][0], sf::BlendAdd);
			
			// pc_record_type is the records that contains chaarcters
			// main_status determins 0 - not exist, 1 - alive, OK, 2 - dead, 3 - stoned, 4 - dust
			if(univ.party[i].main_status != eMainStatus::ABSENT) { // PC exists?
				from_rect = from_base;
				// draw PC graphic
				from_rect.offset(56 * (univ.party[i].which_graphic / 8),36 * (univ.party[i].which_graphic % 8));
				rect_draw_some_item(pc_gworld,from_rect,mainPtr,pc_area_buttons[i][1],sf::BlendAlpha);
				
				//frame_dlog_rect(GetWindowPort(mainPtr),pc_area_buttons[i][1],0);
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
							//Draw in race
							if(univ.party[i].race == eRace::HUMAN)
								win_draw_string(mainPtr,pc_race_rect,"Human   ",eTextMode::CENTRE,style);
							if(univ.party[i].race == eRace::NEPHIL)
								win_draw_string(mainPtr,pc_race_rect,"Nephilim   ",eTextMode::CENTRE,style);
							if(univ.party[i].race == eRace::SLITH)
								win_draw_string(mainPtr,pc_race_rect,"Slithzerikai  ",eTextMode::CENTRE,style);
							// TODO: Vahnatai
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
							for( k = 0; k < 19 ; ++k) {
								temp_rect = pc_skills_rect[k];
								temp_rect.left = pc_skills_rect[k].left + 80;
								
								win_draw_string(mainPtr,pc_skills_rect[k],get_str("skills",string_num),eTextMode::WRAP,style);
								
								eSkill skill = eSkill(k);
								win_draw_string(mainPtr,temp_rect,std::to_string(univ.party[i].skills[skill]),eTextMode::WRAP,style);
								//frame_dlog_rect(GetWindowPort(mainPtr),pc_skills_rect[k],0);
								string_num+=2;
							}
							style.lineHeight = 10;
							//end skills
							
							//Write in pc Status
							style.pointSize = 10;
							style.font = FONT_BOLD;
							win_draw_string(mainPtr,status_rect,"Status:",eTextMode::WRAP,style);
							
							style.pointSize = 9;
							style.font = FONT_PLAIN;
							style.lineHeight = 9;
							
							
							//for(k = 0 ; k < 10; k++)
							//frame_dlog_rect(GetWindowPort(mainPtr),pc_status_rect[k],0);
							if(univ.party[i].status[eStatus::POISONED_WEAPON] > 0)
								if(cur_rect <= 9) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Poisoned Weap.",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::BLESS_CURSE] > 0)
								if(cur_rect <= 9) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Blessed",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::BLESS_CURSE] < 0)
								if(cur_rect <= 9) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Cursed",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::POISON] > 0)
								if(cur_rect <= 9) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Poisoned",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::HASTE_SLOW] > 0)
								if(cur_rect <= 9) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Hasted",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::HASTE_SLOW] < 0)
								if(cur_rect <= 9) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Slowed",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::INVULNERABLE] > 0)
								if(cur_rect <= 9) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Invulnerable",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::MAGIC_RESISTANCE] > 0)
								if(cur_rect <= 9) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Magic Resistant",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::WEBS] > 0)
								if(cur_rect <= 9) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Webbed",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::DISEASE] > 0)
								if(cur_rect <= 9) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Diseased",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::INVISIBLE] > 0)
								if(cur_rect <= 9) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Sanctury",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::DUMB] > 0)
								if(cur_rect <= 9) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Dumbfounded",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::MARTYRS_SHIELD] > 0)
								if(cur_rect <= 9) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Martyr's Shield",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::ASLEEP] > 0)
								if(cur_rect <= 9) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Asleep",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::PARALYZED] > 0)
								if(cur_rect <= 9) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Paralyzed",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].status[eStatus::ACID] > 0)
								if(cur_rect <= 9) {
									win_draw_string(mainPtr,pc_status_rect[cur_rect],"Acid",eTextMode::WRAP,style);
									cur_rect++;
								}
							style.lineHeight = 10;
							//end pc status section
							
							//Write in Traits
							style.pointSize = 10;
							style.font = FONT_BOLD;
							win_draw_string(mainPtr,traits_rect,"Traits:",eTextMode::WRAP,style);
							//for(k = 0 ; k < 16; k++)
							//frame_dlog_rect(GetWindowPort(mainPtr),pc_traits_rect[k],0);
							style.pointSize = 9;
							style.font = FONT_PLAIN;
							style.lineHeight = 9;
							
							cur_rect=0;
							if(univ.party[i].traits[eTrait::TOUGHNESS])
								if(cur_rect <= 15) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Toughness",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::MAGICALLY_APT])
								if(cur_rect <= 15) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Magically Apt",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::AMBIDEXTROUS])
								if(cur_rect <= 15) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Ambidextrous",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::NIMBLE])
								if(cur_rect <= 15) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Nimble Fingers",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::CAVE_LORE])
								if(cur_rect <= 15) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Cave Lore",eTextMode::WRAP,style);
									cur_rect++;
								}
							
							if(univ.party[i].traits[eTrait::WOODSMAN])
								if(cur_rect <= 15) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Woodsman",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::GOOD_CONST])
								if(cur_rect <= 15) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Good Constitution",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::HIGHLY_ALERT])
								if(cur_rect <= 15) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Highly Alert",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::STRENGTH])
								if(cur_rect <= 15) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Exceptional Str.",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::RECUPERATION])
								if(cur_rect <= 15) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Recuperation",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::SLUGGISH])
								if(cur_rect <= 15) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Sluggish",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::MAGICALLY_INEPT])
								if(cur_rect <= 15) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Magically Inept",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::FRAIL])
								if(cur_rect <= 15) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Frail",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::CHRONIC_DISEASE])
								if(cur_rect <= 15) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Chronic Disease",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::BAD_BACK])
								if(cur_rect <= 15) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Bad Back",eTextMode::WRAP,style);
									cur_rect++;
								}
							if(univ.party[i].traits[eTrait::PACIFIST])
								if(cur_rect <= 15) {
									win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Pacifist",eTextMode::WRAP,style);
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
				//frame_dlog_rect(GetWindowPort(mainPtr),pc_area_buttons[i][0],0);
				
				
			}
			
		} // Closes the for i=6 loop
		
		for(i = 0; i < 5; i++) {
			//frame_dlog_rect(GetWindowPort(mainPtr),edit_rect[i][0],0);
			//frame_dlog_rect(GetWindowPort(mainPtr),edit_rect[i][1],0);
			from_rect = (current_pressed_button == i + 10) ? ed_buttons_from[1] : ed_buttons_from[0];
			rect_draw_some_item(buttons_gworld,from_rect,mainPtr,edit_rect[i][0]);
			style.colour = sf::Color::White;
			switch(i) {
				case 0:
					win_draw_string(mainPtr,edit_rect[0][1],"  Add|Mage|Spells",eTextMode::WRAP,style);
					break;
				case 1:
					win_draw_string(mainPtr,edit_rect[1][1],"  Add|Priest|Spells",eTextMode::WRAP,style);
					break;
				case 2:
					win_draw_string(mainPtr,edit_rect[2][1]," Edit|Traits",eTextMode::WRAP,style);
					break;
				case 3:
					win_draw_string(mainPtr,edit_rect[3][1]," Edit|Skills",eTextMode::WRAP,style);
					break;
				case 4:
					win_draw_string(mainPtr,edit_rect[4][1]," Edit|   XP",eTextMode::WRAP,style);
					break;
				default:
					break;
			}
		}
		style.colour = sf::Color::Black;
		
		// TODO: Maybe find a better place to print this
		rectangle dest_rect = title_from;
		dest_rect.offset(100,60);
		std::ostringstream to_draw;
		to_draw << " Gold: " << std::setw(0) << univ.party.gold << "Food: " << univ.party.food;
		win_draw_string(mainPtr,dest_rect,to_draw.str(),eTextMode::WRAP,style);
	}
}

void make_cursor_sword() {
	set_cursor(sword_curs);
}

