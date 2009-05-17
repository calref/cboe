
#include <stdio.h>
#include <string.h>
#include "scen.global.h"
#include "classes.h"
#include "graphtool.h"
#include "scen.graphics.h"
#include <math.h>
#include "dlgtool.h"
#include "scen.keydlgs.h"
#include "soundtool.h"
#include "mathutil.h"
#include "boe.consts.h" // TODO: Put these constants in a global file

#include "scen.core.h"
#include "scen.townout.h"

void load_main_screen();
void load_terrain_template();
GWorldPtr load_pict();
short terrain_in_index();
void put_terrain_in_template();
void place_location();
//void draw_cur_string();
void undo_clip();

short find_index_spot();
bool is_s_d();
void sort_specials();

extern cOutdoors current_terrain;
extern WindowPtr	mainPtr;
extern cTown* current_town;
extern short cen_x, cen_y,current_terrain_type,cur_town;
extern cTown* town;
//extern big_tr_type t_d;
//extern template_town_type town_template;
//extern short town_type;  // 0 - big 1 - ave 2 - small
extern short cur_viewing_mode;
extern eScenMode overall_mode;
extern eDrawMode draw_mode = DRAW_TERRAIN;
extern short available_dlog_buttons[NUM_DLOG_B];
extern bool editing_town;
//extern short max_dim[3];
extern short dungeon_font_num,geneva_font_num;
extern Rect windRect;
//extern piles_of_stuff_dumping_type *data_store;
extern cScenario scenario;
extern Rect world_screen;

extern Rect left_button[NLS];
extern Rect right_buttons[NRSONPAGE];
extern Rect right_scrollbar_rect;
extern Rect right_area_rect;
extern unsigned char borders[4][50];
extern ControlHandle right_sbar;

extern bool left_buttons_active,right_buttons_active;
extern short left_button_status[NLS]; // 0 - clear, 1 - text, 2 - title text, +10 - button
extern short right_button_status[NRS];
//extern unsigned char m_pic_index[200];
extern btn_t buttons[];
extern location cur_out;

short num_ir[3] = {12,10,4};

GWorldPtr monst_gworld[11];
GWorldPtr terrain_gworld[7];
GWorldPtr editor_mixed;
GWorldPtr terrain_buttons_gworld;
GWorldPtr anim_gworld;
GWorldPtr fields_gworld;
GWorldPtr dialog_gworld;
GWorldPtr items_gworld;
GWorldPtr tiny_obj_gworld;
GWorldPtr small_ter_gworld;
GWorldPtr spec_scen_g = NULL;
//GWorldPtr dlg_buttons_gworld[NUM_BUTTONS][2]
GWorldPtr ter_draw_gworld;
GWorldPtr dlogpics_gworld;
GWorldPtr talkfaces_gworld;
GWorldPtr roads_gworld;
GWorldPtr mixed_gworld;
PixPatHandle map_pat[25];

// begin new stuff
Rect blue_button_from = {120,91,134,107};
Rect start_button_from = {120,70,127,91};
Rect base_small_button_from = {120,0,127,7};
extern Rect palette_buttons[10][6];
extern Rect palette_buttons_from[71];
extern short town_buttons[6][10], out_buttons[6][10];
extern bool good_palette_buttons[2][6][10];
Rect palette_button_base = {0,0,18,25};
Rect terrain_buttons_rect = {0,0,410,294};
extern Rect left_buttons[NLS][2]; // 0 - whole, 1 - blue button
Rect left_button_base = {5,5,21,200};
Rect right_button_base = {RIGHT_AREA_UL_Y,RIGHT_AREA_UL_X,17,RIGHT_AREA_UL_Y};
Rect terrain_rect = {0,0,340,272};
Str255 current_string = "\p";
Str255 current_string2 = "\p";
extern Rect terrain_rects[256];
extern char strings_ls[NLS][40];
extern char strings_rs[NRS][40];

short map_pats[220] = {50,50,1,1,1,6,6,6,6,6,
	6,6,6,6,6,6,6,6,2,2,
	2,2,2,2,2,2,2,2,2,2,
	2,2,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,3,3,3,3,
	3,3,3,3,3,3,3,3,3,0, // 50
	0,0,0,0,0,0,0,23,23,23,
	15,15,50,50,0,0,0,0,0,7,
	7,8,8,14,14,9,9,9,5,5,
	0,0,0,0,0,0,0,0,0,0,
	18,18,0,0,0,0,0,0,0,0, // 100
	22,22,0,0,0,0,0,0,0,0,
	0,0,0,10,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,11,0,0, // 150
	0,0,0,12,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0, // 200
	0,0,0,0,0,0,0,0,0,0				
};


unsigned char small_what_drawn[64][64];
extern bool small_any_drawn;

void init_dialogs(){
	cd_init_dialogs(
					&anim_gworld,
					&talkfaces_gworld,
					&items_gworld,
					&tiny_obj_gworld,
					NULL, // &pc_gworld
					&dlogpics_gworld,
					monst_gworld, // don't take address because it's already an array.
					terrain_gworld, // ditto
					&small_ter_gworld,
					NULL,
					NULL,
					NULL,/*
	 NULL,
	 NULL,
	 NULL,
	 NULL,
	 NULL,
	 NULL,
	 NULL,*/
					NULL,
					&spec_scen_g
					);
	//	switch (wind_hit) {
	//		case -1: break;
	//		//case 958: _event_filter(item_hit); break;
	cd_register_event_filter(970,display_strings_event_filter);
	cd_register_event_filter(971,display_strings_event_filter);
	cd_register_event_filter(972,display_strings_event_filter);
	cd_register_event_filter(973,display_strings_event_filter);
	cd_register_event_filter(800,edit_make_scen_1_event_filter);
	cd_register_event_filter(801,edit_make_scen_2_event_filter);
	//cd_register_event_filter(802,user_password_filter);
	cd_register_event_filter(803,edit_scen_details_event_filter);
	cd_register_event_filter(804,edit_scen_intro_event_filter);
	cd_register_event_filter(805,set_starting_loc_filter);
	cd_register_event_filter(806,edit_spec_item_event_filter);
	cd_register_event_filter(807,edit_save_rects_event_filter);
	cd_register_event_filter(808,edit_horses_event_filter);
	cd_register_event_filter(809,edit_boats_event_filter);
	cd_register_event_filter(810,edit_add_town_event_filter);
	cd_register_event_filter(811,edit_scenario_events_event_filter);
	cd_register_event_filter(812,edit_item_placement_event_filter);
	cd_register_event_filter(813,edit_ter_type_event_filter);
	cd_register_event_filter(814,edit_monst_type_event_filter);
	cd_register_event_filter(815,edit_monst_abil_event_filter);
	cd_register_event_filter(816,edit_text_event_filter);
	cd_register_event_filter(817,edit_talk_node_event_filter);
	cd_register_event_filter(818,edit_item_type_event_filter);
	cd_register_event_filter(819,choose_graphic_event_filter);
	cd_register_event_filter(820,choose_text_res_event_filter);
	cd_register_event_filter(821,edit_basic_dlog_event_filter);
	cd_register_event_filter(822,edit_spec_enc_event_filter);
	//cd_register_event_filter(823,give_password_filter);
	cd_register_event_filter(824,edit_item_abil_event_filter);
	cd_register_event_filter(825,edit_special_num_event_filter);
	cd_register_event_filter(826,edit_spec_text_event_filter);
	cd_register_event_filter(830,new_town_event_filter);
	cd_register_event_filter(831,edit_sign_event_filter);
	cd_register_event_filter(832,edit_town_details_event_filter);
	cd_register_event_filter(833,edit_town_events_event_filter);
	cd_register_event_filter(834,edit_advanced_town_event_filter);
	cd_register_event_filter(835,edit_town_wand_event_filter);
	cd_register_event_filter(836,edit_placed_item_event_filter);
	cd_register_event_filter(837,edit_placed_monst_event_filter);
	cd_register_event_filter(838,edit_placed_monst_adv_event_filter);
	cd_register_event_filter(839,edit_town_strs_event_filter);
	cd_register_event_filter(840,edit_area_rect_event_filter);
	cd_register_event_filter(841,pick_import_town_event_filter);
	cd_register_event_filter(842,edit_dialog_text_event_filter);
	cd_register_event_filter(850,edit_out_strs_event_filter);
	cd_register_event_filter(851,outdoor_details_event_filter);
	cd_register_event_filter(852,edit_out_wand_event_filter);
	cd_register_event_filter(854,pick_out_event_filter);
	cd_register_event_filter(855,pick_town_num_event_filter);
	cd_register_event_filter(856,pick_town_num_event_filter);
	cd_register_event_filter(857,change_ter_event_filter);
	cd_register_default_event_filter(fancy_choice_dialog_event_filter);
	cd_set_bg_pat_num(16);
	RGBColor c = {0,0,0};
	cd_set_text_clr(c);
	//return tmp;
}

short get_small_icon(unsigned short ter){
	short icon;
	switch(scenario.ter_types[ter].special){
		case TER_SPEC_NONE:
			icon = scenario.ter_types[ter].flag1;
			break;
		case TER_SPEC_CHANGE_WHEN_STEP_ON:
			icon = 23;
			break;
		case TER_SPEC_DAMAGING:
			switch(scenario.ter_types[ter].flag3){
				case DAMAGE_WEAPON:
					icon = 40;
					break;
				case DAMAGE_FIRE:
					icon = 37;
					break;
				case DAMAGE_POISON:
					icon = 43;
					break;
				case DAMAGE_MAGIC:
					icon = 39;
					break;
				case DAMAGE_UNBLOCKABLE:
					icon = 39;
					break;
				case DAMAGE_COLD:
					icon = 38;
					break;
				case DAMAGE_UNDEAD:
					icon = 18;
					break;
				case DAMAGE_DEMON:
					icon = 19;
					break;
			}
			break;
		case TER_SPEC_BRIDGE:
			icon = 42;
			break;
		case TER_SPEC_BED:
			icon = -1;
			break;
		case TER_SPEC_DANGEROUS:
			switch(scenario.ter_types[ter].flag3){
				case STATUS_POISONED_WEAPON: // TODO: Do something here
					break;
				case STATUS_BLESS: // TODO: Do something here
					break;
				case STATUS_POISON:
					icon = 35;
					break;
				case STATUS_HASTE: // TODO: Do something here
					break;
				case STATUS_INVULNERABLE: // TODO: Do something here
					break;
				case STATUS_MAGIC_RESISTANCE: // TODO: Do something here
					break;
				case STATUS_WEBS: // TODO: Do something here
					break;
				case STATUS_DISEASE:
					icon = 33;
					break;
				case STATUS_INVISIBLE: // TODO: Do something here
					break;
				case STATUS_DUMB: // TODO: Do something here
					break;
				case STATUS_MARTYRS_SHIELD: // TODO: Do something here
					break;
				case STATUS_ASLEEP:
					icon = 44;
					break;
				case STATUS_PARALYZED: // TODO: Do something here
					break;
				case STATUS_ACID:
					icon = 41;
					break;
				case 14: // bless TODO: Do something here
					break;
				case 15: // haste TODO: Do something here
					break;
			}
			break;
		case TER_SPEC_CRUMBLING:
			icon = 34;
			break;
		case TER_SPEC_LOCKABLE:
			icon = 30;
			break;
		case TER_SPEC_UNLOCKABLE:
			if (scenario.ter_types[ter].flag2 >= 5)
				icon = (scenario.ter_types[ter].flag2 == 10) ? 32 : 31;
			else icon = 30;
			break;
		case TER_SPEC_IS_A_SIGN:
			icon = 26;
			break;
		case TER_SPEC_CALL_SPECIAL:
			icon = scenario.ter_types[ter].flag3;
			break;
		case TER_SPEC_IS_A_CONTAINER:
			icon = 36;
			break;
		case TER_SPEC_WATERFALL:
			icon = -1;
			break;
		case TER_SPEC_CONVEYOR:
			switch(scenario.ter_types[ter].flag1){ // TODO: Consider the other four possible directions
				case DIR_N:
					icon = 27;
					break;
				case DIR_E:
					icon = 28;
					break;
				case DIR_S:
					icon = 29;
					break;
				case DIR_W:
					icon = 20;
					break;
			}
			break;
		case TER_SPEC_BLOCKED_TO_MONSTERS:
			icon = 21;
			break;
		case TER_SPEC_TOWN_ENTRANCE:
			icon = 22;
			break;
		case TER_SPEC_CHANGE_WHEN_USED:
			icon = -1;
			break;
		case TER_SPEC_CALL_SPECIAL_WHEN_USED:
			icon = scenario.ter_types[ter].flag3;
			break;
	}
	if(icon == 255) icon = -1;
	return icon;
}

void Set_up_win () {
	short i,j;
	for (i = 0; i < 70; i++){
		palette_buttons_from[i] = palette_button_base;
		OffsetRect(&palette_buttons_from[i],(i%10) * 25, (i/10) * 17);
		palette_buttons_from[i].right++;
	}
	for (i = 0; i < 10; i++)
		for (j = 0; j < 6; j++) {
			palette_buttons[i][j] = palette_button_base;
			OffsetRect(&palette_buttons[i][j],i * 25, j * 17);
			if (i == 10 || !good_palette_buttons[1][j][i+1])
				palette_buttons[i][j].right++;
		}
	for (i = 0; i < 10; i++)
		for (j = /*2*/0; j < 6; j++) 
			OffsetRect(&palette_buttons[i][j],0,3);
	for (i = 0; i < 10; i++)
		for (j = /*3*/0; j < 6; j++) 
			OffsetRect(&palette_buttons[i][j],0,3);
	for (i = 0; i < 10; i++)
		for (j = /*4*/0; j < 6; j++) 
			OffsetRect(&palette_buttons[i][j],0,3);
	
	for (i = 0; i < NLS; i++) {
		left_buttons[i][0] = left_button_base;
		OffsetRect(&left_buttons[i][0],0,i * 16);
		left_buttons[i][1] = left_buttons[i][0];
		left_buttons[i][1].top += 1;
		left_buttons[i][1].bottom -= 1;
		left_buttons[i][1].left += 0;
		left_buttons[i][1].right = left_buttons[i][1].left + 16;
	}		
	right_button_base.left = RIGHT_AREA_UL_X + 1;
	right_button_base.top = RIGHT_AREA_UL_Y + 1;
	right_button_base.bottom = right_button_base.top + 12;
	right_button_base.right = right_button_base.left + RIGHT_AREA_WIDTH - 20;
	for (i = 0; i < NRSONPAGE; i++) {
		right_buttons[i] = right_button_base;
		OffsetRect(&right_buttons[i],0,i * 12);
	}		
	load_main_screen();
}

void run_startup_g() {
	Rect pict_rect = {0,0,480,640};
	PicHandle	pict_to_draw;
	
	SetPortWindowPort(mainPtr);
	OffsetRect(&pict_rect,-25,-25);
	pict_to_draw = GetPicture(3002);
	DrawPicture(pict_to_draw, &pict_rect);
	ReleaseResource((Handle) pict_to_draw);
	play_sound(-95);
	EventRecord event;
	WaitNextEvent(keyDownMask + mDownMask + mUpMask,&event,120,NULL);
	
}

void load_graphics(){
	int i;
	//for (i = 0; i < NUM_BUTTONS; i++)
	//	for (j = 0; j < 2; j++)
	//		dlg_buttons_gworld[i][j] = load_pict(2000 + (2 * i) + j); // move to dlogtool
	for (i = 0; i < 14; i++) 
	    bg[i] = GetPixPat (128 + i);
	for (i = 0; i < 25; i++) 
	    map_pat[i] = GetPixPat (200 + i);
	
	for (i = 0; i < 11; i++)
		monst_gworld[i] = load_pict(1100 + i);
	for (i = 0; i < 7; i++)
		terrain_gworld[i] = load_pict(800 + i);
	editor_mixed = load_pict(906);
	anim_gworld = load_pict(820);
	fields_gworld = load_pict(821);
	roads_gworld = load_pict(822);
	talkfaces_gworld = load_pict(860);
	items_gworld = load_pict(901);
	tiny_obj_gworld = load_pict(900);
	dlogpics_gworld = load_pict(850);
	small_ter_gworld = load_pict(904);
	mixed_gworld = load_pict(903);
}

void load_main_screen() {
	Str255 fn1 = "\pGeneva";
	Str255 fn2 = "\pDungeon Bold";
	Str255 fn3 = "\pPalatino";
	short i,j;
	
	GetFNum(fn1,&geneva_font_num);
	if (geneva_font_num == 0)
		GetFNum(fn3,&geneva_font_num);
	GetFNum(fn2,&dungeon_font_num);
	if (dungeon_font_num == 0)
		GetFNum(fn3,&dungeon_font_num);
	
	NewGWorld(&terrain_buttons_gworld, 0,&terrain_buttons_rect, NULL, NULL, kNativeEndianPixMap);
	
	SetPort( terrain_buttons_gworld);
	TextFont(geneva_font_num);
	TextSize(10);
	TextFace(bold);
	SetPortWindowPort(mainPtr);
	NewGWorld(&ter_draw_gworld, 0,&terrain_rect, NULL, NULL, kNativeEndianPixMap);
	world_screen = terrain_rect;
	OffsetRect(&world_screen,TER_RECT_UL_X,TER_RECT_UL_Y);
	
	//draw_main_screen();
}

void redraw_screen() {
	GrafPtr		old_port;
	
	
	GetPort (&old_port);
	SetPortWindowPort (mainPtr);
	FillCRect(&windRect,bg[20]);
	draw_main_screen();
	if (overall_mode < MODE_MAIN_SCREEN);
	draw_terrain();
	SetPort (old_port);
}

void draw_main_screen() {
	short i;
	Rect draw_rect;
	
	
	// draw left buttons (always active)
	//for (i = 0; i < NLS; i++) {
	//	draw_lb_slot(i);
	//	}
	draw_lb();
	
	// draw right buttons (only when not editing terrain)
	if (overall_mode >= MODE_MAIN_SCREEN) {
		draw_rect.left = RIGHT_AREA_UL_X;
		draw_rect.top = RIGHT_AREA_UL_Y;
		draw_rect.right = RIGHT_AREA_UL_X + RIGHT_AREA_WIDTH - 16;
		draw_rect.bottom = RIGHT_AREA_UL_Y + RIGHT_AREA_HEIGHT;
		
		FrameRect(&draw_rect);
		InsetRect(&draw_rect,1,1);
		FillCRect(&draw_rect,bg[17]);
		
		draw_rb();
		Draw1Control(right_sbar);
	}
	
	// draw terrain palette
	if ((overall_mode < MODE_MAIN_SCREEN) || (overall_mode == MODE_EDIT_TYPES)) {
		//draw_rect = terrain_buttons_rect;
		//OffsetRect(&draw_rect,RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
		//rect_draw_some_item(terrain_buttons_gworld,terrain_buttons_rect,
		//	terrain_buttons_gworld,draw_rect,0,1);
		place_location();
	}
	
	
}

void draw_lb() {
	Rect temp_rect;
	short i;
	
	temp_rect = windRect;
	temp_rect.right = RIGHT_AREA_UL_X - 2;
	FillCRect(&temp_rect,bg[20]);
	for (i = 0; i < NLS; i++)
		draw_lb_slot(i,0);
}

// mode 0 normal 1 click
void draw_lb_slot (short which,short mode)  {
	Rect text_rect,from_rect;
	
 	FillCRect(&left_buttons[which][0],bg[20]);
	if (left_button_status[which] == 0)
		return;
	text_rect = left_buttons[which][0];
	if (left_button_status[which] >= 10) {
		text_rect.left += 18;
		from_rect = blue_button_from;
		if (mode > 0)
			OffsetRect(&from_rect,from_rect.right - from_rect.left,0);
		rect_draw_some_item(editor_mixed,from_rect,editor_mixed,left_buttons[which][1],0,1);
	}
	if (left_button_status[which] % 10 == 3) 
		text_rect.left += 16;
	if (left_button_status[which] % 10 == 2) {
		TextSize(14);
		TextFace(bold);
	}
	else OffsetRect(&text_rect,0,2);
	if (mode > 0)
		ForeColor(blueColor);
	char_win_draw_string(mainPtr,text_rect,(char *)strings_ls[which],0,12,true);
	ForeColor(blackColor);
	
	//char_win_draw_string(mainPtr,text_rect,"FIsh.",1,12);
	TextSize(10);
	TextFace(bold);
}

void draw_rb() {
	short i,pos;
	
 	pos = GetControlValue(right_sbar);
	for (i = pos; i < pos + NRSONPAGE; i++)
		draw_rb_slot(i,0);
}

// mode 0 normal 1 pressed
void draw_rb_slot (short which,short mode)  {
	Rect text_rect;
 	short pos;
 	
 	pos = GetControlValue(right_sbar);
	if ((which < pos) || (which >= pos + NRSONPAGE))
		return;
	
 	FillCRect(&right_buttons[which - pos],bg[17]);
	if (right_button_status[which] == 0)
		return;
	text_rect = right_buttons[which - pos];
	
	if (mode > 0)
		ForeColor(redColor);
	char_win_draw_string(mainPtr,text_rect,(char *)strings_rs[which],0,12,true);
	ForeColor(blackColor);
	TextSize(10);
	TextFace(bold);
}

void set_up_terrain_buttons() {
	short i,j,pic,small_i;
	Rect ter_from,ter_to,ter_from_base = {0,0,36,28};
	Rect tiny_from,tiny_to;
	
	Rect palette_from,palette_to = palette_button_base;
	
 	SetPort( terrain_buttons_gworld);
	FillCRect(&terrain_buttons_rect,bg[17]);
	FrameRect(&terrain_buttons_rect);
 	
	// first make terrain buttons
	switch(draw_mode){
		case DRAW_TERRAIN:
			for (i = 0; i < 256; i++) {
				ter_from = ter_from_base;
				pic = scenario.ter_types[i].picture;
				if (pic >= 1000) {
					ter_from = get_custom_rect(pic % 1000);
					rect_draw_some_item(spec_scen_g,
										ter_from,terrain_buttons_gworld,terrain_rects[i],0,0);
				}
				else if (pic < 400)	{
					pic = pic % 50;
					OffsetRect(&ter_from,28 * (pic % 10), 36 * (pic / 10));
					rect_draw_some_item(terrain_gworld[scenario.ter_types[i].picture/50],
										ter_from,terrain_buttons_gworld,terrain_rects[i],0,0);
				}
				else {
					pic = pic % 50;
					ter_from.left = 112 * (pic / 5);
					ter_from.right = ter_from.left + 28;
					ter_from.top = 36 * (pic % 5);
					ter_from.bottom = ter_from.top + 36;
					rect_draw_some_item(anim_gworld,
										ter_from,terrain_buttons_gworld,terrain_rects[i],0,0);
					
				}
				small_i = get_small_icon(i);
//				if (i == 82)
//					small_i = 3;
//				if (i == 83)
//					small_i = 2;
//				if ((i == 7) || (i == 10) || (i == 13) || (i == 16))
//					small_i = 23;
				tiny_from = base_small_button_from;
				OffsetRect(&tiny_from,7 * (small_i % 10),7 * (small_i / 10));
				tiny_to = terrain_rects[i];
				tiny_to.top = tiny_to.bottom - 7;
				tiny_to.left = tiny_to.right - 7;
				if (small_i > 0 && small_i < 255)
					rect_draw_some_item(editor_mixed,tiny_from,terrain_buttons_gworld,tiny_to,0,0);
			}
			break;
		case DRAW_MONST:
			break;
		case DRAW_ITEM:
			break;
	}
	
	if (overall_mode < MODE_MAIN_SCREEN) {
		//		palette_to.left = 5;
		//		palette_to.top = terrain_rects[255].bottom + 5;
		//		if (editing_town == true) {
		//			palette_from.bottom = palette_buttons[0][5].bottom;
		//			palette_from.right = palette_buttons[7][5].right;
		//			}
		//			else {
		//				palette_from.bottom = palette_buttons[0][2].bottom;
		//				palette_from.right = palette_buttons[7][2].right;
		//				}
		//		palette_to.right = palette_to.left + palette_from.right;
		//		palette_to.bottom = palette_to.top + palette_from.bottom;
		//		rect_draw_some_item(editor_mixed,
		//			palette_from,terrain_buttons_gworld,palette_to,1,0);
		OffsetRect(&palette_to,5,terrain_rects[255].bottom + 14);
		palette_to.right++;
		//printf("terrain_rects[255].bottom = %i\n", terrain_rects[255].bottom);
		for(i = 0; i < 10; i++){
			for(j = 0; j < 6; j++){
				if(editing_town && town_buttons[j][i] >= 0)
					palette_from = palette_buttons_from[town_buttons[j][i]];
				else if(!editing_town && out_buttons[j][i] >= 0)
					palette_from = palette_buttons_from[out_buttons[j][i]];
				else palette_from = palette_button_base;
				//printf("palette_from = {top = %i, left = %i, bottom = %i, right = %i\n",
				//	   palette_from.top, palette_from.left, palette_from.bottom, palette_from.right);
				//printf("palette_to = {top = %i, left = %i, bottom = %i, right = %i\n",
				//	   palette_to.top, palette_to.left, palette_to.bottom, palette_to.right);
				if(good_palette_buttons[editing_town][j][i]){
					//printf("Drawing button %i at col %i, row %i\n",editing_town?town_buttons[j][i]:out_buttons[j][i],i,j);
					rect_draw_some_item(editor_mixed,palette_from,terrain_buttons_gworld,palette_to,1,0);
				}
				OffsetRect(&palette_to,0,17);
			}
			OffsetRect(&palette_to,25,-6*17);
		}
	}
	
	SetPortWindowPort(mainPtr);
}

void draw_terrain(){
	short q,r,x,y,i,small_i;
	location which_pt,where_draw;
	Rect draw_rect,clipping_rect = {8,8,332,260};
	unsigned char t_to_draw;
	Rect source_rect,tiny_to,tiny_to_base = {37,29,44,36},tiny_from,from_rect,to_rect;
	Rect boat_rect[4] = {{0,0,36,28}, {0,28,36,56},{0,56,36,84},{0,84,36,112}};
	
	if (overall_mode >= MODE_MAIN_SCREEN)
		return;
	
	OffsetRect(&boat_rect[0],61,0);
	
	if (cur_viewing_mode == 0) {
		SetPort( ter_draw_gworld);
		FillCRect(&terrain_rect,bg[17]);
		FrameRect(&terrain_rect);
		SetPortWindowPort(mainPtr);
		for (q = 0; q < 9; q++) 
			for (r = 0; r < 9; r++)
			{
				where_draw.x = q;
				where_draw.y = r;
				if (editing_town == true) {
					t_to_draw = town->terrain(cen_x + q - 4,cen_y + r - 4);
				}
				else {
					if (cen_x + q - 4 == -1) 
						t_to_draw = borders[3][cen_y + r - 4];
					else if (cen_x + q - 4 == 48) 
						t_to_draw = borders[1][cen_y + r - 4];
					else if (cen_y + r - 4 == -1) 
						t_to_draw = borders[0][cen_x + q - 4];
					else if (cen_y + r - 4 == 48) 
						t_to_draw = borders[2][cen_x + q - 4];
					else t_to_draw = current_terrain.terrain[cen_x + q - 4][cen_y + r - 4];
				}
				draw_one_terrain_spot(q,r,t_to_draw);
				
				if((editing_town  && is_field_type(cen_x + q - 4,cen_y + r - 4, 2)) ||
				   (!editing_town && current_terrain.special_spot[cen_x + q - 4][cen_y + r - 4]))
					Draw_Some_Item(roads_gworld, calc_rect(6, 0), ter_draw_gworld, where_draw, 1, 0);
				
				which_pt.x = cen_x + q - 4;
				which_pt.y =cen_y + r - 4;
				
				tiny_to = tiny_to_base;
				OffsetRect(&tiny_to,28 * q, 36 * r);
				
				// draw start icon, if starting point
				if ((editing_town == true) && 
					(cur_town == scenario.which_town_start) && (scenario.where_start.x == cen_x + q - 4)
					&& (scenario.where_start.y == cen_y + r - 4)) {
					from_rect = start_button_from;
					to_rect = tiny_to;
					to_rect.left -= 14;
					rect_draw_some_item(editor_mixed,from_rect,ter_draw_gworld,to_rect,0,0);
					OffsetRect(&tiny_to,0,-7);
				}
				if ((editing_town == false)
					&& (scenario.out_sec_start.x == cur_out.x)
					&& (scenario.out_sec_start.y == cur_out.y) 
					&& (scenario.out_start.x == cen_x + q - 4)
					&& (scenario.out_start.y == cen_y + r - 4)) {
					from_rect = start_button_from;
					to_rect = tiny_to;
					to_rect.left -= 14;
					rect_draw_some_item(editor_mixed,from_rect,ter_draw_gworld,to_rect,0,0);
					OffsetRect(&tiny_to,0,-7);
				}
				small_i = get_small_icon(t_to_draw);
				tiny_from = base_small_button_from;
				OffsetRect(&tiny_from,7 * (small_i % 10),7 * (small_i / 10));
				if (small_i > 0) {
					rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
					OffsetRect(&tiny_to,0,-7);
				}
				
				if (is_special(cen_x + q - 4,cen_y + r - 4) == true) {
					tiny_from = base_small_button_from;
					OffsetRect(&tiny_from,7 * (7),7 * (0));
					rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
					OffsetRect(&tiny_to,0,-7);
				}	
				if ((t_to_draw == 7) || (t_to_draw == 10) || (t_to_draw == 13) || (t_to_draw == 16)) {
					tiny_from = base_small_button_from;
					OffsetRect(&tiny_from,7 * (3),7 * (2));
					rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
					OffsetRect(&tiny_to,0,-7);
				}	
				//if (is_s_d(cen_x + q - 4,cen_y + r - 4) == true) {
				//	}	
				if (editing_town == false) {
					for (i = 0; i < 4; i++)		
						if ((cen_x + q - 4 == current_terrain.wandering_locs[i].x) &&
							(cen_y + r - 4 == current_terrain.wandering_locs[i].y)) {
							tiny_from = base_small_button_from;
							OffsetRect(&tiny_from,7 * (2),7 * (1));
							rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
							OffsetRect(&tiny_to,0,-7);
							i = 4;
						}			
					
				}
				
				if (editing_town == true) {
					for (i = 0; i < 30; i++) {
						if ((scenario.boats[i].which_town == cur_town) &&
							(scenario.boats[i].loc.x == cen_x + q - 4) &&
							(scenario.boats[i].loc.y == cen_y + r - 4))
							Draw_Some_Item(mixed_gworld,boat_rect[0],ter_draw_gworld,where_draw,1,0);
						
					}	
					for (i = 0; i < 30; i++) {
						source_rect = boat_rect[0];
						OffsetRect(&source_rect,0,74);
						OffsetRect(&source_rect,56,36);
						if ((scenario.horses[i].which_town == cur_town) &&
							(scenario.horses[i].loc.x == cen_x + q - 4) &&
							(scenario.horses[i].loc.y == cen_y + r - 4))
							Draw_Some_Item(mixed_gworld,source_rect,ter_draw_gworld,where_draw,1,0);
						
					}	
					for (i = 0; i < 4; i++)		
						if ((cen_x + q - 4 == town->start_locs[i].x) &&
							(cen_y + r - 4 == town->start_locs[i].y)) {
							tiny_from = base_small_button_from;
							OffsetRect(&tiny_from,7 * (6 + i),7 * (1));
							rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
							OffsetRect(&tiny_to,0,-7);
						}			
					for (i = 0; i < 4; i++)		
						if ((cen_x + q - 4 == town->wandering_locs[i].x) &&
							(cen_y + r - 4 == town->wandering_locs[i].y)) {
							tiny_from = base_small_button_from;
							OffsetRect(&tiny_from,7 * (2),7 * (1));
							rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
							OffsetRect(&tiny_to,0,-7);
							i = 4;
						}			
					if (is_web(cen_x + q - 4,cen_y + r - 4) == true) {
						from_rect = calc_rect(5,0);
						Draw_Some_Item(fields_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
					}	
					if (is_crate(cen_x + q - 4,cen_y + r - 4) == true) {
						from_rect = calc_rect(6,0);
						Draw_Some_Item(fields_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
					}	
					if (is_barrel(cen_x + q - 4,cen_y + r - 4) == true) {
						from_rect = calc_rect(7,0);
						Draw_Some_Item(fields_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
					}	
					if (is_fire_barrier(cen_x + q - 4,cen_y + r - 4) == true) {
						from_rect = calc_rect(0,2);
						Draw_Some_Item(fields_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
					}	
					if (is_quickfire(cen_x + q - 4,cen_y + r - 4) == true) {
						from_rect = calc_rect(7,1);
						Draw_Some_Item(fields_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
					}	
					if (is_force_barrier(cen_x + q - 4,cen_y + r - 4) == true) {
						from_rect = calc_rect(2,2);
						Draw_Some_Item(fields_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
					}	
					for (i = 0; i < 8; i++) 
						if (is_sfx(cen_x + q - 4,cen_y + r - 4,i)) {
							from_rect = calc_rect(i,3);
							Draw_Some_Item(fields_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
						}
					for (x = 0; x < 64; x++)
						if ((cen_x + q - 4 == town->preset_items[x].loc.x) &&
							(cen_y + r - 4 == town->preset_items[x].loc.y) && (town->preset_items[x].code >= 0)) {
						}
					for (x = 0; x < 60; x++)
						if ((cen_x + q - 4 == town->creatures(x).start_loc.x) &&
							(cen_y + r - 4 == town->creatures(x).start_loc.y) && (town->creatures(x).number != 0)) {
						}
					
				}
			}
		if (editing_town == true) {
			draw_monsts();
			draw_items();
		}
		
		SetPort( ter_draw_gworld);
		ClipRect(&clipping_rect);
		
		if (editing_town == true) {
			// draw info rects
			for (i = 0; i < 16; i++)
				if (town->room_rect(i).left > 0) {
					draw_rect.left = 22 + 28 * (town->room_rect(i).left - cen_x + 4);
					draw_rect.right = 22 + 28 * (town->room_rect(i).right - cen_x + 4);
					draw_rect.top = 24 + 36 * (town->room_rect(i).top - cen_y + 4);
					draw_rect.bottom = 24 + 36 * (town->room_rect(i).bottom - cen_y + 4);
					ForeColor(redColor);
					FrameRect(&draw_rect);
					ForeColor(blackColor);
				}
			// draw border rect
			draw_rect.left = 21 + 28 * (town->in_town_rect.left - cen_x + 4);
			draw_rect.right = 21 + 28 * (town->in_town_rect.right - cen_x + 4);
			draw_rect.top = 25 + 36 * (town->in_town_rect.top - cen_y + 4);
			draw_rect.bottom = 25 + 36 * (town->in_town_rect.bottom - cen_y + 4);
			ForeColor(whiteColor);
			FrameRect(&draw_rect);
			ForeColor(blackColor);
		}
		if (editing_town == false) {
			// draw info rects
			for (i = 0; i < 8; i++)
				if (current_terrain.info_rect[i].left > 0) {
					draw_rect.left = 22 + 28 * (current_terrain.info_rect[i].left - cen_x + 4);
					draw_rect.right = 22 + 28 * (current_terrain.info_rect[i].right - cen_x + 4);
					draw_rect.top = 24 + 36 * (current_terrain.info_rect[i].top - cen_y + 4);
					draw_rect.bottom = 24 + 36 * (current_terrain.info_rect[i].bottom - cen_y + 4);
					ForeColor(redColor);
					FrameRect(&draw_rect);
					ForeColor(blackColor);
				}
		}
		ClipRect(&terrain_rect);
		SetPortWindowPort(mainPtr);
		
		small_any_drawn = false;
		//if (cur_viewing_mode == 0) 
		//	draw_frames();
	}
	
	if (cur_viewing_mode == 1) {
		SetPort( ter_draw_gworld);
		if (small_any_drawn == false) {
		 	FillCRect(&terrain_rect,bg[17]);
			FrameRect(&terrain_rect);
		}
		for (q = 0; q < (editing_town ? town->max_dim() : 48); q++) 
			for (r = 0; r < (editing_town ? town->max_dim() : 48); r++) {
				t_to_draw = editing_town ? town->terrain(q,r) : current_terrain.terrain[q][r];
				if ((small_what_drawn[q][r] != t_to_draw) || (small_any_drawn == false)) {
					draw_one_tiny_terrain_spot(q,r,t_to_draw);
					small_what_drawn[q][r] = t_to_draw;
				}
			}
		SetPortWindowPort(mainPtr);
		small_any_drawn = true;
	}
	
	//draw_cur_string();
	place_location();
	
	//to_rect = world_screen;
	//OffsetRect(&to_rect,TER_RECT_UL_X,TER_RECT_UL_Y);
	rect_draw_some_item(ter_draw_gworld,terrain_rect,ter_draw_gworld,world_screen,0,1);
}

void draw_monsts() {
	short i,k,width,height,m_start_pic;
	GWorldPtr from_gworld;
	Rect source_rect,draw_rect;
	location where_draw,store_loc;
	GrafPtr cur_port;
	
	for (i = 0; i < 60; i++)
		if (town->creatures(i).number != 0) {
			where_draw.x = town->creatures(i).start_loc.x - cen_x + 4;
			where_draw.y = town->creatures(i).start_loc.y - cen_y + 4;
			width = scenario.scen_monsters[town->creatures(i).number].x_width;
			height = scenario.scen_monsters[town->creatures(i).number].y_width;
			
			for (k = 0; k < width * height; k++) {
				store_loc = where_draw;
				if ((where_draw.x == minmax(0,8,where_draw.x)) && 
					(where_draw.y == minmax(0,8,where_draw.y)) && 
					(scenario.scen_monsters[town->creatures(i).number].picture_num >= 1000)) {
					source_rect = get_custom_rect((scenario.scen_monsters[town->creatures(i).number].picture_num + k) % 1000);
					store_loc.x += k % width;
					store_loc.y += k / width;
					Draw_Some_Item(spec_scen_g, source_rect, ter_draw_gworld, store_loc, 1, 0);
				}
				else if (scenario.scen_monsters[town->creatures(i).number].picture_num < 1000) {
					m_start_pic = m_pic_index[scenario.scen_monsters[town->creatures(i).number].picture_num].i + k;
					from_gworld = monst_gworld[m_start_pic / 20];
					m_start_pic = m_start_pic % 20;
					source_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
					store_loc.x += k % width;
					store_loc.y += k / width;
					Draw_Some_Item(from_gworld, source_rect, ter_draw_gworld, store_loc, 1, 0);
				}
			}
		}
}

// Returns rect for drawing an item, if num < 25, rect is in big item template,
// otherwise in small item template
Rect get_item_template_rect (short type_wanted) {
	Rect store_rect;
	
	if (type_wanted < 45) {
		store_rect.top = (type_wanted / 5) * BITMAP_HEIGHT;
		store_rect.bottom = store_rect.top + BITMAP_HEIGHT;
		store_rect.left = (type_wanted % 5) * BITMAP_WIDTH;
		store_rect.right = store_rect.left + BITMAP_WIDTH;
	}
	else {
		store_rect.top = (type_wanted / 10) * 18;
		store_rect.bottom = store_rect.top + 18;
		store_rect.left = (type_wanted % 10) * 18;
		store_rect.right = store_rect.left + 18;
	}
	
	return store_rect;
}

void draw_items() {
	short i;
	Rect source_rect,dest_rect;
	location where_draw;
	short pic_num;
	GrafPtr cur_port;
	
	for (i = 0; i < 64; i++) {
		if (town->preset_items[i].code >= 0) {
			where_draw.x = town->preset_items[i].loc.x - cen_x + 4;
			where_draw.y = town->preset_items[i].loc.y - cen_y + 4;
			pic_num = scenario.scen_items[town->preset_items[i].code].graphic_num;
			if ((where_draw.x >= 0) && (where_draw.x <= 8) &&
				(where_draw.y >= 0) && (where_draw.y <= 8))  {
				
				if (pic_num >= 150) {
					source_rect = get_custom_rect(pic_num - 150);
					dest_rect = calc_rect(where_draw.x,where_draw.y);
					rect_draw_some_item(spec_scen_g,
										source_rect, ter_draw_gworld, dest_rect, 1, 0);
				}
				else {
					source_rect = get_item_template_rect(pic_num);
					dest_rect = calc_rect(where_draw.x,where_draw.y);
					OffsetRect(&dest_rect,8,8);
					if (pic_num >= 45) {
						dest_rect.top += 9;
						dest_rect.bottom -= 9;
						dest_rect.left += 5;
						dest_rect.right -= 5;
					}
					rect_draw_some_item((pic_num < 45) ? items_gworld : tiny_obj_gworld,
										source_rect, ter_draw_gworld, dest_rect, 1, 0);
				}
			}
		}
	}
}


void force_tiny_redraw() {
	//	short q,r;
	//	for (q = 0; q < 8; q++) 
	//		for (r = 0; r < 64; r++)
	//			ter_changed[q][r] = 255;
	
}

void redraw_selected_ter() {
	//	draw_one_terrain_spot(18,8,(unsigned char) current_terrain_type);
}

void draw_one_terrain_spot (short i,short j,unsigned char terrain_to_draw) {
	location where_draw;
	Rect source_rect;
	short picture_wanted;
	GWorldPtr source_gworld;
	
	picture_wanted = scenario.ter_types[terrain_to_draw].picture;
	//	if (picture_wanted >= 1000) {
	//		terrain_to_draw = 90;
	//		picture_wanted = 74;
	//		}
	
	where_draw.x = (char) i;
	where_draw.y = (char) j;
	
	if ((picture_wanted >= 1000) && (spec_scen_g != NULL)) {
		source_gworld = spec_scen_g;
		source_rect = get_custom_rect(picture_wanted % 1000);
	}
	else if (picture_wanted >= 400)	{
		source_gworld = anim_gworld;
		picture_wanted -= 400;
		source_rect.left = 112 * (picture_wanted / 5);
		source_rect.right = source_rect.left + 28;
		source_rect.top = 36 * (picture_wanted % 5);
		source_rect.bottom = source_rect.top + 36;
	}
	else {
		source_rect = get_template_rect(terrain_to_draw);
		source_gworld = terrain_gworld[picture_wanted / 50];
	}
	
	Draw_Some_Item(source_gworld, source_rect, ter_draw_gworld, where_draw, 0, 0);
}

void draw_one_tiny_terrain_spot (short i,short j,unsigned char terrain_to_draw) {
	
	location where_draw;
	Rect source_rect,dest_rect = {0,0,4,4},from_rect = {0,0,4,4};
	short picture_wanted,k;
	GWorldPtr source_gworld;
	
	picture_wanted = scenario.ter_types[terrain_to_draw].picture;
	
	where_draw.x = (char) i;
	where_draw.y = (char) j;
	OffsetRect(&dest_rect,8 + 4 * i,8 + 4 * j);
	switch (picture_wanted) {
			
		case 0: case 1: case 73: case 72:
			FillCRect(&dest_rect,map_pat[0]);
			break;
		case 2: case 3: case 4:
			FillCRect(&dest_rect,map_pat[1]);
			break;
			
		default:
			if ((picture_wanted < 170) && (map_pats[picture_wanted] > 0)) {
				FillCRect(&dest_rect,map_pat[map_pats[picture_wanted]]);
			}
			else if (picture_wanted >= 1000)	{
				from_rect = get_custom_rect(picture_wanted % 1000);
				rect_draw_some_item(spec_scen_g, from_rect, ter_draw_gworld, dest_rect, 0, 0);
			}
			else if (picture_wanted >= 400)	{
				source_gworld = anim_gworld;
				picture_wanted -= 400;
				if (picture_wanted == 0) FillCRect(&dest_rect,map_pat[13]);
				else if (picture_wanted == 4) FillCRect(&dest_rect,map_pat[21]);
				else if (picture_wanted == 7) FillCRect(&dest_rect,map_pat[20]);
				else if (picture_wanted == 8) FillCRect(&dest_rect,map_pat[19]);
				else if (picture_wanted == 9) FillCRect(&dest_rect,map_pat[20]);
				else if (picture_wanted == 10) FillCRect(&dest_rect,map_pat[19]);
				else {
					//source_rect.left = 112 * (picture_wanted / 5);
					//source_rect.right = source_rect.left + 28;
					//source_rect.top = 36 * (picture_wanted % 5);
					//source_rect.bottom = source_rect.top + 36;
					OffsetRect(&from_rect,(picture_wanted / 5) * 24 + 1,(picture_wanted % 5) * 6 + 1 + 156);
					rect_draw_some_item(small_ter_gworld, from_rect, ter_draw_gworld, dest_rect, 0, 0);
					//rect_draw_some_item(source_gworld, source_rect, ter_draw_gworld, dest_rect, 0, 0);
				}
			}
			else {
				//source_rect = get_template_rect(terrain_to_draw);
				//source_gworld = terrain_gworld[picture_wanted / 50];
				OffsetRect(&from_rect,(picture_wanted % 10) * 6 + 1,(picture_wanted / 10) * 6 + 1);
				rect_draw_some_item(small_ter_gworld, from_rect, ter_draw_gworld, dest_rect, 0, 0);
			}
			break;
	}
}

/* Draw a bitmap in the world window. hor in 0 .. 8, vert in 0 .. 8,
 object is ptr. to bitmap to be drawn, and masking is for Copybits. */
void Draw_Some_Item ( GWorldPtr src_gworld,Rect src_rect,GWorldPtr targ_gworld,location target,
					 char masked,short main_win) {
	Rect	destrec;
	PixMapHandle	test1, test2;
	RGBColor	test_color, store_color;
	BitMap store_dest;
	
	if ((target.x < 0) || (target.x > 8) || (target.y < 0) || (target.y > 8))
		return;
	destrec.left = 8 + BITMAP_WIDTH * target.x;
	destrec.right = destrec.left + BITMAP_WIDTH;
	destrec.top = 8 + BITMAP_HEIGHT * target.y;
	destrec.bottom = destrec.top + BITMAP_HEIGHT;
	
	destrec.left = destrec.right - (src_rect.right - src_rect.left);
	destrec.top = destrec.bottom - (src_rect.bottom - src_rect.top);
	
	rect_draw_some_item(src_gworld,src_rect,ter_draw_gworld,destrec,masked,0);
	
}

/* Input terrain currently trying to draw. Get back Rect in terrain template containing 
 desired pixmap, or Rect to darkness if desired map not present */
Rect get_template_rect (unsigned char type_wanted) {
	Rect store_rect;
	short i,j;
	short picture_wanted;
	
	picture_wanted = scenario.ter_types[type_wanted].picture;
	if (picture_wanted >= 1000)
		picture_wanted = 0;
	picture_wanted = picture_wanted % 50;
	store_rect.top = 0 + (picture_wanted / 10) * BITMAP_HEIGHT;
	store_rect.bottom = store_rect.top + BITMAP_HEIGHT;
	store_rect.left = 0 + (picture_wanted % 10) * BITMAP_WIDTH;
	store_rect.right = store_rect.left + BITMAP_WIDTH;
	
	return store_rect;
}

void draw_frames() {
	char q,r,i,k;
	location which_pt;
	Rect draw_rect;
	
	for (q = 0; q < 9; q++) {
		for (r = 0; r < 9; r++)
		{
			which_pt.x = cen_x + q - 4;
			which_pt.y = cen_y + r - 4;
			draw_rect.top = 23 + r * 36;
			draw_rect.bottom = 58 + r * 36;
			draw_rect.left = 23 + q * 28;
			draw_rect.right = 50 + q * 28;
			for (i = 0; i < 4; i++)
				if ((which_pt.x == town->wandering_locs[i].x) &&
					(which_pt.y == town->wandering_locs[i].y)) {
					
					ForeColor(redColor);
					FrameRect(&draw_rect);
					ForeColor(blackColor);
				}
			for (i = 0; i < 4; i++)
				if ((which_pt.x == town->start_locs[i].x) &&
					(which_pt.y == town->start_locs[i].y)) {
					ForeColor(magentaColor);
					FrameRect(&draw_rect);
					ForeColor(blackColor);
				}	
			
			
		}
	}
	
}


void place_location() {
	Rect text_rect = {367,290,384,510};
	Str255 draw_str;
	Rect from_rect,draw_rect,source_rect,erase_rect;
	short picture_wanted;
	
	/*
	 from_rect = terrain_buttons_rect;
	 from_rect.top = from_rect.bottom - 150;
	 draw_rect = from_rect;
	 OffsetRect(&draw_rect,RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
	 rect_draw_some_item(terrain_buttons_gworld,from_rect,
	 terrain_buttons_gworld,draw_rect,0,1);
	 */
	
	//EraseRect(&text_rect);
	SetPort( terrain_buttons_gworld);
	
	erase_rect.left = 2;//terrain_rects[255].left + 17;
	erase_rect.right = RIGHT_AREA_WIDTH - 1;
	erase_rect.top = terrain_rects[255].top + 26 - 9;//12 - 9;
	erase_rect.bottom = erase_rect.top + 12;
	FillCRect(&erase_rect,bg[17]);
	
	//MoveTo(terrain_rects[255].left + 20 ,terrain_rects[255].top + 12);
	MoveTo(5 ,terrain_rects[255].top + 26);
	if (overall_mode < MODE_MAIN_SCREEN)
		sprintf((char *) draw_str,"Center: x = %d, y = %d  ",cen_x,cen_y);
	else {
		//MoveTo(5 ,terrain_rects[255].top + 28);
		sprintf((char *) draw_str,"Click terrain to edit. ",cen_x,cen_y);
	}
	c2pstr((char*) draw_str);
	DrawString(draw_str);
	//draw_cur_string();
	
	MoveTo(260 ,terrain_rects[255].top + 26);
	sprintf((char*)draw_str,"%i",current_terrain_type);
	c2pstr((char*) draw_str);
	DrawString(draw_str);
	
	erase_rect.left = 2;
	erase_rect.right = RIGHT_AREA_WIDTH - 1;
	erase_rect.top = terrain_rects[255].bottom + 117;
	erase_rect.bottom = RIGHT_AREA_HEIGHT + 6;
	FillCRect(&erase_rect,bg[17]);
	
	if (overall_mode < MODE_MAIN_SCREEN) {
		MoveTo(5,terrain_rects[255].bottom + 129);
		DrawString(current_string);
		MoveTo(RIGHT_AREA_WIDTH / 2,terrain_rects[255].bottom + 129);
		DrawString(current_string2);
	}
	
	SetPortWindowPort(mainPtr);
	
	draw_rect.top = palette_buttons[0][0].top + terrain_rects[255].bottom + 5;
	draw_rect.left = palette_buttons[9][0].right + 10; // + 17;
	draw_rect.bottom = draw_rect.top + 36;
	draw_rect.right = draw_rect.left + 28;
	picture_wanted = scenario.ter_types[current_terrain_type].picture;
	
	if (overall_mode < MODE_MAIN_SCREEN) {
		if (picture_wanted >= 1000)	{
			source_rect = get_custom_rect(picture_wanted % 1000);
			rect_draw_some_item(spec_scen_g,
								source_rect,terrain_buttons_gworld,draw_rect,0,0);
		}
		else if (picture_wanted >= 400)	{
			picture_wanted -= 400;
			source_rect.left = 112 * (picture_wanted / 5);
			source_rect.right = source_rect.left + 28;
			source_rect.top = 36 * (picture_wanted % 5);
			source_rect.bottom = source_rect.top + 36;
			rect_draw_some_item(anim_gworld,source_rect,terrain_buttons_gworld,draw_rect,0,0);
		}
		else {
			source_rect = get_template_rect(current_terrain_type);
			rect_draw_some_item(terrain_gworld[picture_wanted / 50],source_rect,
								terrain_buttons_gworld,draw_rect,0,0);
		}
		short small_i = get_small_icon(current_terrain_type);
		Rect tiny_to = draw_rect;
		tiny_to.top = tiny_to.bottom - 7;
		tiny_to.left = tiny_to.right - 7;
		Rect tiny_from = base_small_button_from;
		OffsetRect(&tiny_from,7 * (small_i % 10),7 * (small_i / 10));
		if (small_i > 0 && small_i < 255)
			rect_draw_some_item(editor_mixed,tiny_from,terrain_buttons_gworld,tiny_to,0,0);
	}
	
	draw_rect = terrain_buttons_rect;
	OffsetRect(&draw_rect,RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
	rect_draw_some_item(terrain_buttons_gworld,terrain_buttons_rect,
						terrain_buttons_gworld,draw_rect,0,1);
}

// klugde for speed ...exactly like place location above, but just writes location
void place_just_location() {
	Rect text_rect = {367,290,384,510};
	Str255 draw_str;
	Rect from_rect,draw_rect,source_rect,erase_rect;
	short picture_wanted;
	
	
	SetPort( terrain_buttons_gworld);
	
	erase_rect.left = terrain_rects[255].left + 17;
	erase_rect.right = RIGHT_AREA_WIDTH - 1;
	erase_rect.top = terrain_rects[255].top + 12 - 9;
	erase_rect.bottom = erase_rect.top + 12;
	FillCRect(&erase_rect,bg[17]);
	
	//MoveTo(terrain_rects[255].left + 20 ,terrain_rects[255].top + 12);
	MoveTo(5 ,terrain_rects[255].top + 26);
	if (overall_mode < MODE_MAIN_SCREEN)
		sprintf((char *) draw_str,"Center: x = %d, y = %d  ",cen_x,cen_y);
	else {
		//MoveTo(5 ,terrain_rects[255].top + 28);
		sprintf((char *) draw_str,"Click terrain to edit. ");
	}
	c2pstr((char*) draw_str);
	DrawString(draw_str);
	
	SetPortWindowPort(mainPtr);
	
	
	from_rect = terrain_buttons_rect;
	from_rect.top = erase_rect.top;
	from_rect.bottom = erase_rect.bottom;
	draw_rect = from_rect;
	OffsetRect(&draw_rect,RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
	rect_draw_some_item(terrain_buttons_gworld,from_rect,
						terrain_buttons_gworld,draw_rect,0,1);
}

void set_string(char *string,char *string2) {
	strcpy((char *)current_string,string);
	c2pstr((char*) current_string);
	//	if (strlen(string2) == 0)
	//		current_string2[0] = 0;
	//		else 
	//	sprintf((char *)current_string2,"Bob");
	strcpy((char *)current_string2,string2);
	c2pstr((char*) current_string2);
	
	place_location();
}

/*
 void draw_cur_string() {
 Rect from_rect,draw_rect;
 
 from_rect = terrain_buttons_rect;
 from_rect.top = from_rect.bottom - 40;
 draw_rect = from_rect;
 OffsetRect(&draw_rect,RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
 rect_draw_some_item(terrain_buttons_gworld,from_rect,
 terrain_buttons_gworld,draw_rect,0,1);
 MoveTo(RIGHT_AREA_UL_X + 5,terrain_rects[255].bottom + 120);
 DrawString(current_string);
 MoveTo(RIGHT_AREA_UL_X + 5,terrain_rects[255].bottom + 132);
 DrawString(current_string2);
 } */

void undo_clip() {
	Rect c = {0,0,480,640};
	
	ClipRect(&c);
}


bool is_special(short i,short j) {
	short k;
	
	if (editing_town == true)
		for (k = 0; k < 50; k++)
			if ((town->special_locs[k].x == i) && (town->special_locs[k].y == j))
				return true;
	if (editing_town == false)
		for (k = 0; k < 18; k++)
			if ((current_terrain.special_locs[k].x == i) && (current_terrain.special_locs[k].y == j))
				return true;
	
	return false;
}

void take_special(short i,short j) {
}

void make_special(short i,short j) {
}

void sort_specials() {
}

bool is_field_type(short i,short j,short field_type) {
	short k;
	
	for (k = 0; k < town->preset_fields.size(); k++)
		if ((town->preset_fields[k].type == field_type) &&
			(town->preset_fields[k].loc.x == i) &&
			(town->preset_fields[k].loc.y == j))
			return true;
	return false;
}

void make_field_type(short i,short j,short field_type) {
	short k;
	
	if (is_field_type(i,j,field_type) == true)
		return;
	for (k = 0; k < town->preset_fields.size(); k++)
		if (town->preset_fields[k].type == 0) {
			town->preset_fields[k].loc.x = i;
			town->preset_fields[k].loc.y = j;
			town->preset_fields[k].type = field_type;
			return;
		}
	//give_error("Each town can have at most 50 fields and special effects (webs, barrels, blood stains, etc.). To place more, use the eraser first.","",0);
	cTown::cField the_field;
	the_field.loc.x = i;
	the_field.loc.y = j;
	the_field.type = field_type;
	town->preset_fields.push_back(the_field);
}


void take_field_type(short i,short j,short field_type) {
	short k;
	
	for (k = 0; k < town->preset_fields.size(); k++)
		if ((town->preset_fields[k].type == field_type) &&
			(town->preset_fields[k].loc.x == i) &&
			(town->preset_fields[k].loc.y == j)) {
			town->preset_fields[k].type = 0;
			return;
		}
}

bool is_web(short i,short j) {
	return is_field_type(i,j,3);
	
}
void make_web(short i,short j) {
	make_field_type(i,j,3);
}
void take_web(short i,short j) {
	take_field_type(i,j,3);
}

bool is_crate(short i,short j) {
	return is_field_type(i,j,4);
}
void make_crate(short i,short j) {
	make_field_type(i,j,4);
	
}
void take_crate(short i,short j) {
	take_field_type(i,j,4);
}

bool is_barrel(short i,short j) {
	return is_field_type(i,j,5);
}
void make_barrel(short i,short j) {
	make_field_type(i,j,5);
}
void take_barrel(short i,short j) {
	take_field_type(i,j,5);
}

bool is_fire_barrier(short i,short j) {
	return is_field_type(i,j,6);
}
void make_fire_barrier(short i,short j) {
	make_field_type(i,j,6);
}
void take_fire_barrier(short i,short j) {
	take_field_type(i,j,6);
}

bool is_force_barrier(short i,short j) {
	return is_field_type(i,j,7);
}
void make_force_barrier(short i,short j) {
	make_field_type(i,j,7);
}
void take_force_barrier(short i,short j) {
	take_field_type(i,j,7);
	
}

bool is_sfx(short i,short j,short type) {
	return is_field_type(i,j,type + 14);
}
void make_sfx(short i,short j,short type) {
	make_field_type(i,j,type + 14);
}
void take_sfx(short i,short j,short type) {
	take_field_type(i,j,type + 14);
}


bool is_quickfire(short i,short j) {
	return is_field_type(i,j,8);
}
void make_quickfire(short i,short j) {
	make_field_type(i,j,8);
}
void take_quickfire(short i,short j) {
	take_field_type(i,j,8);
}

bool container_there(location l) {
	
	unsigned char ter;
	
	if (editing_town == false)
		return false;
	if (scenario.ter_types[town->terrain(l.x,l.y)].special == TER_SPEC_IS_A_CONTAINER)
		return true;
	if (is_barrel(l.x,l.y) == true)
		return true;
	if (is_crate(l.x,l.y) == true)
		return true;
	return 0;
}

//void get_str(Str255 str,short i, short j)
//{
//	if (i == -1) {
//		strcpy((char *) str,data_store->scen_item_list.monst_names[j]);
//		return;
//	}
//	if (i == -2) {
//		strcpy((char *) str,data_store->scen_item_list.scen_items[j].full_name);
//		return;
//	}
//	if (i == -3) {
//		strcpy((char *) str,buttons[available_dlog_buttons[j]].str);
//		return;
//	}
//	if (i == -4) {
//		strcpy((char *) str,data_store->scen_item_list.ter_names[j]);
//		return;
//	}
//	if (i == -5) {
//		get_str(str,40,j * 7 + 1);
//		return;
//	}
//	GetIndString(str, i, j);
//	p2cstr(str);
//}
void record_display_strings(){}
