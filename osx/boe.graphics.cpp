
#include <cstring>
#include <cstdio>

//#include "item.h"

#include "boe.global.h"

#include "classes.h"
#include "boe.graphics.h"
#include "boe.newgraph.h"
#include "boe.graphutil.h"
#include "boe.monster.h"
#include "dlgtool.h"
#include "boe.locutils.h"
#include "boe.fields.h"
#include "boe.text.h"
#include "graphtool.h"
#include "soundtool.h"
#include "gamma.h"
#include "mathutil.h"
#include "dlgutil.h"

#include "boe.party.h"
#include "boe.town.h"
#include "boe.items.h"
#include "boe.dlgutil.h"
#include "boe.infodlg.h"

extern WindowPtr mainPtr;
extern Rect	windRect;
extern short stat_window,give_delays;
extern eGameMode overall_mode;
extern short current_spell_range,town_type,store_anim_type;
extern bool in_startup_mode,anim_onscreen,play_sounds,frills_on,startup_loaded,party_in_memory;
//extern short town_size[3];
extern short anim_step;
//extern party_record_type univ.party;
//extern big_tr_type t_d;
//extern cOutdoors univ.out.outdoors[2][2];
//extern current_town_type univ.town;
//extern town_item_list t_i;
//extern unsigned char univ.out[96][96],out_e[96][96];
extern ter_num_t combat_terrain[64][64];
extern effect_pat_type current_pat;
extern bool web,crate,barrel,fire_barrier,force_barrier,quickfire,force_wall,fire_wall,antimagic,scloud,ice_wall,blade_wall;
extern Point ul;
extern location pc_pos[6],pc_dir[6],center;
extern short which_combat_type,current_pc;
extern bool monsters_going,boom_anim_active,cartoon_happening,skip_boom_delay;
extern 	PicHandle	spell_pict;
extern short current_ground;
extern short terrain_pic[256];
//extern short pc_moves[6];
extern short num_targets_left;
extern location spell_targets[8];
extern short display_mode;
extern ControlHandle text_sbar,item_sbar,shop_sbar;
extern Rect sbar_rect,item_sbar_rect,shop_sbar_rect,startup_top;
extern Rect talk_area_rect, word_place_rect;
extern PixPatHandle map_pat[25];
extern Point store_anim_ul;
extern long register_flag;
extern long ed_flag,ed_key;
extern bool fast_bang;
//extern unsigned char m_pic_index[200];
extern PixPatHandle	bg[];
extern KeyMap key_state;
extern bool fry_startup;
//extern piles_of_stuff_dumping_type *data_store;
extern cScenario scenario;
extern cUniverse univ;
//extern outdoor_strs_type outdoor_text[2][2];
extern GWorldPtr spec_scen_g;
extern DialogPtr modeless_dialogs[18];
extern bool modeless_exists[18];
//extern dlg_filter_t give_password_filter,pick_prefab_scen_event_filter,pick_a_scen_event_filter,tip_of_day_event_filter;
//extern dlg_filter_t talk_notes_event_filter,adventure_notes_event_filter,journal_event_filter,display_strings_event_filter;
//extern dlg_filter_t display_strings_event_filter,display_strings_event_filter,display_strings_event_filter;
//extern dlg_filter_t display_item_event_filter,pick_trapped_monst_event_filter,edit_party_event_filter,display_pc_event_filter;
//extern dlg_filter_t display_alchemy_event_filter,display_help_event_filter,display_pc_item_event_filter,display_monst_event_filter;
//extern dlg_filter_t spend_xp_event_filter,get_num_of_items_event_filter,pick_race_abil_event_filter,sign_event_filter;
//extern dlg_filter_t get_text_response_event_filter,get_text_response_event_filter,select_pc_event_filter;
//extern dlg_filter_t give_pc_info_event_filter,alch_choice_event_filter,pc_graphic_event_filter,pc_name_event_filter;
//extern dlg_filter_t give_reg_info_event_filter,do_registration_event_filter,display_spells_event_filter;
//extern dlg_filter_t display_skills_event_filter,pick_spell_event_filter,prefs_event_filter,fancy_choice_dialog_event_filter;
extern bool show_startup_splash;

RgnHandle clip_region;

PaletteHandle new_palette,old_palette,wank_palette;
//***********************
Rect		menuBarRect;
short		menuBarHeight;
RgnHandle	originalGrayRgn, newGrayRgn, underBarRgn;

short terrain_there[9][9]; // this is an optimization variabel. Keeps track of what terrain
	// is in the terrain spot, so things don't get redrawn. 
	// 0 - 299 just terrain graphic in place
	// 300 - blackness
	// -1 - nothign worth saving 
short startup_anim_pos = 43;

long anim_ticks = 0;

// 0 - terrain   1 - buttons   2 - pc stats 
// 3 - item stats   4 - text bar   5 - text area (not right)
Rect win_from_rects[6] = {{0,0,351,279},{0,0,37,258},{0,0,115,288},{0,0,143,288},{0,0,21,279},{0,0,0,288}};
Rect win_to_rects[6] = {{5,5,356,284},{383,5,420,263},{0,0,116,271},{0,0,144,271},{358,5,379,284},{0,0,138,256}};

// 0 - title  1 - button  2 - credits  3 - base button
Rect startup_from[4] = {{0,0,274,602},{274,0,322,301},{0,301,67,579},{274,301,314,341}}; ////
extern Rect startup_button[6];
	
//	Rect trim_rects[8] = {{0,0,5,28},{31,0,36,28},{0,0,36,5},{0,24,36,28},
//						{0,0,5,5},{0,24,5,28},{31,24,36,28},{31,0,36,5}};

Rect	top_left_rec = {0,0,36,28};
short which_graphic_index[6] = {50,50,50,50,50,50};

char combat_graphics[5] = {28,29,36,79,2};
short debug_nums[6] = {0,0,0,0,0,0};					     
short remember_tiny_text = 300; // Remembers what's in the tiny text-bar, to prevent redrawing.
	// 50 indicates area name, other number indicates which-rect.
	// Add 200 if last mess. was in town

char light_area[13][13];
char unexplored_area[13][13];

// Declare the graphics
GWorldPtr status_gworld = NULL;
GWorldPtr invenbtn_gworld = NULL;
GWorldPtr vehicle_gworld = NULL;
GWorldPtr pc_stats_gworld = NULL;
GWorldPtr item_stats_gworld = NULL;
GWorldPtr text_area_gworld = NULL;
GWorldPtr storage_gworld = NULL; // TODO: Abolish
GWorldPtr terrain_screen_gworld = NULL;
GWorldPtr text_bar_gworld = NULL;
GWorldPtr orig_text_bar_gworld = NULL;
GWorldPtr buttons_gworld = NULL;
GWorldPtr party_template_gworld = NULL;
GWorldPtr items_gworld = NULL;
GWorldPtr tiny_obj_gworld = NULL;
GWorldPtr fields_gworld = NULL;
GWorldPtr boom_gworld = NULL;
GWorldPtr roads_gworld = NULL;
GWorldPtr map_gworld = NULL;
GWorldPtr small_ter_gworld = NULL;
GWorldPtr missiles_gworld = NULL;
GWorldPtr dlogpics_gworld = NULL;
GWorldPtr anim_gworld = NULL;
GWorldPtr talkfaces_gworld = NULL;
GWorldPtr pc_gworld = NULL;
GWorldPtr monst_gworld[11] = {0};
GWorldPtr terrain_gworld[7] = {0};

// Startup graphics, will die when play starts
GWorldPtr startup_gworld = NULL;
GWorldPtr startup_button_orig = NULL;
GWorldPtr startup_button_g = NULL;
GWorldPtr anim_mess = NULL;

// Graphics storage vals
short which_g_stored[STORED_GRAPHICS];
// 0 - 299 terrain graphic   300 + i  monster graphic i, face right  600 + i face left
// 1000 + 100 * j + i   anim i pos j    2300 + i  combat monster graphic i, face right  2600 + i face left
short wish_list[STORED_GRAPHICS];
short storage_status[STORED_GRAPHICS]; // 0 - empty 1 - in use 2 - there, not in use

bool has_run_anim = false,currently_loading_graphics = false;
//short anim_step = 0;
//short overall_anim_step = 0;

Rect main_win_rect = {0,0,410,250};
Rect main_win2_source_rect = {0,0,410,265};
Rect main_win2_rect = {0,250,410,515};

Rect tiny_world_1_source_rect = {0,0,190,145},
	tiny_world_1_rect = {195,242,385,475};

Rect share_mess_source_rect = {0,0,59,120},
	share_mess_rect = {120,384,179,504};
//Rect start_buttons_source_rect = {0,0,180,180},
//	start_buttons_rect = {224,30,405,210};
Rect start_buttons_source_rect = {0,0,186,190},
	start_buttons_rect = {214,30,400,220};

// Variables to store trim. Makes game faster, but wastes 15K. We'll see how it works...
//char out_trim[96][96],town_trim[64][64];

// Array to store which spots have been seen. Time-saver for drawing fields
char spot_seen[9][9];

char anim_str[60];
location anim_str_loc;

PatHandle bw_pats[15];

extern short combat_posing_monster , current_working_monster ; // 0-5 PC 100 + x - monster x
bool supressing_some_spaces = false;
location ok_space[4] = {loc(),loc(),loc(),loc()};
	PicHandle	hold_pict;
	PictInfo p,p2;



void init_dialogs(){
	cd_init_dialogs(
		&anim_gworld,
		&talkfaces_gworld,
		&items_gworld,
		&tiny_obj_gworld,
		&pc_gworld,
		&dlogpics_gworld,
		monst_gworld,
		terrain_gworld,
		&small_ter_gworld,
		&fields_gworld,
		&pc_stats_gworld,
		&item_stats_gworld,/*
		&text_area_gworld,
		&storage_gworld,
		&terrain_screen_gworld,
		&text_bar_gworld,
		&orig_text_bar_gworld,
		&buttons_gworld,
		&party_template_gworld,*/
		NULL,
		&spec_scen_g
	);
	//cd_register_event_filter(823,give_password_filter);
	cd_register_event_filter(869,pick_prefab_scen_event_filter);
	cd_register_event_filter(947,pick_a_scen_event_filter);
	cd_register_event_filter(958,tip_of_day_event_filter);
	cd_register_event_filter(960,talk_notes_event_filter);
	cd_register_event_filter(961,adventure_notes_event_filter);
	cd_register_event_filter(962,journal_event_filter);
	cd_register_event_filter(987,display_item_event_filter);
	cd_register_event_filter(988,pick_trapped_monst_event_filter);
	cd_register_event_filter(989,edit_party_event_filter);
	cd_register_event_filter(991,display_pc_event_filter);
	cd_register_event_filter(996,display_alchemy_event_filter);
	cd_register_event_filter(997,display_help_event_filter);
	cd_register_event_filter(998,display_pc_item_event_filter);
	cd_register_event_filter(999,display_monst_event_filter);
	cd_register_event_filter(1010,spend_xp_event_filter );
	cd_register_event_filter(1012,get_num_of_items_event_filter );
	cd_register_event_filter(1013,pick_race_abil_event_filter );
	cd_register_event_filter(1014,sign_event_filter );
	cd_register_event_filter(873,get_text_response_event_filter );
	cd_register_event_filter(1017,get_text_response_event_filter );
	cd_register_event_filter(1018,select_pc_event_filter );
	cd_register_event_filter(1019,give_pc_info_event_filter);
	cd_register_event_filter(1047,alch_choice_event_filter);
	cd_register_event_filter(1050,pc_graphic_event_filter);
	cd_register_event_filter(1051,pc_name_event_filter);
	cd_register_event_filter(1073,give_reg_info_event_filter );
	cd_register_event_filter(1075,do_registration_event_filter );
	cd_register_event_filter(1096,display_spells_event_filter );
	cd_register_event_filter(1097,display_skills_event_filter );
	cd_register_event_filter(1098,pick_spell_event_filter );
	cd_register_event_filter(1099,prefs_event_filter );
	cd_register_default_event_filter(fancy_choice_dialog_event_filter);
	//return &tmp;
}

void adjust_window_mode()
{
	Rect r;

	if (display_mode == 5) {
		ul.h = 14; ul.v = 2;
		SizeWindow(mainPtr,605,430, true);
		MoveWindow(mainPtr,(windRect.right - 573) / 2,(windRect.bottom - 430) / 2 + 20,true);
		GetWindowPortBounds(mainPtr, &r);
	}
	else {
			MoveWindow(mainPtr,-6,-6,true);	
			SizeWindow(mainPtr,windRect.right + 12, windRect.bottom + 12, true);
			switch (display_mode) {
				case 0: ul.h = (windRect.right - 560) / 2; ul.v = (windRect.bottom - 422) / 2 + 14; break;
				case 1:	ul.h = 10; ul.v = 28; break;
				case 2: ul.h = windRect.right - 570 - 6; ul.v = 28;	break; // was 560. not 570
				case 3: ul.h = 10; ul.v = windRect.bottom - 422	- 6; break;
				case 4: ul.h = windRect.right - 570 - 6; ul.v = windRect.bottom - 422	- 6; break;
				}

			}
	create_clip_region();
	undo_clip();
	if (overall_mode != MODE_STARTUP) { // TODO: This is odd – fix it
		if (in_startup_mode == true)
			draw_startup(0);
		if (in_startup_mode == false)
			//refresh_screen(0); 
			redraw_screen();
		}
	if (text_sbar != NULL) {
		MoveControl(text_sbar,ul.h + 546,ul.v + 283);
		MoveControl(item_sbar,ul.h + 546,ul.v + 146);
		MoveControl(shop_sbar,ul.h + 258,ul.v + 67);
		}
}


void plop_fancy_startup()
{
	GWorldPtr	pict_to_draw;
	short i,j;
	
	GDHandle md;
	
	Rect whole_window,from_rect;
	Rect logo_from = {0,0,350,350};
	Rect intro_from = {0,0,483,643};
	GDHandle	mainScreen;
	
	// store current gray region that displays
	// menu bar and the current height of the menu bar
//	originalGrayRgn 	= LMGetGrayRgn(); 		
//	menuBarHeight	= LMGetMBarHeight(); 	

	// calculate the rect of the menu bar to test 
	// if mouse down is in if desired
	mainScreen 			= GetMainDevice();
	menuBarRect 			= (**mainScreen).gdRect;
	menuBarRect.bottom 	= menuBarHeight;
	
	md = GetMainDevice();
	
	if (fry_startup == false)  { // TODO: What on earth is this for?
		//color_pat = GetCTable(72);
		//old_palette = NewPalette(256,color_pat,pmTolerant + pmExplicit,0x0000);
		//GetPictInfo(pict_to_draw,&p2,1,256,0,0);
		//old_palette = NewPalette(256,color_pat,pmTolerant + pmExplicit,0x0000);
		//old_palette = NewPalette(256,p2.theColorTable,pmTolerant + pmExplicit,0x0000);
		//NSetPalette(mainPtr,old_palette,pmAllUpdates);
		//ActivatePalette(mainPtr);

		/*HLock((Handle) md);
		hPixMap = (*md)->gdPMap;
		HLock((Handle)hPixMap);
		hColorTable = (*hPixMap)->pmTable;
		old_palette = NewPalette(256,hColorTable,pmTolerant + pmExplicit,0x0000);
		HUnlock((Handle)hPixMap);
		HUnlock((Handle) md); */
		wank_palette = NewPalette(256,NULL,pmTolerant + pmExplicit,0x0000);

		//HideMenuBar();
		/*
		GetPort(&oldPort);
		GetDeskTopGrafPort(&wMgrPort);
		SetPort(wMgrPort);
		oldClip = NewRgn();
		GetClip(oldClip);
		ClipRect(&wMgrPort->portRect);
		r = wMgrPort->portRect;
		GetForeColor(&store_color);
		ForeColor(blackColor);
		r.bottom = LMGetMBarHeight();
		PaintRect(&r);
		RGBForeColor(&store_color);
		SetClip(oldClip);
		DisposeRgn(oldClip);
		SetPort(oldPort);
		*/
		}
	// initialize buffers and rects TODO: Not needed
	for (i = 0; i < STORED_GRAPHICS; i++) {
		which_g_stored[i] = (i < 50) ? i : 0;
		wish_list[i] = 0;
		storage_status[i] = (i < 50) ? 1 : 0; 
		}
//	for (i = 0;i < 8; i++)
//		OffsetRect(&trim_rects[i],61,37);
	for (i = 0; i < 9; i++)
		for (j = 0; j < 9; j++)
			terrain_there[i][j] = -1;
			
	OffsetRect(&win_to_rects[5],TEXT_WIN_UL_X,TEXT_WIN_UL_Y);
	OffsetRect(&win_to_rects[2],PC_WIN_UL_X,PC_WIN_UL_Y);
	OffsetRect(&win_to_rects[3],ITEM_WIN_UL_X,ITEM_WIN_UL_Y);
		
	GetWindowPortBounds(mainPtr, &whole_window);
//	whole_window = mainPtr->portRect;
	ForeColor(blackColor);
	PaintRect(&whole_window);
	
	OffsetRect(&logo_from,(whole_window.right - logo_from.right) / 2,(whole_window.bottom - logo_from.bottom) / 2);
//	pict_to_draw = GetPicture(3001); // TODO: Read from file instead
//	DrawPicture(pict_to_draw, &logo_from);
//	ReleaseResource((Handle) pict_to_draw);
	pict_to_draw = load_pict("spidlogo.png");
	GetPortBounds(pict_to_draw,&from_rect);
	rect_draw_some_item(pict_to_draw, from_rect, pict_to_draw, logo_from, 0, 1);
	DisposeGWorld(pict_to_draw);
	
	RgnHandle wholeRegion = NewRgn();
	GetWindowRegion(mainPtr, kWindowContentRgn, wholeRegion);
	QDFlushPortBuffer(GetWindowPort(mainPtr), wholeRegion);
	
	
	play_sound(-95);

	//Delay(30,&dummy);

	for (i = 0; i < 14; i++) 
	    bg[i] = GetPixPat (128 + i);
	for (i = 0; i < 25; i++) 
	    map_pat[i] = GetPixPat (200 + i);

	// Do bulk of graphics loading!!!

	Set_up_win();

	init_startup();
	
	int delay = 220;
	if(show_startup_splash){
		//init_anim(0);
		//pict_to_draw = GetPicture(3000); // TODO: Read from file instead

		if (fry_startup == false) {
			//for(i=100; i >= 0; i-=2) DoOneGammaFade(md, i);
		
//			PaintRect(&whole_window);
//			// load and switch to new palette
//			GetPictInfo(pict_to_draw,&p,2,256,0,0);
//			new_palette = p.thePalette;
//			SetPalette(mainPtr,new_palette,false);
//			ActivatePalette(mainPtr);
// reerase menu
/*	GetPort(&oldPort);
	GetDeskTopGrafPort(&wMgrPort);
	SetPort(wMgrPort);
	oldClip = NewRgn();
	GetClip(oldClip);
	ClipRect(&wMgrPort->portRect);
	r = wMgrPort->portRect;
	GetForeColor(&store_color);
	ForeColor(blackColor);
	r.bottom = LMGetMBarHeight();
	PaintRect(&r);
	RGBForeColor(&store_color);
	SetClip(oldClip);
	DisposeRgn(oldClip);
	SetPort(oldPort);	*/
		}
// menu erased
	
		PaintRect(&whole_window);
		OffsetRect(&intro_from,(whole_window.right - intro_from.right) / 2,(whole_window.bottom - intro_from.bottom) / 2);
//		DrawPicture(pict_to_draw, &intro_from);
		//		ReleaseResource((Handle) pict_to_draw);	
		pict_to_draw = load_pict("startsplash.png");
		GetPortBounds(pict_to_draw,&from_rect);
		rect_draw_some_item(pict_to_draw, from_rect, pict_to_draw, intro_from, 0, 1);
		DisposeGWorld(pict_to_draw);

//	NSetPalette(mainPtr,old_palette,pmAllUpdates);
//	ActivatePalette(mainPtr);
	} else delay = 60;
	QDFlushPortBuffer(GetWindowPort(mainPtr), wholeRegion);
	if (fry_startup == false) {
		
		play_sound(-22);
		EventRecord event;
		WaitNextEvent(mDownMask + keyDownMask + mUpMask,&event,delay,NULL);
	}
	
	DisposeRgn(wholeRegion);
}

void fancy_startup_delay()
{

	GDHandle md;
	PaletteHandle default_palette;
	
	md = GetMainDevice();
	
	if (fry_startup == false){
		//ShowMenuBar();
//		LMSetGrayRgn( originalGrayRgn );	
		SetPalette(mainPtr,wank_palette,false);
		ActivatePalette(mainPtr);
		default_palette = GetPalette((WindowPtr) -1);
		NSetPalette(mainPtr,default_palette,pmAllUpdates);
		ActivatePalette(mainPtr);
		RestoreDeviceClut(md);
//		NSetPalette(mainPtr,old_palette,pmAllUpdates);
//		ActivatePalette(mainPtr);
//		DisposePalette(p.thePalette);
		}
	
	// OK try this ... destroy and recreate window.
	/*DisposeWindow(mainPtr);
	mainPtr = GetNewCWindow(128,NULL,IN_FRONT);
	SetPort(mainPtr);						/* set window to current graf port 
	text_sbar = NewControl(mainPtr,&sbar_rect,tit,false,58,0,58,scrollBarProc,1);
	item_sbar = NewControl(mainPtr,&item_sbar_rect,tit,false,0,0,16,scrollBarProc,2);
	shop_sbar = NewControl(mainPtr,&shop_sbar_rect,tit,false,0,0,16,scrollBarProc,3);
	adjust_window_mode();
	RestoreDeviceClut(GetMainDevice()); */
	
	TextFace(0);
	
	draw_startup(0);

	
	FlushEvents(mDownMask,0);				
	FlushEvents(keyDownMask,0);
}

void init_startup()
{
	startup_loaded = true;

//	startup_gworld = load_pict(830);
//	startup_button_orig = load_pict(832);
//	startup_button_g = load_pict(832);
//	anim_mess = load_pict(831);
	startup_gworld = load_pict("startup.png");
	startup_button_orig = load_pict("startbut.png");
	startup_button_g = load_pict("startbut.png");
	anim_mess = load_pict("startanim.png");
}

void init_animation()
{}

void next_animation_step() 
{}


void draw_startup(short but_type)
{
	Rect to_rect;
	Rect r1 = {-1000,-1000,5,1000},r2 = {-1000,-1000,1000,5},r3 = {418,-1000,2000,1000},
		r4 = {-1000,579,1000,2500};
	short i;
	GrafPtr old_port;
	
	if (startup_loaded == false)
		return;
	
	GetPort(&old_port);	
	SetPort(GetWindowPort(mainPtr));
	r1.bottom = ul.v + 5;
	FillCRect(&r1,bg[4]);
	r2.right = ul.h - 13;
	FillCRect(&r2,bg[4]);
	r3.top += ul.v + 5;
	FillCRect(&r3,bg[4]);
	r4.left += ul.h - 13;
	FillCRect(&r4,bg[4]);
	to_rect = startup_from[0];
	OffsetRect(&to_rect,-13,5);
	rect_draw_some_item(startup_gworld,startup_from[0],startup_gworld,to_rect,0,1);
	to_rect = startup_top;
	OffsetRect(&to_rect,ul.h,ul.v);
	//PaintRect(&to_rect);
	
	for (i = 0; i < 5; i++) {
		rect_draw_some_item(startup_gworld,startup_from[1],startup_gworld,startup_button[i],0,1);
		draw_start_button(i,but_type);
		}
	draw_startup_anim();
	
	draw_startup_stats();
	SetPort(old_port);
}

void draw_startup_anim()
{
	Rect anim_to = {4,1,44,276},anim_from;
	Rect anim_size = {0,0,48,301};
	
	anim_from = anim_to;
	OffsetRect(&anim_from,-1,-4 + startup_anim_pos);
	startup_anim_pos = (startup_anim_pos + 1) % 542;
	rect_draw_some_item(startup_button_orig,anim_size,startup_button_g,anim_size,0,0);
	rect_draw_some_item(anim_mess,anim_from,startup_button_g,anim_to,1,0);
	rect_draw_some_item(startup_button_g,anim_size,startup_button_g,startup_button[5],0,1);
}

void draw_startup_stats()
{
	Rect from_rect,to_rect,party_from = {0,0,36,28},pc_rect,frame_rect;
	short i;
	Str255 str;
	
	TextFont(dungeon_font_num);
	TextFace(0);
	TextSize(24);

	to_rect = startup_top;
	OffsetRect(&to_rect, 20 - 18, 35);

	ForeColor(whiteColor);

	if (party_in_memory == false) {
		TextSize(20);
		OffsetRect(&to_rect,175,40);
		char_win_draw_string(mainPtr,to_rect,
			"No Party in Memory",0,18,true);
		}
	if (party_in_memory == true) {
		frame_rect = startup_top;
		InsetRect(&frame_rect,50,50);
		frame_rect.top += 30;
		ForeColor(whiteColor);
		OffsetRect(&frame_rect,ul.h - 9,ul.v + 10);
		FrameRect(&frame_rect);

		OffsetRect(&to_rect,203,37);
		char_win_draw_string(mainPtr,to_rect,
			"Your party:",0,18,true);
		TextSize(12);	
		TextFace(bold);
		TextFont(geneva_font_num);
		for (i = 0; i < 6; i++) {
			pc_rect = startup_top;
			pc_rect.right = pc_rect.left + 300;
			pc_rect.bottom = pc_rect.top + 79;
			OffsetRect(&pc_rect,60 + 232 * (i / 3) - 9,95 + 45 * (i % 3));
			
			if (univ.party[i].main_status > 0) {
				from_rect = party_from;
				OffsetRect(&from_rect,56 * (i / 3),36 * (i % 3));
				to_rect = party_from,
				OffsetRect(&to_rect,pc_rect.left,pc_rect.top);
				rect_draw_some_item(party_template_gworld,from_rect,party_template_gworld,to_rect,1,1);

				TextSize(14);	
				OffsetRect(&pc_rect,35,0);
				char_win_draw_string(mainPtr,pc_rect,
					(char*)univ.party[i].name.c_str(),0,18,true);
				OffsetRect(&to_rect,pc_rect.left + 8,pc_rect.top + 8);
				
				}
			TextSize(12);	
			OffsetRect(&pc_rect,12,16);
			switch (univ.party[i].main_status) {
				case MAIN_STATUS_ALIVE:
					switch (univ.party[i].race) {
						case RACE_HUMAN: sprintf((char *) str,"Level %d Human",univ.party[i].level); break;
						case RACE_NEPHIL: sprintf((char *) str,"Level %d Nephilim",univ.party[i].level); break;
						case RACE_SLITH: sprintf((char *) str,"Level %d Slithzerikai",univ.party[i].level); break;
						case RACE_VAHNATAI: sprintf((char *) str,"Level %d Vahnatai",univ.party[i].level); break;
						case RACE_REPTILE: sprintf((char *) str,"Level %d Reptile",univ.party[i].level); break;
						case RACE_BEAST: sprintf((char *) str,"Level %d Beast",univ.party[i].level); break;
						case RACE_IMPORTANT: sprintf((char *) str,"Level %d V.I.P.",univ.party[i].level); break;
						case RACE_MAGE: sprintf((char *) str,"Level %d Human Mage",univ.party[i].level); break;
						case RACE_PRIEST: sprintf((char *) str,"Level %d Human Priest",univ.party[i].level); break;
						case RACE_HUMANOID: sprintf((char *) str,"Level %d Humanoid",univ.party[i].level); break;
						case RACE_DEMON: sprintf((char *) str,"Level %d Demon",univ.party[i].level); break;
						case RACE_UNDEAD: sprintf((char *) str,"Level %d Undead",univ.party[i].level); break;
						case RACE_GIANT: sprintf((char *) str,"Level %d Giant",univ.party[i].level); break;
						case RACE_SLIME: sprintf((char *) str,"Level %d Slime",univ.party[i].level); break;
						case RACE_STONE: sprintf((char *) str,"Level %d Golem",univ.party[i].level); break;
						case RACE_BUG: sprintf((char *) str,"Level %d Bug",univ.party[i].level); break;
						case RACE_DRAGON: sprintf((char *) str,"Level %d Dragon",univ.party[i].level); break;
						case RACE_MAGICAL: sprintf((char *) str,"Level %d Magical Creature",univ.party[i].level); break;
						case RACE_PLANT: sprintf((char *) str,"Level %d Plant",univ.party[i].level); break;
						case RACE_BIRD: sprintf((char *) str,"Level %d Bird",univ.party[i].level); break;
						default: sprintf((char *) str,"Level %d *ERROR INVALID RACE*",univ.party[i].level); break;
						}
					char_win_draw_string(mainPtr,pc_rect,(char *) str,0,18,true);
					OffsetRect(&pc_rect,0,13);
					sprintf((char *) str,"Health %d, Spell pts. %d",
						univ.party[i].max_health,univ.party[i].max_sp);
					char_win_draw_string(mainPtr,pc_rect,(char *) str,0,18,true);
					break;
				case MAIN_STATUS_DEAD:
					char_win_draw_string(mainPtr,pc_rect,"Dead",0,18,true);
					break;
				case MAIN_STATUS_DUST:
					char_win_draw_string(mainPtr,pc_rect,"Dust",0,18,true);
					break;
				case MAIN_STATUS_STONE:
					char_win_draw_string(mainPtr,pc_rect,"Stone",0,18,true);
					break;
				case MAIN_STATUS_FLED:
					char_win_draw_string(mainPtr,pc_rect,"Fled",0,18,true);
					break;
				default: //absent, and all variations thereof; do nothing
					break;
				}
			}
		}
	
	TextFont(geneva_font_num);
	TextSize(10);	
	TextFace(bold);
	pc_rect = startup_from[0];
	OffsetRect(&pc_rect,5,5);
	pc_rect.top = pc_rect.bottom - 25;
	pc_rect.left = pc_rect.right - 300;
	char_win_draw_string(mainPtr,pc_rect,"Copyright 1997, All Rights Reserved, v1.0.2",0,18,true);
		
	ForeColor(blackColor);
}



void draw_start_button(short which_position,short which_button)
{
	Rect from_rect,to_rect;
	char *button_labels[] = {"Load Game","Make New Party","How To Order",
		"Start Scenario","Custom Scenario","Quit"};
	RGBColor base_color = {0,0,14472};
	//RGBColor lit_color = {0,0,14472},base_color = {0,0,43144};
	
	from_rect = startup_from[3];
	OffsetRect(&from_rect, (which_button > 0) ? 40 : 0,0);
	to_rect = startup_button[which_position];
	to_rect.left += 4; to_rect.top += 4;
	to_rect.right = to_rect.left + 40;
	to_rect.bottom = to_rect.top + 40;
	rect_draw_some_item(startup_gworld,from_rect,startup_gworld,to_rect,0,1);
	
	TextFont(dungeon_font_num);
	TextFace(0);
	TextSize(24);
	to_rect = startup_button[which_position];
	//to_rect.left += 80;
	OffsetRect(&to_rect, 10, 10);
	if (which_button == 5)
		which_button = 4;
	base_color.blue = 14472 + (12288 * which_button);
	RGBForeColor(&base_color);
	if (which_position == 3)
		OffsetRect(&to_rect,-7,0);
	char_win_draw_string(mainPtr,to_rect,
		(char *) button_labels[which_position],1,18,true);
	ForeColor(blackColor);
	TextFont(geneva_font_num);
	TextFace(bold);
	TextSize(10);
	
}

void main_button_click(short mode,Rect button_rect)
{
	const BitMap *	store2;

	store2 = GetPortBitMapForCopyBits(GetWindowPort(mainPtr));

	OffsetRect(&button_rect,ul.h,ul.v);
	ClipRect(&button_rect);

	draw_buttons(1);
	if (play_sounds == true)
		play_sound(37);
		else FlushAndPause(5);
	draw_buttons(0);			
	undo_clip();
}

void arrow_button_click(Rect button_rect)
{
	const BitMap *	store2;
	
	store2 = GetPortBitMapForCopyBits(GetWindowPort(mainPtr));

	OffsetRect(&button_rect,ul.h,ul.v);
	ClipRect(&button_rect);
	
	refresh_stat_areas(1);
	if (play_sounds == true)
		play_sound(37);
		else FlushAndPause(5);
	refresh_stat_areas(0);
	undo_clip();
}


void reload_startup()////
{
	if (startup_loaded == true)
		return;
	
	HideWindow(GetDialogWindow(modeless_dialogs[5])); 
	modeless_exists[5] = false;				
	SelectWindow(mainPtr);
	SetPort(GetWindowPort(mainPtr));
	init_startup();

	startup_loaded = true;
	//load_main_screen();
}

void end_startup()
{
	if (startup_loaded == false)
		return;

	DisposeGWorld(startup_gworld);
	DisposeGWorld(startup_button_orig);
	DisposeGWorld(startup_button_g);
	DisposeGWorld(anim_mess);

	startup_loaded = false;
	load_main_screen();
}

// This loads the graphics at the top of the game.
void Set_up_win ()
{
	QDErr err;
	Rect temp_rect = {0,0,0,280},map_world_rect;
	Rect map_rect = {0,0,384,384};
	Rect pc_rect = {0,0,216,113};
	GrafPtr old_port;
	GWorldPtr temp_gworld;
	Str255 fn1 = "\pGeneva";
	Str255 fn2 = "\pDungeon Bold";
	Str255 fn3 = "\pPalatino";
	short i;
	Rect r;
	
	
	GetFNum(fn1,&geneva_font_num);
	if (geneva_font_num == 0)
		GetFNum(fn3,&geneva_font_num);
	GetFNum(fn2,&dungeon_font_num);
	if (dungeon_font_num == 0)
		GetFNum(fn3,&dungeon_font_num);

	temp_rect.bottom = (STORED_GRAPHICS / 10) * 36;
	err = NewGWorld(&storage_gworld, 0 /*8*/,&temp_rect, NULL, NULL, kNativeEndianPixMap);
	if (err != 0) {
		SysBeep(2);
		ExitToShell();
		}
	// TODO: This relates to the storage gworld, and hence should be abolished
	temp_gworld = load_pict("ter1.png");
	GetPortBounds(temp_gworld, &r);
	rect_draw_some_item(temp_gworld,r,storage_gworld,r,0,0);
	DisposeGWorld(temp_gworld);
	
	terrain_screen_gworld = load_pict("terscreen.png");
	err = NewGWorld(&party_template_gworld,  0 /*8*/,&pc_rect, NULL, NULL, kNativeEndianPixMap);
	if (err != 0)
		SysBeep(2);
	// TODO: These should be loaded later, when a scenario is loaded. Exceptions: dialog pictures and pc graphics
//	items_gworld = load_pict(901);
//	tiny_obj_gworld = load_pict(900);
//	fields_gworld = load_pict(821);
//	roads_gworld = load_pict(822);
//	boom_gworld = load_pict(823);
//	missiles_gworld = load_pict(880);
//	dlogpics_gworld = load_pict(850);
//	
//	// possibly not ideal place for this, but...
//	for (i = 0; i < 11; i++)
//		monst_gworld[i] = load_pict(1100 + i);	
//	for (i = 0; i < 7; i++)
//		terrain_gworld[i] = load_pict(800 + i);
//	anim_gworld = load_pict(820);
//	talkfaces_gworld = load_pict(860);

	for (i = 0; i < 7; i++)
		bw_pats[i] = GetPattern(128 + i * 2);


	// Create and initialize map gworld
	err = NewGWorld(&map_gworld,  0 /*8*/,&map_rect, NULL, NULL, kNativeEndianPixMap);
	if (err != 0) {
		play_sound(2,3);
		ExitToShell();		
		}
		else {		
		GetPort(&old_port);
		SetPort(map_gworld);
		GetPortBounds(map_gworld,&map_world_rect);
		ForeColor(whiteColor);
		PaintRect(&map_world_rect);
		ForeColor(blackColor);
		SetPort(old_port);
		}

}

void load_main_screen()
{
	if (invenbtn_gworld != NULL)
		return;
	
	invenbtn_gworld = load_pict("invenbtns.png");
	pc_stats_gworld = load_pict("statarea.png");
	item_stats_gworld = load_pict("inventory.png");
	text_area_gworld = load_pict("transcript.png");
	text_bar_gworld = load_pict("textbar.png");
	orig_text_bar_gworld = load_pict("textbar.png");
	buttons_gworld = load_pict("buttons.png");
   
	set_gworld_fonts(geneva_font_num);
	
	create_clip_region();
}

void create_clip_region()
{
	short i;
	Rect store_rect;
	Rect scrollbar_rect;

	DisposeRgn(clip_region);
	clip_region = NewRgn();
	OpenRgn();

	Rect tempRect;
	GetWindowPortBounds(mainPtr,&tempRect);
	FrameRect(&tempRect);
	for (i = 0; i < 6; i++) {
		store_rect = win_to_rects[i];
		OffsetRect(&store_rect,ul.h,ul.v);
		if ((is_out()) || (is_town()) || (is_combat()) ||
			(i == 2) || (i == 3) || (i == 5))
				FrameRect(&store_rect);
		}
	if ((overall_mode == MODE_TALKING) || (overall_mode == MODE_SHOPPING) ){
		store_rect = talk_area_rect;
		OffsetRect(&store_rect,ul.h,ul.v);
		FrameRect(&store_rect);
		}
	scrollbar_rect = sbar_rect;
	OffsetRect(&scrollbar_rect,ul.h ,ul.v);
	FrameRect(&scrollbar_rect);
	scrollbar_rect = item_sbar_rect;
	OffsetRect(&scrollbar_rect,ul.h ,ul.v);
	FrameRect(&scrollbar_rect);

	CloseRgn(clip_region);
	
}

void set_gworld_fonts(short font_num)
{
	GrafPtr old_port;

	GetPort(&old_port);	
	SetPort(GetWindowPort(mainPtr));
	TextFont(font_num);
	TextFace(bold);
	TextSize(10);
	SetPort(pc_stats_gworld);
	TextFont(font_num);
	TextFace(bold);
	TextSize(10);
	SetPort(item_stats_gworld);
	TextFont(font_num);
	TextFace(bold);
	TextSize(10);
	SetPort(text_area_gworld);
	TextFont(font_num);
	TextFace(bold);
	TextSize(10);
	SetPort(text_bar_gworld);
	TextFont(font_num);
	TextFace(bold);
	TextSize(10);
	SetPort(old_port);	
}

void draw_main_screen()
{
	if (overall_mode == MODE_TALKING) {
		put_background();
		}
		else {
			rect_draw_some_item (terrain_screen_gworld, win_from_rects[0], terrain_screen_gworld, win_to_rects[0], 0, 1);
 
			draw_buttons(0);
			if (overall_mode == MODE_COMBAT)
				draw_pcs(pc_pos[current_pc],1);
			if (overall_mode == MODE_FANCY_TARGET)
				draw_targets(center);	
			}	
	draw_text_area(0);
	ShowControl(text_sbar);
	Draw1Control(text_sbar);
	ShowControl(item_sbar);
	Draw1Control(item_sbar);
	if (overall_mode == MODE_SHOPPING) {
		ShowControl(shop_sbar);
		Draw1Control(shop_sbar);
		}
		else HideControl(shop_sbar);

}

// redraw_screen does the very first redraw, and any full redraw
void redraw_screen(){
	if(in_startup_mode)
		draw_startup(0);
	else{
		switch (overall_mode) {
			case MODE_TALKING:
				put_background();
				refresh_talking();
				break;
			case MODE_SHOPPING:
				put_background();
				refresh_shopping();
				break;
			default:
				draw_main_screen();
				draw_terrain(0);
				draw_text_bar(1);
				if (overall_mode == MODE_COMBAT)
					draw_pcs(pc_pos[current_pc],1);
				if (overall_mode == MODE_FANCY_TARGET)
					draw_targets(center);
				break;
		}
		put_pc_screen();
		put_item_screen(stat_window,0);
		print_buf();
		ShowControl(text_sbar);
		Draw1Control(text_sbar);
		ShowControl(item_sbar);
		Draw1Control(item_sbar);
		if (overall_mode == MODE_SHOPPING) {
			ShowControl(shop_sbar);
			Draw1Control(shop_sbar);
		}
		else HideControl(shop_sbar);
	}
}

//void refresh_screen(short mode)
//{
//	if (overall_mode == MODE_TALKING) {
//		put_background();
//		refresh_talking();
//		}
//	else if (overall_mode == MODE_SHOPPING) { 
//		put_background();
//		refresh_shopping();
//		}
//		else {
//		draw_buttons(0);
//		redraw_terrain();
//		if (overall_mode == MODE_COMBAT)
//			draw_pcs(pc_pos[current_pc],1);
//		if (overall_mode == MODE_FANCY_TARGET)
//			draw_targets(center);
//		draw_text_bar(1);
//		}
//
//	draw_text_area(0);
//	ShowControl(text_sbar);
//	Draw1Control(text_sbar);
//	ShowControl(item_sbar);
//	Draw1Control(item_sbar);
//	if (overall_mode == MODE_SHOPPING) {
//		ShowControl(shop_sbar);
//		Draw1Control(shop_sbar);
//		}
//		else HideControl(shop_sbar);
//}

void put_background()
{
	PixPatHandle bg_pict;

	if (is_out()) {
		if (univ.party.outdoor_corner.x >= 7)
			bg_pict = bg[0]; 
			else bg_pict = bg[10];
		}
	else if (is_combat()) {
		if (univ.party.outdoor_corner.x >= 7)
			bg_pict = bg[2]; 
			else bg_pict = bg[4];
		}
	else {
		if (univ.town->lighting_type > 0) {
			if (univ.party.outdoor_corner.x >= 7)
				bg_pict = bg[1]; 
				else bg_pict = bg[9]; 
			}
			else if ((univ.party.outdoor_corner.x >= 7) && (univ.town.num != 21))
				bg_pict = bg[8]; 
				else bg_pict = bg[13]; 
		}
	FillCRgn(clip_region,bg_pict);
	ShowControl(text_sbar);
	Draw1Control(text_sbar);
	ShowControl(item_sbar);
	Draw1Control(item_sbar);
}

void draw_buttons(short mode)
//mode; // 0 - regular   1 - button action
{
	PixMapHandle store_source;
	const BitMap * store_dest;
	Rect	source_rect = {0,0,37,258}, dest_rec;
	CGrafPtr	origPort;
	GDHandle	origDev;
	GWorldPtr	buttons_to_draw;
	bool spec_draw = false;
	
	if (mode == 1) {
		spec_draw = true;
		mode -= 100;
		}
		
	GetGWorld (&origPort, &origDev);

	store_dest = GetPortBitMapForCopyBits(GetWindowPort(mainPtr));
 
	buttons_to_draw = buttons_gworld;

	store_source = GetPortPixMap(buttons_to_draw);
	if (is_combat()) { // TODO: Draw buttons one at a time instead of singly
		source_rect.top += 37;
		source_rect.bottom += 37;
		}
	if (is_town()) {
		source_rect.top += 74;
		source_rect.bottom += 74;
		}
	
	dest_rec = win_to_rects[1];
	OffsetRect(&dest_rec,ul.h,ul.v);
		
	if (spec_draw == true)
		ForeColor(blueColor);
	CopyBits ( (BitMap *) *store_source ,
				store_dest ,
				&source_rect, &dest_rec, 
				0, NULL);
				//((spec_draw == false) ? 0 : addOver) , NULL);
	if (spec_draw == true)
		ForeColor(blackColor);

	SetGWorld (origPort, origDev);	
	put_background();

}

// In general, refreshes any area that has text in it, the stat areas, the text bar
void draw_text_area(short mode)
//short mode unused
{
	refresh_stat_areas(0);

	draw_text_bar(0);
}

void reset_text_bar()
{
remember_tiny_text = 300;
}


void draw_text_bar(short mode)
//short mode; // 0 - no redraw  1 - forced
{
	//short num_rect[3] = {12,10,4}; // Why? Just... why?
	short i;
	location loc;
	string combat_string;
	
	loc = (is_out()) ? global_to_local(univ.party.p_loc) : univ.town.p_loc;

	if (mode == 1)
		remember_tiny_text = 500;	   
	if ((PSD[SDF_PARTY_STEALTHY] > 0)    || (PSD[SDF_PARTY_FLIGHT] > 0) ||
		(PSD[SDF_PARTY_DETECT_LIFE] > 0) || (PSD[SDF_PARTY_FIREWALK] > 0) )
		remember_tiny_text = 500;
	if (is_out()) {
		for (i = 0; i < 8; i++)
			if (loc.in(univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].info_rect[i])) 
				if ((remember_tiny_text == i) && (mode == 0))
					return;
					else {
						put_text_bar(univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].out_strs(i + 1));
						remember_tiny_text = i;
						return;
						}
		if (remember_tiny_text != 50 + univ.party.i_w_c.x + univ.party.i_w_c.y) {
			put_text_bar((char *) univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].out_strs(0));
			remember_tiny_text = 50 + univ.party.i_w_c.x + univ.party.i_w_c.y;
			}
		}
	if (is_town()) {
		for (i = 0; i < 16; i++)
			if (loc.in(univ.town->room_rect(i))) 
				if ((remember_tiny_text == 200 + i) && (mode == 0))
					return;
					else {
						put_text_bar(univ.town->town_strs(i + 1));
						remember_tiny_text = 200 + i;
						return;
						}
		if (remember_tiny_text != 250) {
			put_text_bar((char *) univ.town->town_strs(0)); ////
			remember_tiny_text = 250;
			}
	
		}
	if ((is_combat()) && (current_pc < 6) && (monsters_going == false)) {
		std::ostringstream sout;
		sout << univ.party[current_pc].name << " (ap: " << univ.party[current_pc].ap << ')';
		combat_string = sout.str();
		put_text_bar((char *) combat_string.c_str());
		remember_tiny_text = 500;
		}
	if ((is_combat()) && (monsters_going == true))	// Print bar for 1st monster with >0 ap -
	   // that is monster that is going
	   for (i = 0; i < univ.town->max_monst(); i++)
	   	if ((univ.town.monst[i].active > 0) && (univ.town.monst[i].ap > 0)) {
	   		combat_string = print_monster_going(univ.town.monst[i].number,univ.town.monst[i].ap);
			put_text_bar((char *) combat_string.c_str());
			remember_tiny_text = 500;
			i = 400;	   
	   }
}

void put_text_bar(char *str)
{
	GrafPtr old_port;
	char status_str[60];
	short xpos = 205;
	
	
	rect_draw_some_item (orig_text_bar_gworld, win_from_rects[4], text_bar_gworld, win_from_rects[4], 0, 0);
	GetPort(&old_port);	
	SetPort(text_bar_gworld);
	ForeColor(whiteColor);
	MoveTo(5,14);
	drawstring(str);

	if (monsters_going == false) {
		if (PSD[SDF_PARTY_STEALTHY] > 0) {
			MoveTo(xpos,14);
			sprintf((char *) status_str,"Stealth");
			drawstring(status_str);
			xpos -= 60;
			}
		if (PSD[SDF_PARTY_FLIGHT] > 0) {
			MoveTo(xpos,14);
			sprintf((char *) status_str,"Flying");
			drawstring(status_str);
			xpos -= 60;
			}
		if (PSD[SDF_PARTY_DETECT_LIFE] > 0) {
			MoveTo(xpos,14);
			sprintf((char *) status_str,"Detect Life");
			drawstring(status_str);
			xpos -= 60;
			}
		if (PSD[SDF_PARTY_FIREWALK] > 0) {
			MoveTo(xpos,14);
			sprintf((char *) status_str,"Firewalk");
			drawstring(status_str);
			xpos -= 60;
			}
		}

	ForeColor(blackColor);
	SetPort(old_port);
	rect_draw_some_item (text_bar_gworld, win_from_rects[4], text_bar_gworld, win_to_rects[4], 0, 1);
}

// This is called when a new situation is entered. It figures out what graphics are needed,
// sets up which_g_stored, and loads them.
void load_area_graphics()
{
	short i;
	
	currently_loading_graphics = true;
	
	// Set all graphics as loseable
	for (i = 50; i < STORED_GRAPHICS; i++)
		if (storage_status[i] == 1)
			storage_status[i] = 2;
	for (i = 0; i < STORED_GRAPHICS; i++)
		wish_list[i] = 0;
		
	// Build wish list
	if (is_out())
		load_outdoor_graphics();
	if ((is_town()) || (is_combat()))
		load_town_graphics();
	
	// Reserve all in wish list not taken
	for (i = 0; i < STORED_GRAPHICS; i++)
		if (wish_list[i] > 49)
			if (reserve_graphic_num_in_array(wish_list[i]) == true)
				wish_list[i] = 0;
				
	// Place all graphics not found in array.
	for (i = 0; i < STORED_GRAPHICS; i++)
		if (wish_list[i] > 49) {
			place_graphic_num_in_array(wish_list[i]);
			wish_list[i] = 0;
			}
			
	// Finally, load graphics.
	put_graphics_in_template();
	
	currently_loading_graphics = false;
}

void add_to_wish_list(short which_g)
{
	short i;

	
	if (which_g < 50)
		return;
	for (i = 0; i < STORED_GRAPHICS; i++) {
		if (wish_list[i] == which_g)
			return;
		if (wish_list[i] == 0) {
			wish_list[i] = which_g;
			return;
			}
		}
	add_string_to_buf("No room for graphic.");
}

// Used to set up array. If graphic there, sets it to be saved, otherwise leaves.
// Returns true is already there
bool reserve_graphic_num_in_array(short which_g)
{
	short i;

	if (which_g < 50)
		return true;
	for (i = 50; i < STORED_GRAPHICS; i++)
		if (which_g_stored[i] == which_g) {
			storage_status[i] = 1;
			return true;
			}
	return false;
}

// Otherwise, puts in array. Note ... if graphic is alreayd here and locked (i.e. 
// storage status is 1, this will add a new copy.
void place_graphic_num_in_array(short which_g)
{
	short i;

	for (i = 50; i < STORED_GRAPHICS; i++)
		if ((storage_status[i] == 2) || (storage_status[i] == 0)) {
			which_g_stored[i] = which_g;
			storage_status[i] = 3;
			return;
			}
	// if we get here, couldn't find a space. Time to flush excess crap.
	
	if (currently_loading_graphics == false)
		load_area_graphics(); // calling this is nice and fast, because game won't try to reload 
			// graphics already there. It'll only purge the trash.
	
	// try again
	for (i = 50; i < STORED_GRAPHICS; i++)
		if ((storage_status[i] == 2) || (storage_status[i] == 0)) {
			which_g_stored[i] = which_g;
			storage_status[i] = 3;
			return;
			}
	add_string_to_buf("No room for graphic.");
	print_nums(0,0,which_g);
}

void add_one_graphic(short which_g)
{
	short i;
	for (i = 0; i < STORED_GRAPHICS; i++) 
		if (which_g_stored[i] == which_g) {
			// Good. We got it. Now lock it and leave
			storage_status[i] = 1;
			return;	
			}
			
	// otherwise, load it in
	place_graphic_num_in_array(which_g);
	put_graphics_in_template();
}


void add_terrain_to_wish_list(unsigned short ter)////
{
	if (terrain_pic[ter] >= 1000) 
		return;
	else if (terrain_pic[ter] >= 400) {
		add_to_wish_list(600 + terrain_pic[ter]);
		add_to_wish_list(700 + terrain_pic[ter]);
		add_to_wish_list(800 + terrain_pic[ter]);
		add_to_wish_list(900 + terrain_pic[ter]);
		}
		else switch (terrain_pic[ter]) {

			case 143:
				add_to_wish_list(230);			
				add_to_wish_list(143);			
				break;
			case 213: case 214:
				add_to_wish_list(213);			
				add_to_wish_list(214);						
				break;

			case 215:
				add_to_wish_list(215);
				add_to_wish_list(218);
				add_to_wish_list(219);
				add_to_wish_list(220);
				add_to_wish_list(221);
				break;
			case 216:
				add_to_wish_list(216);
				add_to_wish_list(222);
				add_to_wish_list(223);
				add_to_wish_list(224);
				add_to_wish_list(225);
				break;
			case 68: case 69:
				add_to_wish_list(68);			
				add_to_wish_list(69);						
				break;
			case 86: case 87:
				add_to_wish_list(86);			
				add_to_wish_list(87);						
				break;
				
			default:
				add_to_wish_list((short) terrain_pic[ter]);
				break;
			}
}

void load_outdoor_graphics() ////
{
	short l,m,i,j;
	short pict;

	for (i = 0; i < 96; i++)
		for (j = 0; j < 96; j++) 
			add_terrain_to_wish_list(univ.out[i][j]);

	for (l = 0; l < 2; l++)
	for (m = 0; m < 2; m++) 
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 7; j++)
			if (univ.out.outdoors[l][m].wandering[i].monst[j] != 0) {
				pict = get_monst_picnum(univ.out.outdoors[l][m].wandering[i].monst[j]);
				//add_monst_graphic(pict,0);
				add_monst_graphic(univ.out.outdoors[l][m].wandering[i].monst[j],0);

				j = 8;
				}
		for (j = 0; j < 7; j++)
			if (univ.out.outdoors[l][m].special_enc[i].monst[j] != 0) {
				pict = get_monst_picnum(univ.out.outdoors[l][m].special_enc[i].monst[j]);
				//add_monst_graphic(pict,0);
				add_monst_graphic(univ.out.outdoors[l][m].special_enc[i].monst[j],0);
				j = 8;
				}
		}

	for (i = 0; i < 10; i++)
		if (univ.party.out_c[i].exists == true)
			for (j = 0; j < 7; j++)
				if (univ.party.out_c[i].what_monst.monst[j] != 0) {
					pict = get_monst_picnum(univ.party.out_c[i].what_monst.monst[j]);
					//add_monst_graphic(pict, 0);
					add_monst_graphic(univ.party.out_c[i].what_monst.monst[j], 0);
					j = 8;
					}
}


void add_monst_graphic(unsigned short m,short mode)////
// mode 0 - just put in list, 1 - actually add graphics
{
	short x,y,i,pict;
	get_monst_dims(m,&x,&y);
	for (i = 0; i < x * y; i++) {
		pict = get_monst_picnum(m);
		if (pict >= NUM_MONST_G)
			return;
		pict = m_pic_index[pict].i;
		if (mode == 0) {
			add_to_wish_list(300 + pict + i);
			add_to_wish_list(600 + pict + i);
			add_to_wish_list(2300 + pict + i);
			add_to_wish_list(2600 + pict + i);
			}
			else {
				add_one_graphic(300 + pict + i);
				add_one_graphic(600 + pict + i);
				add_one_graphic(2300 + pict + i);
				add_one_graphic(2600 + pict + i);
				}
		}
}

void load_town_graphics() // Setting up town monsters takes some finess, due to the flexibility
					// of the situation
// This can be used for town or beginning outdoor combat
{
	short i,j;
	
	for (i = 0; i < univ.town->max_dim(); i++)
		for (j = 0; j < univ.town->max_dim(); j++) 
			if (is_combat())
				add_terrain_to_wish_list(combat_terrain[i][j]);
				else add_terrain_to_wish_list(univ.town->terrain(i,j));

	for (i = 0; i < univ.town->max_monst(); i++)
		if ((univ.town.monst[i].number != 0) && (univ.town.monst[i].active > 0))
			add_monst_graphic(univ.town.monst[i].number,0);
	if (is_town())
		for (i = 0; i < 4; i++)
			for (j = 0; j < 4; j++) {
				add_monst_graphic(univ.town->wandering[i].monst[j],0);
				}
}

void update_pc_graphics() // TODO: The party_template_gworld isn't really necessary; abolish it
{
	short i;
	GWorldPtr temp_gworld;//,temp_gworld2;
	Rect template_rect = {0,0,36,28};
	PixMapHandle store_source, store_dest;
	Rect	source_rect;
	GrafPtr old_port;
	
	if (party_in_memory == false)
		return;
		
	GetPort(&old_port);
	SetPortWindowPort(mainPtr);	
//	temp_gworld = load_pict(902);
//	temp_gworld2 = load_pict(905);
	temp_gworld = load_pict("pcs.png");

	for (i = 0; i < 6; i++)
		if (univ.party[i].main_status > 0) 
			if (univ.party[i].which_graphic != which_graphic_index[i]) {
				template_rect.left = (i / 3) * 56;
				template_rect.right = template_rect.left + 56;
				template_rect.top = (i % 3) * 36;
				template_rect.bottom = template_rect.top + 36;

				
				store_source = GetPortPixMap(temp_gworld);
				source_rect.left = (univ.party[i].which_graphic / 8) * 56;
				source_rect.right = source_rect.left + 56;
//				source_rect.top = (univ.party[i].which_graphic % 8) * 36;
//				source_rect.bottom = template_rect.top + 36;
				source_rect.top = 36 * (univ.party[i].which_graphic % 8);
				source_rect.bottom = 36 * (univ.party[i].which_graphic % 8) + 36;
				store_dest = GetPortPixMap(party_template_gworld);
	
				CopyBits ( (BitMap *) *store_source ,
							(BitMap *) *store_dest ,
							&source_rect, &template_rect, 
							0 , NULL);	
			
				OffsetRect(&source_rect,0,288);
				OffsetRect(&template_rect,0,108);
				CopyBits ( (BitMap *) *store_source ,
							(BitMap *) *store_dest ,
							&source_rect, &template_rect, 
							0 , NULL);	
			
				which_graphic_index[i] = univ.party[i].which_graphic;
				}
	DisposeGWorld (temp_gworld);
	//DisposeGWorld (temp_gworld2);
				
	SetPort(old_port);	

}


// This one is complicated, but that's because it's optimized for efficiency.
// Goes through, and loads graphics for anything with storage_status of 3////
void put_graphics_in_template() // TODO: Get rid of this! It's not necessary! Just keep everything in memory!
{
	GWorldPtr temp_gworld;
	short i,j,which_position,offset;
	bool this_graphic_needed = false;
	Rect from_rect,to_rect;
	
	// First, load all terrains
	for (j = 1; j < 6; j++) {
		for (i = 50; i < STORED_GRAPHICS; i++)
			if ((which_g_stored[i] >= j * 50) && (which_g_stored[i] < j * 50 + 50) &&
				(storage_status[i] == 3)) {
					this_graphic_needed = true;
					}
		if (this_graphic_needed == true) {
			temp_gworld = load_pict(800 + j);
			for (i = 50; i < STORED_GRAPHICS; i++)
				if ((which_g_stored[i] >= j * 50) && (which_g_stored[i] < j * 50 + 50) &&
					(storage_status[i] == 3)) {
					which_position = which_g_stored[i] - j * 50;
					from_rect = calc_rect(which_position % 10,which_position / 10);
					to_rect = calc_rect(i % 10,i / 10);
				
					rect_draw_some_item(temp_gworld,from_rect,storage_gworld,to_rect,0,0);
					storage_status[i] = 1;
					}
			DisposeGWorld (temp_gworld);		
			}
		this_graphic_needed = false;
		}

	// Now, load all monsters
	for (j = 0; j < 10; j++) {
		for (i = 50; i < STORED_GRAPHICS; i++)
			if ((which_g_stored[i] >= 300 + j * 20) && (which_g_stored[i] < 300 + j * 20 + 20) &&
				(storage_status[i] == 3))
					this_graphic_needed = true;
		for (i = 50; i < STORED_GRAPHICS; i++)
			if ((which_g_stored[i] >= 600 + j * 20) && (which_g_stored[i] < 600 + j * 20 + 20) &&
				(storage_status[i] == 3))
					this_graphic_needed = true;

		if (this_graphic_needed == true) {
			temp_gworld = load_pict(1100 + j);
			for (i = 50; i < STORED_GRAPHICS; i++)
				if ((which_g_stored[i] >= 300 + j * 20) && (which_g_stored[i] < 300 + j * 20 + 20) &&
					(storage_status[i] == 3)) {
					which_position = which_g_stored[i] % 20;
					from_rect = calc_rect((which_position / 10) * 2,which_position % 10);
					to_rect = calc_rect(i % 10,i / 10);
				
					rect_draw_some_item(temp_gworld,from_rect,storage_gworld,to_rect,0,0);
				
					storage_status[i] = 1;
					}
			for (i = 50; i < STORED_GRAPHICS; i++)
				if ((which_g_stored[i] >= 600 + j * 20) && (which_g_stored[i] < 600 + j * 20 + 20) &&
					(storage_status[i] == 3)) {
					which_position = which_g_stored[i] % 20;
					from_rect = calc_rect((which_position / 10) * 2 + 1,which_position % 10);
					to_rect = calc_rect(i % 10,i / 10);
				
					rect_draw_some_item(temp_gworld,from_rect,storage_gworld,to_rect,0,0);
				
					storage_status[i] = 1;
					}
			DisposeGWorld (temp_gworld);		
			}
		this_graphic_needed = false;
		}

	// Now, load all monster combat poses
	for (j = 0; j < 10; j++) {
		for (i = 50; i < STORED_GRAPHICS; i++)
			if ((which_g_stored[i] >= 2300 + j * 20) && (which_g_stored[i] < 2300 + j * 20 + 20) &&
				(storage_status[i] == 3))
					this_graphic_needed = true;
		for (i = 50; i < STORED_GRAPHICS; i++)
			if ((which_g_stored[i] >= 2600 + j * 20) && (which_g_stored[i] < 2600 + j * 20 + 20) &&
				(storage_status[i] == 3))
					this_graphic_needed = true;

		if (this_graphic_needed == true) {
			temp_gworld = load_pict(1200 + j);
			for (i = 50; i < STORED_GRAPHICS; i++)
				if ((which_g_stored[i] >= 2300 + j * 20) && (which_g_stored[i] < 2300 + j * 20 + 20) &&
					(storage_status[i] == 3)) {
					which_position = which_g_stored[i] % 20;
					from_rect = calc_rect((which_position / 10) * 2,which_position % 10);
					to_rect = calc_rect(i % 10,i / 10);
				
					rect_draw_some_item(temp_gworld,from_rect,storage_gworld,to_rect,0,0);
				
					storage_status[i] = 1;
					}
			for (i = 50; i < STORED_GRAPHICS; i++)
				if ((which_g_stored[i] >= 2600 + j * 20) && (which_g_stored[i] < 2600 + j * 20 + 20) &&
					(storage_status[i] == 3)) {
					which_position = which_g_stored[i] % 20;
					from_rect = calc_rect((which_position / 10) * 2 + 1,which_position % 10);
					to_rect = calc_rect(i % 10,i / 10);
				
					rect_draw_some_item(temp_gworld,from_rect,storage_gworld,to_rect,0,0);
				
					storage_status[i] = 1;
					}
			DisposeGWorld (temp_gworld);		
			}
		this_graphic_needed = false;
		}


	// Now, anim terrains
	for (i = 50; i < STORED_GRAPHICS; i++) 
		if ((which_g_stored[i] >= 1000) && (which_g_stored[i] < 1400) &&
			(storage_status[i] == 3))
				this_graphic_needed = true;
	if (this_graphic_needed == true) {
		temp_gworld = load_pict(820);
		for (i = 50; i < STORED_GRAPHICS; i++)
			if ((which_g_stored[i] >= 1000) && (which_g_stored[i] < 1400) &&
				(storage_status[i] == 3)) {
				which_position = which_g_stored[i] % 100;
				offset = (which_g_stored[i] - 1000) / 100;
				from_rect = calc_rect(4 * (which_position / 5) + offset,which_position % 5);
				to_rect = calc_rect(i % 10,i / 10);
				
				rect_draw_some_item(temp_gworld,from_rect,storage_gworld,to_rect,0,0);
			
				storage_status[i] = 1;
				}
		DisposeGWorld (temp_gworld);		
		}
}

GWorldPtr load_pict(short picture_to_get)
{
	PicHandle	current_pic_handle;
	Rect	pic_rect;
	short	pic_wd,pic_hgt,x;
	GWorldPtr	myGWorld;
	CGrafPtr	origPort;
	GDHandle	origDev;
	QDErr		check_error;
	PixMapHandle	offPMHandle;
	char good;
	char d_s[60];
	
    current_pic_handle = GetPicture (picture_to_get);
    x = ResError();
    if (x != 0) {
    	if (picture_to_get == 1) // custom graphic
    		return NULL;
    	SysBeep(50);
    	print_nums(10,1000,x);
    	}
	if (current_pic_handle == NULL)  {
		sprintf((char *) d_s, "Stuck on %d  ",(short) picture_to_get);
		add_string_to_buf((char *)d_s);
		Alert(1076,NULL);
		return NULL;
	}
	QDGetPictureBounds(current_pic_handle, &pic_rect);
	pic_wd = pic_rect.right - pic_rect.left;
	pic_hgt = pic_rect.bottom - pic_rect.top;  
	GetGWorld (&origPort, &origDev);
	check_error = NewGWorld (&myGWorld,  0 /*8*/,
				&pic_rect,
				NULL, NULL, kNativeEndianPixMap);
	if (check_error != noErr)  {
    	if (picture_to_get == 1) { // custom graphic
 			ReleaseResource ((Handle) current_pic_handle);
   			return NULL;
   			}
		sprintf((char *) d_s, "Stuck on %d  error %d ",(short) picture_to_get,check_error);
		add_string_to_buf((char *)d_s);
		print_buf(); 
		Alert(1076,NULL);
		return NULL;
		}
	
	SetGWorld(myGWorld, NULL);
	
	offPMHandle = GetGWorldPixMap (myGWorld);
	good = LockPixels (offPMHandle);
	if (good == false)  {
		SysBeep(50); }
	SetRect (&pic_rect, 0, 0, pic_wd, pic_hgt);
	DrawPicture (current_pic_handle, &pic_rect);
	SetGWorld (origPort, origDev);
	UnlockPixels (offPMHandle);
	ReleaseResource ((Handle) current_pic_handle);

	return myGWorld;
}

// this is used for determinign whether to round off walkway corners
// right now, trying a restrictive rule (just cave floor and grass, mainly)
bool is_nature(char x, char y, unsigned char ground_t)
{
	ter_num_t ter_type;
	
	ter_type = coord_to_ter((short) x,(short) y);
	return ground_t == scenario.ter_types[ter_type].ground_type;
//	pic = scenario.ter_types[ter_type].picture;
//	if ((pic >= 0) && (pic <= 45))
//		return true;
//	if ((pic >= 67) && (pic <= 73))
//		return true;
//	if ((pic >= 75) && (pic <= 87))
//		return true;
//	if ((pic >= 121) && (pic <= 122))
//		return true;
//	if ((pic >= 179) && (pic <= 208))
//		return true;
//	if ((pic >= 211) && (pic <= 212))
//		return true;
//	if ((pic >= 217) && (pic <= 246))
//		return true;
//
//	return false;
}

ter_num_t get_ground_from_ter(ter_num_t ter){
	return get_ter_from_ground(scenario.ter_types[ter].ground_type);
}

ter_num_t get_ter_from_ground(unsigned char ground){
	for(int i = 0; i < 256; i++)
		if(scenario.ter_types[i].ground_type == ground)
			return i;
	return 0;
}

void draw_terrain(short	mode)
//mode ... if 1, don't place on screen after redoing
// if 2, only redraw over active monst
{
	char q,r;
	location where_draw;
	location sector_p_in,view_loc;
	char can_draw;
	ter_num_t spec_terrain;
	bool off_terrain = false,draw_frills = true;
	short i,j;
	GrafPtr old_port;
	
	if(overall_mode == MODE_TALKING || overall_mode == MODE_SHOPPING || overall_mode == MODE_STARTUP)
		return;
	
	if (mode == 2) {
		if (current_working_monster < 0) return;
		supressing_some_spaces = true;
		for (i = 0; i < 4; i++) ok_space[i].x = -1;
		if (current_working_monster >= 100) {
			for (i = 0; i < univ.town.monst[current_working_monster - 100].x_width; i++)
				for (j = 0; j < univ.town.monst[current_working_monster - 100].y_width; j++) {
					ok_space[i + 2 * j].x = univ.town.monst[current_working_monster - 100].cur_loc.x + i;
					ok_space[i + 2 * j].y = univ.town.monst[current_working_monster - 100].cur_loc.y + j;
					ok_space[i + 2 * j].x = ok_space[i + 2 * j].x - center.x + 4;
					ok_space[i + 2 * j].y = ok_space[i + 2 * j].y - center.y + 4;
					}
			}
		if (current_working_monster < 6) {
			ok_space[0] = pc_pos[current_working_monster];
			ok_space[0].x = ok_space[0].x - center.x + 4;
			ok_space[0].y = ok_space[0].y - center.y + 4;	
			}
		mode = 0;
		}

	GetPort(&old_port);	
	SetPort(GetWindowPort(mainPtr));
	
	for (i = 0; i < 13; i++)
		for (j = 0; j < 13; j++) {
			light_area[i][j] = 0;
			unexplored_area[i][j] = 0;
			}
	
			
	sector_p_in.x = univ.party.outdoor_corner.x + univ.party.i_w_c.x;
	sector_p_in.y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;

	anim_ticks++;
	anim_onscreen = false;

	if (is_town())
		view_loc = univ.town.p_loc;
	if (is_combat())
		view_loc = pc_pos[(current_pc < 6) ? current_pc : first_active_pc()];
	
	for (i = 0; i < 13; i++)
		for (j = 0; j < 13; j++) {
			where_draw =  (is_out()) ? univ.party.p_loc : center;
			where_draw.x += i - 6;
			where_draw.y += j - 6;
			if (!(is_out())) 
				light_area[i][j] = (is_town()) ? pt_in_light(view_loc,where_draw) : combat_pt_in_light(where_draw);
			if (!(is_out()) && ((where_draw.x < 0) || (where_draw.x > univ.town->max_dim() - 1) 
				|| (where_draw.y < 0) || (where_draw.y > univ.town->max_dim() - 1))) 
					unexplored_area[i][j] = 0;
				else unexplored_area[i][j] = 1 - is_explored(where_draw.x,where_draw.y);
			}

	for (q = 0; q < 9; q++) {
		for (r = 0; r < 9; r++) {
			where_draw = (is_out()) ? univ.party.p_loc : center;
			where_draw.x += q - 4;
			where_draw.y += r - 4;
			off_terrain = false;
			
			draw_frills = true;
			if (!(is_out()) && ((where_draw.x < 0) || (where_draw.x > univ.town->max_dim() - 1) 
								|| (where_draw.y < 0) || (where_draw.y > univ.town->max_dim() - 1))) {
				draw_frills = false;
				// Warning - this section changes where_draw
				if (where_draw.x < 0)
					where_draw.x = -1;
				if (where_draw.x > univ.town->max_dim() - 1)
					where_draw.x = univ.town->max_dim();
				if (where_draw.y < 0)
					where_draw.y = -1;
				if (where_draw.y > univ.town->max_dim() - 1) 
					where_draw.y = univ.town->max_dim();
				if (can_see(view_loc,where_draw,0) < 5)
					can_draw = 1;
				else can_draw = 0;
				spec_terrain = 0;
			}
			else if (is_out()) {
				if ((where_draw.x < 0) || (where_draw.x > 95) 
					|| (where_draw.y < 0) || (where_draw.y > 95))
					can_draw = 0;
				else {
					spec_terrain = univ.out[where_draw.x][where_draw.y];
					can_draw = univ.out.out_e[where_draw.x][where_draw.y];
				}
			}
			else if (is_combat()) {
				spec_terrain = combat_terrain[where_draw.x][where_draw.y];
				if (cartoon_happening == true)
					can_draw = true;
				else can_draw = (((is_explored(where_draw.x,where_draw.y)) ||
								  (which_combat_type == 0) || (monsters_going == true) || (overall_mode != MODE_COMBAT))
								 && (party_can_see(where_draw) < 6)) ? 1 : 0;
			}
			else {
				spec_terrain = univ.town->terrain(where_draw.x,where_draw.y);
				can_draw = is_explored(where_draw.x,where_draw.y);
				
				if (can_draw > 0) {
					if (pt_in_light(univ.town.p_loc,where_draw) == false)
						can_draw = 0;
				}
				if ((overall_mode == MODE_LOOK_TOWN) && (can_draw == 0))
					can_draw = (party_can_see(where_draw) < 6) ? 1 : 0;
			}
			spot_seen[q][r] = can_draw;
			
			if ((can_draw != 0) && (overall_mode != MODE_RESTING)) { // if can see, not a pit, and not resting
				if ((is_combat()) && (cartoon_happening == false)) {
					anim_ticks = 0;
				}
				
				eTrimType trim = scenario.ter_types[spec_terrain].trim_type;
				
				// Finally, draw this terrain spot
				if(trim == TRIM_WALKWAY){
					int trim = -1;
					unsigned char ground_t = scenario.ter_types[spec_terrain].trim_ter;
					ter_num_t ground_ter = get_ter_from_ground(ground_t);
					if (!loc_off_act_area(where_draw)) {
						if(is_nature(where_draw.x - 1,where_draw.y,ground_t)){ // check left
							if(is_nature(where_draw.x,where_draw.y - 1,ground_t)){ // check up
								if(is_nature(where_draw.x + 1,where_draw.y,ground_t)){ // check right
									if(is_nature(where_draw.x,where_draw.y + 1,ground_t)) // check down
										trim = 8;
									else trim = 4;
								}else if(is_nature(where_draw.x,where_draw.y + 1,ground_t)) // check down
									trim = 7;
								else trim = 1;
							}else if(is_nature(where_draw.x,where_draw.y + 1,ground_t)){ // check down
								if(is_nature(where_draw.x + 1,where_draw.y,ground_t)) // check right
									trim = 6;
								else trim = 0;
							}
						}else if(is_nature(where_draw.x,where_draw.y - 1,ground_t)){ // check up
							if(is_nature(where_draw.x + 1,where_draw.y,ground_t)){ // check right
								if(is_nature(where_draw.x,where_draw.y + 1,ground_t)) // check down
									trim = 5;
								else trim = 2;
							}
						}else if(is_nature(where_draw.x + 1,where_draw.y,ground_t)){ // check right
							if(is_nature(where_draw.x,where_draw.y + 1,ground_t)) // check down
								trim = 3;
						}
					}
					draw_one_terrain_spot(q,r,trim < 0 ? spec_terrain : ground_ter,0);
					if(trim >= 0)
						draw_trim(q,r,trim + 50,spec_terrain);
				}else if(trim == TRIM_ROAD || trim == TRIM_N || trim == TRIM_S ||
						 trim == TRIM_W || trim == TRIM_E) {
					draw_one_terrain_spot(q,r,spec_terrain,0);
					place_road(q,r,where_draw,trim == TRIM_ROAD);
				}else if(spec_terrain == 65535) {
					draw_one_terrain_spot(q,r,-1,0);
				}else{
					current_ground = get_ground_from_ter(spec_terrain);
					draw_one_terrain_spot(q,r,spec_terrain,0);
				}
			}
			else {  // Can't see. Place darkness.
				draw_one_terrain_spot(q,r,-1,0);
			}
			
			if ((can_draw != 0) && (overall_mode != MODE_RESTING) && (frills_on == true)
				&& (draw_frills == true) && (cartoon_happening == false)) {  // Place the trim TODO: Alter trim
				place_trim((short) q,(short) r,where_draw,spec_terrain);
			}
//			if((is_town() && univ.town.is_spot(where_draw.x,where_draw.y)) ||
//			   (is_out() && univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].special_spot[where_draw.x][where_draw.y]))
//				Draw_Some_Item(roads_gworld, calc_rect(6, 0), terrain_screen_gworld, loc(q,r), 1, 0);
			// TODO: Move draw_sfx, draw_items, draw_fields, draw_spec_items, etc to here
			
			if (is_town() || is_combat()) {
				draw_items(where_draw);
			}
			draw_fields(where_draw);
			//draw_monsters(where_draw);
			//draw_vehicles(where_draw);
			//if(is_combat) draw_pcs(where_draw); else draw_party(where_draw);
		}
	}
	
//	if ((overall_mode != MODE_RESTING) && (!is_out())) 
//		draw_sfx();
//		
//	// Now place items
//	if ((overall_mode > MODE_OUTDOORS) && (overall_mode != MODE_LOOK_OUTDOORS) && (overall_mode != MODE_RESTING))
//		draw_items();		
//		
//	// Now place fields
//	if ((overall_mode != MODE_RESTING) && (!is_out())) {
//		draw_fields();
//		draw_spec_items();
//		}
//
	// Not camping. Place misc. stuff
	if (overall_mode != MODE_RESTING) {
		if (is_out())
			draw_outd_boats(univ.party.p_loc);
			else if ((is_town()) || (which_combat_type == 1))
				draw_town_boat(center);		
		draw_monsters();
		}

	if ((overall_mode < MODE_COMBAT) || (overall_mode == MODE_LOOK_OUTDOORS) || ((overall_mode == MODE_LOOK_TOWN) && (point_onscreen(univ.town.p_loc,center) == true))
		|| (overall_mode == MODE_RESTING))
		draw_party_symbol(mode,center);
		else if (overall_mode != MODE_LOOK_TOWN)
			draw_pcs(center,0);
	
	// Now do the light mask thing
	apply_light_mask();
	apply_unseen_mask();
	
	if (mode == 0) {
		redraw_terrain();
		if (cartoon_happening == false) {
			draw_text_bar(0);
			if ((overall_mode >= MODE_COMBAT) && (overall_mode != MODE_LOOK_OUTDOORS) && (overall_mode != MODE_LOOK_TOWN) && (overall_mode != MODE_RESTING))
				draw_pcs(center,1);
			if (overall_mode == MODE_FANCY_TARGET)
				draw_targets(center);
		}
	}
	SetPort(old_port);
	supressing_some_spaces = false;
}


ter_num_t get_ground_for_shore(ter_num_t ter){
	if(scenario.ter_types[ter].block_horse) return current_ground;
	else if(scenario.ter_types[ter].blockage > 2) return current_ground;
	else return ter;
}

void place_trim(short q,short r,location where,ter_num_t ter_type)
{
	bool at_top = false,at_bot = false,at_left = false,at_right = false;
	location targ;
	
	// FIrst quick check ... if a pit or barrier in outdoor combat, no trim
	if ((is_combat()) && (which_combat_type == 0) && (ter_type == 90))
		return;
	if (PSD[SDF_NO_SHORE_FRILLS] > 0)
		return;
		
	targ.x = q;
	targ.y = r;
	if (supressing_some_spaces && (targ != ok_space[0]) && (targ != ok_space[1]) &&
		(targ != ok_space[2]) && (targ != ok_space[3]))
			return;
	
					
	if (where.x == 0)
		at_left = true;
	if (where.y == 0)
		at_top = true;
	if ((overall_mode == MODE_OUTDOORS) || (overall_mode == MODE_LOOK_OUTDOORS)) {
		if (where.x == 95)
			at_right = true;
		if (where.y == 95)
			at_bot = true;
		}
		else {
		if (where.x == univ.town->max_dim())
			at_right = true;
		if (where.y == univ.town->max_dim())
			at_bot = true;
		}
		
	// First, trim for fluids
	if(is_fluid(ter_type)){
		short trim = get_fluid_trim(where, ter_type);
		if (trim != 0) {
			ter_num_t shore;
			if (trim & 2){ // upper right
				shore = coord_to_ter(where.x + 1, where.y - 1);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,5,shore);
			}if (trim & 8){ // lower right
				shore = coord_to_ter(where.x + 1, where.y + 1);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,7,shore);
			}if (trim & 32){ // lower left
				shore = coord_to_ter(where.x - 1, where.y + 1);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,6,shore);
			}if (trim & 128){ // upper left
				shore = coord_to_ter(where.x - 1, where.y - 1);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,4,shore);
			}if (trim & 1){ // top
				shore = coord_to_ter(where.x, where.y - 1);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,2,shore);
			}if (trim & 4){ // right
				shore = coord_to_ter(where.x + 1, where.y);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,1,shore);
			}if (trim & 16){ // bottom
				shore = coord_to_ter(where.x, where.y + 1);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,3,shore);
			}if (trim & 64){ // left
				shore = coord_to_ter(where.x - 1, where.y);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,0,shore);
			}
		}
	}
	
	if(is_wall(ter_type) && !at_top && !at_bot && !at_left && !at_right){
	//if (((ter_type >= 100) && (ter_type <= 137)) && (at_top == false) &&
	//	(at_bot == false) && (at_left == false) && (at_right == false)) {
		ter_num_t to_left = coord_to_ter(where.x - 1,where.y);
		ter_num_t above = coord_to_ter(where.x,where.y - 1);
		ter_num_t to_right = coord_to_ter(where.x + 1,where.y);
		ter_num_t below = coord_to_ter(where.x,where.y + 1);
		if (is_wall(to_left) && is_wall(above) && is_ground(to_right) && is_ground(below))
			draw_trim(q,r,11,to_right);
		
		if (is_wall(to_left) && is_wall(below) && is_ground(to_right) && is_ground(above))
			draw_trim(q,r,9,to_right);
		
		if (is_wall(to_right) && is_wall(above) && is_ground(to_left) && is_ground(below))
			draw_trim(q,r,10,to_left);
			
		if (is_wall(to_right) && is_wall(below) && is_ground(to_left) && is_ground(above))
			draw_trim(q,r,8,to_left);		
			
		if (is_ground(to_left) && is_ground(above) && is_ground(to_right) && is_wall(below)) {
			draw_trim(q,r,8,to_right);
			draw_trim(q,r,9,to_right);
		}

		if (is_wall(to_left) && is_ground(below) && is_ground(to_right) && is_ground(above)) {
			draw_trim(q,r,9,to_right);
			draw_trim(q,r,11,to_right);
		}

		if (is_ground(to_right) && is_wall(above) && is_ground(to_left) && is_ground(below)) {
			draw_trim(q,r,10,to_left);
			draw_trim(q,r,11,to_left);
		}

		if (is_wall(to_right) && is_ground(below) && is_ground(to_left) && is_ground(above)) {
			draw_trim(q,r,8,to_left);
			draw_trim(q,r,10,to_left);
		}
		
		if (is_ground(to_right) && is_ground(below) && is_ground(to_left) && is_ground(above)) {
			draw_trim(q,r,8,to_left);
			draw_trim(q,r,9,to_right);
			draw_trim(q,r,10,to_left);
			draw_trim(q,r,11,to_right);
		}
	//	}
	}
}

void draw_trim(short q,short r,short which_trim,ter_num_t ground_ter)
//which_trim is 3 -> drawing wall trim -> might shift down if ground is grass
//short which_mode;  // 0 top 1 bottom 2 left 3 right 4 up left 5 up right 6 down right 7 down left
{
	// which_trim
	// 0 - left, 1 - right, 2 - top, 3 - bottom, 4 - tl, 5 - tr, 6 - bl, 7 - br
	// 8 - wall tl, 9 - wall tr, 10 - wall bl, 11 - wall br
	// 50 - walkway bl, 51 - walkway tl, 52 - walkway tr, 53 - walkway br
	// 54 - walkway top, 55 - walkway right, 56 - walkway bottom, 57 - walkway left
	// 58 - lone walkway
	Rect from_rect = {0,0,36,28},to_rect,mask_rect;
	GWorldPtr from_gworld;
	static Rect trim_rects[] = {
		{0,0,36,14}, {0,0,36,14},
		{0,0,18,28}, {0,0,18,28},
		{0,0,18,14}, {0,0,18,14}, {0,0,18,14}, {0,0,18,14},
		{0,0,18,14}, {0,0,18,14}, {0,0,18,14}, {0,0,18,14},
	};
	static Rect walkway_rects[] = {
		{0,0,36,28}, {0,0,36,28}, {0,0,36,28}, {0,0,36,28},
		{0,0,36,28}, {0,0,36,28}, {0,0,36,28}, {0,0,36,28},
		{0,0,36,28},
	};
	static bool inited = false;
	if(!inited){
		inited = true;
		OffsetRect(&trim_rects[1],14,0);
		OffsetRect(&trim_rects[2],28,0);
		OffsetRect(&trim_rects[3],28,18);
		OffsetRect(&trim_rects[4],56,0);
		OffsetRect(&trim_rects[5],70,0);
		OffsetRect(&trim_rects[6],56,18);
		OffsetRect(&trim_rects[7],70,18);
		OffsetRect(&trim_rects[8],84,0);
		OffsetRect(&trim_rects[9],98,0);
		OffsetRect(&trim_rects[10],84,18);
		OffsetRect(&trim_rects[11],98,18);
		int i;
		for(i = 0; i < 12; i++) OffsetRect(&trim_rects[i],112,36);
		for(i = 0; i < 8 ; i++) OffsetRect(&walkway_rects[i],(i%4)*28,(i/4)*36);
		OffsetRect(&walkway_rects[8],196,0);
	}
	RGBColor	test_color = {0,0,0}, store_color;

	if (frills_on == false)
		return;
		
	// if current ground is grass, forget trim
//	if ((current_ground == 2) && (which_trim < 3))
//		return; 

	terrain_there[q][r] = -1;
	GetBackColor(&store_color);
	RGBBackColor(&test_color);
		
//	from_rect.left = 28 * which_trim + trim_rects[which_mode].left;
//	from_rect.right = 28 * which_trim + trim_rects[which_mode].right;
//	from_rect.top = trim_rects[which_mode].top;
//	from_rect.bottom = trim_rects[which_mode].bottom;
//	
//	if ((which_trim == 3) && (current_ground == 2)) // trim corner walls with grass instead of cave floor
//		OffsetRect(&from_rect,0,36);
	unsigned short pic = scenario.ter_types[ground_ter].picture;
	if(pic < 400){
		from_gworld = terrain_gworld[pic / 50];
		pic %= 50;
		OffsetRect(&from_rect,28 * (pic % 10), 36 * (pic / 10));
	}else if(pic < 1000){
		from_gworld = anim_gworld;
		pic %= 400;
		OffsetRect(&from_rect,112 * (pic / 5),36 * (pic % 5));
	}else{
		from_gworld = spec_scen_g;
		pic %= 1000;
		from_rect = get_custom_rect(pic);
	}
	if(which_trim < 50) mask_rect = trim_rects[which_trim];
	else mask_rect = walkway_rects[which_trim - 50];
	to_rect = coord_to_rect(q,r);
//	to_rect.right = to_rect.left + trim_rects[which_mode].right;
//	to_rect.left = to_rect.left + trim_rects[which_mode].left;
//	to_rect.bottom = to_rect.top + trim_rects[which_mode].bottom;
//	to_rect.top = to_rect.top + trim_rects[which_mode].top;
	//	OffsetRect(&to_rect,-61,-37);
	if(which_trim == 0 || which_trim == 4 || which_trim == 6 || which_trim == 8 || which_trim == 10){
		from_rect.right -= 14;
		to_rect.right -= 14;
	}else if(which_trim == 1 || which_trim == 5 || which_trim == 7 || which_trim == 9 || which_trim == 11){
		from_rect.left += 14;
		to_rect.left += 14;
	}
	if(which_trim == 2 || which_trim == 4 || which_trim == 5 || which_trim == 8 || which_trim == 9){
		from_rect.bottom -= 18;
		to_rect.bottom -= 18;
	}else if(which_trim == 3 || which_trim == 6 || which_trim == 7 || which_trim == 10 || which_trim == 11){
		from_rect.top += 18;
		to_rect.top += 18;
	}

	PixMapHandle from_bits = GetPortPixMap(from_gworld);
	PixMapHandle mask_bits = GetPortPixMap(roads_gworld);
	PixMapHandle to_bits = GetPortPixMap(terrain_screen_gworld);
	LockPixels(to_bits);
	LockPixels(mask_bits);
	LockPixels(from_bits);
	CopyDeepMask ((BitMap*) *from_bits,
				  (BitMap*) *mask_bits,
				  (BitMap*) *to_bits,
				  &from_rect, &mask_rect, &to_rect,
				  transparent, NULL
	);
	UnlockPixels(from_bits);
	UnlockPixels(mask_bits);
	UnlockPixels(to_bits);

	RGBBackColor(&store_color);
}


bool extend_road_terrain(ter_num_t ter)
{
	eTrimType trim = scenario.ter_types[ter].trim_type;
	eTerSpec spec = scenario.ter_types[ter].special;
	ter_num_t flag = scenario.ter_types[ter].flag1.u;
	if(trim == TRIM_ROAD || trim == TRIM_CITY || trim == TRIM_WALKWAY)
		return true;
	if(spec == TER_SPEC_BRIDGE)
		return true;
	if(spec == TER_SPEC_TOWN_ENTRANCE && trim != TRIM_NONE)
		return true; // cave entrance, most likely
	if(spec == TER_SPEC_UNLOCKABLE || spec == TER_SPEC_CHANGE_WHEN_STEP_ON)
		return true; // closed door, possibly locked; or closed portcullis
	if(spec == TER_SPEC_CHANGE_WHEN_USED && scenario.ter_types[flag].special == TER_SPEC_CHANGE_WHEN_STEP_ON && scenario.ter_types[flag].flag1.u == ter)
		return true; // open door (I think) TODO: Verify this works
	if(spec == TER_SPEC_LOCKABLE)
		return true; // open portcullis (most likely)
	if(trim == TRIM_N || trim == TRIM_S || trim == TRIM_W || trim == TRIM_E)
		return true; // connect roads to trim boundaries
	return false;
}

bool connect_roads(ter_num_t ter){
	eTrimType trim = scenario.ter_types[ter].trim_type;
	eTerSpec spec = scenario.ter_types[ter].special;
	if(trim == TRIM_ROAD || trim == TRIM_CITY)
		return true;
	if(spec == TER_SPEC_TOWN_ENTRANCE && trim != TRIM_NONE)
		return true; // cave entrance, most likely
	return false;
}

void place_road(short q,short r,location where, bool here)
{
	location draw_loc;
	ter_num_t ter;
	Rect to_rect;
	//Rect road_rects[2] = {{76,112,80,125},{72,144,90,148}}; // 0 - rl partial  1 - ud partial
	static const Rect road_rects[4] = {
		{4,112,8,125},	// horizontal partial
		{0,144,18,148},	// vertical partial
		{0,112,4,140},	// horizontal full
		{0,140,36,144},	// vertical full
	};
	//Rect road_dest_rects[4] = {{0,12,18,16},{16,15,20,28},{18,12,36,16},{16,0,20,13}}; // top right bottom left
	static const Rect road_dest_rects[6] = {
		{0,12,18,16},	// top
		{16,15,20,28},	// right
		{18,12,36,16},	// bottom
		{16,0,20,13},	// left
		{0,12,36,16},	// top + bottom
		{16,0,20,28},	// right + left
	};
	draw_loc.x = q;
	draw_loc.y = r;
	
	terrain_there[q][r] = -1;
	
	if(here){
		if (where.y > 0)
			ter = coord_to_ter(where.x,where.y - 1);
		if ((where.y == 0) || extend_road_terrain(ter)) {
			to_rect = road_dest_rects[0];
			OffsetRect(&to_rect,13 + q * 28,13 + r * 36);
			rect_draw_some_item (roads_gworld, road_rects[1], terrain_screen_gworld, to_rect, 0, 0);
		}
		
		if (((is_out()) && (where.x < 96)) || (!(is_out()) && (where.x < univ.town->max_dim() - 1)))
			ter = coord_to_ter(where.x + 1,where.y);
		if (((is_out()) && (where.x == 96)) || (!(is_out()) && (where.x == univ.town->max_dim() - 1)) 
			|| extend_road_terrain(ter)) {
			to_rect = road_dest_rects[1];
			OffsetRect(&to_rect,13 + q * 28,13 + r * 36);
			rect_draw_some_item (roads_gworld, road_rects[0], terrain_screen_gworld, to_rect, 0, 0);
		}
		
		if (((is_out()) && (where.y < 96)) || (!(is_out()) && (where.y < univ.town->max_dim() - 1)))
			ter = coord_to_ter(where.x,where.y + 1);
		if (((is_out()) && (where.y == 96)) || (!(is_out()) && (where.y == univ.town->max_dim() - 1)) 
			|| extend_road_terrain(ter)) {
			to_rect = road_dest_rects[2];
			OffsetRect(&to_rect,13 + q * 28,13 + r * 36);
			rect_draw_some_item (roads_gworld, road_rects[1], terrain_screen_gworld, to_rect, 0, 0);
		}
		
		if (where.x > 0)
			ter = coord_to_ter(where.x - 1,where.y);
		if ((where.x == 0) || extend_road_terrain(ter)) {
			to_rect = road_dest_rects[3];
			OffsetRect(&to_rect,13 + q * 28,13 + r * 36);
			rect_draw_some_item (roads_gworld, road_rects[0], terrain_screen_gworld, to_rect, 0, 0);
		}
	}else{
		bool horz = false, vert = false;
		if (where.y > 0)
			ter = coord_to_ter(where.x,where.y - 1);
		if ((where.y == 0) || connect_roads(ter))
			vert = true;
		
		if (((is_out()) && (where.x < 96)) || (!(is_out()) && (where.x < univ.town->max_dim() - 1)))
			ter = coord_to_ter(where.x + 1,where.y);
		if (((is_out()) && (where.x == 96)) || (!(is_out()) && (where.x == univ.town->max_dim() - 1)) 
			|| connect_roads(ter))
			horz = true;
		
		if(vert){
			if (((is_out()) && (where.y < 96)) || (!(is_out()) && (where.y < univ.town->max_dim() - 1)))
				ter = coord_to_ter(where.x,where.y + 1);
			if (((is_out()) && (where.y == 96)) || (!(is_out()) && (where.y == univ.town->max_dim() - 1)) 
				|| connect_roads(ter))
				vert = true;
			else vert = false;
		}
		
		if(horz){
			if (where.x > 0)
				ter = coord_to_ter(where.x - 1,where.y);
			if ((where.x == 0) || connect_roads(ter)) 
				horz = true;
			else horz = false;
		}
		
		if(horz){
			to_rect = road_dest_rects[5];
			OffsetRect(&to_rect,13 + q * 28,13 + r * 36);
			rect_draw_some_item (roads_gworld, road_rects[2], terrain_screen_gworld, to_rect, 0, 0);
		}
		if(vert){
			to_rect = road_dest_rects[4];
			OffsetRect(&to_rect,13 + q * 28,13 + r * 36);
			rect_draw_some_item (roads_gworld, road_rects[3], terrain_screen_gworld, to_rect, 0, 0);
		}
	}
}

void draw_rest_screen()
{
	eGameMode store_mode;

	store_mode = overall_mode;
	overall_mode = MODE_RESTING;
	draw_terrain(0);
	overall_mode = store_mode ;
}

void boom_space(location where,short mode,short type,short damage,short sound)
// if mode is 100, force
//short type; // 0 - flame 1 - magic 2 - poison 3 - blood 4 - cold
	// 10s digit indicates sound  0 - normal ouch  1 - small sword  2 - loud sword
	// 3 - pole  4 - club  5 - fireball hit  6 - squish  7 - cold
	// 8 - acid  9 - claw  10 - bite  11 - slime  12 - zap  13 - missile hit
{
	location where_draw(4,4);
	Rect source_rect = {0,0,36,28},text_rect,dest_rect = {13,13,49,41},big_to = {13,13,337,265},store_rect;
	short del_len;
	char dam_str[20];
	short x_adj = 0,y_adj = 0,which_m;
	short sound_to_play[20] = {97,69,70,71,72, 73,55,75,42,86,
			87,88,89,98,0, 0,0,0,0,0};

	//sound_key = type / 10;
	//type = type % 10;
	
//	if ((cartoon_happening == true) && (anim_step < 140))
//		return;
	if ((cartoon_happening == false) && ((mode != 100) && (party_can_see(where) == 6)))
		return;
	if ((type < 0) || (type > 4))
		return;
	if ((boom_anim_active == true) && (type != 3))
		return;
	if ((cartoon_happening == false) && (PSD[SDF_NO_FRILLS] > 0) && (fast_bang == true))
		return;
	if (is_out())
		return;
			
	// Redraw terrain in proper position
	if ((((point_onscreen(center,where) == false) && (overall_mode >= MODE_COMBAT)) || (overall_mode == MODE_OUTDOORS))
		) {
		play_sound(sound_to_play[sound]);

		return;
		}
		else if (is_combat()) {
			if (which_combat_type == 1)
				draw_terrain(0);
				else draw_terrain(0);
			}
			else if (fast_bang < 2) {
				draw_terrain(0);
				if (fast_bang == 1)
					fast_bang = 2;
				}

	where_draw.x = where.x - center.x + 4;
	where_draw.y = where.y - center.y + 4;
//	source_rect.left += 28 * type;
//	source_rect.right += 28 * type;
	
	// adjust for possible big monster
	which_m = monst_there(where);
	if (which_m < 90) {
		x_adj += 14 * (univ.town.monst[which_m].x_width - 1);
		y_adj += 18 * (univ.town.monst[which_m].y_width - 1);
		}
	OffsetRect(&dest_rect,where_draw.x * 28,where_draw.y * 36);
	source_rect = store_rect = dest_rect;
	OffsetRect(&dest_rect,x_adj,y_adj);
	SectRect(&dest_rect,&big_to,&dest_rect);

	if (cartoon_happening == false) 
		OffsetRect(&dest_rect,win_to_rects[0].left,win_to_rects[0].top);
		else if (store_anim_type == 0) 				
			OffsetRect(&dest_rect,306,5);
			else OffsetRect(&dest_rect,store_anim_ul.h,store_anim_ul.v);

	OffsetRect(&source_rect,-1 * store_rect.left + 28 * type,-1 * store_rect.top);
	rect_draw_some_item(boom_gworld,source_rect,terrain_screen_gworld,dest_rect,1,1);
	
	if ((cartoon_happening == false) && (dest_rect.right - dest_rect.left >= 28)
		&& (dest_rect.bottom - dest_rect.top >= 36)) {
		sprintf((char *) dam_str,"%d",damage);
		TextSize(10);
		TextFace(bold);
		//text_rect = coord_to_rect(where_draw.x,where_draw.y);
		//OffsetRect(&text_rect,x_adj,y_adj);
		text_rect = dest_rect;
		text_rect.top += 10;
		if ((damage < 10) && (dest_rect.right - dest_rect.left > 19))
			text_rect.left += 10;
		OffsetRect(&text_rect,-4,-5);
		char_win_draw_string(mainPtr,text_rect,(char *) dam_str,1,10,true);
		TextSize(0);
		TextFace(0);
		}
		play_sound((skip_boom_delay?-1:1)*sound_to_play[sound]);
		if ((sound == 6) && (fast_bang == 0) && (!skip_boom_delay))
			FlushAndPause(12);

	
	if (fast_bang == 0 && !skip_boom_delay) {
		del_len = PSD[SDF_GAME_SPEED] * 3 + 4;
		if (play_sounds == false)
			FlushAndPause(del_len);
		}
	redraw_terrain();
	if ((cartoon_happening == false) && (overall_mode >= MODE_COMBAT/*9*/) && (overall_mode != MODE_LOOK_OUTDOORS) && (overall_mode != MODE_LOOK_TOWN) && (overall_mode != MODE_RESTING))
		draw_pcs(center,1);	
}
	

void draw_pointing_arrows() 
{
	//Rect sources[4] = {{65,46,73,54},{56,46,64,54},{56,37,64,45},{65,37,73,45}};
	Rect sources[4] = {
		{352,58,360,76}, // up
		{352,20,360,38}, // left
		{352,01,360,19}, // down
		{352,39,360,57}  // right
	};
	Rect dests[8] = {{7,100,15,108},{7,170,15,178},{140,7,148,15},{212,7,220,15},
		{346,100,354,108},{346,170,354,178},{140,274,148,282},{212,274,220,282}};
	short i;
	
	if ((monsters_going == true) || /*(overall_mode <= MODE_TOWN) ||*/ (overall_mode <= MODE_COMBAT)
		|| (overall_mode == MODE_LOOK_OUTDOORS)) 
			return;
	for (i = 0; i < 4; i++) {
		rect_draw_some_item (terrain_screen_gworld,sources[i],terrain_screen_gworld,dests[i * 2],1,1);
		rect_draw_some_item (terrain_screen_gworld,sources[i],terrain_screen_gworld,dests[i * 2 + 1],1,1);
		}
}

void redraw_terrain()
{
	Rect to_rect;
	
	if (cartoon_happening == false)
		to_rect = win_to_rects[0];
		else {
			GetPortBounds(terrain_screen_gworld, &to_rect);
			if (store_anim_type == 0) 
				OffsetRect(&to_rect,306,5);
				else OffsetRect(&to_rect,store_anim_ul.h,store_anim_ul.v);
			}
	rect_draw_some_item (terrain_screen_gworld, win_from_rects[0], terrain_screen_gworld, to_rect, 0, 1);


	// Now place arrows
	draw_pointing_arrows();
	

}


void draw_targets(location center)
{
	Rect source_rect = {74,36,85,47},dest_rect;
	short i = 0;

	if (party_toast() == true)
		return;

	for (i = 0; i < 8; i++)
		if ((spell_targets[i].x != 120) && (point_onscreen(center,spell_targets[i]) == true)) {
			dest_rect = coord_to_rect(spell_targets[i].x - center.x + 4,spell_targets[i].y - center.y + 4);
			//OffsetRect(&dest_rect,5,5);
			//InsetRect(&dest_rect,8,12);
			rect_draw_some_item (roads_gworld,calc_rect(6,0),roads_gworld,dest_rect,1,1);
			}
}

void frame_space(location where,short mode,short width,short height)
//mode;  // 0 - red   1 - green
{
	location where_put;
	Rect to_frame;
	
	if (point_onscreen(center,where) == false)
		return;

	where_put.x = 4 + where.x - center.x;
	where_put.y = 4 + where.y - center.y;
	
	to_frame.top = 18 + where_put.y * 36;
	to_frame.left = 18 + where_put.x * 28;
	to_frame.bottom = 54 + where_put.y * 36 + 36 * (height - 1);
	to_frame.right = 46 + where_put.x * 28 + 28 * (width - 1);				
	OffsetRect(&to_frame, ul.h,ul.v);
					
	ForeColor((mode == 0) ? redColor : greenColor);
	FrameRoundRect(&to_frame,8,8);
	ForeColor(blackColor);	

}


void erase_spot(short i,short j)
{
	Rect to_erase;
	
	to_erase = coord_to_rect(i,j);
	EraseRect(&to_erase);
	
}


void undo_clip()
{
	Rect overall_rect;
	GrafPtr cur_port;

	GetPort(&cur_port);	
	GetPortBounds(cur_port,&overall_rect);
	ClipRect(&overall_rect);
}



void draw_targeting_line(Point where_curs)
{
	location which_space,store_loc;
	short i,j,k,l;
	Rect redraw_rect,redraw_rect2,terrain_rect = {0,0,351,279},target_rect;
	location from_loc;
	Rect on_screen_terrain_area = {23, 23, 346, 274};
	
	if (overall_mode >= MODE_COMBAT)
		from_loc = pc_pos[current_pc];
		else from_loc = univ.town.p_loc;
	if ((overall_mode == MODE_SPELL_TARGET) || (overall_mode == MODE_FIRING) || (overall_mode == MODE_THROWING) || (overall_mode == MODE_FANCY_TARGET)
	  || ((overall_mode == MODE_TOWN_TARGET) && (current_pat.pattern[4][4] != 0))) {
		GlobalToLocal(&where_curs);	
		
		OffsetRect(&on_screen_terrain_area,ul.h,ul.v);
		if (PtInRect (where_curs, &on_screen_terrain_area) == true) {
		// && (point_onscreen(center,pc_pos[current_pc]) == true)){		
			i = (where_curs.h - 23 - ul.h) / 28;
			j = (where_curs.v - 23 - ul.v) / 36;
			which_space.x = center.x + (short) i - 4;
			which_space.y = center.y + (short) j - 4;
			
			k = (short) (from_loc.x - center.x + 4);
			l = (short) (from_loc.y - center.y + 4);
			k = (k * 28) + 32 + ul.h;
			l = (l * 36) + 36 + ul.v;
			
			if ((can_see(from_loc,which_space,0) < 5) 
			 && (dist(from_loc,which_space) <= current_spell_range)) {
			 	InsetRect(&terrain_rect,13,13); 
			 	OffsetRect(&terrain_rect,5 + ul.h,5 + ul.v);
				ClipRect(&terrain_rect);
				PenMode (patXor);
				PenSize(2,2);
				MoveTo(where_curs.h,where_curs.v);
				LineTo(k,l);
				
				redraw_rect.left = min(where_curs.h,k) - 4;
				redraw_rect.right = max(where_curs.h,k) + 4;
				redraw_rect.top = min(where_curs.v,l) - 4;
				redraw_rect.bottom = max(where_curs.v,l) + 4;
				SectRect(&redraw_rect,&terrain_rect,&redraw_rect2);
				
				// Now place targeting pattern
				for (i = 0; i < 9; i++)
					for (j = 0; j < 9; j++) {
						store_loc.x = center.x + i - 4;
						store_loc.y = center.y + j - 4;
						if ((abs(store_loc.x - which_space.x) <= 4) && 
							(abs(store_loc.y - which_space.y) <= 4) && 
							(current_pat.pattern[store_loc.x - which_space.x + 4][store_loc.y - which_space.y + 4] != 0)) {
								target_rect.left = 13 + BITMAP_WIDTH * i + 5 + ul.h;
								target_rect.right = target_rect.left + BITMAP_WIDTH;
								target_rect.top = 13 + BITMAP_HEIGHT * j + 5 + ul.v;
								target_rect.bottom = target_rect.top + BITMAP_HEIGHT;
								FrameRect(&target_rect);
								InsetRect(&target_rect,-5,-5);
								UnionRect(&target_rect,&redraw_rect2,&redraw_rect2);
								
								// Now place number of shots left, if drawing center of target
								if ((overall_mode == MODE_FANCY_TARGET) && (store_loc.x - which_space.x + 4 == 4)
								 && (store_loc.y - which_space.y + 4 == 4)) {
									ForeColor(whiteColor);
									MoveTo(((target_rect.left + target_rect.right) / 2) - 3,
									 (target_rect.top + target_rect.bottom) / 2);
									DrawChar((char) (num_targets_left + 48));
									ForeColor(blackColor);
									}
								
								}
						}
											
				FlushAndPause(4);
				
				InsetRect(&redraw_rect2,-5,-5);	
				redraw_partial_terrain(redraw_rect2);
				PenNormal();
				undo_clip();
				if (is_combat())
					draw_pcs(center,1);
					else draw_party_symbol(0,center);
				if (overall_mode == MODE_FANCY_TARGET)
					draw_targets(center);	
				}
			}
	}
}


bool party_toast()
{
	short i;
	
	for (i = 0; i < 6; i++)
		if (univ.party[i].main_status == 1)
			return false;
	return true;
}

void redraw_partial_terrain(Rect redraw_rect)
{
	Rect from_rect;

	from_rect = redraw_rect;
	OffsetRect(&from_rect,-1 * ul.h,-1 * ul.v);

	// as rect_draw_some_item will shift redraw_rect before drawing, we need to compensate
	OffsetRect(&redraw_rect,-1 * ul.h + 5,-1 * ul.v + 5);
	
	rect_draw_some_item(terrain_screen_gworld,from_rect,terrain_screen_gworld,redraw_rect,0,1);

}

void FlushAndPause(unsigned long ticks)
{
	RgnHandle portRegion = NewRgn();
	GrafPtr port;
	GetPort(&port);
	GetPortVisibleRegion(port,portRegion);
	QDFlushPortBuffer(port,portRegion);
	DisposeRgn(portRegion);
	
	unsigned long dummy;
	Delay(ticks,&dummy);
}

void dump_gworld()
{
	Rect from_rect,to_rect;
	
	GetPortBounds(storage_gworld,&from_rect);
	from_rect.top += 180;
	to_rect = from_rect;
	OffsetRect(&to_rect,0,-180);
	to_rect.bottom = to_rect.top + (to_rect.bottom + to_rect.top) / 2;
	rect_draw_some_item(storage_gworld,from_rect,storage_gworld,to_rect,0,1);
	play_sound(0);
	FlushAndPause(60);
	
}

// This tells the dialog engine to kill the dialog, and refreshes the screen
//void final_process_dialog(short which_dlog)
//{
//	GrafPtr old_port;
//
//	cd_kill_dialog(which_dlog,0);
//
//	GetPort(&old_port);
//	SetPort(GetWindowPort(mainPtr));
//	BeginUpdate(mainPtr);
//	if (in_startup_mode == false)
//		refresh_screen(0);
//		else draw_startup(0);
//	EndUpdate(mainPtr);
//	SetPort(old_port);
//}
/*
void HideMenuBar( void )
{
	GDHandle		mainScreen;
	Rect			mainScreenBounds;
	RgnHandle		mainScreenRgn;
	GrafPtr			windowPort;
	GrafPtr			oldPort;
	WindowPtr		frontWindow;
	
	// get the gdhandle of the main screen which
	// is the screen with the menubar
	mainScreen = GetMainDevice();
	mainScreenBounds = ( *mainScreen )->gdRect;
	
	// get new region encompassing entire screen
	// including area under menu bar and corners
	mainScreenRgn 	= NewRgn();
	newGrayRgn 		= NewRgn();
	underBarRgn 	= NewRgn();
	RectRgn( mainScreenRgn, &mainScreenBounds );
//	UnionRgn( mainScreenRgn, originalGrayRgn, newGrayRgn );
	DiffRgn( newGrayRgn, originalGrayRgn, underBarRgn );
	DisposeRgn( mainScreenRgn );
	
	// Set gray region to entire screen
//	LMSetGrayRgn( newGrayRgn );
	
	GetPort( &oldPort );
	GetWMgrPort( &windowPort );
	SetPort( windowPort );
	SetClip( newGrayRgn );
	
	// redraw the desktop to draw over the menu bar
	PaintOne( nil, newGrayRgn );
	
	// in case any part of a window is covered
	// redraw it and recalculate the visible region
	frontWindow = FrontWindow();
	PaintOne( ( WindowRef )frontWindow, underBarRgn );
	PaintBehind( ( WindowRef )frontWindow, underBarRgn );
	CalcVis( ( WindowRef )frontWindow );
	CalcVisBehind( ( WindowRef )frontWindow, underBarRgn );
	
	// set menu bar height to 0
	LMSetMBarHeight( 0 );
	
	// Restore the port
	SetPort( oldPort );
	
}//HideMenuBar
*/
/*
void ShowMenuBar( void )
{
	WindowPtr	frontWindow;
	GrafPtr		windowPort;
	GrafPtr		oldPort;
	
	// Reset the menu bar height
//	LMSetMBarHeight( menuBarHeight );
	
	// Restore the original gray region
//	LMSetGrayRgn( originalGrayRgn );
	
	frontWindow = FrontWindow();
	CalcVis( frontWindow );
	CalcVisBehind(frontWindow, newGrayRgn );
	
	// Reset the clipping regions of the window mgr port
	GetPort( &oldPort );
	GetWMgrPort( &windowPort );
	SetPort( windowPort );
	SetClip( newGrayRgn );
	SetPort( oldPort );

	// Redraw the menu bar
	HiliteMenu( 0 );
	DrawMenuBar();	
}
*/
/*
void HideShowMenuBar( )
{
	GDHandle	mainScreen;
	
	// store current gray region that displays
	// menu bar and the current height of the menu bar
	originalGrayRgn 	= LMGetGrayRgn(); 		
	menuBarHeight	= LMGetMBarHeight(); 	

	// calculate the rect of the menu bar to test 
	// if mouse down is in if desired
	mainScreen 			= GetMainDevice();
	menuBarRect 			= (**mainScreen).gdRect;
	menuBarRect.bottom 	= menuBarHeight;

	HideMenuBar();
		
	while( !Button() );

	ShowMenuBar();
	
	// restore the original gray region to 
	// make the menu bar visible
	LMSetGrayRgn( originalGrayRgn );	
}	

*/