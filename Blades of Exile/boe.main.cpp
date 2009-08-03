#include <windows.h>
#include "global.h"
#include "boe.graphics.h"
#include "boe.newgraph.h"
#include "boe.fileio.h"
#include "boe.actions.h"
#include "boe.text.h"
#include "boe.party.h"
#include "tools/exlsound.h"
#include "boe.items.h"
#include "boe.fields.h"
#include "boe.town.h"
#include "tools/dlogtool.h"
#include "boe.dlgutil.h"
#include "boe.infodlg.h"
#include "boe.graphutil.h"
#include "boe.main.h"

#include "globvar.h"

void startup_load()
{
	load_file();
	if (in_startup_mode == FALSE)
	{
		end_startup();
		post_load();
	}
	else
	{
		menu_activate(0);
		draw_startup(0);
	}
}

Boolean handle_startup_press(POINT the_point)
{
	short i,scen;
	long dummy;
	
	the_point.x -= ulx;
	the_point.y -= uly;
	
	for (i = 0; i < 5; i++) 
		if (PtInRect(&startup_button[i],the_point) == TRUE)
		{
		draw_start_button(i,5);
		
		if (play_sounds == TRUE) play_sound(37);
			else Delay(5,&dummy);
			
		draw_start_button(i,0);	
		
			switch (i) {
				case 0:
					startup_load();
					break;
					
				case 1:
					draw_startup(0);
					start_new_game();
					draw_startup(0);
					break;
			
				case 2:
					/* how to order button */
					//draw_startup(0);
					break;

				case 3:
					if (party_in_memory == FALSE) {   // check if party is loaded
						FCD(867,0);                   //no party loaded error message
						break;
						}
						
					scen = pick_prefab_scen();             // return a short in scen specifying the chosen scenario
					if (scen < 0)
						break;

					switch (scen) {                                                  //put scenario filename in party.scen_name
						case 0: sprintf(party.scen_name,"valleydy.exs"); break;
						case 1: sprintf(party.scen_name,"stealth.exs"); break;
						case 2: sprintf(party.scen_name,"zakhazi.exs"); break;
						}

					put_party_in_scen();                                              //put the party in the scenario and begin the game
					break;

				case 4: // intro
					if (party_in_memory == FALSE) {               // check if party is loaded
						FCD(867,0);                               //no party loaded error message
						break;
						}
					scen = pick_a_scen();                              // return a short to scen specifying the chosen scenario

/*   				if (scen_headers[scen].prog_make_ver[0] >= 2) { => warning if scen = -1 (no scenarios in directory), then it will crash !!!
						FCD(912,0);
						break;
						}*/

					if (scen >= 0) {
						sprintf(party.scen_name,"%s",data_store2[scen].scen_names); //put scenario filename in party.scen_name retrieving it using scen
						put_party_in_scen();                                        //put the party in the scenario and begin the game
						}

					break;

				default:
					Delay(50,&dummy);
					return TRUE;
				}
			}
	return FALSE;
}

void loadCursors()
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			arrow_curs[i][j] =
				LoadCursor(store_hInstance,MAKEINTRESOURCE(100 + (i - 1) + 10 * (j - 1)));
	
	sword_curs 	=	LoadCursor(store_hInstance,MAKEINTRESOURCE(120));
	key_curs 	= 	LoadCursor(store_hInstance,MAKEINTRESOURCE(122));
	target_curs	= 	LoadCursor(store_hInstance,MAKEINTRESOURCE(124));
	talk_curs 	= 	LoadCursor(store_hInstance,MAKEINTRESOURCE(126));
	look_curs 	= 	LoadCursor(store_hInstance,MAKEINTRESOURCE(129));
}

void loadFonts()
{
	AddFontResource("maidword.ttf"); // add font to the system font table
	font = CreateFont(12,0,0,0,0, 0,0,0, 0,0, 0,0,0,"MS Sans Serif");
	small_bold_font = CreateFont(12,0,0,0,700, 0,0,0, 0,0, 0,0,0,"MS Sans Serif");
	italic_font = CreateFont(12,0,0,0,700, 1,0,0, 0,0, 0,0,0,"MS Sans Serif");
	underline_font = CreateFont(12,0,0,0,0, 0,1,0, 0,0,	0,0,0,"MS Sans Serif");
	bold_font = CreateFont(14,0,0,0,700, 0,0,0, 0,0, 0,0,0,"MS Sans Serif");
	tiny_font = font;
	fantasy_font = CreateFont(22,0,0,0,0, 0,0,0, 0,0, 0,0,0,"MaidenWord");
	if (fantasy_font == NULL)
	{
		fantasy_font = CreateFont(22,0,0,0,0, 0,0,0, 0,0, 0,0,0,"MS Sans Serif");
	}
}

int WINAPI WinMain(HINSTANCE hInstance,	HINSTANCE hPrevInstance, LPSTR, int	nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass, wndclass2;
    RECT windRECT;
    HINSTANCE boeSoundsDLL;

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

	mainPtr = CreateWindow (szAppName, "Classic Blades of Exile 1.0",
			WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
			0,
			0,
			588,
			478,
			NULL,
			NULL,
			hInstance,
			NULL);

	if (!hPrevInstance) 
	{ // initialize
		Get_Path(file_path_name);
		
		store_hInstance = hInstance;
		accel = LoadAccelerators(hInstance, MAKEINTRESOURCE(1));
		
		boeSoundsDLL = LoadLibrary("./boesounds.dll");
		
		if (!boeSoundsDLL)
		{
			MessageBox(mainPtr, "BOESOUNDS.DLL not found", "Error", MB_OK | MB_ICONEXCLAMATION);
			return (-1);
		}
		
		load_sounds(boeSoundsDLL);
		loadFonts();
		loadCursors();
	
		SetCursor(sword_curs);
		current_cursor = 124;
		/* cursors loaded */
		
		data_store = new piles_of_stuff_dumping_type;
		data_store3 = new piles_of_stuff_dumping_type3;
		data_store4 = new piles_of_stuff_dumping_type4;
		data_store5 = new piles_of_stuff_dumping_type5;
		scen_item_list = new scen_item_data_type;

		srand(GetCurrentTime());

		get_reg_data();

		if (display_mode != 5)
			max_window(mainPtr);
			else {
					GetWindowRect(GetDesktopWindow(),&windRECT);
					MoveWindow(mainPtr,(windRECT.right - (588 + 10)) / 2,
					(windRECT.bottom - (425 + 52)) / 2 ,
					588 + 10,425 + 52,TRUE);
				}

		shop_sbar = CreateWindow("scrollbar",NULL,
			WS_CHILD | WS_TABSTOP | SBS_VERT, shop_sbar_rect.left,shop_sbar_rect.top,shop_sbar_rect.right,shop_sbar_rect.bottom,
			mainPtr,(HMENU) 3, store_hInstance,NULL);
        lpsi.fMask = SIF_RANGE;
        lpsi.nMax = 16;
        SetScrollInfo(shop_sbar,SB_CTL,&lpsi,FALSE);
//		SetScrollRange(shop_sbar,SB_CTL,0,16,FALSE);

		ShowWindow(mainPtr, nCmdShow);

		plop_fancy_startup();
		
		init_screen_locs();

		FlushEvents(2);

		SetTimer(mainPtr,1,620,NULL);
		SetTimer(mainPtr,2,200,NULL);
		
		file_initialize();
		
		if (GetDeviceCaps(main_dc,BITSPIXEL) * GetDeviceCaps(main_dc,PLANES) < 8)
		{
			MessageBox(mainPtr,"Blades of Exile is designed for 256 colors. The current graphics device is set for less. Exile 3 is playable with less colors, but will look somewhat odd."	,
		  "Not 256 colors!",MB_OK | MB_ICONEXCLAMATION);
			MessageBox(mainPtr,"For tips on how to get 256 colors, hit F1 for help, and then select 'Getting 256 Colors' from the table of contents."	,
		  "Not 256 colors!",MB_OK | MB_ICONEXCLAMATION);
		}
		
		menu_activate(0);
		cursor_stay();
		showcursor(TRUE);
		reset_text_bar();

		adjust_window_mode();

		cd_init_dialogs();

		if (game_run_before == FALSE) {
         FCD(986,0);
         WritePrivateProfileString("Blades of Exile", "game_run_before", "1", "./blades.ini");
        }
		else if (give_intro_hint == TRUE) tip_of_day();

		}
		event_handled = FALSE;
		
		while(GetMessage(&msg,NULL,0,0))
		{
			if (event_handled == FALSE)
			{
				if (!TranslateAccelerator(mainPtr, accel, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}		
		
		delete scen_item_list;
        delete data_store5;
		delete data_store4;
		delete data_store3;
        if(data_store2 != NULL)
    		delete[] data_store2;
        if(scen_headers != NULL)
   		   delete[] scen_headers;
		delete data_store;
		
		lose_graphics();
		
		FreeLibrary((HMODULE) boeSoundsDLL);
				
		return msg.wParam;
}

long CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	POINT press;
	HMENU menu;
	RECT dlg_rect;
	RECT wind_rect;
	
	short handled = 0;
	short store_ulx;
	short store_uly;
	short sbar_pos;
	short which_sbar;
	short old_setting;
	short smin;
	short smax;

// First, handle window size
	GetWindowRect(mainPtr,&wind_rect);
	
	switch (display_mode) { //case 0 is center
		case 1:	store_ulx = 0;
				store_uly = 0;
				break;
		case 2: store_ulx = wind_rect.right - 550 - 21;
				store_uly = 0;
				break;
		case 3: store_ulx = 0;
				store_uly = wind_rect.bottom - 422 - 24 - 21;
				break;
		case 4: store_ulx = wind_rect.right - 550 - 21;
				store_uly = wind_rect.bottom - 422 - 24 - 21;
				break;
		case 5: store_ulx = 0;
				store_uly = 0;
				break;
		default:
				store_ulx = ((wind_rect.right - wind_rect.left) - 536) / 2 - 30;
				store_uly = ((wind_rect.bottom - wind_rect.top) - 478) / 2 + 8;
				break;
		}
		
	store_ulx -= store_ulx % 8;
	store_uly -= store_uly % 8;
	
	if ((store_ulx != ulx) || (store_uly != uly))
	{
		ulx = store_ulx;
		uly = store_uly;
		adjust_window_mode();
	}

	switch (message) {
	case WM_KEYDOWN:
		if (hwnd != mainPtr)
			check_cd_event(hwnd,message,wParam,lParam);
			else {
				cursor_go();
				All_Done = handle_syskeystroke(wParam,lParam,&handled);
				check_game_done();
				}
		if (handled == 1)
			FlushEvents(0);
			
		return 0;

	case WM_CHAR:
		if (hwnd != mainPtr)
			check_cd_event(hwnd,message,wParam,lParam);
			else {
				cursor_go();
				All_Done = handle_keystroke(wParam,lParam);
				check_game_done();
				}
		return 0;

	case WM_LBUTTONDOWN:
		cursor_stay();
		if (hwnd != mainPtr) check_cd_event(hwnd,message,wParam,lParam);
		else {
				SetFocus(hwnd);
				press = MAKEPOINT(lParam);
				if (in_startup_mode == FALSE) 
						All_Done = handle_action(press, wParam,lParam);
				else 	All_Done = handle_startup_press(press);
				change_cursor(press);
				check_game_done();
				}
		return 0;
		break;

	case WM_RBUTTONDOWN:
		cursor_stay();
		if (hwnd != mainPtr)
			check_cd_event(hwnd,message,wParam,lParam);
			else {
				SetFocus(hwnd);
				press = MAKEPOINT(lParam);
				if (in_startup_mode == FALSE)
					All_Done = handle_action(press, wParam,-2);
					else All_Done = handle_startup_press(press);
				change_cursor(press);
				check_game_done();
				}
		return 0;
		break;
		
	case WM_ACTIVATE:
		if (hwnd == mainPtr)
		{
			WORD fActive = LOWORD(wParam);
			
			if ((fActive == WA_ACTIVE) ||(fActive == WA_CLICKACTIVE))
			{
				window_in_front = TRUE;
				GetClientRect(hwnd,&dlg_rect);
				InvalidateRect(hwnd,&dlg_rect,FALSE);
			}
			
			if (fActive == WA_INACTIVE)
			{
				cursor_stay();
				anim_onscreen = FALSE;
				window_in_front = FALSE;
			}
		}
		return 0;

	case WM_ERASEBKGND:
		if (hwnd != mainPtr) break;
		block_erase = FALSE;
		return 1;
		
	case WM_MOUSEMOVE:
		if (store_mouse != lParam) cursor_stay();
		store_mouse = lParam;
		press = MAKEPOINT(lParam);
		if (hwnd == mainPtr) {
			change_cursor(press);
			if ((overall_mode > 10) && (overall_mode < 20))
				draw_targeting_line(press);
			}
		return 0;

	case WM_PAINT:
		if ((block_erase == TRUE) && (hwnd == mainPtr)) {
			block_erase = FALSE;
			}
		hdc = BeginPaint(hwnd,&ps);
		EndPaint(hwnd,&ps);

		if (hwnd != mainPtr)
			check_cd_event(hwnd,message,wParam,lParam);
			else {
				string_added = TRUE;
				reset_text_bar();
				if (in_startup_mode == TRUE) draw_startup(0);
				else if (first_update == TRUE) 
				{
					first_update = FALSE;
					if (overall_mode == 0) redraw_screen(0);
					if ((overall_mode > 0) & (overall_mode < 10)) redraw_screen(1);
				// 1st update never combat
				}
			else refresh_screen(overall_mode);
			}
		return 0;

	case WM_TIMER:
		switch(wParam) {
			case 1:
				if ((in_startup_mode == FALSE) && (anim_onscreen == TRUE)
				 && (window_in_front == TRUE) && (give_delays == FALSE)) {
						anim_ticks++;
						initiate_redraw();
					}
				break;
			case 2:
				if ((overall_mode > 10) && (overall_mode < 20)) {
					GetCursorPos(&press);
					press.x -= wind_rect.left + 3;
					press.y -= wind_rect.top + 41;
					draw_targeting_line(press);
					}
				if ((in_startup_mode == TRUE) && (window_in_front == TRUE))
					draw_startup_anim();
				break;
			}
		return 0;

	case WM_COMMAND:    // 1006 is dummy event
		// creation events 2000 spend xp 2001 pick graphic 2002 finish up
		if (hwnd == mainPtr) {
			menu = GetMenu(mainPtr);
			All_Done = handle_menu((short) LOWORD(wParam), menu);
			check_game_done();
			}
			else check_cd_event(hwnd,message,wParam,lParam);
			
		return 0;

	case WM_VSCROLL:
		which_sbar = GetWindowLong((HWND) lParam, GWL_ID);
		switch (which_sbar) {// 1 text  2 items  3 shop
			case 1:
				sbar_pos = GetScrollPos(text_sbar,SB_CTL);
				switch (wParam ) {
					case SB_PAGEDOWN: sbar_pos += 11; break;
					case SB_LINEDOWN: sbar_pos++; break;
					case SB_PAGEUP: sbar_pos -= 11; break;
					case SB_LINEUP: sbar_pos--; break;
					case SB_TOP: sbar_pos = 0; break;
					case SB_BOTTOM: sbar_pos = 57; break;
					case SB_THUMBPOSITION:
					case SB_THUMBTRACK:
						sbar_pos = LOWORD(lParam);
						break;
					}
				if (sbar_pos < 0) sbar_pos = 0;
				if (sbar_pos > 58) sbar_pos = 58;{
//				SetScrollPos(text_sbar,SB_CTL,sbar_pos,TRUE);
                lpsi.nPos = sbar_pos;
                lpsi.fMask = SIF_POS;
                SetScrollInfo(text_sbar,SB_CTL,&lpsi,TRUE);
                }
				string_added = TRUE;
				print_buf();
			break;
			case 2:
				old_setting = sbar_pos = GetScrollPos(item_sbar,SB_CTL);
				GetScrollRange(item_sbar,SB_CTL,(int *) &smin,(int *) &smax);
				switch (wParam ) {
					case SB_PAGEDOWN: sbar_pos += (stat_window == 7) ? 2 : 8; break;
					case SB_LINEDOWN: sbar_pos++; break;
					case SB_PAGEUP: sbar_pos -= (stat_window == 7) ? 2 : 8; break;
					case SB_LINEUP: sbar_pos--; break;
					case SB_TOP: sbar_pos = 0; break;
					case SB_BOTTOM: sbar_pos = smax; break;
					case SB_THUMBPOSITION:
					case SB_THUMBTRACK:
						sbar_pos = LOWORD(lParam);
						break;
					}
				if (sbar_pos < 0) sbar_pos = 0;
				if (sbar_pos > smax) sbar_pos = smax;{
//				SetScrollPos(item_sbar,SB_CTL,sbar_pos,TRUE);
                lpsi.nPos = sbar_pos;
                lpsi.fMask = SIF_POS;
                SetScrollInfo(item_sbar,SB_CTL,&lpsi,TRUE);
                }
				if (sbar_pos != old_setting)
					put_item_screen(stat_window,1);
			break;
			case 3:
				old_setting = sbar_pos = GetScrollPos(shop_sbar,SB_CTL);
				GetScrollRange(shop_sbar,SB_CTL,(int *) &smin,(int *) &smax);
				switch (wParam ) {
					case SB_PAGEDOWN: sbar_pos += 8; break;
					case SB_LINEDOWN: sbar_pos++; break;
					case SB_PAGEUP: sbar_pos -= 8; break;
					case SB_LINEUP: sbar_pos--; break;
					case SB_TOP: sbar_pos = 0; break;
					case SB_BOTTOM: sbar_pos = smax; break;
					case SB_THUMBPOSITION:
					case SB_THUMBTRACK:
						sbar_pos = LOWORD(lParam);
						break;
					}
				if (sbar_pos < 0) sbar_pos = 0;
				if (sbar_pos > smax) sbar_pos = smax;{
//				SetScrollPos(shop_sbar,SB_CTL,sbar_pos,TRUE);
                lpsi.nPos = sbar_pos;
                lpsi.fMask = SIF_POS;
                SetScrollInfo(shop_sbar,SB_CTL,&lpsi,TRUE);
                }
				if (sbar_pos != old_setting)
					draw_shop_graphics(0,shop_sbar_rect);			
			break;
			}
			
	    SetFocus(mainPtr);
		return 0;

	case WM_DESTROY:
	case WM_CLOSE:
		if (hwnd == mainPtr)
		{
			lose_graphics();
			PostQuitMessage(0);
		}
		return 0;
	case WM_QUIT:
		if (hwnd == mainPtr)
			lose_graphics();
		break;
	}

return DefWindowProc(hwnd,message,wParam,lParam);
}

void check_game_done()
{
	through_sending();
	
	if (All_Done == TRUE)
	{
		lose_graphics();
		showcursor(TRUE);
		PostQuitMessage(0);
	}
}

Boolean handle_menu (short item, HMENU)
{
	short choice,i;
	POINT x = {1001,0},pass_point;
	Boolean to_return = FALSE;

	switch (item) {
		case 1:  // File Menu
			if (in_startup_mode == TRUE) startup_load();
			else do_load();
			break;
		case 2:
			do_save(0);
			break;
		case 3:
			if (in_startup_mode == TRUE) save_file(1);
			else do_save(1);
			break;
		case 4:
			if (in_startup_mode == FALSE)
			{
				choice = FCD(1091,0);
				if (choice == 1) return FALSE;
				for (i = 0; i < 6; i++)
					adven[i].main_status = MAIN_STATUS_ABSENT;
				party_in_memory = FALSE;
				reload_startup();
				in_startup_mode = TRUE;
				draw_startup(0);
			}
			start_new_game();
			draw_startup(0);
			break;
		case 6:
			pick_preferences();
			break;
		case 7:
            pick_compatibility();
            break;
		case 8:  // Quit
			if (in_startup_mode == TRUE) {
				to_return = All_Done = TRUE;
				break;
				}
			if (overall_mode > 1) {
				choice = FCD(1067,0);
				if (choice == 1) return All_Done;
				}
				else {
					choice = FCD(1066,0);
					if (choice == 3)
						break;
					if (choice == 1)
						save_file(0);
					}
			to_return = All_Done = TRUE;
			break;

		// Options menu
		case 21:
			choice = char_select_pc(0,0,"New graphic for who?");
			if (choice < 6)
				pick_pc_graphic(choice,1,0);
			initiate_redraw();
			break;
			
		case 22:
			choice = select_pc(0,0);
			if (choice < 6)
				pick_pc_name(choice,0);
			put_pc_screen();
			put_item_screen(stat_window,0);
			break;
			
			
		case 23:
			if (!(is_town())) {
				add_string_to_buf("Add PC: Town mode only.");
				print_buf();
				break;
				}
			for (i = 0; i < NUM_OF_PCS; i++)
				if (adven[i].main_status == MAIN_STATUS_ABSENT)
					i = 20;
			if (i == INVALID_PC) {
				ASB("Add PC: You already have 6 PCs.");
				print_buf();
				}
			if (c_town.town_num == scenario.which_town_start) {
				give_help(56,0,0);
				create_pc(6,0);
				}
				else {
					add_string_to_buf("Add PC: You can only make new");
					add_string_to_buf("  characters in the town you ");
					add_string_to_buf("  started in.");
					}
			print_buf();
			put_pc_screen();
			put_item_screen(stat_window,0);
			break;	
			
		case 24:
			if (prime_time() == FALSE) {
				ASB("Finish what you're doing first.");
				print_buf();
				}
				else {
					choice = char_select_pc(0,0,"Delete who?");
					if (choice < 6) {
						if ((i = FCD(1053,0)) == 2)
							adven[choice].kill(0);
						}
					initiate_redraw();
					}
			break;
			
		case 27:	
			if (overall_mode == MODE_TALKING) {
				ASB("Talking notes: Can't read while talking.");
				print_buf();
				return to_return;
				}
			talk_notes();
			break;
		case 28:
			adventure_notes();
			break;
		case 29:
			if (in_startup_mode == FALSE) print_party_stats();
			break;

		// Help menu
		case 41: FCD(1079,0);
				break;
		case 42: FCD(1080,0); break;
		case 43: FCD(1081,0); break;
		case 44: FCD(1072,0); break; // magic barriers
		case 46: FCD(1084,0); break;
		case 47: FCD(1088,0); break;

		// Library
		case 61: display_spells(0,100,0); break;
		case 62: display_spells(1,100,0); break;
		case 63: display_skills(100,0); break;
		case 64: display_help(0,0); break;
		case 65: tip_of_day(); break;
		case 67: FCD(986,0); break;

		// Actions
		case 81:
			if (overall_mode != 1) {
				ASB("Alchemy: In town mode only.");
				print_buf();
				break;
            }
				pass_point.x = 1000;
				pass_point.y = 405;
			to_return = handle_action(pass_point,(WPARAM) 0,(LPARAM)-1);
			break;
		case 82:
			to_return = handle_action(x,(WPARAM) 0,(LPARAM)-1);
			break;
		case 84:
			if (prime_time() == FALSE) {
				ASB("Finish what you're doing first.");
				print_buf();
				}
				else {
					give_help(62,0,0);
					display_map();
					}
			SetCursor(sword_curs);
			break;

		// Mage is 399
		case 399: give_help(209,0,0);
			party.help_received[9] = FALSE; break;

		// Priest is 499
		case 499: give_help(209,0,0); 
			party.help_received[9] = FALSE; break;
		
		// Monsters is 599
		case 599: give_help(212,0,0); break;
		
		case 100: // Index
			WinHelp(mainPtr,"Blades of Exile.hlp",HELP_CONTENTS,0L);
			break;
			
		case 200: // About
			FCD(1062,0);
			break;

		default:
			if ((item >= 400) && (item < 500))  { // mage spell
				if (prime_time() == FALSE) {
					ASB("Finish what you're doing first.");
					print_buf();
					}
					else handle_menu_spell(item - 400,0);
				break;
				}
			if ((item >= 500) && (item < 600))  { // priest spell
				if (prime_time() == FALSE) {
					ASB("Finish what you're doing first.");
					print_buf();
					}
					else handle_menu_spell(item - 500,1);
				break;
				}
			if ((item >= 600) && (item < 700))  { // monster spell
				display_monst(item - 600,(creature_data_type *) NULL,1);
				break;
				}
			break;
		}
	if (in_startup_mode == TRUE)
		menu_activate(0);
		else menu_activate(1);
	return to_return;
}

void change_cursor(POINT where_curs)
{
	short curs_types[50] = {0,0,126,124,122,122,0,0,0,0,
					0,124,124,124,124,122,0,0,0,0,
					120,120,0,0,0,0,0,0,0,0,
					0,0,0,0,0,129,129,129,0,0,
					0,0,0,0,0,120,0,0,0,0},cursor_needed;
	location cursor_direction;
	RECT world_screen = {23, 23, 274, 346};

	where_curs.x -= ulx;
	where_curs.y -= uly;

	if (PtInRect(&world_screen,where_curs) == FALSE)
		cursor_needed = 120;
		else cursor_needed = curs_types[overall_mode];

	if (in_startup_mode == TRUE)
		cursor_needed = 120;
		else if (cursor_needed == 0) {
		if ((PtInRect(&world_screen,where_curs) == FALSE) || (in_startup_mode == TRUE))
			cursor_needed = 120;
			else {
				cursor_direction = get_cur_direction(where_curs);
				cursor_needed = 100 + (cursor_direction.x) + 10 * ( cursor_direction.y);
				if (cursor_needed == 100)
					cursor_needed = 120;
				}
		}
	current_cursor = cursor_needed;

	switch (cursor_needed) {
		case 120:
			SetCursor(sword_curs);
			break;
		case 121:
		case 122:
			SetCursor(key_curs);
			break;
		case 124:
			SetCursor(target_curs);
			break;
		case 126:
			SetCursor(talk_curs);
			break;
		case 129:
			SetCursor(look_curs);
			break;
		default:  // an arrow
			SetCursor(arrow_curs[cursor_direction.x + 1][cursor_direction.y + 1]);
			break;
		}	
}

void cursor_go()
{
	if (in_startup_mode == TRUE) return;
	
	if (cursor_shown == TRUE)
	{
		cursor_shown = FALSE;
		showcursor(FALSE);
	}
}

void cursor_stay()
{
	if ((cursor_shown == FALSE) || (in_startup_mode == TRUE))
	{
		cursor_shown = TRUE;
		showcursor(TRUE);
      }
}

void move_sound(unsigned char ter,short step)
{
	short spec;
	
	spec = scenario.ter_types[ter].special;
	
	if ((monsters_going == false) && (overall_mode < 10) && (party.in_boat >= 0)) {
		if (spec == 21)
			return;
		play_sound(48);
		}
	else if ((monsters_going == false) && (overall_mode < 10) && (party.in_horse >= 0)) {////
		play_sound(85);
		}
	else if(spec == 5) //if poisoned land don't play squish sound : BoE legacy behavior, can be removed safely
		  return;
//  else if(spec == 6) //if diseased land do the same
//		  return;
		else switch(scenario.ter_types[ter].step_sound){
            case 0:
                if (step % 2 == 0)          //footsteps alternate sound
					play_sound(49);
				else play_sound(50);
                break;
            case 1:
                play_sound(55);         //squish
                break;
            case 2:
                play_sound(47);         //crunch
                break;
            case 3:
                break;                //silence : do nothing
            default:
                 if (step % 2 == 0)          //safety footsteps valve
					play_sound(49);
				else play_sound(50);
        }
}
