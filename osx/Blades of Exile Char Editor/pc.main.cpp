
//#include <cMemory>
#include <iostream>
#include <cstring>
#include "pc.global.h"
#include "classes.h"
#include "pc.graphics.h" 
#include "pc.editors.h" 
#include "pc.action.h" 
#include "pc.fileio.h" 
#include "soundtool.h" 
#include "dlgtool.h"
#include "dlgconsts.h"
#include "graphtool.h"
#include "boe.consts.h"
#include "dlgutil.h"
#include "fileio.h"

cUniverse univ;

Rect pc_area_buttons[6][4] ; // 0 - whole 1 - pic 2 - name 3 - stat strs 4,5 - later
Rect item_string_rects[24][4]; // 0 - name 1 - drop  2 - id  3 - 
Rect pc_info_rect; // Frame that holds a pc's basic info and items
Rect name_rect; //Holds pc name inside pc_info_rect
Rect info_area_rect;
Rect hp_sp_rect;   // Holds hit points and spells points for pc
Rect skill_rect;	// Holds "Skills:" string
Rect pc_skills_rect[19]; //Holds current pc's  skill levels
Rect status_rect;  //Holds the string "Status:"
Rect pc_status_rect[10]; //Holds first 8 effects on pc
Rect traits_rect; //Holds the string "Traits:"
Rect pc_traits_rect[16]; //Holds pc traits
Rect pc_race_rect; //Holds current pc's race
Rect edit_rect[5][2]; //Buttons that bring up pc edit dialog boxs

short current_active_pc = 0;
//short dialog_answer;

/* Mac stuff globals */
Rect	windRect, Drag_Rect;
bool Multifinder_Present, All_Done = false,diff_depth_ok = false;
EventRecord	event;
WindowPtr	mainPtr;	
Handle menu_bar_handle;
MenuHandle apple_menu,file_menu,reg_menu,extra_menu,edit_menu,items_menu[4];
bool gInBackground = false,file_in_mem = false,save_blocked = false;
long start_time;
bool party_in_scen = false;
bool scen_items_loaded = false;

/* Adventure globals */
//party_record_type party;
//outdoor_record_type outdoors[2][2];
//current_town_type c_town;
//big_tr_type t_d;
//town_item_list	t_i;
//unsigned char out[96][96],out_e[96][96];
//setup_save_type setup_save;
//unsigned char misc_i[64][64],sfx[64][64];
//unsigned char template_terrain[64][64];

short store_flags[3];

//town_record_type anim_town;
//tiny_tr_type anim_t_d;

//stored_items_list_type stored_items[3];
//stored_town_maps_type maps;
//stored_town_maps_type town_maps;
//stored_outdoor_maps_type o_maps;

short old_depth = 16;
extern FSSpec file_to_load;

/* Display globals */
bool sys_7_avail;
short give_delays = 0; /* XXX this wasn't defined anywhere, is this right? -jmr */

/* Prototypes */
int main(void);
void Initialize(void);
void Set_Window_Drag_Bdry();
void Handle_One_Event();
void Handle_Activate();
bool handle_dialog_event() ;
void Handle_Update();
void Mouse_Pressed();
void handle_menu_choice(long choice);
void handle_apple_menu(int item_hit);
void handle_file_menu(int item_hit);
void handle_reg_menu(int item_hit);
void handle_extra_menu(int item_hit);
void handle_edit_menu(int item_hit);
void update_item_menu();
void find_quickdraw();
void check_sys_7();
pascal OSErr handle_open_app(AppleEvent *theAppleEvent,AppleEvent *reply,long handlerRefcon);
pascal OSErr handle_open_doc(AppleEvent *theAppleEvent,AppleEvent *reply,long handlerRefcon);
pascal OSErr handle_quit(AppleEvent *theAppleEvent,AppleEvent *reply,long handlerRefcon);
bool verify_restore_quit(short mode);
void set_up_apple_events();
void set_pixel_depth();
void restore_depth();
void handle_item_menu(int item_hit);
//item_record_type convert_item (short_item_record_type s_item);
bool cur_scen_is_mac, mac_is_intel;
std::string progDir;
// File io
short specials_res_id;
Str255 start_name;
ResFileRefNum graphicsRef, soundRef, mainRef;

//#include "pc.itemdata.h" 
cItemRec item_list[400];
cScenario scenario;

// 
//	Main body of program Exile
//

//MW specified return type was 'void', changed to ISO C style for Carbonisation -jmr
int main(void)
{
	start_time = TickCount();
	
	Initialize();
	init_fileio();
	init_main_buttons();
	Set_up_win();
	init_graph_tool(redraw_screen);
	init_snd_tool();
	find_quickdraw();
	set_pixel_depth();
	Set_Window_Drag_Bdry();
	
//	init_buf();

//	register_flag = get_reg_data();
	set_up_apple_events();

	menu_bar_handle = GetNewMBar(128);
	if (menu_bar_handle == NULL) {
		SysBeep(2); 
		ExitToShell();
	}
	SetMenuBar(menu_bar_handle);
	DisposeHandle(menu_bar_handle);

	apple_menu = GetMenuHandle(500);
	file_menu = GetMenuHandle(550);
	reg_menu = GetMenuHandle(600);
	extra_menu = GetMenuHandle(650);
	edit_menu = GetMenuHandle(700);
	items_menu[0] = GetMenuHandle(750);
	items_menu[1] = GetMenuHandle(751);
	items_menu[2] = GetMenuHandle(752);
	items_menu[3] = GetMenuHandle(753);

	//init_fonts();

	DrawMenuBar();
		
	init_dialogs();
	
	/* Multifinder_Present = (NGetTrapAddress(_WaitNextEvent, ToolTrap) != 
		NGetTrapAddress(_Unimplemented, ToolTrap)); */
      /* no need to check this with Carbon -jmr */
	Multifinder_Present = true;
	
	while (All_Done == false) 
		Handle_One_Event();
	restore_depth();
	clean_up_graphtool();
      return 0;
}



// 
//	Initialize everything for the program, make sure we can run
//

//MW specified argument and return type.
void check_for_intel();
void Initialize(void)
{

	OSErr		error;
	//SysEnvRec	theWorld;
    //  unsigned long randSeed;
      BitMap screenBits;
	
	check_for_intel();
	//Open the resource files we'll be needing
	char cPath[768];
	CFBundleRef mainBundle=CFBundleGetMainBundle();
	CFURLRef graphicsURL = CFBundleCopyResourceURL(mainBundle,CFSTR("bladespced.rsrc"),CFSTR(""),NULL);
	CFStringRef graphicsPath = CFURLCopyFileSystemPath(graphicsURL, kCFURLPOSIXPathStyle);
	CFStringGetCString(graphicsPath, cPath, 512, kCFStringEncodingUTF8);
	FSRef gRef, sRef;
	FSPathMakeRef((UInt8*)cPath, &gRef, false);
	error = FSOpenResourceFile(&gRef, 0, NULL, fsRdPerm, &mainRef);
	if (error != noErr) {
		printf("Error! Main resource file not found.\n");
		ExitToShell();
	}
	char *path = "Scenario Editor/Blades of Exile Graphics";
	error = FSPathMakeRef((UInt8*) path, &gRef, false);
	error = FSOpenResourceFile(&gRef, 0, NULL, fsRdPerm, &graphicsRef);
	if (error != noErr) {
		//SysBeep(1);
		printf("Error! File Blades of Exile Graphics not found.\n");
		ExitToShell();
	}
	path = "Scenario Editor/Blades of Exile Sounds";
	FSPathMakeRef((UInt8*) path, &sRef, false);
	error = FSOpenResourceFile(&sRef, 0, NULL, fsRdPerm, &soundRef);
	if (error != noErr) {
		//SysBeep(1);
		printf("Error! File Blades of Exile Sounds not found.\n");
		ExitToShell();
	}
	
	CFStringRef progURL = CFURLCopyFileSystemPath(CFBundleCopyBundleURL(mainBundle), kCFURLPOSIXPathStyle);
	const char* tmp = CFStringGetCStringPtr(progURL, kCFStringEncodingASCII);//kCFStringEncodingUTF8);
	if(tmp == NULL){
		bool success = CFStringGetCString(progURL, cPath, sizeof(cPath), kCFStringEncodingUTF8);
		if(success) {
			progDir = cPath;
			std::cout << cPath << "\n\n" << progDir << "\n\n";
		} else {
			std::cout << "Couldn't retrieve application path.\n";
			exit(1);
		}
	}else progDir = tmp;
	//progDir = cPath;
	size_t last_slash = progDir.find_last_of('/');
	progDir.erase(last_slash);
	std::cout<<progDir<<'\n';
	//
	//	Test the computer to be sure we can do color.  
	//	If not we would crash, which would be bad.  
	//	If we can’t run, just beep and exit.
	//

      // not needed in Carbon
	//error = SysEnvirons(1, &theWorld);
	//if (theWorld.hasColorQD == false) {
	//	SysBeep(50);
	//	ExitToShell();					/* If no color QD, we must leave. */
	//}
	
	/* Initialize all the needed managers. */
	/*InitGraf(&qd.thePort);*/ /* not needed in Carbon -jmr */
	//InitFonts();
	//InitWindows();
	//InitMenus();
	//TEInit();
	//InitDialogs(nil);
	InitCursor();

	//
	//	To make the Random sequences truly random, we need to make the seed start
	//	at a different number.  An easy way to do this is to put the current time
	//	and date into the seed.  Since it is always incrementing the starting seed
	//	will always be different.  Don’t for each call of Random, or the sequence
	//	will no longer be random.  Only needed once, here in the init.
	//
	//GetDateTime(&randSeed);
    //  SetQDGlobalsRandomSeed((long)randSeed);
	srand(time(NULL));

	//
	//	Make a new window for drawing in, and it must be a color window.  
	//	The window is full screen size, made smaller to make it more visible.
	//
      GetQDGlobalsScreenBits(&screenBits);
	windRect = screenBits.bounds;
	InsetRect(&windRect, 63, 34);
	windRect.top = windRect.top + 15;
	windRect.bottom = windRect.bottom + 15;
		
//	mainPtr = NewCWindow(nil, &windRect, "\pExile", true, zoomDocProc, 
//						(WindowPtr) -1, false, 0);
	//error = CreateWindowFromResource(128, &mainPtr);
	//Rect winBounds={38,53,478,643};
	//error = CreateNewWindow(kDocumentWindowClass, kWindowCloseBoxAttribute, &windRect, &mainPtr);
	mainPtr = GetNewCWindow(128,NULL,IN_FRONT);
	ActivateWindow(mainPtr,true);
//	DrawGrowIcon(mainPtr);	
	SetPort(GetWindowPort(mainPtr));						/* set window to current graf port */
	/*stored_key = open_pref_file();
	if (stored_key == -100) {
		stored_key = open_pref_file();
		if (stored_key == -100) {
			Alert(983,NULL);
			ExitToShell();	
			}
		}*/	
}

void Set_Window_Drag_Bdry()
{
      BitMap screenBits;
      GetQDGlobalsScreenBits(&screenBits);
	Drag_Rect = screenBits.bounds;
//	Drag_Rect = (**(GrayRgn)).rgnBBox;
	Drag_Rect.left += DRAG_EDGE;
	Drag_Rect.right -= DRAG_EDGE;
	Drag_Rect.bottom -= DRAG_EDGE;
}

void Handle_One_Event()
{
	short chr,chr2;
	long menu_choice,cur_time;
	GrafPtr old_port;
	
	//ed_reg = false;
	
	if (Multifinder_Present == true) {
		WaitNextEvent(everyEvent, &event, 0, NULL);
		cur_time = TickCount();

		GetPort(&old_port);		
	}
	else{
                  //not reached with Carbon
//			through_sending();
			//SystemTask();
			//GetNextEvent( everyEvent, &event);
	}
	
	init_main_buttons();

	switch (event.what){
		case keyDown: case autoKey:
			chr = event.message & charCodeMask;
			chr2 = (char) ((event.message & keyCodeMask) >> 8);
			if ((event.modifiers & cmdKey) != 0) {
				if (event.what != autoKey) {
					BringToFront(mainPtr);
					SetPort(GetWindowPort(mainPtr));
					menu_choice = MenuKey(chr);
					//kludge to prevent stupid system from messing up the shortcut for 'Save As'
					if(HiWord(menu_choice)==550 && LoWord(menu_choice)==1 && (event.modifiers & shiftKey)!=0)
						menu_choice = (550<<16) + 2;
					handle_menu_choice(menu_choice);
				}
			}		
			break;
		
		case mouseDown:
			Mouse_Pressed();
			break;
		
		case activateEvt:
			Handle_Activate();
			break;
		
		case updateEvt:
			set_pixel_depth();
			Handle_Update();
			break;

		case kHighLevelEvent: 
			AEProcessAppleEvent(&event);
			break;

		case osEvt:
		/*	1.02 - must BitAND with 0x0FF to get only low byte */
			switch ((event.message >> 24) & 0x0FF) {		/* high byte of message */
				case suspendResumeMessage:		/* suspend/resume is also an activate/deactivate */
					gInBackground = (event.message & 1) == 0;  // 1 is Resume Mask
//					DoActivate(FrontWindow(), !gInBackground);
//					current_cursor = 300;
					break;
			}
			break;
		}
}


void Handle_Activate()
{
	GrafPtr old_port;
	
	GetPort(&old_port);				
	if (FrontWindow() == mainPtr)
		SetPort(GetWindowPort(mainPtr));
	set_cursor(sword_curs);	
}


void Handle_Update()
{
	WindowPtr the_window;
	GrafPtr		old_port;
	
	the_window = (WindowPtr) event.message;
	
	GetPort (&old_port);
	SetPort (GetWindowPort(the_window));
	
	BeginUpdate(the_window);
		
	redraw_screen();
		
	EndUpdate(the_window);
	
	SetPort(old_port);
}


void Mouse_Pressed()
{
	WindowPtr	the_window;
      BitMap screenBits;
	short	the_part;
	long menu_choice;
	bool try_to_end;
		
	the_part = FindWindow( event.where, &the_window);
		
	switch (the_part)
	{
	
		case inMenuBar:
			menu_choice = MenuSelect(event.where);
			handle_menu_choice(menu_choice);
			break;
		
		case inSysWindow:
			break;
		
		case inDrag:
                  GetQDGlobalsScreenBits(&screenBits);
			DragWindow(the_window, event.where, &(screenBits.bounds));
			break;
		
		case inGoAway:
			if (the_window == mainPtr) {

				All_Done = verify_restore_quit(0);
			}
			else {
			/*	for (i = 0; i < 12; i++)
					if ((the_window == modeless_dialogs[i]) && (modeless_exists[i] == true)) {
						CloseDialog(modeless_dialogs[i]);
						modeless_exists[i] = false;
						SelectWindow(mainPtr);
						SetPort(mainPtr);		
						}*/
			}
			break;
		
		case inContent:
			if ((the_window != NULL) && (FrontWindow() != the_window)) {
				SetPort(GetWindowPort(the_window));
				SelectWindow(the_window);
				SetPort(GetWindowPort(the_window));
			}
			else{ 
				if (the_window == mainPtr) {
					try_to_end = handle_action(event,0);
					if (try_to_end == true)
						All_Done = verify_restore_quit(0);
				}
			}
			break;
	}
}


void handle_menu_choice(long choice)
{
	int menu,menu_item;

	if (choice != 0) {
		menu = HiWord(choice);
		menu_item = LoWord(choice);

		switch (menu) {
			case 500:
				handle_apple_menu(menu_item);
				break;
			case 550:
				handle_file_menu(menu_item);
				break;
			case 650:
				handle_extra_menu(menu_item);
				break;			
			case 700:
				handle_edit_menu(menu_item);
				break;			
			case 750: case 751:case 752:case 753:
				handle_item_menu(menu_item + 100 * (menu - 750) - 1);
				break;			
			}
		}
 
	HiliteMenu(0);
}

void handle_apple_menu(int item_hit)
{
	//Str255 desk_acc_name;
	//short desk_acc_num;
	
	switch (item_hit) {
		case 1:
			FCD(1062,0);
			break;
		default:
			//GetItem (apple_menu,item_hit,desk_acc_name);
			//desk_acc_num = OpenDeskAcc(desk_acc_name);
			break;
		}
}

void handle_file_menu(int item_hit)
{
	FSSpec file;
	
	switch (item_hit) {
		case 1://save
			save_party(file_to_load);
			break;
		case 2://save as
			try{
				file = nav_put_party();
				save_party(file);
			} catch(no_file_chosen){}
			break;
		case 3://open
			if (verify_restore_quit(1) == true){
				try{
					file_to_load = nav_get_party();
					load_party(file_to_load);
				} catch(no_file_chosen){}
			}
			break;
		case 5://how to order
			give_reg_info();
			break;
		case 7://quit
			All_Done = verify_restore_quit(0);
			break;
		}
}

void check_for_intel(){
	SInt32 response;
	OSErr err;
	err = Gestalt(gestaltSysArchitecture,&response);
	if(err != noErr){
		printf("Gestalt error %i\n",err);
		exit(1);
	}
	if(response == gestaltIntel) mac_is_intel = true;
	else mac_is_intel = false;
}

void handle_extra_menu(int item_hit)
{
	short i;
	//cVehicle v_boat = {{12,17},{0,0},{0,0},80,true,false};
	
	if (file_in_mem == false) {
		display_strings(20,5,0,0,"Editing party",57,7,PICT_DLG,0);
		return;
	}
	switch(item_hit) {
		case 1:
				edit_gold_or_food(0);
		break;
		case 2:
				edit_gold_or_food(1);
		break;
		
		case 4:
			if (PSD[SDF_IS_PARTY_SPLIT] > 0) {
				FCD(909,0);
				break;
				}
			FCD(901,0);
			leave_town();
			break;
	
		case 5:
			if (PSD[SDF_IS_PARTY_SPLIT] == 0) {
				FCD(911,0);
				break;
				}
			FCD(910,0);
			univ.town.p_loc.x = PSD[SDF_PARTY_SPLIT_X];
			univ.town.p_loc.y = PSD[SDF_PARTY_SPLIT_Y];
			PSD[SDF_IS_PARTY_SPLIT] = 0;
			for (i = 0; i < 6; i++)
				if (univ.party[i].main_status >= MAIN_STATUS_SPLIT)
					univ.party[i].main_status -= MAIN_STATUS_SPLIT;
			break;
			

		case 6:
			display_strings(20,20,0,0,"Editing party",57,7,PICT_DLG,0);
			for (i = 0; i < 4; i++)
				univ.party.creature_save[i].which_town = 200;
			break;
		case 8: // damage
			display_strings(20,1,0,0,"Editing party",57,15,PICT_DLG,0);
			for (i = 0; i < 6; i++)
				univ.party[i].cur_health = univ.party[i].max_health;
			break;
		case 9: // spell pts
			display_strings(20,2,0,0,"Editing party",57,15,PICT_DLG,0);
			for (i = 0; i < 6; i++)
				univ.party[i].cur_sp = univ.party[i].max_sp;
			break;
		case 10: // raise dead
			display_strings(20,3,0,0,"Editing party",57,15,PICT_DLG,0);
			for (i = 0; i < 6; i++)
				if ((univ.party[i].main_status == MAIN_STATUS_DEAD) || (univ.party[i].main_status == MAIN_STATUS_DUST) ||
					(univ.party[i].main_status == MAIN_STATUS_STONE))
						univ.party[i].main_status = MAIN_STATUS_ALIVE;
			break;
		case 11: // conditions
			display_strings(20,4,0,0,"Editing party",57,15,PICT_DLG,0);
			for (i = 0; i < 6; i++) {
				univ.party[i].status[2] = 0;
				if (univ.party[i].status[3] < 0)
					univ.party[i].status[3] = 0;
				univ.party[i].status[6] = 0;
				univ.party[i].status[7] = 0;
				univ.party[i].status[9] = 0;
				univ.party[i].status[11] = 0;
				univ.party[i].status[12] = 0;
				univ.party[i].status[13] = 0;
			}
			break;
			
		case 13:
			if (party_in_scen == false) {
				display_strings(20,25,0,0,"Editing party",57,15,PICT_DLG,0);
				break;
			}
			if (FCD(912,0) != 1)
				break;
			remove_party_from_scen();
			PSD[SDF_IS_PARTY_SPLIT] = 0;
			break;
	}
	redraw_screen();
}

void handle_edit_menu(int item_hit)
{
	short choice,i,j,k;

	if (file_in_mem == false) {
		display_strings(20,5,0,0,"Editing party",57,7,PICT_DLG,0);
		return;
		}
	if (save_blocked == false)
		if ((choice = FCD(904,0)) == 1)
			return;
			else save_blocked = true;
	switch(item_hit) {
		case 1:
			 display_alchemy();
			break;
		case 2: // all property
			display_strings(20,6,0,0,"Editing party",57,7,PICT_DLG,0);
			for (i = 0; i < 30; i++) {
				univ.party.boats[i].property = false;
				univ.party.horses[i].property = false;
				}
			break;
		case 4: // edit day
			edit_day();
			break;
		case 6: // ouit maps
			if (party_in_scen == false) {
				display_strings(20,25,0,0,"Editing party",57,15,PICT_DLG,0);
				break;
				}
			display_strings(20,13,0,0,"Editing party",57,15,PICT_DLG,0);
			for (i = 0; i < 100; i++)
				for (j = 0; j < 6; j++)
					for (k = 0; k < 48; k++)
						univ.out_maps[i][j][k] = 255;
			break;
		case 7: // town maps
			if (party_in_scen == false) {
				display_strings(20,25,0,0,"Editing party",57,15,PICT_DLG,0);
				break;
				}
			display_strings(20,14,0,0,"Editing party",57,15,PICT_DLG,0);
			for (i = 0; i < 200; i++)
				for (j = 0; j < 8; j++)
					for (k = 0; k < 64; k++)
						univ.town_maps[i][j][k] = 255;
			break;
				case 9:
					display_pc(current_active_pc,0,0);
					break;
				case 10:
			 		display_pc(current_active_pc,1,0);
					break;
				case 11: 
					pick_race_abil(&univ.party[current_active_pc],0,0);
					break;
				case 12: 
					spend_xp(current_active_pc,1,0);
					break;
				case 13: 
					edit_xp(&univ.party[current_active_pc]);
					break;
		}
}

//item_record_type convert_item (short_item_record_type s_item) {
//	item_record_type i;
//	location l = {0,0};
//	short temp_val;
//	
//	i.variety = (short) s_item.variety;
//	i.item_level = (short) s_item.item_level;
//	i.awkward = (short) s_item.awkward;
//	i.bonus = (short) s_item.bonus;
//	i.protection = (short) s_item.protection;
//	i.charges = (short) s_item.charges;
//	i.type = (short) s_item.type;
//	i.graphic_num = (short) s_item.graphic_num;
//	if (i.graphic_num >= 25)
//		i.graphic_num += 20;
//	i.ability = (short) s_item.real_abil;
//	i.type_flag = (short) s_item.type_flag;
//	i.is_special = (short) s_item.is_special;
//	i.value = (short) s_item.value;
//	i.weight = s_item.weight;
//	i.special_class = 0;
//	i.item_loc = l;
//	strcpy((char *)i.full_name,(char *)s_item.full_name);
//	strcpy((char *)i.name,(char *)s_item.name);
//
//	if (i.charges > 0)
//		temp_val = i.value * i.charges;
//		else temp_val = i.value;
//	if (temp_val >= 15)
//		i.treas_class = 1;
//	if (temp_val >= 100)
//		i.treas_class = 2;
//	if (temp_val >= 900)
//		i.treas_class = 3;
//	if (temp_val >= 2400)
//		i.treas_class = 4;
//		
//	i.magic_use_type = s_item.magic_use_type;
//	i.ability_strength = s_item.ability_strength;
//	i.reserved1 = 0;
//	i.reserved2 = 0;
//	i.item_properties = 0;
//	if (s_item.identified == true)
//		i.item_properties = i.item_properties | 1;
//	if ((s_item.ability == 14) || (s_item.ability == 129) || (s_item.ability == 95))
//		i.item_properties = i.item_properties | 16;
//	if (s_item.magic == true)
//		i.item_properties = i.item_properties | 4;
//
//	return i;
//}

void handle_item_menu(int item_hit)
{
	short choice;
	cItemRec store_i;
	
	if (file_in_mem == false) {
		display_strings(20,5,0,0,"Editing party",57,7,PICT_DLG,0);
		return;
		}
	if (save_blocked == false)
		if ((choice = FCD(904,0)) == 1)
			return;
			else save_blocked = true;
	store_i = item_list[item_hit];
	store_i.item_properties = store_i.item_properties | 1;
	give_to_pc(current_active_pc,store_i,false);
	draw_items(1);
}

void update_item_menu()
{
	short i,j;
	MenuHandle item_menu[4];
	Str255 item_name;
	
	for (i = 0; i < 4; i++)
		item_menu[i] = GetMenuHandle(750 + i);
	for (j = 0; j < 4; j++){
		DeleteMenuItems(item_menu[j],1,100);
		for (i = 0; i < 100; i++) {
			sprintf((char *) item_name, " %s",item_list[i + j * 100].full_name.c_str());
			c2pstr((char *) item_name);
			AppendMenu(item_menu[j],item_name);
		} 
	}
}

//void set_cursor(CursHandle which_curs)
//{
//	HLock ((Handle) which_curs);
//	SetCursor (*which_curs);
//	HUnlock((Handle) which_curs);
//}

void find_quickdraw() {
	OSErr err;
	SInt32 response;
	short choice;
	err = Gestalt(gestaltQuickdrawVersion, &response);
	if (err == noErr) {
		if (response == 0x0000) {
			choice = choice_dialog(0,1070);
			if (choice == 2)
				ExitToShell();
			else
				diff_depth_ok = true;
		}
	}
	else  {
		SysBeep(2);
		ExitToShell();
	}
}

void set_pixel_depth() {
	GDHandle cur_device;
	PixMapHandle screen_pixmap_handle;
	OSErr err;
	short choice;
	static bool diff_depth_ok = false;
	short pixel_depth;
	
	cur_device = GetGDevice();	
	
	screen_pixmap_handle = (**(cur_device)).gdPMap;
	pixel_depth = (**(screen_pixmap_handle)).pixelSize;
	
	if ((diff_depth_ok == false) && ((pixel_depth <= 16) && (HasDepth(cur_device,16,1,1)) == 0)) {
		choice = choice_dialog(0,1070);
		if (choice == 1)
			ExitToShell();
		if (choice == 2)
			diff_depth_ok = true;
	}
	
	if ((pixel_depth != 16) && (diff_depth_ok == true))
		return;
	
	if (pixel_depth < 16) {
		choice = choice_dialog(0,1071);
		if (choice == 3)
			diff_depth_ok = true;
		if (choice == 2)
			ExitToShell();
		if (choice == 1) {
			err = SetDepth(cur_device,16,1,1);
			old_depth = pixel_depth;
		}
	}
}

void restore_depth(){
	GDHandle cur_device;
	PixMapHandle screen_pixmap_handle;
	OSErr err;
	cur_device = GetGDevice();	
	screen_pixmap_handle = (**(cur_device)).gdPMap;
	if(old_depth!=16)
		err=SetDepth(cur_device,old_depth,1,1);
}

void check_sys_7()
{
	OSErr err;
	long response;
	err = Gestalt(gestaltSystemVersion, &response);
	if ((err == noErr) && (response >= 0x0700))
		sys_7_avail = true;
	else 
		sys_7_avail = false;
}

pascal OSErr handle_open_app(AppleEvent *theAppleEvent,AppleEvent *reply,long handlerRefcon)
{
	return noErr;
}

pascal OSErr handle_open_doc(AppleEvent *theAppleEvent,AppleEvent *reply,long handlerRefcon)
{
//	FSSpec myFSS;
//	AEDescList docList;
//	OSErr myErr, ignoreErr;
//	long index, itemsInList;
//	Size actualSize;
//	AEKeyword keywd;
//	DescType returnedType;

/*	myErr = AEGetParamDesc(theAppleEvent,keyDirectObject, typeAEList, &docList);
	if (myErr == noErr) {
		myErr == AECountItems(&docList,&itemsInList);
		if (myErr == noErr) {	
			myErr = AEGetNthPtr(&docList,1,typeFSS, 
						&keywd,&returnedType,&myFSS,
						sizeof(myFSS),&actualSize);
			if (myErr == noErr) {
				do_apple_event_open(myFSS);
				if ((in_startup_mode == false) && (startup_loaded == true)) 
					end_startup();
				if (in_startup_mode == false) {
					post_load();
					}
				}
			}
		}*/
	
	
	return noErr;
}

pascal OSErr handle_quit(AppleEvent *theAppleEvent,AppleEvent *reply,long handlerRefcon)
{
			
	All_Done = verify_restore_quit(0);
	return noErr;
}

bool verify_restore_quit(short mode)
//short mode; // 0 - quit  1- restore
{
	short choice;

	if (file_in_mem == false)
		return true;
	choice = FCD(1066 + mode,0);
	if (choice == 3)
		return false;
	if (choice == 2)
		return true;
	save_party(file_to_load);
	return true;
}

void set_up_apple_events()
{
	OSErr myErr;

	myErr = AEInstallEventHandler(kCoreEventClass,kAEOpenApplication,
		(AEEventHandlerProcPtr) handle_open_app, 0, false);
			
	if (myErr != noErr)
		SysBeep(2);

	myErr = AEInstallEventHandler(kCoreEventClass,kAEOpenDocuments,
		(AEEventHandlerProcPtr) handle_open_doc, 0, false);
			
	if (myErr != noErr)
		SysBeep(2);

	myErr = AEInstallEventHandler(kCoreEventClass,kAEQuitApplication,
		(AEEventHandlerProcPtr) handle_quit, 0, false);
			
	if (myErr != noErr)
		SysBeep(2);
}
 
//pascal bool cd_event_filter (DialogPtr hDlg, EventRecord *event, short *dummy_item_hit)
//{	
//	char chr,chr2;
//	short the_type,wind_hit,item_hit;
//	Handle the_handle = NULL;
//	Rect the_rect,button_rect;
//	Point the_point;
//	CWindowPtr w;
//	RgnHandle updateRgn;
//	
//	dummy_item_hit = 0;
//	
//	switch (event->what) {
//		case updateEvt:
//			w = GetDialogWindow(hDlg);
//			updateRgn = NewRgn();
//			GetWindowRegion(w, kWindowUpdateRgn, updateRgn);
//			if (EmptyRgn(updateRgn) == true) {
//				return true;
//			}
//			BeginUpdate(GetDialogWindow(hDlg));
//			cd_redraw(hDlg);
//			EndUpdate(GetDialogWindow(hDlg));
//			DrawDialog(hDlg);
//			return true;
//			break;
//		
//		case keyDown:
//			chr = event->message & charCodeMask;
//			chr2 = (char) ((event->message & keyCodeMask) >> 8);
//			switch (chr2) {
//				case 126: chr = 22; break;
//				case 124: chr = 21; break;
//				case 123: chr = 20; break;
//				case 125: chr = 23; break;
//				case 53: chr = 24; break;
//				case 36: chr = 31; break;
//				case 76: chr = 31; break;
//			}
//			// specials ... 20 - <-  21 - ->  22 up  23 down  24 esc
//			// 25-30  ctrl 1-6  31 - return
//
//			wind_hit = cd_process_keystroke(hDlg,chr,&item_hit);
//			break;
//	
//		case mouseDown:
//			the_point = event->where;
//			GlobalToLocal(&the_point);	
//			wind_hit = cd_process_click(hDlg,the_point, event->modifiers,&item_hit);
//			break;
//
//		default: wind_hit = -1; break;
//	}
//	switch (wind_hit) {
//		case -1: break;
//
//		case 917: edit_day_event_filter(item_hit); break;
//		case 970: case 971: case 972: case 973: display_strings_event_filter(item_hit); break;
//		case 991: display_pc_event_filter(item_hit); break;
//		case 996: display_alchemy_event_filter(item_hit); break;
//		case 1010: spend_xp_event_filter (item_hit); break;
//		case 1012: case 947: edit_gold_or_food_event_filter (item_hit); break;
//		case 1013: pick_race_abil_event_filter (item_hit); break;
//		case 1018: select_pc_event_filter (item_hit); break;
//		case 1024: edit_xp_event_filter (item_hit); break;
//		case 1073: give_reg_info_event_filter (item_hit); break;
//		default: fancy_choice_dialog_event_filter (item_hit); break;
//	}
//	return(wind_hit != -1);
//}