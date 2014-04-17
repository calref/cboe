//#include <cMemory>
#include <cstdio>
#include <cstring>
#include "pc.global.h"
#include "classes.h"
#include "pc.graphics.h"
#include "pc.editors.h"
#include "pc.action.h"
#include "soundtool.h"
#include "graphtool.h"
#include "dlogutil.h"
#include "restypes.hpp"
#include "message.h"
#include "mathutil.h"

extern std::string get_str(std::string, short);

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
RECT whole_win_rect = {0,0,440,590};
RECT title_from = {0,0,70,380};
extern RECT pc_area_buttons[6][4] ; // 0 - whole 1 - pic 2 - name 3 - stat strs 4,5 - later
extern RECT item_string_rects[24][4]; // 0 - name 1 - drop  2 - id  3 - 
extern RECT pc_info_rect; // Frame that holds a pc's basic info and items
extern RECT name_rect;
extern RECT pc_race_rect;
extern RECT info_area_rect;
extern RECT hp_sp_rect;
extern RECT skill_rect;
extern RECT pc_skills_rect[19];
extern RECT status_rect; 
extern RECT pc_status_rect[10]; 
extern RECT traits_rect;
extern RECT pc_traits_rect[16]; 
extern RECT edit_rect[5][2]; 

short store_str1a;
short store_str1b;
short store_str2a;
short store_str2b;
short store_which_string_dlog;
short store_page_on,store_num_i;
// TODO: The duplication of RECT here shouldn't be necessary...
RECT ed_buttons_from[2] = {RECT{0,0,57,57},RECT{0,57,57,114}};
short current_pressed_button = -1;
bool init_once = false;
sf::Texture spec_scen_g; // not actually needed; just here to silence compiler because it's reference in fileio.h
// (actually, it WILL be needed eventually; the same is true about most of the rest of these.)
sf::Texture items_gworld,tiny_obj_gworld,fields_gworld,roads_gworld,boom_gworld,missiles_gworld;
sf::Texture monst_gworld[NUM_MONST_SHEETS],terrain_gworld[NUM_TER_SHEETS],anim_gworld,talkfaces_gworld;
sf::Texture vehicle_gworld, small_ter_gworld;

void init_main_buttons()
{

	short i;
	int	indent = 0, indent2 = 0;
	
	RECT base_rect;
	
	if (init_once == false) {
		init_once = true;
		TEXT.font = "Silom";
		TEXT.style = sf::Text::Regular;
		TEXT.pointSize = 10;
		}
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
	
	for(i=0; i<6; i++)
	{ 
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

//	if (name_rect.right == name_rect.left) {
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
/*	if (name_rect.right == name_rect.left) {
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
	
	//if (name_rect.right == name_rect.left) {
	//	SysBeep(50); SysBeep(50);
	//	}
	//Initialize skills_rect
	base_rect.top = skill_rect.bottom + 1;
	base_rect.left = skill_rect.left + 1;
	base_rect.right = name_rect.right - 1;
	base_rect.bottom = base_rect.top + (pc_info_rect.bottom - skill_rect.bottom)/30;
	
	for (i = 0; i < 19; i++) {
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
	for (i = 0; i < 10; i++) {
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
	for (i = 0; i < 16; i++) {
		pc_traits_rect[i] = base_rect;
		pc_traits_rect[i].offset((i / 8) * ((name_rect.right)-(name_rect.left)), (i % 8) * 9);
	}


	item_string_rects[0][0].top = pc_info_rect.top + 3;
	item_string_rects[0][0].left = pc_info_rect.left + (pc_info_rect.right - pc_info_rect.left)/2;
	item_string_rects[0][0].right = pc_info_rect.right;
	item_string_rects[0][0].bottom = item_string_rects[0][0].top + 12;
	for (i = 1; i < 24; i++) {
		item_string_rects[i][0] = item_string_rects[0][0];
		item_string_rects[i][0].offset(0,13 * i);
	}
	for (i = 0; i < 24; i++) {
		item_string_rects[i][1] = item_string_rects[i][0];
		item_string_rects[i][1].right -= 14;
		item_string_rects[i][1].left = item_string_rects[i][1].right - 14;
		item_string_rects[i][2] = item_string_rects[i][0];
		item_string_rects[i][2].left = item_string_rects[i][2].right - 14;
	}		

}

void Set_up_win ()
{
	short i;
	title_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("pcedtitle"));
	invenbtn_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("invenbtns"));
	status_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("staticons"));
	dlogpics_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("dlogpics"));
	buttons_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("pcedbuttons"));
	pc_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("pcs"));
}


void redraw_screen()
{
	draw_main_screen();
	display_party(6,1);
	draw_items(1);
	mainPtr.display();
}

static void frame_dlog_rect(sf::RenderWindow& target, RECT rect) {
	cTextMsg text(target);
	text.setFormat(TXT_FRAME, true);
	text.setFormat(TXT_FRAMESTYLE, 1);
	text.setBounds(rect);
	text.draw();
}

void draw_main_screen()
{
	RECT	source_rect, dest_rec,dest_rect;
	RECT reg_rect;

	tileImage(mainPtr,whole_win_rect,bg_gworld,bg[12]); // fill whole window with background texture
	dest_rec = source_rect = title_from; // initializes, to draw title 
		 // title_from is a RECT constant
	dest_rec.offset(20,0);
	
	rect_draw_some_item(title_gworld,source_rect,mainPtr,dest_rec,sf::BlendAlpha);
	
	dest_rect = dest_rec;
	dest_rect.top = dest_rect.bottom;
	dest_rect.bottom = dest_rect.top + 50;
		// initialize rectangle to draw text into
	// TODO: Is this needed?
#if 0
	TEXT.pointSize = 12;
	TEXT.style = sf::Text::Underlined;
		// set the pen
	win_draw_string(mainPtr,dest_rect,"Characters",0,10);
		// This draws a chunk of text on the screen
#endif
	TEXT.pointSize = 10;
	TEXT.style = sf::Text::Regular;
	
	frame_dlog_rect(mainPtr,pc_info_rect); // draw the frame
	//i = pc_info_rect.left-pc_info_rect.right;
	//sprintf((char *)temp_str,"Width of pc_info_rect %d ",
				//(short) i);
	//win_draw_string(mainPtr,pc_info_rect,temp_str,0,12);

	TEXT.colour = sf::Color::Black;
	dest_rect = pc_area_buttons[5][0]; 
	dest_rect.right = whole_win_rect.right - 30; //What is this for? Commenting it out has no effect.
	dest_rect.left += 60;
	//Off0setRect(&dest_rect,0,45);
	dest_rect.offset(0,21);
	if(!file_in_mem.empty())
		win_draw_string(mainPtr,dest_rect,"Click on character to edit it.",0,10);
	else
		win_draw_string(mainPtr,dest_rect,"Select Open from File menu.",0,10);
	if(!file_in_mem.empty() && party_in_scen && !scen_items_loaded){
		dest_rect.offset(200,0);
		win_draw_string(mainPtr,dest_rect,"Warning: Scenario item data could not be loaded.",0,10);
		dest_rect.offset(-200,0);
	}
	dest_rect.offset(0,12);
	if(!file_in_mem.empty())
		win_draw_string(mainPtr,dest_rect,"Press 'I' button to identify item, and 'D' button to drop item.",0,10);
	TEXT.pointSize = 12;
	dest_rect.offset(0,16);
	if(!file_in_mem.empty())
		win_draw_string(mainPtr,dest_rect,"Back up save file before editing it!",0,10);
	TEXT.pointSize = 10;
	TEXT.font = "Geneva";
	dest_rect.offset(280,0);
	win_draw_string(mainPtr,dest_rect,"Created in 1997 by Spiderweb Software, Inc.",0,10);
	TEXT.font = "Silom";
	
	reg_rect = whole_win_rect;
	reg_rect.left = reg_rect.right - 170;
	reg_rect.top += 8;
	reg_rect.right -= 3;
	
}

void do_button_action(short which_pc,short which_button)
{
	unsigned long dummy;

	current_pressed_button = which_button;
	display_party(6,0);
	play_sound(34);
	sf::sleep(time_in_ticks(10));
	current_pressed_button = -1;
	display_party(6,0);
}

//extern RECT pc_area_buttons[6][6] ; // 0 - whole 1 - pic 2 - name 3 - stat strs 4,5 - later
//extern RECT item_string_rects[24][4]; // 0 - name 1 - drop  2 - id  3 - 
void draw_items(short clear_first)
//short clear_first; // 0 - redraw over, 1 - don't redraw over
{
	short i;
	char to_draw[256];
	RECT d_from = {12,28,24,42},i_from = {12,42,24,56},dest_rect;

	if(file_in_mem.empty())  // save file loaded
		return;

	dest_rect = item_string_rects[0][0];
	dest_rect.bottom += 3;
	dest_rect.offset(0,-14);

	// First erase crap there already by painting background texture over it
	if (clear_first == 1) {
		for (i = 0; i < 24; i++)
			tileImage(mainPtr,item_string_rects[i][0],bg_gworld,bg[12]);
		tileImage(mainPtr,dest_rect,bg_gworld,bg[12]);
		}
	
	// First, draw "Fred's Items:"
	//sprintf((char *)to_draw,"%s items:",univ.party[current_active_pc].name);
	//TextSize(12);
	//ClipRect(&dest_rect);
	//win_draw_string(mainPtr,item_string_rects[0][0],to_draw,0,12);
	//undo_clip();
	//TextSize(10);

	if (univ.party[current_active_pc].main_status != 1){
		frame_dlog_rect(mainPtr,pc_info_rect); // re draw entire frame 
		frame_dlog_rect(mainPtr,info_area_rect); // draw the frame
		frame_dlog_rect(mainPtr,pc_race_rect); // draw the frame	
		return; // If PC is dead, it has no items
	}
	for (i = 0; i < 24; i++) // Loop through items and draw each
		if (univ.party[current_active_pc].items[i].variety > 0) { // i.e. does item exist
			strcpy((char *) to_draw, "");
			if (!univ.party[current_active_pc].items[i].ident)
				sprintf((char *) to_draw, "%d. %s  ",i + 1,univ.party[current_active_pc].items[i].name.c_str());
				else if (univ.party[current_active_pc].items[i].charges > 0)
					sprintf((char *) to_draw, "%d. %s (%d)",i + 1,univ.party[current_active_pc].items[i].full_name.c_str(),
					univ.party[current_active_pc].items[i].charges);
				else sprintf((char *) to_draw, "%d. %s ",i + 1,univ.party[current_active_pc].items[i].full_name.c_str());

			//if (i % 2 == 0)
			//	sprintf((char *) to_draw, "%d %d %d %d",
			//	pc_info_rect.left,pc_info_rect.right,pc_info_rect.top,pc_info_rect.bottom);
			//	else sprintf((char *) to_draw, "%d %d %d %d",
			//	name_rect.left,name_rect.right,name_rect.top,name_rect.bottom);

			win_draw_string(mainPtr,item_string_rects[i][0],(char *) to_draw,0,10);

			//Draw id/drop buttons
			rect_draw_some_item(invenbtn_gworld,d_from,mainPtr,item_string_rects[i][1],sf::BlendAlpha);
			rect_draw_some_item(invenbtn_gworld,i_from,mainPtr,item_string_rects[i][2],sf::BlendAlpha);
			}
	frame_dlog_rect(mainPtr,pc_info_rect); // re draw entire frame 
	frame_dlog_rect(mainPtr,name_rect); // draw the frame
	frame_dlog_rect(mainPtr,pc_race_rect); // draw the frame
	frame_dlog_rect(mainPtr,info_area_rect); // draw the frame

}

void display_party(short mode,short clear_first)
//short mode; // 0 - 5 this pc, 6 - all
//short clear_first; // 1 - redraw over what's already there, 0 - don't redraw over
{
	short i,k,string_num, cur_rect=0;
	const char* to_draw;
	const char* skill_value;
	RECT from_base = {0,0,36,28},from_rect,no_party_rect,temp_rect;
	
	// TODO: Is this needed?
#if 0
	// lots of stuff is global. Like ...
	// bool file_in_mem
	// short current_active_pc
	if (clear_first == 1) { // first erase what's already there
		for (i = 0; i < 6; i++)
			tileImage(mainPtr,pc_area_buttons[i][0],bg_gworld,bg[12]);
		tileImage(mainPtr,name_rect,bg_gworld,bg[12]);
		tileImage(mainPtr,pc_race_rect,bg_gworld,bg[12]);
		tileImage(mainPtr,info_area_rect,bg_gworld,bg[12]);
		frame_dlog_rect(mainPtr,pc_info_rect); // re-draw the frame
	}
#endif
	
	if(file_in_mem.empty()) { // what if no party loaded?
		no_party_rect=pc_info_rect;
		no_party_rect.top+=5;
		no_party_rect.left+=5;
		win_draw_string(mainPtr,no_party_rect,"No party loaded.",0,10);
	}
	else {
		from_rect = pc_info_rect;
		from_rect.top = from_rect.bottom - 14;
		if (party_in_scen == false)
			win_draw_string(mainPtr,from_rect,"Party not in a scenario.",0,10);
		else
			win_draw_string(mainPtr,from_rect,"Party is in a scenario.",0,10);
		for (i = 0; i < 6; i++) {
			// TODO: This appears to be expecting a tint?
			if (i == current_active_pc) // active pc is drawn in blue
				TEXT.colour = sf::Color::Blue;
			else TEXT.colour = sf::Color::Black;
			
			from_rect = (current_pressed_button == i) ? ed_buttons_from[1] : ed_buttons_from[0];
			
			if ((current_pressed_button < 0) || (current_pressed_button == i))
				rect_draw_some_item(buttons_gworld,from_rect,mainPtr,pc_area_buttons[i][0]);
			TEXT.colour = sf::Color::Black;
			
			// pc_record_type is the records that contains chaarcters
			// main_status determins 0 - not exist, 1 - alive, OK, 2 - dead, 3 - stoned, 4 - dust
			if (univ.party[i].main_status != 0) { // PC exists?
				from_rect = from_base;
				// draw PC graphic
				from_rect.offset(56 * (univ.party[i].which_graphic / 8),36 * (univ.party[i].which_graphic % 8));
				rect_draw_some_item(pc_gworld,from_rect,mainPtr,pc_area_buttons[i][1],sf::BlendAlpha);
				
				//frame_dlog_rect(GetWindowPort(mainPtr),pc_area_buttons[i][1],0); 
				// draw name
				TEXT.pointSize = 9;
				if( (univ.party[i].name.length()) >= 10) {
					TEXT.font = "Geneva";
					TEXT.pointSize = 6;
					sprintf((char *) to_draw, "%-s  ", (char *) univ.party[i].name.c_str());
				}
				else {
					sprintf((char *) to_draw, "%-s ", (char *) univ.party[i].name.c_str());	
				}
				
				TEXT.colour = sf::Color::White;
				win_draw_string(mainPtr,pc_area_buttons[i][2],to_draw,1,10);
				TEXT.font = "Silom";
				TEXT.pointSize = 10;
				
				if (i == current_active_pc){
					sprintf((char *) to_draw, "%-.18s  ", (char *) univ.party[i].name.c_str());
					if( (univ.party[i].name.length()) > 12)
						TEXT.pointSize = 8;
					TEXT.colour = sf::Color::Black;
					win_draw_string(mainPtr,name_rect,to_draw,1,10);
					TEXT.pointSize = 10;
					
				}
				if ((current_pressed_button < 0) || (current_pressed_button == i))
					switch (univ.party[i].main_status) {
							// draw statistics
						case 1:
							if (i == current_active_pc) {
								//Draw in race
								if (univ.party[i].race == 0)
									win_draw_string(mainPtr,pc_race_rect,"Human   ",1,10);
								if (univ.party[i].race == 1)
									win_draw_string(mainPtr,pc_race_rect,"Nephilim   ",1,10);
								if (univ.party[i].race == 2)
									win_draw_string(mainPtr,pc_race_rect,"Slithzerikai  ",1,10);
								// Draw in skills	
								
								sprintf((char *) to_draw, "Skills:");
								win_draw_string(mainPtr,skill_rect,to_draw,0,10);
								sprintf((char *) to_draw, "Hp: %d/%d  Sp: %d/%d",univ.party[i].cur_health,univ.party[i].max_health,univ.party[i].cur_sp,
										univ.party[i].max_sp);
								win_draw_string(mainPtr,hp_sp_rect,to_draw,0,10);
								
								
								TEXT.pointSize = 9;
								TEXT.font = "Geneva";
								string_num=1;
								for( k = 0; k < 19 ; ++k)
								{
									temp_rect = pc_skills_rect[k];
									temp_rect.left = pc_skills_rect[k].left + 80;
									
									win_draw_string(mainPtr,pc_skills_rect[k],get_str("skills",string_num),0,9);
									
									sprintf((char *) skill_value,"%d",univ.party[i].skills[k]);
									win_draw_string(mainPtr,temp_rect,skill_value,0,9);	
									//frame_dlog_rect(GetWindowPort(mainPtr),pc_skills_rect[k],0);
									string_num+=2;
								}
								//end skills
								
								//Write in pc Status
								TEXT.pointSize = 10;
								TEXT.font = "Silom";
								sprintf((char *) to_draw, "Status:");
								win_draw_string(mainPtr,status_rect,to_draw,0,10);
								
								TEXT.pointSize = 9;
								TEXT.font = "Geneva";
								
								
								
								//for(k = 0 ; k < 10; k++)
								//frame_dlog_rect(GetWindowPort(mainPtr),pc_status_rect[k],0);
								if (univ.party[i].status[0] > 0) 
									if(cur_rect <= 9) {
										win_draw_string(mainPtr,pc_status_rect[cur_rect],"Poisoned Weap.",0,9);
										cur_rect++;
									}
								if (univ.party[i].status[1] > 0) 
									if(cur_rect <= 9) {
										win_draw_string(mainPtr,pc_status_rect[cur_rect],"Blessed",0,9);
										cur_rect++;
									}
									else if(univ.party[i].status[1] < 0)
										if(cur_rect <= 9) {
											win_draw_string(mainPtr,pc_status_rect[cur_rect],"Cursed",0,9);
											cur_rect++;
										}
								if (univ.party[i].status[2] > 0) 
									if(cur_rect <= 9) {
										win_draw_string(mainPtr,pc_status_rect[cur_rect],"Poisoned",0,9);
										cur_rect++;
									}	
								if (univ.party[i].status[3] > 0) 
									if(cur_rect <= 9) {
										win_draw_string(mainPtr,pc_status_rect[cur_rect],"Hasted",0,9);
										cur_rect++;
									}
									else if(univ.party[i].status[3] < 0)
										if(cur_rect <= 9) {
											win_draw_string(mainPtr,pc_status_rect[cur_rect],"Slowed",0,9);
											cur_rect++;
										}
								if (univ.party[i].status[4] > 0) 
									if(cur_rect <= 9) {
										win_draw_string(mainPtr,pc_status_rect[cur_rect],"Invulnerable",0,9);
										cur_rect++;
									}
								if (univ.party[i].status[5] > 0) 
									if(cur_rect <= 9) {
										win_draw_string(mainPtr,pc_status_rect[cur_rect],"Magic Resistant",0,9);
										cur_rect++;
									}
								if (univ.party[i].status[6] > 0) 
									if(cur_rect <= 9) {
										win_draw_string(mainPtr,pc_status_rect[cur_rect],"Webbed",0,9);
										cur_rect++;
									}
								if (univ.party[i].status[7] > 0) 
									if(cur_rect <= 9) {
										win_draw_string(mainPtr,pc_status_rect[cur_rect],"Diseased",0,9);
										cur_rect++;
									}
								if (univ.party[i].status[8] > 0) 
									if(cur_rect <= 9) {
										win_draw_string(mainPtr,pc_status_rect[cur_rect],"Sanctury",0,9);
										cur_rect++;
									}
								if (univ.party[i].status[9] > 0) 
									if(cur_rect <= 9) {
										win_draw_string(mainPtr,pc_status_rect[cur_rect],"Dumbfounded",0,9);
										cur_rect++;
									}
								if (univ.party[i].status[10] > 0) 
									if(cur_rect <= 9) {
										win_draw_string(mainPtr,pc_status_rect[cur_rect],"Martyr's Shield",0,9);
										cur_rect++;
									}
								if (univ.party[i].status[11] > 0) 
									if(cur_rect <= 9) {
										win_draw_string(mainPtr,pc_status_rect[cur_rect],"Asleep",0,9);
										cur_rect++;
									}
								if (univ.party[i].status[12] > 0) 
									if(cur_rect <= 9) {
										win_draw_string(mainPtr,pc_status_rect[cur_rect],"Paralyzed",0,9);
										cur_rect++;
									}
								if (univ.party[i].status[13] > 0) 
									if(cur_rect <= 9) {
										win_draw_string(mainPtr,pc_status_rect[cur_rect],"Acid",0,9);
										cur_rect++;
									}
								//end pc status section
								
								//Write in Traits
								TEXT.pointSize = 10;
								TEXT.font = "Silom";
								sprintf((char *) to_draw, "Traits:");
								win_draw_string(mainPtr,traits_rect,to_draw,0,10);
								//for(k = 0 ; k < 16; k++)
								//frame_dlog_rect(GetWindowPort(mainPtr),pc_traits_rect[k],0);
								TEXT.pointSize = 9;
								TEXT.font = "Geneva";
								
								cur_rect=0;
								if (univ.party[i].traits[0] == 1) 
									if(cur_rect <= 15) {
										win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Toughness",0,9);
										cur_rect++;
									}
								if (univ.party[i].traits[1] == 1) 
									if(cur_rect <= 15) {
										win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Magically Apt",0,9);
										cur_rect++;
									}		
								if (univ.party[i].traits[2] == 1) 
									if(cur_rect <= 15) {
										win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Ambidextrous",0,9);
										cur_rect++;
									}
								if (univ.party[i].traits[3] == 1) 
									if(cur_rect <= 15) {
										win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Nimble Fingers",0,9);
										cur_rect++;
									}
								if (univ.party[i].traits[4] == 1) 
									if(cur_rect <= 15) {
										win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Cave Lore",0,9);
										cur_rect++;
									}
								
								if (univ.party[i].traits[5] == 1) 
									if(cur_rect <= 15) {
										win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Woodsman",0,9);
										cur_rect++;
									}
								if (univ.party[i].traits[6] == 1) 
									if(cur_rect <= 15) {
										win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Good Constitution",0,9);
										cur_rect++;
									}		
								if (univ.party[i].traits[7] == 1) 
									if(cur_rect <= 15) {
										win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Highly Alert",0,9);
										cur_rect++;
									}
								if (univ.party[i].traits[8] == 1) 
									if(cur_rect <= 15) {
										win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Exceptional Str.",0,9);
										cur_rect++;
									}
								if (univ.party[i].traits[9] == 1) 
									if(cur_rect <= 15) {
										win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Recuperation",0,9);
										cur_rect++;
									}
								if (univ.party[i].traits[10] == 1) 
									if(cur_rect <= 15) {
										win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Sluggish",0,9);
										cur_rect++;
									}
								if (univ.party[i].traits[11] == 1) 
									if(cur_rect <= 15) {
										win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Magically Inept",0,9);
										cur_rect++;
									}		
								if (univ.party[i].traits[12] == 1) 
									if(cur_rect <= 15) {
										win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Frail",0,9);
										cur_rect++;
									}
								if (univ.party[i].traits[13] == 1) 
									if(cur_rect <= 15) {
										win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Chronic Disease",0,9);
										cur_rect++;
									}
								if (univ.party[i].traits[14] == 1) 
									if(cur_rect <= 15) {
										win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Bad Back",0,9);
										cur_rect++;
									}
								
								//end traits
							}
							
							TEXT.colour = sf::Color::White;
							TEXT.pointSize = 9;
							TEXT.font = "Geneva";
							win_draw_string(mainPtr,pc_area_buttons[i][3],"Alive ",1,10);
							TEXT.font = "Silom";
							TEXT.pointSize = 10;
							break;
							case 2:
							TEXT.colour = sf::Color::White;
							TEXT.pointSize = 9;
							TEXT.font = "Geneva";
							win_draw_string(mainPtr,pc_area_buttons[i][3],"Dead ",1,10);
							TEXT.font = "Silom";
							TEXT.pointSize = 10;
							break;
							case 3:
							TEXT.colour = sf::Color::White;
							TEXT.pointSize = 9;
							TEXT.font = "Geneva";
							win_draw_string(mainPtr,pc_area_buttons[i][3],"Dust ",1,10);
							TEXT.font = "Silom";
							TEXT.pointSize = 10;
							break;
							case 4:
							TEXT.colour = sf::Color::White;
							TEXT.pointSize = 9;
							TEXT.font = "Geneva";
							win_draw_string(mainPtr,pc_area_buttons[i][3],"Stone ",1,10);
							TEXT.font = "Silom";
							TEXT.pointSize = 10;
							break;
							case 5:
							TEXT.colour = sf::Color::White;
							TEXT.pointSize = 9;
							TEXT.font = "Geneva";
							win_draw_string(mainPtr,pc_area_buttons[i][3],"Fled ",1,10);
							TEXT.font = "Silom";
							TEXT.pointSize = 10;
							break;
							case 6:
							TEXT.colour = sf::Color::White;
							TEXT.pointSize = 9;
							TEXT.font = "Geneva";
							win_draw_string(mainPtr,pc_area_buttons[i][3],"Surface ",1,10);
							TEXT.font = "Silom";
							TEXT.pointSize = 10;
							break;
							default:
							TEXT.colour = sf::Color::White;
							TEXT.pointSize = 9;
							TEXT.font = "Geneva";
							win_draw_string(mainPtr,pc_area_buttons[i][3],"Absent ",1,10);
							TEXT.font = "Silom";
							TEXT.pointSize = 10;
							break;
					}
				//frame_dlog_rect(GetWindowPort(mainPtr),pc_area_buttons[i][0],0); 
				
				
			}
			
		} // Closes the for i=6 loop
		TEXT.colour = sf::Color::Black;
		
		
		
		for(i = 0; i < 5; i++) 
			if ((current_pressed_button < 0) || (current_pressed_button == i + 10)) {	
				if (clear_first == 1) { // first erase what's already there
					tileImage(mainPtr,edit_rect[i][0],bg_gworld,bg[12]);
				}		
				//frame_dlog_rect(GetWindowPort(mainPtr),edit_rect[i][0],0);
				//frame_dlog_rect(GetWindowPort(mainPtr),edit_rect[i][1],0);
				from_rect = (current_pressed_button == i + 10) ? ed_buttons_from[1] : ed_buttons_from[0];
				rect_draw_some_item(buttons_gworld,from_rect,mainPtr,edit_rect[i][0]);
				TEXT.colour = sf::Color::White;
				switch(i) {
					case 0:
						win_draw_string(mainPtr,edit_rect[0][1],"  Add  Mage Spells ",0,10);
						break;
					case 1:
						win_draw_string(mainPtr,edit_rect[1][1],"  Add Priest Spells ",0,10);
						break;
					case 2: 
						win_draw_string(mainPtr,edit_rect[2][1]," Edit  Traits",0,10);
						break;
					case 3:
						win_draw_string(mainPtr,edit_rect[3][1]," Edit  Skills",0,10);
						break;
					case 4:
						win_draw_string(mainPtr,edit_rect[4][1]," Edit   XP",0,10);
						break;
					default:
						break;	
				}		
				TEXT.colour = sf::Color::Black;
				
			}
		//			MoveTo(start_h + 10, start_v + 127);	
		//			sprintf((char *) to_draw, " Gold: %d       Food: %d ",(short) party.gold, (short) party.food);
		//			DrawString(to_draw);
	}
	
	
	TEXT.colour = sf::Color::Black;
}
	
//void rect_draw_some_item (GWorldPtr	src_gworld, RECT	src_rect, GWorldPtr	targ_gworld,RECT targ_rect, 
//char masked,short main_win)
////char	 masked; // if 10 - make AddOver
////short   main_win; // if 2, drawing onto dialog
//{
//	RECT	destrec;
//	PixMapHandle	test1, test2;
//	const BitMap *store_dest;
//	GrafPtr cur_port;
//	RGBColor	store_color;
//
//	if (main_win == 2) {
//		GetBackColor(&store_color);
//		BackColor(whiteColor);
//		}
//		
//	GetPort(&cur_port);	
//	store_dest = GetPortBitMapForCopyBits(cur_port);
//	
//	test1 = GetPortPixMap(src_gworld);
//
//	LockPixels(test1);
//	if (main_win == 0) {
//		test2 = GetPortPixMap(targ_gworld);
//		LockPixels(test2);
//		if (masked == 1) 
//			CopyBits ( (BitMap *) *test1 ,
//					(BitMap *) *test2 ,
//					&src_rect, &targ_rect, 
//					 transparent , NULL);	
//			else CopyBits ( (BitMap *) *test1 ,
//					(BitMap *) *test2 ,
//					&src_rect, &targ_rect, 
//					  (masked == 10) ? addOver : 0, NULL);
//		UnlockPixels(test2);
//		}  
//		else {
//		if (masked == 1) 
//			CopyBits ( (BitMap *) *test1 ,
//					store_dest ,
//					&src_rect, &targ_rect, 
//					 transparent , NULL);
//			else CopyBits ( (BitMap *) *test1 ,
//					store_dest ,
//					&src_rect, &targ_rect, 
//					  (masked == 10) ? addOver : 0, NULL);   
//			}
//	UnlockPixels(test1);
//
//	if (main_win == 2) 
//		RGBBackColor(&store_color);
//}

//short string_length(char *str)
//{
//	short text_len[257];
//	short total_width = 0,i,len;
//	char p_str[256];
//	
//	for (i = 0; i < 257; i++)
//		text_len[i]= 0;
//	
//	strcpy((char *) p_str,str);
//	c2pstr((char*) p_str);
//	MeasureText(256,p_str,text_len);
//	len = strlen((char *)str);
//	
//	for (i = 0; i < 257; i++)
//		if ((text_len[i] > total_width) && (i <= len))
//			total_width = text_len[i];
//	return total_width;
//}

//void char_win_draw_string(GrafPtr dest_window,RECT dest_rect,char *str,short mode,short line_height)
//{
//	char store_s[256];
//	strcpy((char *) store_s,str);
//	win_draw_string( dest_window, dest_rect,store_s, mode, line_height);
//}
//
//// mode: 0 - align up and left, 1 - center on one line
//// str is a c string, 256 characters
//// uses current font
//void win_draw_string(GrafPtr dest_window,RECT dest_rect,Str255 str,short mode,short line_height)
//{
//	GrafPtr old_port;
//	const char* p_str,str_to_draw,str_to_draw2,c_str;
//	char null_s[256] = "                                                                                                                                                                                                                                                              ";
//	short str_len,i;
//	short last_line_break = 0,last_word_break = 0,on_what_line = 0;
//	short text_len[257];
//	short total_width = 0;
//	bool end_loop,force_skip = false;
//	KeyMap key_state;
//	long dummy3;
//	RgnHandle current_clip;
//	short adjust_x = 0,adjust_y = 0;
//	strcpy((char *) p_str,(char *) str);
//	strcpy((char *) c_str,(char *) str);
//	c2pstr((char*) p_str);
//	for (i = 0; i < 257; i++)
//		text_len[i]= 0;
//	MeasureText(256,p_str,text_len);
//	str_len = (short) strlen((char *)str);
//	if (str_len == 0) {
//		return;
//	}
//	GetPort(&old_port);	
//	SetPort(dest_window);
//	
//	//FrameRect(&dest_rect);
//	
//	current_clip = NewRgn();
//	GetClip(current_clip);
//	
//	dest_rect.bottom += 5;
//	//ClipRect(&dest_rect);
//	dest_rect.bottom -= 5;
//	
//	for (i = 0; i < 257; i++)
//		if ((text_len[i] > total_width) && (i <= str_len))
//			total_width = text_len[i];
//	if ((mode == 0) && (total_width < dest_rect.right - dest_rect.left))
//		mode = 2;
//	for (i = 0; i < 257; i++)
//		if ((i <= str_len) && (c_str[i] == '|') && (mode == 2))
//			mode = 0;
//	
//	switch (mode) {
//		case 0: 
//			MoveTo(dest_rect.left + 1 + adjust_x, dest_rect.top + 1 + line_height * on_what_line + adjust_y + 9);
//			for (i = 0;text_len[i] != text_len[i + 1], i < str_len;i++) {
//				if (((text_len[i] - text_len[last_line_break] > (dest_rect.right - dest_rect.left - 6))  && (last_word_break > last_line_break)) || (c_str[i] == '|')) {
//				  	if (c_str[i] == '|') {
//				  		c_str[i] = ' ';
//				  		force_skip = true;
//					}
//					sprintf((char *)str_to_draw,"%s",(char *)null_s);
//					strncpy ((char *) str_to_draw,(char *) c_str + last_line_break,(size_t) (last_word_break - last_line_break - 1));
//					sprintf((char *)str_to_draw2," %s",str_to_draw);
//					str_to_draw2[0] = (char) strlen((char *)str_to_draw);
//					DrawString(str_to_draw2);
//					on_what_line++;
//					MoveTo(dest_rect.left + 1 + adjust_x, dest_rect.top + 1 + line_height * on_what_line + adjust_y + 9);
//					last_line_break = last_word_break;
//					if (force_skip == true) {
//						force_skip = false;
//						i++;
//						last_line_break++;
//						last_word_break++;
//					}
//				}
//				if (c_str[i] == ' ')
//					last_word_break = i + 1;
//				//if (on_what_line == LINES_IN_TEXT_WIN - 1)
//				//	i = 10000;
//			}
//			if (i - last_line_break > 1) {
//				strcpy((char *)str_to_draw,(char *)null_s);
//				strncpy ((char *) str_to_draw,(char *) c_str + last_line_break,(size_t) (i - last_line_break));
//				sprintf((char *)str_to_draw2," %s",str_to_draw);
//				if (strlen((char *) str_to_draw2) > 3) {
//					str_to_draw2[0] = (char) strlen((char *)str_to_draw);
//					DrawString(str_to_draw2);
//				}
//			}
//			break;
//		case 1:
//			MoveTo((dest_rect.right + dest_rect.left) / 2 - (4 * total_width) / 9 + adjust_x, 
//			  (dest_rect.bottom + dest_rect.top - line_height) / 2 + 9 + adjust_y);	
//			DrawString(p_str);
//			break;
//		case 2:
//			MoveTo(dest_rect.left + 1 + adjust_x, 
//			  dest_rect.top + 1 + adjust_y + 9);
//			DrawString(p_str);					
//			break;
//		case 3:
//			MoveTo(dest_rect.left + 1 + adjust_x, 
//			  dest_rect.top + 1 + adjust_y + 9 + (dest_rect.bottom - dest_rect.top) / 6);
//			DrawString(p_str);					
//			break;
//		}
//	SetClip(current_clip);
//	DisposeRgn(current_clip);
//	SetPort(old_port);
//}

//void display_strings_event_filter (short item_hit)
//{
//	short i;
//	bool had1 = false, had2 = false;
//	
//	switch (item_hit) {
//		case 1:
//			toast_dialog();
//			break;
//		}
//}

void record_display_strings(){}

//void display_strings(short str1a,short str1b,short str2a,short str2b,
//	char *title,short sound_num,short graphic_num,short graphic_type,short parent_num)
//{
//	short item_hit;
//	char text[256];
//	location view_loc;
//	bool sound_done = false;
//
//	make_cursor_sword();
//	
//	store_str1a = str1a;
//	store_str1b = str1b;
//	store_str2a = str2a;
//	store_str2b = str2b;
//	
//	if ((str1a <= 0) || (str1b <= 0))
//		return;
//	store_which_string_dlog = 970;
//	if (strlen(title) > 0)
//		store_which_string_dlog += 2;
//	if ((str2a > 0) && (str2b > 0))
//		store_which_string_dlog++;
//	cd_create_dialog_parent_num(store_which_string_dlog,parent_num);
//	
//	cd_activate_item(store_which_string_dlog,2,0);
//	
//	csp(store_which_string_dlog,store_which_string_dlog,graphic_num,graphic_type);
//	
//	get_str(text,str1a,str1b);
//	csit(store_which_string_dlog,4,(char *) text);
//	if ((str2a > 0) && (str2b > 0)) {
//		get_str(text,str2a,str2b);
//		csit(store_which_string_dlog,5,(char *) text);
//		}
//	if (strlen(title) > 0)
//		csit(store_which_string_dlog,6,title);
//	csp(store_which_string_dlog,3,graphic_num,graphic_type);
//	if (sound_num >= 0)
//		play_sound(sound_num);
//	
//	item_hit = cd_run_dialog();
//	cd_kill_dialog(store_which_string_dlog,0);
//}

//void get_str(const char* str,short i, short j)
//{
//	GetIndString(str, i, j);
//	p2cstr(str);
//}

void make_cursor_sword() 
{
	set_cursor(sword_curs);
}

