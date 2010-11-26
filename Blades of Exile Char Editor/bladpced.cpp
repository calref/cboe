// Blades of Exile Editor
// May God have mercy on all our souls.

#include <windows.h>

#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "math.h"

#include "global.h"
#include "graphics.h"
#include "editors.h"
#include "edfileio.h"
#include "edsound.h"
#include "dlogtool.h"
#include "edaction.h"
#include "graphutl.h"

void check_game_done();
void load_cursors();
void set_cursor(HCURSOR which_curs);
void change_cursor(POINT where_curs);
void check_colors();
void cursor_go();
void cursor_stay();
Boolean verify_restore_quit(short mode);
void max_window(HWND window);
void update_item_menu(short mode);
void check_cd_event(HWND hwnd,UINT message,UINT wparam,LONG lparam);

scen_item_data_type scen_item_list;
short item_menus_lock=1;

/* Mac stuff globals */
RECT	windRECT, Drag_RECT;
Boolean All_Done = FALSE;
short had_text_freeze = 0,num_fonts;
Boolean in_startup_mode = TRUE,app_started_normally = FALSE,file_in_mem = FALSE;
Boolean play_sounds = TRUE,first_startup_update = TRUE,party_in_scen = FALSE;
Boolean diff_depth_ok = FALSE,first_sound_played = FALSE,spell_forced = FALSE,startup_loaded = FALSE;
Boolean save_maps = FALSE,first_time = FALSE,palette_suspect = FALSE,window_in_front = TRUE;

HWND sbar;

Boolean bgm_on = FALSE,bgm_init = FALSE;

Boolean gInBackground = FALSE;
long start_time;

// Cursors
short current_cursor = 120;
HCURSOR arrow_curs[3][3], sword_curs, boot_curs, key_curs, target_curs,talk_curs,look_curs;

RECT pc_area_buttons[6][4] ; // 0 - whole 1 - pic 2 - name 3 - stat strs 4,5 - later
RECT item_string_rects[24][4]; // 0 - name 1 - drop  2 - id  3 -
RECT pc_info_rect; // Frame that holds a pc's basic info and items
RECT name_rect; //Holds pc name inside pc_info_rect
RECT info_area_rect;
RECT hp_sp_rect;   // Holds hit points and spells points for pc
RECT skill_rect;	// Holds "Skills:" string
RECT pc_skills_rect[19]; //Holds current pc's  skill levels
RECT status_rect;  //Holds the string "Status:"
RECT pc_status_rect[10]; //Holds first 8 effects on pc
RECT traits_rect; //Holds the string "Traits:"
RECT pc_traits_rect[16]; //Holds pc traits
RECT pc_race_rect; //Holds current pc's race
RECT edit_rect[5][2]; //Buttons that bring up pc edit dialog boxs

short store_flags[3];
char town_strs[180][256];

/* Adventure globals */
party_record_type party;
pc_record_type adven[6];
outdoor_record_type outdoors[2][2];
current_town_type  c_town;
big_tr_type  t_d;
stored_items_list_type 	t_i;
unsigned char out[96][96];
 unsigned char out_e[96][96];
setup_save_type setup_save;
unsigned char misc_i[64][64],sfx[64][64];

short current_active_pc = 0;

stored_items_list_type stored_items[3];
stored_town_maps_type town_maps;
stored_outdoor_maps_type o_maps;


/* Display globals */
short overall_mode = 45,current_spell_range;
Boolean first_update = TRUE,anim_onscreen = FALSE,frills_on = TRUE,sys_7_avail,suppress_stat_screen = FALSE;
short stat_window = 0,store_modifier;
Boolean monsters_going = FALSE;
short give_delays = 0;
Boolean dialog_not_toast = TRUE;

// storage for dialogs
short store_chosen_pc;
short store_choice_mode;
location store_choice_loc;

short town_size[3] = {64,48,24};
short which_item_page[6] = {0,0,0,0,0,0}; // Remembers which of the 2 item pages pc looked at
short pixel_depth,dialog_answer;


// Spell casting globals
short store_mage = 0, store_priest = 0;
short store_mage_lev = 0, store_priest_lev = 0;
short store_spell_target = 6,pc_casting;
short num_targets_left = 0;
location spell_targets[8];


/* Windoze stuff globals */
Boolean cursor_shown = TRUE;
short store_pc_being_created;

short ulx = 0, uly = 0;

HWND mainPtr;
HWND force_dlog = NULL;
HFONT font,small_bold_font,italic_font,underline_font,bold_font,tiny_font;
HBITMAP bmap = NULL;
//HPALETTE hpal;
HDC main_dc,main_dc2,main_dc3;
HINSTANCE store_hInstance;
HACCEL accel;
BOOL event_handled;

char szWinName[] = "Blades of Exile dialogs";
char szAppName[] = "Blades of Exile Editor";
char file_path_name[256];

Boolean block_erase = FALSE;

long CALLBACK _export WndProc (HWND, UINT, WPARAM, LPARAM);
Boolean handle_menu (short, HMENU);

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass,wndclass2;
	short seed;

	if (!hPrevInstance) {
		wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
		wndclass.lpfnWndProc = WndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInstance;
		wndclass.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(10));
		wndclass.hCursor = NULL;
		wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
		wndclass.lpszMenuName = MAKEINTRESOURCE(1);
		wndclass.lpszClassName = szAppName;

		RegisterClass(&wndclass);

		wndclass2.style = CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
		wndclass2.lpfnWndProc = WndProc;
		wndclass2.cbClsExtra = 0;
		wndclass2.cbWndExtra = 0;
		wndclass2.hInstance = hInstance;
		wndclass2.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(10));
		wndclass2.hCursor = NULL;
		wndclass2.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
		wndclass2.lpszMenuName = NULL;
		wndclass2.lpszClassName = szWinName;

		RegisterClass(&wndclass2);
		}

		mainPtr = CreateWindow (szAppName,
			"Classic Blades of Exile Character Editor build_26.11.2010",
			WS_OVERLAPPEDWINDOW,
			0,
			0,
			536,
			478,
			NULL,
			NULL,
			hInstance,
			NULL);

	if (!hPrevInstance) { // initialize
		Get_Path(file_path_name); //store the path to the executable
  		store_hInstance = hInstance;
		accel = LoadAccelerators(hInstance, MAKEINTRESOURCE(1));

        get_reg_data();

		load_cursors();
		seed = (short) GetCurrentTime();
		srand(seed);

		max_window(mainPtr);
		Set_up_win ();

		font = CreateFont(-9,0,0,0,0, 0,0,0, 0,0,
			0,0,0,"MS Sans Serif");
		small_bold_font = CreateFont(12,0,0,0,700, 0,0,0, 0,0,
			0,0,0,"MS Sans Serif");
		italic_font = CreateFont(12,0,0,0,0, 1,0,0, 0,0,
			0,0,0,"MS Sans Serif");
		underline_font = CreateFont(12,0,0,0,0, 0,1,0, 0,0,
			0,0,0,"MS Sans Serif");
		bold_font = CreateFont(14,0,0,0,700, 0,0,0, 0,0,
			0,0,0,"MS Sans Serif");
		tiny_font = font;
		load_sounds();

		ShowWindow(mainPtr,nCmdShow);

		init_main_buttons();

		file_initialize();
		check_colors();
		cursor_stay();
		update_item_menu(1);

		cd_init_dialogs();

		FCD(900,0);
		}

		event_handled = FALSE;
		while(GetMessage(&msg,NULL,0,0)) {
			if (event_handled == FALSE) {
				if (!TranslateAccelerator(mainPtr, accel, &msg)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
					}
				}
			}
		return msg.wParam;
}

long CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
HDC hdc;
PAINTSTRUCT ps;
POINT press;
HMENU menu;

	switch (message) {
	case WM_KEYDOWN:
		if (hwnd != mainPtr)
			check_cd_event(hwnd,message,wParam,lParam);
		return 0;
		break;

	case WM_CHAR:
		if (hwnd != mainPtr)
			check_cd_event(hwnd,message,wParam,lParam);
		return 0;
		break;

	case WM_LBUTTONDOWN:
		cursor_stay();
		if (hwnd != mainPtr)
			check_cd_event(hwnd,message,wParam,lParam);
			else {
				SetFocus(hwnd);
				press = MAKEPOINT(lParam);

				All_Done = handle_action(press, wParam,lParam);
				check_game_done();
				}
		return 0;
		break;


	case WM_RBUTTONDOWN:
		cursor_stay();
		if (hwnd != mainPtr)
			check_cd_event(hwnd,message,wParam,lParam);
		return 0;
		break;

	case WM_ACTIVATE:
		if (hwnd == mainPtr) {
			if ((LOWORD(wParam) == WA_ACTIVE) ||(LOWORD(wParam) == WA_CLICKACTIVE)) {
				window_in_front = TRUE;
				 }
			if (LOWORD(wParam) == WA_INACTIVE) {
				window_in_front = FALSE;
				}
			}
		return 0;


	case WM_MOUSEMOVE:
		set_cursor(sword_curs);
      return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd,&ps);
		EndPaint(hwnd,&ps);
		if (hwnd != mainPtr)
			check_cd_event(hwnd,message,wParam,lParam);
			else redraw_screen();
		return 0;


	case WM_COMMAND:
		if (hwnd == mainPtr) {

			menu = GetMenu(mainPtr);
			handle_menu((short) LOWORD(wParam), menu);
			check_game_done();
			}
			else {
				check_cd_event(hwnd,message,wParam,lParam);
				}
		return 0;


	case WM_DESTROY:
	case WM_CLOSE:
		if (hwnd == mainPtr) {
		lose_graphics();
		PostQuitMessage(0);
			}
		return 0;
	case WM_QUIT:
      if (hwnd == mainPtr){
			lose_graphics();
			PostQuitMessage(0);
			}
		break;
	}

return DefWindowProc(hwnd,message,wParam,lParam);
}

void check_game_done()
{
  //	through_sending();
	if (All_Done == TRUE) {
		lose_graphics();
		PostQuitMessage(0);
		}
}

Boolean handle_menu (short item, HMENU)
{
	short i,j,k;
	Boolean to_return = FALSE;
   item_record_type store_i;


	if ((file_in_mem == FALSE) && ((item == 1) || (item == 3) ||
		(item >= 40)) && (item != 200) && (item != 100)) {
		display_strings(20,5,0,0,"Editing party",57,707,0);
		item = -1;
		}

	switch (item) {
   	case -1: break;
		case 1:
			save_file(0);
			break;
		case 2:
			if (verify_restore_quit(1) == TRUE)
				load_file();
                update_item_menu(item_menus_lock);
			break;
		case 3:
            save_file(1);
            break;
		case 4:
			All_Done = verify_restore_quit(0);
			break;

		case 41:
				edit_gold_or_food(0);
				redraw_screen();

		break;
		case 42:
				edit_gold_or_food(1);
				redraw_screen();
		break;

		case 44:
			if (party.stuff_done[304][0] > 0) {
				FCD(909,0);
				break;
				}
			FCD(901,0);
			leave_town();
			break;

		case 45:
			if (party.stuff_done[304][0] == 0) {
				FCD(911,0);
				break;
				}
			FCD(910,0);
			c_town.p_loc.x = party.stuff_done[304][1];
			c_town.p_loc.y = party.stuff_done[304][2];
			party.stuff_done[304][0] = 0;
			for (i = 0; i < 6; i++)
				if (adven[i].main_status >= 10)
					adven[i].main_status -= 10;
			redraw_screen();
			break;


		case 48:
			display_strings(20,20,0,0,"Editing party",57,707,0);
			for (i = 0; i < 4; i++)
				party.creature_save[i].which_town = 200;
			break;
		case 51: // damage
			display_strings(20,1,0,0,"Editing party",57,715,0);
			for (i = 0; i < 6; i++)
				adven[i].cur_health = adven[i].max_health;
			redraw_screen();
			break;
		case 52: // spell pts
			display_strings(20,2,0,0,"Editing party",57,715,0);
			for (i = 0; i < 6; i++)
				adven[i].cur_sp = adven[i].max_sp;
			redraw_screen();
			break;
		case 53: // raise dead
			display_strings(20,3,0,0,"Editing party",57,715,0);
			for (i = 0; i < 6; i++)
				if ((adven[i].main_status == 2) || (adven[i].main_status == 3) ||
					(adven[i].main_status == 4))
						adven[i].main_status = 1;
			redraw_screen();
			break;
		case 54: // conditions
			display_strings(20,4,0,0,"Editing party",57,715,0);
			for (i = 0; i < 6; i++) {
				adven[i].status[2] = 0;
				if (adven[i].status[3] < 0)
					adven[i].status[3] = 0;
				adven[i].status[6] = 0;
				adven[i].status[7] = 0;
				adven[i].status[9] = 0;
				adven[i].status[11] = 0;
				adven[i].status[12] = 0;
				adven[i].status[13] = 0;
				}
			redraw_screen();
			break;

		case 55:
			if (party_in_scen == FALSE) {
				display_strings(20,25,0,0,"Editing party",57,715,0);
				break;
				}
			if (FCD(912,0) != 1)
				break;
			remove_party_from_scen();
			party.stuff_done[304][0] = 0;
			redraw_screen();
			break;
		case 62:
			 display_alchemy();
			break;
		case 63: // all property
			display_strings(20,6,0,0,"Editing party",57,707,0);
			for (i = 0; i < 30; i++) {
				party.boats[i].property = FALSE;
				party.horses[i].property = FALSE;
				}
			break;
		case 70: // edit day
			edit_day();
			break;
		case 71: // out maps
			if (party_in_scen == FALSE) {
				display_strings(20,25,0,0,"Editing party",57,715,0);
				break;
				}
			display_strings(20,13,0,0,"Editing party",57,715,0);
			for (i = 0; i < 96; i++)
				for (j = 0; j < 96; j++)
					out_e[i][j] = 1;
			for (i = 0; i < 100; i++)
				for (j = 0; j < 6; j++)
					for (k = 0; k < 48; k++)
						o_maps.outdoor_maps[i][j][k] = 255;

			break;
		case 72: // town maps
			if (party_in_scen == FALSE) {
				display_strings(20,25,0,0,"Editing party",57,715,0);
				break;
				}
			display_strings(20,14,0,0,"Editing party",57,715,0);
			for (i = 0; i < 64; i++)
				for (j = 0; j < 64; j++)
					c_town.explored[i][j] = 1;
			for (i = 0; i < 200; i++)
				for (j = 0; j < 8; j++)
					for (k = 0; k < 64; k++)
						town_maps.town_maps[i][j][k] = 255;
			break;
		case 100: // index
			WinHelp(mainPtr,"Blades of Exile.hlp",HELP_CONTENTS,0L);
			break;
		case 200: // about
			FCD(1062,0);
			break;
		default:
			item -= 600;
			store_i = scen_item_list.scen_items[item];
			if(store_i.variety == 3){
		        party.gold += get_ran(1,1,20);
               	redraw_screen();
                break;
            }
			if(store_i.variety == 11){
                party.food += get_ran(1,1,20);
               	redraw_screen();
                break;
            }
			give_to_pc(current_active_pc,store_i,FALSE);
			draw_items(1);
			break;

		}

	return to_return;
}

void load_cursors()
{
	sword_curs = LoadCursor(store_hInstance,MAKEINTRESOURCE(120));

	set_cursor(sword_curs);
	current_cursor = 124;
}

void set_cursor(HCURSOR which_curs)
{
	SetCursor (which_curs);
}


void check_colors()
{
	short a,b;

	a = GetDeviceCaps(main_dc,BITSPIXEL);
	b = GetDeviceCaps(main_dc,PLANES);
	if (a * b < 8) {
		MessageBox(mainPtr,"The Blades of Exile Editor is designed for 256 colors. The current graphics device is set for less. Exile is playable with less colors, but will look somewhat odd."	,
	  "Not 256 colors!",MB_OK | MB_ICONEXCLAMATION);
		MessageBox(mainPtr,"For tips on how to get 256 colors, hit F1 for help, and then select 'Getting 256 Colors' from the table of contents."	,
	  "Not 256 colors!",MB_OK | MB_ICONEXCLAMATION);
	  }
}

void cursor_go()
{
	if (in_startup_mode == TRUE)
		return;
	if (cursor_shown == TRUE) {
		cursor_shown = FALSE;
		showcursor(FALSE);
		}
}

void cursor_stay()
{
	if ((cursor_shown == FALSE) || (in_startup_mode == TRUE)) {
		cursor_shown = TRUE;
		showcursor(TRUE);
      }
}

Boolean verify_restore_quit(short mode)
//short mode; // 0 - quit  1- restore
{
	short choice;

	if (file_in_mem == FALSE)
		return TRUE;
	choice = FCD(1066 + mode,0);
	if (choice == 3)
		return FALSE;
	if (choice == 2)
		return TRUE;
	save_file(0);
	return TRUE;
}

void update_item_menu(short mode)
//mode 0 - display item menus 1 - lock menus
{
	short i,j;

	HMENU menu[10],big_menu;
	char item_name[256];

	big_menu = GetMenu(mainPtr);

	for (i = 0; i < 10; i++)
		menu[i] = GetSubMenu(big_menu,3 + i);
	for(j=0;j<10;j++){  //first let's clean the menu
       DeleteMenu(menu[j],1000 + j,MF_BYCOMMAND); //If there is any dummy, flush it
	   for (i=0;i<40;i++)
       		DeleteMenu(menu[j],600+(40*j)+i,MF_BYCOMMAND);
    }
    switch(mode){
    case 0:
	for (j = 0; j < 10; j++) { //then populate it
		for (i = 0; i < 40; i++) {
				sprintf(item_name, "%s",scen_item_list.scen_items[i + j * 40].full_name);
				if ((i % 20 == 0) && (i > 0))
					AppendMenu(menu[j],MF_MENUBREAK | MF_BYCOMMAND | MF_ENABLED | MF_STRING, 600 + (40 * j) + i, item_name);
					else AppendMenu(menu[j],MF_BYCOMMAND | MF_ENABLED | MF_STRING, 600 + (40 * j) + i, item_name);
				}
		}
    break;

    case 1:
    for (j = 0; j < 10; j++) //then lock menus
	       AppendMenu(menu[j],MF_MENUBREAK | MF_BYCOMMAND | MF_GRAYED | MF_STRING, 600 + (40 * j), "None");
    break;
    }
}

void max_window(HWND window)
{
	RECT main_rect,wind_rect;
	short width,height;

	cursor_shown = TRUE;
	showcursor(TRUE);

	GetWindowRect(GetDesktopWindow(),&main_rect);
	GetWindowRect(window,&wind_rect);
	width = main_rect.right - main_rect.left;
	height = main_rect.bottom - main_rect.top;
	MoveWindow(window,0,0,width,height,TRUE);

}

void check_cd_event(HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam)
{
	POINT press;
	short wind_hit = -1,item_hit = -1;

	switch (message) {
		case WM_COMMAND:
			if (LOWORD(wparam) == 150)
				break;
  			cd_find_dlog(hwnd,&wind_hit,&item_hit); // item_hit is dummy
			item_hit = (short) LOWORD(wparam);
			break;
		case WM_KEYDOWN:
			wind_hit = cd_process_syskeystroke(hwnd, wparam, lparam,&item_hit);
			break;

		case WM_CHAR:
			wind_hit = cd_process_keystroke(hwnd, wparam, lparam,&item_hit);
			break;

		case WM_LBUTTONDOWN:
			press = MAKEPOINT(lparam);
			wind_hit = cd_process_click(hwnd,press, wparam, lparam,&item_hit);
			break;
		case WM_RBUTTONDOWN:
			press = MAKEPOINT(lparam);
			wparam = wparam | MK_CONTROL;
			wind_hit = cd_process_click(hwnd,press, wparam, lparam,&item_hit);
			break;

		case WM_PAINT:
			cd_redraw(hwnd);
			break;
		}
	if (wind_hit < 0)
		return;
	switch (wind_hit) {
		case -1: break;
		case 917: edit_day_event_filter(item_hit); break;
		case 970: case 971: case 972: case 973: display_strings_event_filter(item_hit); break;
		case 991: display_pc_event_filter(item_hit); break;
		case 996: display_alchemy_event_filter(item_hit); break;
		case 1010: spend_xp_event_filter (item_hit); break;
		case 1012: case 947: edit_gold_or_food_event_filter (item_hit); break;
		case 1013: pick_race_abil_event_filter (item_hit); break;
		case 1018: select_pc_event_filter (item_hit); break;
		case 1024: edit_xp_event_filter (item_hit); break;
		default: fancy_choice_dialog_event_filter (item_hit); break;
  		}
}
