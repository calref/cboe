/*
 *  viewdlog.cpp
 *  Editor
 *
 *  Created by Celtic Minstrel on 15/04/09.
 *
 */

#include "dlgtool.h"
#include "graphtool.h"
#include "soundtool.h"

void Initialize();
void init_dialogs();
void display_strings(char *text1, char *text2, char *title,short graphic_num,short graphic_type,short parent_num);

GWorldPtr anim_gworld, talkfaces_gworld, items_gworld, tiny_obj_gworld, pc_gworld, dlog_gworld, monst_gworld[10], ter_gworld[7], small_ter_gworld, fields_gworld, pc_stats_gworld, item_stats_gworld, mixed_gworld;
ResFileRefNum graphicsRef, soundsRef, boeRef, scenRef, pcRef;
bool All_Done = false;
WindowPtr mainPtr = (WindowPtr) -1;

int main()
{
	short error;
	OSErr err;
	MenuHandle cur_menu;
	FSSpec spec;
	short refNum, localizedRefNum;
	CFBundleRef bundle;
	//start_time = TickCount();
	
	Initialize();
	Point p = {0,0};
	init_graph_tool(NULL,p);
	init_snd_tool();
	
//	menu_bar_handle = GetNewMBar(128);
//	if (menu_bar_handle == NIL) {
//		SysBeep(2); 
//		ExitToShell();
//	}
//	SetMenuBar(menu_bar_handle);
//	DisposeHandle(menu_bar_handle);
//	
//	apple_menu = GetMenuHandle(500);
//	file_menu = GetMenuHandle(550);
//	reg_menu = GetMenuHandle(600);
//	extra_menu = GetMenuHandle(650);
//	edit_menu = GetMenuHandle(700);
//	items_menu[0] = GetMenuHandle(750);
//	items_menu[1] = GetMenuHandle(751);
//	items_menu[2] = GetMenuHandle(752);
//	items_menu[3] = GetMenuHandle(753);
	
	//init_fonts();
	
//	DrawMenuBar();
	
	init_dialogs();
	
	
	while (!All_Done){
		if(cd_create_dialog_parent_num(8000,0) == -3){
			//printf("Dialog not found.\n");
			display_strings("The dialog id 8000 could not be loaded.", "", "Dialog not found.",23,4,0);
			exit(1);
		}
		//Handle_One_Event();
		cd_run_dialog();
		cd_kill_dialog(8000,0);
	}
	
	
	return 0;
}

void Initialize(void)
{
	
	OSErr		error;
	unsigned long randSeed;
	BitMap screenBits;
	FSRef gRef;
	
	//char cPath[768];
//	CFBundleRef mainBundle=CFBundleGetMainBundle();
//	CFURLRef graphicsURL = CFBundleCopyResourceURL(mainBundle,CFSTR("bladespced.rsrc"),CFSTR(""),NULL);
//	CFStringRef graphicsPath = CFURLCopyFileSystemPath(graphicsURL, kCFURLPOSIXPathStyle);
//	CFStringGetCString(graphicsPath, cPath, 512, kCFStringEncodingUTF8);//	FSPathMakeRef((UInt8*)cPath, &gRef, false);
//	error = FSOpenResourceFile(&gRef, 0, NULL, fsRdPerm, &mainRef);
//	if (error != noErr) {
//		printf("Error! PC Editor resource file not found.\n");
//		ExitToShell();
	//	}
	{
		char *path = "Scenario Editor/Blades of Exile Graphics";
		error = FSPathMakeRef((UInt8*) path, &gRef, false);
		error = FSOpenResourceFile(&gRef, 0, NULL, fsRdPerm, &graphicsRef);
		if (error != noErr) {
			//SysBeep(1);
			printf("Error! File Blades of Exile Graphics not found.\n");
			ExitToShell();
		}
	}
	{
		char *path = "Scenario Editor/Blades of Exile Sounds";
		error = FSPathMakeRef((UInt8*) path, &gRef, false);
		error = FSOpenResourceFile(&gRef, 0, NULL, fsRdPerm, &soundsRef);
		if (error != noErr) {
			//SysBeep(1);
			printf("Error! File Blades of Exile Sounds not found.\n");
			ExitToShell();
		}
	}
	{
		char *path = "Blades of Exile.app/Contents/Resources/bladesofexile.rsrc";
		error = FSPathMakeRef((UInt8*) path, &gRef, false);
		error = FSOpenResourceFile(&gRef, 0, NULL, fsRdPerm, &boeRef);
		if (error != noErr) {
			//SysBeep(1);
			printf("Error! Blades of Exile resource not found.\n");
			ExitToShell();
		}
	}
	{
		char *path = "Blades of Exile Character Editor.app/Contents/Resources/boechared.rsrc";
		error = FSPathMakeRef((UInt8*) path, &gRef, false);
		error = FSOpenResourceFile(&gRef, 0, NULL, fsRdPerm, &scenRef);
		if (error != noErr) {
			//SysBeep(1);
			printf("Error! Blades of Exile PC Editor resource not found.\n");
			ExitToShell();
		}
	}
	{
		char *path = "Scenario Editor/BoE Scenario Editor/Contents/Resources/boescen.rsrc";
		error = FSPathMakeRef((UInt8*) path, &gRef, false);
		error = FSOpenResourceFile(&gRef, 0, NULL, fsRdPerm, &pcRef);
		if (error != noErr) {
			//SysBeep(1);
			printf("Error! Blades of Exile Scenario Editor resources not found.\n");
			ExitToShell();
		}
	}
//	path = "Scenario Editor/Blades of Exile Sounds";
//	FSPathMakeRef((UInt8*) path, &sRef, false);
//	error = FSOpenResourceFile(&sRef, 0, NULL, fsRdPerm, &soundRef);
//	if (error != noErr) {
//		//SysBeep(1);
//		printf("Error! File Blades of Exile Sounds not found.\n");
//		ExitToShell();
//	}
	
	InitCursor();
	
	//
	//	Make a new window for drawing in, and it must be a color window.  
	//	The window is full screen size, made smaller to make it more visible.
	//
	GetQDGlobalsScreenBits(&screenBits);
	Rect windRect = screenBits.bounds;
	InsetRect(&windRect, 63, 34);
	windRect.top = windRect.top + 15;
	windRect.bottom = windRect.bottom + 15;
	
	mainPtr = GetNewCWindow(128,NULL,mainPtr);
	ActivateWindow(mainPtr,true);
	SetPort(GetWindowPort(mainPtr));						/* set window to current graf port */
}

void display_strings_event_filter (short item_hit)////
{
	short i;
	Boolean had1 = FALSE, had2 = FALSE;
	
	switch (item_hit) {
		case 1:
			toast_dialog();
			break;
	}
}

void display_strings(char *text1, char *text2, char *title,short graphic_num,short graphic_type,short parent_num)
{
	
	short item_hit;
	short store_which_string_dlog;
	
	set_cursor(sword_curs);
	
	//	if ((str1a <= 0) || (str1b <= 0))
	//		return;
	store_which_string_dlog = 970;
	if (strlen(title) > 0)
		store_which_string_dlog += 2;
	if ((text2 != NULL) && (text2[0] != 0))
		store_which_string_dlog++;
	cd_create_dialog_parent_num(store_which_string_dlog,parent_num);
	
	csp(store_which_string_dlog,store_which_string_dlog,graphic_num,graphic_type);
	
	csit(store_which_string_dlog,4,(char *) text1);
	if (text2 != NULL) {
		csit(store_which_string_dlog,5,(char *) text2);
	}
	if (strlen(title) > 0)
		csit(store_which_string_dlog,6,title);
	csp(store_which_string_dlog,3,graphic_num,graphic_type);
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(store_which_string_dlog,0);
}

void choose_dialog_event_filter(short item_hit){
	switch(item_hit){
		case 3:
			short n = cd_retrieve_text_edit_num(8000, 2);
			if(cd_create_dialog_parent_num(n,8000) == -3){
				char msg[100];
				sprintf(msg,"The dialog id %i could not be loaded.",n);
				display_strings(msg, "", "Dialog not found.",23,4,0);
				return;
			}
			cd_run_dialog();
			cd_kill_dialog(n,0);
			break;
		case 4:
			toast_dialog();
			All_Done = true;
			break;
	}
}

void preview_event_filter(short item_hit){
	toast_dialog();
}

void init_dialogs(){
	anim_gworld = load_pict(820);
	talkfaces_gworld = load_pict(860);
	items_gworld = load_pict(901);
	tiny_obj_gworld = load_pict(900);
	pc_gworld = load_pict(902);
	dlog_gworld = load_pict(850);
	for(int i = 0; i < 10; i++)
		monst_gworld[i] = load_pict(1100 + i);
	for(int i = 0; i < 7; i++)
		ter_gworld[i] = load_pict(800 + i);
	small_ter_gworld = load_pict(904);
	fields_gworld = load_pict(821);
	pc_stats_gworld = load_pict(1400);
	item_stats_gworld = load_pict(1401);
	mixed_gworld = load_pict(903);
	cd_init_dialogs(&anim_gworld, &talkfaces_gworld, &items_gworld, &tiny_obj_gworld, &pc_gworld, &dlog_gworld, monst_gworld, ter_gworld, &small_ter_gworld, &fields_gworld, &pc_stats_gworld, &item_stats_gworld, &mixed_gworld, NULL);
	cd_register_event_filter(8000,choose_dialog_event_filter);
	cd_register_event_filter(970,display_strings_event_filter);
	cd_register_event_filter(971,display_strings_event_filter);
	cd_register_event_filter(972,display_strings_event_filter);
	cd_register_event_filter(973,display_strings_event_filter);
	cd_register_default_event_filter(preview_event_filter);
}