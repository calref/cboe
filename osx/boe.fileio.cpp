
#include <string.h>
#include "boe.global.h"
#include <stdio.h>
#include "boe.fileio.h"
#include "boe.text.h"
#include "boe.town.h"
#include "boe.items.h"
#include "boe.graphics.h"
#include "boe.locutils.h"
#include "boe.fields.h"
#include "boe.newgraph.h"
#include "boe.dlgutil.h"
#include "boe.infodlg.h"
#include "boe.graphutil.h"
#include "graphtool.h"
#include "soundtool.h"
#include "mathutil.h"

#include <vector>
#include <string>
using std::vector;
using std::string;

#define	DONE_BUTTON_ITEM	1
#define IN_FRONT	(WindowPtr)-1L
#define	NIL		0L

DialogPtr	the_dialog;
extern party_record_type	party;
extern pc_record_type adven[6];
extern outdoor_record_type outdoors[2][2];
extern unsigned char out[96][96],out_e[96][96];
extern short overall_mode,give_delays,stat_screen_mode;
extern Boolean in_startup_mode,registered,play_sounds,sys_7_avail,save_maps,party_in_memory,in_scen_debug;
extern current_town_type	c_town;
extern town_item_list	t_i;
extern location center;
extern long register_flag;
extern WindowPtr mainPtr;
extern stored_items_list_type stored_items[3];
extern stored_town_maps_type maps;
extern stored_outdoor_maps_type o_maps;
extern big_tr_type t_d;
extern short town_size[3];
extern short town_type,current_pc;
extern Boolean web,crate,barrel,fire_barrier,force_barrier,quickfire,force_wall,fire_wall,antimagic,scloud,ice_wall,blade_wall;
extern Boolean sleep_field;
extern setup_save_type setup_save;
extern unsigned char misc_i[64][64],sfx[64][64];
extern unsigned char template_terrain[64][64];
extern tiny_tr_type anim_t_d;
extern Boolean modeless_exists[18];
extern location monster_targs[T_M];
extern DialogPtr modeless_dialogs[18] ;
extern short which_combat_type;
extern char terrain_blocked[256];
extern short terrain_pic[256],cur_town_talk_loaded;
extern scenario_data_type scenario;
extern piles_of_stuff_dumping_type *data_store;
extern talking_record_type talking;
extern outdoor_strs_type outdoor_text[2][2];
extern vector<scen_header_type> scen_headers;
extern unsigned char combat_terrain[64][64];
extern Boolean belt_present;

typedef struct {
	char expl[96][96];
	}	out_info_type;

Boolean loaded_yet = FALSE, got_nagged = FALSE,ae_loading = FALSE;
Str63 last_load_file = "\pBlades of Exile Save";
FSSpec file_to_load;
FSSpec store_file_reply;
short jl;

Boolean cur_scen_is_mac = TRUE;

void print_write_position ();
void save_outdoor_maps();
void add_outdoor_maps();

short specials_res_id,data_dump_file_id;
Str255 start_name;
short start_volume,data_volume;
long start_dir,data_dir/*,scen_dir*/;
string progPath;

outdoor_record_type dummy_out;////
town_record_type dummy_town;

// Trying this to fix bug. Hope it works.
	tiny_tr_type tiny_t;
	ave_tr_type ave_t;

template_town_type town_template;
	CInfoPBRec myCPB;
GWorldPtr spec_scen_g = NULL;
ResFileRefNum mainRef, graphicsRef, soundRef;

void init_directories()
{
	short error;
	Str255 folder_name = "\p::::Blades of Exile Scenarios";
	
//	HGetVol((StringPtr) start_name,&start_volume,&start_dir);	
//	HGetVol((StringPtr) data_name,&data_volume,&data_dir);
//	HOpenResFile(start_volume,start_dir,"\p::::bladesofexile.rsrc",1);
//	error = HOpenResFile(start_volume,start_dir,"\p::::Scenario Editor:Blades of Exile Sounds",1);
//	if (ResError() != 0) {
//		Alert(984,NIL);
//		ExitToShell();
//		}
//	error = HOpenResFile(start_volume,start_dir,"\p::::Scenario Editor:Blades of Exile Graphics",1);
//	if (ResError() != 0) {
//		Alert(984,NIL);
//		ExitToShell();
//	}
	char cPath[768];
	CFBundleRef mainBundle=CFBundleGetMainBundle();
	CFURLRef graphicsURL = CFBundleCopyResourceURL(mainBundle,CFSTR("bladesofexile.rsrc"),CFSTR(""),NULL);
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

	// now I generate the directory ID of the folder which contains the scenarios
	// code copied from Mac Prog FAQ book
//	myCPB.dirInfo.ioNamePtr = folder_name;
//	myCPB.dirInfo.ioVRefNum = start_volume;
//	myCPB.dirInfo.ioFDirIndex = 0;
//	myCPB.dirInfo.ioDrDirID = start_dir;
//	error = PBGetCatalogInfoSync(&myCPB); // false means not async
//	
//	scen_dir = myCPB.dirInfo.ioDrDirID;
	
	CFStringRef progURL = CFURLCopyFileSystemPath(CFBundleCopyBundleURL(mainBundle), kCFURLPOSIXPathStyle);
	CFStringGetCString(progURL, cPath, 768, kCFStringEncodingUTF8);
	progPath = cPath;
}


void do_apple_event_open(FSSpec file_info)
{
	ae_loading = TRUE;
	file_to_load = file_info;
	load_file();
	ae_loading = FALSE;
}



void load_file()
{
	
	NavReplyRecord reply;
	long file_size;
	OSErr error;
	NavTypeList type_list; // To be able to have more than one type, this wouldn't do.
	short file_id,i,j,k;
	Boolean town_restore = FALSE;
	Boolean maps_there = FALSE;
	Boolean in_scen = FALSE;
	
	char flag_data[8];
	
	long len,store_len,count;
	char *party_ptr;
	char *pc_ptr;
	flag_type flag;
	flag_type *flag_ptr;
	
	
	short flags[3][2] = {{5790,1342}, // slot 0 ... 5790 - out  1342 - town
		{100,200}, // slot 1 100  in scenario, 200 not in
		{3422,5567}}; // slot 2 ... 3422 - no maps  5567 - maps
	
	
	type_list.componentSignature = kNavGenericSignature;
	type_list.osTypeCount = 1;
	type_list.osType[0] = 'beSV';
	
	NavTypeListPtr type_list_p = &type_list;
	
	NavChooseFile(NULL,&reply,NULL,NULL,NULL,NULL,&type_list_p,NULL);
	
	if (!reply.validRecord)
		return;
	
	AEKeyword keyword;
	DescType descType;
	Size actualSize;
	
	AEGetNthPtr(&reply.selection,1,typeFSS,&keyword,&descType,&file_to_load,sizeof(FSSpec),&actualSize);
	
	
	if ((error = FSpOpenDF(&file_to_load,1,&file_id)) != 0) {
		FCD(1064,0);
		SysBeep(2);
		return;
	}		
	
	
	file_size = sizeof(party_record_type);
	
	len = sizeof(flag_type);
	
	//	sprintf((char *) debug, "  Len %d               ", (short) len);
	//	add_string_to_buf((char *) debug);
	
	for (i = 0; i < 3; i++) {
		if ((error = FSRead(file_id, &len, (char *) flag_data)) != 0) {
			FSClose(file_id);
			FCD(1064,0);
			return;
		}
		flag_ptr = (flag_type *) flag_data;
		flag = *flag_ptr;
		if ((flag.i != flags[i][0]) && (flag.i != flags[i][1])) { // OK Exile II save file?
			FSClose(file_id);
			FCD(1063,0);
			return;
		} 
		
		if ((i == 0) && (flag.i == flags[i][1]))
			town_restore = TRUE;
		if ((i == 1) && (flag.i == flags[i][0])) {
			in_scen = TRUE;
		}
		if ((i == 2) && (flag.i == flags[i][1]))
			maps_there = TRUE;
	}
	
	// LOAD PARTY	
	len = (long) sizeof(party_record_type);
	store_len = len;
	party_ptr = (char *) &party;
	if ((error = FSRead(file_id, &len, (char *) party_ptr)) != 0){
		FSClose(file_id);
		SysBeep(2);
		FCD(1064,0);
		return;
	}
	for (count = 0; count < store_len; count++)
		party_ptr[count] ^= 0x5C;	
	
	// LOAD SETUP
	len = (long) sizeof(setup_save_type);
	if ((error = FSRead(file_id, &len, (char *) &setup_save)) != 0){
		FSClose(file_id);
		SysBeep(2);
		FCD(1064,0);
		return;
	}
	
	// LOAD PCS
	store_len = (long) sizeof(pc_record_type);
	for (i = 0; i < 6; i++) {
		len = store_len;
		pc_ptr = (char *) &adven[i];
		if ((error = FSRead(file_id, &len, (char *) pc_ptr))  != 0){
			FSClose(file_id);
			SysBeep(2);
			FCD(1064,0);
			return;
		}
		for (count = 0; count < store_len; count++)
			pc_ptr[count] ^= 0x6B;	
	}
	
	if (in_scen == TRUE) {
		
		// LOAD OUTDOOR MAP
		len = (long) sizeof(out_info_type);
		if ((error = FSRead(file_id, &len, (char *) out_e)) != 0){
			FSClose(file_id);
			SysBeep(2);
			FCD(1064,0);
			return;
		}
		
		// LOAD TOWN 
		if (town_restore == TRUE) {
			len = (long) sizeof(current_town_type);
			if ((error = FSRead(file_id, &len, (char *) &c_town)) != 0){
				FSClose(file_id);
				SysBeep(2);
				FCD(1064,0);
				return;
			}
			
			len = (long) sizeof(big_tr_type);
			if ((error = FSRead(file_id, &len, (char *) &t_d)) != 0){
				FSClose(file_id);
				SysBeep(2);
				FCD(1064,0);
				return;
			}
			
			len = (long) sizeof(town_item_list);
			if ((error = FSRead(file_id, &len, (char *) &t_i))  != 0){
				FSClose(file_id);
				SysBeep(2);
				FCD(1064,0);
				return;
			}
			
		}
		
		// LOAD STORED ITEMS
		for (i = 0; i < 3; i++) {
			len = (long) sizeof(stored_items_list_type);
			if ((error = FSRead(file_id, &len, (char *) &stored_items[i]))  != 0){
				FSClose(file_id);
				SysBeep(2);
				FCD(1064,0);
				return;
			}		
		}
		
		// LOAD SAVED MAPS
		if (maps_there == TRUE) {
			len = (long) sizeof(stored_town_maps_type);
			if ((error = FSRead(file_id, &len, (char *) &(data_store->town_maps)))  != 0){
				FSClose(file_id);
				SysBeep(2);
				FCD(1064,0);
				return;
			}
			
			len = (long) sizeof(stored_outdoor_maps_type);
			if ((error = FSRead(file_id, &len, (char *) &o_maps)) != 0) {
				FSClose(file_id);
				SysBeep(2);
				FCD(1064,0);
				return;
			}	
		}
		
		// LOAD SFX & MISC_I
		len = (long) (64 * 64);
		if ((error = FSRead(file_id, &len, (char *) sfx))  != 0){
			FSClose(file_id);
			SysBeep(2);
			FCD(1064,0);
			return;
		}	
		if ((error = FSRead(file_id, &len, (char *) misc_i))  != 0){
			FSClose(file_id);
			SysBeep(2);
			FCD(1064,0);
			return;
		}	
		
	} // end if_scen
	
	if ((error = FSClose(file_id))  != 0){
		add_string_to_buf("Load: Can't close file.          ");
		SysBeep(2);
		return;
	} 
	
	party_in_memory = TRUE;
	
	// now if not in scen, this is it.
	if (in_scen == FALSE) {
		if (in_startup_mode == FALSE) {
			reload_startup();
			in_startup_mode = TRUE;
			draw_startup(0);		
		}
		return;
	}
	
	if (load_scenario() == FALSE)
		return;
	
	// if at this point, startup must be over, so make this call to make sure we're ready,
	// graphics wise
	end_startup();
	
	set_up_ter_pics();
	
	load_outdoors(party.outdoor_corner.x + 1,party.outdoor_corner.y + 1,1,1,0,0,NULL);
	load_outdoors(party.outdoor_corner.x,party.outdoor_corner.y + 1,0,1,0,0,NULL);
	load_outdoors(party.outdoor_corner.x + 1,party.outdoor_corner.y,1,0,0,0,NULL);
	load_outdoors(party.outdoor_corner.x,party.outdoor_corner.y,0,0,0,0,NULL);
	
	//end_anim();
	overall_mode = (town_restore == TRUE) ? 1 : 0;
	stat_screen_mode = 0;
	build_outdoors();
	erase_out_specials();
	update_pc_graphics();
	
	
	if (town_restore == FALSE) {
		center = party.p_loc;
		load_area_graphics();
	}
	else {	
		load_town(c_town.town_num,2,-1,NULL);
		load_town(c_town.town_num,1,-1,NULL);
		
		for (i = 0; i < T_M; i++){
			monster_targs[i].x = 0;
			monster_targs[i].y = 0;
		}
		
		town_type = scenario.town_size[c_town.town_num];
		
		// Set up field booleans
		for (j = 0; j < town_size[town_type]; j++)
			for (k = 0; k < town_size[town_type]; k++) {
				if (is_web(j,k) == TRUE)
					web = TRUE;
				if (is_crate(j,k) == TRUE)
					crate = TRUE;
				if (is_barrel(j,k) == TRUE)
					barrel = TRUE;
				if (is_fire_barrier(j,k) == TRUE)
					fire_barrier = TRUE;
				if (is_force_barrier(j,k) == TRUE)
					force_barrier = TRUE;
				if (is_quickfire(j,k) == TRUE)
					quickfire = TRUE;
				if ((scenario.ter_types[t_d.terrain[j][k]].special >= 16) &&
					(scenario.ter_types[t_d.terrain[j][k]].special <= 19))
					belt_present = TRUE;
			}
		force_wall = TRUE;
		fire_wall = TRUE;
		antimagic = TRUE;
		scloud = TRUE;
		ice_wall = TRUE;
		blade_wall = TRUE;
		sleep_field = TRUE;
		center = c_town.p_loc;
		load_area_graphics();
	}
	
	create_clip_region();
	redraw_screen();
	current_pc = first_active_pc();
	loaded_yet = TRUE;
	
	
	strcpy ((char *) last_load_file, (char *) file_to_load.name);
	store_file_reply = file_to_load;
	
	add_string_to_buf("Load: Game loaded.            ");
	
	// Set sounds, map saving, and speed
	if (((play_sounds == TRUE) && (party.stuff_done[SDF_NO_SOUNDS] == 1)) ||
		((play_sounds == FALSE) && (party.stuff_done[SDF_NO_SOUNDS] == 0))) {
		flip_sound();
	}
	give_delays = party.stuff_done[SDF_NO_FRILLS]; 
	if (party.stuff_done[SFD_NO_MAPS] == 0)
		save_maps = TRUE;
	else save_maps = FALSE;
	
	in_startup_mode = FALSE;
	in_scen_debug = FALSE;
	
}

void save_file(short mode)
//mode;  // 0 - normal  1 - save as
{
	NavReplyRecord reply;
	OSErr error;
	short file_id;
	Boolean town_save = FALSE;
	Str63 store_name;
	FSSpec to_load;
	
	short i;

	long len,store_len,count;
	flag_type flag;
	flag_type *store;
	party_record_type *party_ptr;
	setup_save_type	*setup_ptr;
	pc_record_type *pc_ptr;
//	out_info_type store_explored;
	current_town_type *town_ptr;
	big_tr_type *town_data_ptr;
	town_item_list *item_ptr;
	stored_items_list_type *items_ptr;
	stored_town_maps_type *maps_ptr; 
	stored_outdoor_maps_type *o_maps_ptr; 
		
	char *party_encryptor;	

	if ((in_startup_mode == FALSE) && (is_town()))
		town_save = TRUE;

	strcpy ((char *) store_name, (char *) last_load_file);
	

	if ((mode == 1) || (loaded_yet == FALSE)) {
		NavPutFile(NULL,&reply,NULL,NULL,'beSV','blx!',NULL);
		if (reply.validRecord == FALSE)
			return;
		
		AEKeyword keyword;
		DescType descType;
		Size actualSize;
		
		AEGetNthPtr(&reply.selection,1,typeFSS,&keyword,&descType,&to_load,sizeof(FSSpec),&actualSize);
		loaded_yet = TRUE;
		}
		else to_load = store_file_reply;
		
	error = FSpCreate(&to_load,'blx!','beSV',reply.keyScript);
	if ((error != 0) && (error != dupFNErr)) {
		add_string_to_buf("Save: Couldn't create file.         ");
		SysBeep(2);
		return;
		}
	if ((error = FSpOpenDF(&to_load,3,&file_id)) != 0) {
		add_string_to_buf("Save: Couldn't open file.         ");
		SysBeep(2);
		return;
		}			
	strcpy ((char *) last_load_file, (char *) to_load.name);
	store_file_reply = to_load;

		
	store = &flag;	

	len = sizeof(flag_type);

	flag.i = (town_save == TRUE) ? 1342 : 5790;
	if ((error = FSWrite(file_id, &len, (char *) store))  != 0){
		add_string_to_buf("Save: Couldn't write to file.         ");
		FSClose(file_id);
		SysBeep(2);
		}
	flag.i = (in_startup_mode == FALSE) ? 100 : 200;
	if ((error = FSWrite(file_id, &len, (char *) store)) != 0) {
		add_string_to_buf("Save: Couldn't write to file.         ");
		FSClose(file_id);
		SysBeep(2);
		return;
		}
	flag.i = (save_maps == TRUE) ? 5567 : 3422;
	if ((error = FSWrite(file_id, &len, (char *) store))  != 0){
		add_string_to_buf("Save: Couldn't write to file.         ");
		FSClose(file_id);
		SysBeep(2); 
		return;
		}

	// SAVE PARTY
	party_ptr = &party;	
	len = sizeof(party_record_type);

	store_len = len;
	party_encryptor = (char *) party_ptr;
	for (count = 0; count < store_len; count++)
		party_encryptor[count] ^= 0x5C;
	if ((error = FSWrite(file_id, &len, (char *) party_ptr)) != 0) {
		add_string_to_buf("Save: Couldn't write to file.         ");
		FSClose(file_id);
		for (count = 0; count < store_len; count++)
			party_encryptor[count] ^= 0x5C;
			SysBeep(2); 
		return;
		}
	for (count = 0; count < store_len; count++)
		party_encryptor[count] ^= 0x5C;

	// SAVE SETUP
	setup_ptr = &setup_save;	
	len = sizeof(setup_save_type);
	if ((error = FSWrite(file_id, &len, (char *) setup_ptr))  != 0){
		add_string_to_buf("Save: Couldn't write to file.         ");
		FSClose(file_id);
		SysBeep(2); 
		return;
		}
		
	// SAVE PCS	
	store_len = sizeof(pc_record_type);
	for (i = 0; i < 6; i++) {
		pc_ptr = &adven[i];	

		len = store_len;
		party_encryptor = (char *) pc_ptr;
		for (count = 0; count < store_len; count++)
			party_encryptor[count] ^= 0x6B;
		if ((error = FSWrite(file_id, &len, (char *) pc_ptr))  != 0){
			add_string_to_buf("Save: Couldn't write to file.         ");
			FSClose(file_id);
			for (count = 0; count < store_len; count++)
				party_encryptor[count] ^= 0x6B;
				SysBeep(2); 
			return;
			}
		for (count = 0; count < store_len; count++)
			party_encryptor[count] ^= 0x6B;
		}
	
	if (in_startup_mode == FALSE) {
	
	// SAVE OUT DATA
	len = sizeof(out_info_type);
	if ((error = FSWrite(file_id, &len, (char *) out_e)) != 0) {
		add_string_to_buf("Save: Couldn't write to file.         ");
		FSClose(file_id);
		SysBeep(2); 
		return;
		}

	if (town_save == TRUE) {	
			town_ptr = &c_town;	
			len = sizeof(current_town_type);
			if ((error = FSWrite(file_id, &len, (char *) town_ptr)) != 0) {
				add_string_to_buf("Save: Couldn't write to file.         ");
				FSClose(file_id);
				SysBeep(2); 
				return;
				}
			town_data_ptr = &t_d;	
			len = sizeof(big_tr_type);
			if ((error = FSWrite(file_id, &len, (char *) town_data_ptr)) != 0) {
				add_string_to_buf("Save: Couldn't write to file.         ");
				FSClose(file_id);
				SysBeep(2); 
				return;
				}
			item_ptr = &t_i;	
			len = sizeof(town_item_list);
			if ((error = FSWrite(file_id, &len, (char *) item_ptr)) != 0) {
				add_string_to_buf("Save: Couldn't write to file.         ");
				FSClose(file_id);
				SysBeep(2); 
				return;
			}	
		}
	
	// Save stored items 
	for (i = 0; i < 3; i++) {
		items_ptr = &stored_items[i];
		len = (long) sizeof(stored_items_list_type);
		if ((error = FSWrite(file_id, &len, (char *) items_ptr))  != 0){
			add_string_to_buf("Save: Couldn't write to file.         ");
			FSClose(file_id);
			SysBeep(2); 
			return;
			}
		}
			
	// If saving maps, save maps
	if (save_maps == TRUE) {
		maps_ptr = &(data_store->town_maps);
		len = (long) sizeof(stored_town_maps_type);
		if ((error = FSWrite(file_id, &len, (char *) maps_ptr))  != 0){
			add_string_to_buf("Save: Couldn't write to file.         ");
			FSClose(file_id);
			SysBeep(2); 
			return;
			}	

		o_maps_ptr = &o_maps;
		len = (long) sizeof(stored_outdoor_maps_type);
		if ((error = FSWrite(file_id, &len, (char *) o_maps_ptr)) != 0) {
			add_string_to_buf("Save: Couldn't write to file.         ");
			FSClose(file_id);
			SysBeep(2); 
			return;
			}
		} 
	
	// SAVE SFX and MISC_I
		len = (long) (64 * 64);
		if ((error = FSWrite(file_id, &len, (char *) sfx))  != 0){
			add_string_to_buf("Save: Couldn't write to file.         ");
			FSClose(file_id);
			SysBeep(2); 
			return;
			}
		if ((error = FSWrite(file_id, &len, (char *) misc_i))  != 0){
			add_string_to_buf("Save: Couldn't write to file.         ");
			FSClose(file_id);
			SysBeep(2); 
			return;
			}
	


	}
	
	if ((error = FSClose(file_id)) != 0) {
		add_string_to_buf("Save: Couldn't close file.         ");
		SysBeep(2);
		return;
		}
	if (in_startup_mode == FALSE)
		add_string_to_buf("Save: Game saved.              ");
}

void set_terrain(location l, unsigned char terrain_type)
{
	t_d.terrain[l.x][l.y] = terrain_type;
	combat_terrain[l.x][l.y] = terrain_type;
}


void swap_val(unsigned char *val,short a,short b)
{
	if (*val == a)
		*val = b;
		else if (*val == b)
			*val = a;
}
void change_val_4 (unsigned char *val,short a,short b,short c,short d)
{
	if (*val == a)
		*val = b;
		else if (*val == b)
			*val = c;
			else if (*val == c)
			*val = d;
			else if (*val == d)
				*val = a;
}
void change_val (unsigned char *val,short a,short b)
{
	if (*val == a)
		*val = b;
}



void build_scen_file_name (Str255 file_n)
{
	sprintf((char *) file_n,"::::Blades of Exile Scenarios:%s",party.scen_name);
	c2pstr((char*)file_n);
}

// mode 0 want town and talking, 1 talking only, 2 want a string only, and extra is string num
// Hey's let's be kludgy and overload these value again! If extra is -1, and mode 2, that
// means we want to load all the strings and only the strings
void load_town(short town_num,short mode,short extra,char *str)
{
	short file_id;
	short i,j;
	long store = 0;
	short which_town;
	long len,len_to_jump = 0;
	OSErr error;
	Str255 file_name;
	
	if (town_num != minmax(0,scenario.num_towns - 1,town_num)) {
		give_error("The scenario tried to place you into a non-existant town.","",0);
		return;
		}
	
	which_town = town_num;
		
	//HGetVol((StringPtr) start_name,&start_volume,&start_dir);
	build_scen_file_name(file_name);
	
	error = HOpen(start_volume,start_dir,file_name,3,&file_id);	
	if (error != 0) {
		//FCD(949,0);
		SysBeep(50);
		return;
		}
	len_to_jump = sizeof(scenario_data_type);
	len_to_jump += sizeof(scen_item_data_type);
	for (i = 0; i < 300; i++)
		len_to_jump += (long) scenario.scen_str_len[i];
	store = 0;
	for (i = 0; i < 100; i++)
		for (j = 0; j < 2; j++)
			store += (long) (scenario.out_data_size[i][j]);
	for (i = 0; i < which_town; i++)
		for (j = 0; j < 5; j++)
			store += (long) (scenario.town_data_size[i][j]);
	len_to_jump += store;
	
	error = SetFPos (file_id, 1, len_to_jump);
	if (error != 0) {FSClose(file_id);oops_error(35);}
		
	len = sizeof(town_record_type);
	
	if (mode == 0) {
		error = FSRead(file_id, &len , (char *) &c_town.town);
		port_town();
		}
		else error = FSRead(file_id, &len , (char *) &dummy_town);
	if (error != 0) {FSClose(file_id);oops_error(36);}

	switch (scenario.town_size[which_town]) {
		case 0:
			len =  sizeof(big_tr_type);
			if (mode == 0) {
				FSRead(file_id, &len, (char *) &t_d);
				port_t_d();
				}
				else error = SetFPos (file_id, 3, len);
			
			break;
			
		case 1:
				len = sizeof(ave_tr_type);
				if (mode == 0) {
					FSRead(file_id, &len, (char *) &ave_t);
					for (i = 0; i < 48; i++)
						for (j = 0; j < 48; j++) {
							t_d.terrain[i][j] = ave_t.terrain[i][j];
							t_d.lighting[i / 8][j] = ave_t.lighting[i / 8][j];					
							}
					//print_nums(5555,5555,5555);
					//for (i = 0; i < 8; i++)
					//	for (j = 0; j < 48; j++)
					//		if (t_d.lighting[i][j] != 0) {
					//			print_nums(i,j,t_d.lighting[i][j]);
					//			}
							
					for (i = 0; i < 16; i++) {
						t_d.room_rect[i] = ave_t.room_rect[i];
						}
					for (i = 0; i < 40; i++) {
						t_d.creatures[i] = ave_t.creatures[i];
						}
					for (i = 40; i < 60; i++) {
						t_d.creatures[i].number = 0;
						}
					port_t_d();
					}
					else error = SetFPos (file_id, 3, len);

			break;
			
		case 2:
			len = sizeof(tiny_tr_type);
			if (mode == 0) {
				FSRead(file_id,&len , (char *) &tiny_t);
				for (i = 0; i < 32; i++)
					for (j = 0; j < 32; j++) {
						t_d.terrain[i][j] = tiny_t.terrain[i][j];
						t_d.lighting[i / 8][j] = tiny_t.lighting[i / 8][j];					
						}
				for (i = 0; i < 16; i++) {
					t_d.room_rect[i] = tiny_t.room_rect[i];
				}
				for (i = 0; i < 30; i++) {
					t_d.creatures[i] = tiny_t.creatures[i];
					}
				for (i = 30; i < 60; i++) {
					t_d.creatures[i].number = 0;
					}
				port_t_d();
				}
				else error = SetFPos (file_id, 3, len);
			break;
		}
		
	for (i = 0; i < 140; i++) {
		len = (mode == 0) ? (long) (c_town.town.strlens[i]) : (long) (dummy_town.strlens[i]);
		switch (mode) {
			case 0:
				FSRead(file_id, &len, (char *) &(data_store->town_strs[i]));
				data_store->town_strs[i][len] = 0;
				break;
			case 1:
				SetFPos (file_id, 3, len);
				break;
			case 2:
				if (extra < 0) {
					FSRead(file_id, &len, (char *) &(data_store->town_strs[i]));
					data_store->town_strs[i][len] = 0;
					}
				else if (i == extra) {
					FSRead(file_id, &len, (char *) str);
					str[len] = 0;
					}
					else SetFPos (file_id, 3, len);
				break;
			}
		}

	if (mode < 2) {
		len = sizeof(talking_record_type);
		error = FSRead(file_id, &len , (char *) &talking);
		port_talk_nodes();
		if (error != 0) {FSClose(file_id);oops_error(37);}
	
		for (i = 0; i < 170; i++) {
			len = (long) (talking.strlens[i]);
			FSRead(file_id, &len, (char *) &(data_store->talk_strs[i]));
			data_store->talk_strs[i][len] = 0;
			}
		cur_town_talk_loaded = town_num;
		}
	if (mode == 0)
		town_type = scenario.town_size[which_town];
	error = FSClose(file_id);
	if (error != 0) {FSClose(file_id);oops_error(38);}
	
	// Now more initialization is needed. First need to properly create the misc_i array.
	
	// Initialize barriers, etc. Note non-sfx gets forgotten if this is a town recently visited.
	if (mode == 0) {
		for (i = 0; i < 64; i++)
			for (j = 0; j < 64; j++) {
				misc_i[i][j] = 0;
				sfx[i][j] = 0;
				}
		for (i = 0; i < 50; i++)
			if ((c_town.town.spec_id[i] >= 0) && (c_town.town.special_locs[i].x < 100)){
				make_special(c_town.town.special_locs[i].x,c_town.town.special_locs[i].y);
				}
	for (i = 0; i < 50; i++) {
			if ((c_town.town.preset_fields[i].field_type > 0) && (c_town.town.preset_fields[i].field_type < 9))
				misc_i[c_town.town.preset_fields[i].field_loc.x][c_town.town.preset_fields[i].field_loc.y] = 
				 misc_i[c_town.town.preset_fields[i].field_loc.x][c_town.town.preset_fields[i].field_loc.y] | 
			 	(unsigned char) (s_pow(2,c_town.town.preset_fields[i].field_type - 1));
			if ((c_town.town.preset_fields[i].field_type >= 14) && (c_town.town.preset_fields[i].field_type <= 21))
				sfx[c_town.town.preset_fields[i].field_loc.x][c_town.town.preset_fields[i].field_loc.y] = 
				 sfx[c_town.town.preset_fields[i].field_loc.x][c_town.town.preset_fields[i].field_loc.y] | 
				 (unsigned char) (s_pow(2,c_town.town.preset_fields[i].field_type - 14));

			}
		}
}


void shift_universe_left()
{
	short i,j;
	
	make_cursor_watch();
	
	save_outdoor_maps();
	party.outdoor_corner.x--;
	party.i_w_c.x++;
	party.p_loc.x += 48;
	outdoors[1][0] = outdoors[0][0];
	outdoors[1][1] = outdoors[0][1];
	outdoor_text[1][0] = outdoor_text[0][0];
	outdoor_text[1][1] = outdoor_text[0][1];

	for (i = 48; i < 96; i++)
		for (j = 0; j < 96; j++)
			out_e[i][j] = out_e[i - 48][j];

	for (i = 0; i < 48; i++)
		for (j = 0; j < 96; j++)
			out_e[i][j] = 0;

	for (i = 0; i < 10; i++) {
		if (party.out_c[i].m_loc.x > 48)
			party.out_c[i].exists = FALSE;
		if (party.out_c[i].exists == TRUE) 
			party.out_c[i].m_loc.x += 48;
		}

	load_outdoors(party.outdoor_corner.x,party.outdoor_corner.y,0,0,0,0,NULL);
	load_outdoors(party.outdoor_corner.x,party.outdoor_corner.y + 1,0,1,0,0,NULL);
	build_outdoors();


	// reload graphics -- who knows what we added
	load_area_graphics();		
	make_cursor_sword();

}

void shift_universe_right()
{
	short i,j;
	
	make_cursor_watch();
	save_outdoor_maps();
	party.outdoor_corner.x++;
	party.i_w_c.x--;
	party.p_loc.x -= 48;
	outdoors[0][0] = outdoors[1][0];
	outdoors[0][1] = outdoors[1][1];
	outdoor_text[0][0] = outdoor_text[1][0];
	outdoor_text[0][1] = outdoor_text[1][1];
	for (i = 0; i < 48; i++)
		for (j = 0; j < 96; j++)
			out_e[i][j] = out_e[i + 48][j];
	for (i = 48; i < 96; i++)
		for (j = 0; j < 96; j++)
			out_e[i][j] = 0;


	for (i = 0; i < 10; i++) {
		if (party.out_c[i].m_loc.x < 48)
			party.out_c[i].exists = FALSE;
		if (party.out_c[i].exists == TRUE) 
			party.out_c[i].m_loc.x -= 48;
		}
	load_outdoors(party.outdoor_corner.x + 1,party.outdoor_corner.y,1,0,0,0,NULL);
	load_outdoors(party.outdoor_corner.x + 1,party.outdoor_corner.y + 1,1,1,0,0,NULL);
	build_outdoors();

	// reload graphics -- who knows what we added
	load_area_graphics();		
	make_cursor_sword();

}

void shift_universe_up()
{
	short i,j;
	
	make_cursor_watch();
	save_outdoor_maps();
	party.outdoor_corner.y--;
	party.i_w_c.y++;
	party.p_loc.y += 48;
	outdoors[0][1] = outdoors[0][0];
	outdoors[1][1] = outdoors[1][0];

	outdoor_text[0][1] = outdoor_text[0][0];
	outdoor_text[1][1] = outdoor_text[1][0];
		for (i = 0; i < 96; i++)
			for (j = 48; j < 96; j++)
				out_e[i][j] = out_e[i][j - 48];
		for (i = 0; i < 96; i++)
			for (j = 0; j < 48; j++)
				out_e[i][j] = 0;	 

	for (i = 0; i < 10; i++) {
		if (party.out_c[i].m_loc.y > 48)
			party.out_c[i].exists = FALSE;
		if (party.out_c[i].exists == TRUE)
			party.out_c[i].m_loc.y += 48; 
		}
	load_outdoors(party.outdoor_corner.x,party.outdoor_corner.y,0,0,0,0,NULL);
	load_outdoors(party.outdoor_corner.x + 1,party.outdoor_corner.y,1,0,0,0,NULL);

	build_outdoors();

	// reload graphics -- who knows what we added
	load_area_graphics();		
	make_cursor_sword();

}

void shift_universe_down()
{
	short i,j;

	make_cursor_watch();
	
	save_outdoor_maps();
	party.outdoor_corner.y++;
	party.i_w_c.y--;
	party.p_loc.y = party.p_loc.y - 48;
	outdoors[0][0] = outdoors[0][1];
	outdoors[1][0] = outdoors[1][1];

	outdoor_text[0][0] = outdoor_text[0][1];
	outdoor_text[1][0] = outdoor_text[1][1];
		for (i = 0; i < 96; i++)
			for (j = 0; j < 48; j++)
				out_e[i][j] = out_e[i][j + 48];
		for (i = 0; i < 96; i++)
			for (j = 48; j < 96; j++)
				out_e[i][j] = 0;	

	for (i = 0; i < 10; i++) {
		if (party.out_c[i].m_loc.y < 48)
			party.out_c[i].exists = FALSE;
		if (party.out_c[i].exists == TRUE) 
			party.out_c[i].m_loc.y = party.out_c[i].m_loc.y - 48;
		}
	load_outdoors(party.outdoor_corner.x,party.outdoor_corner.y + 1,0,1,0,0,NULL);
	load_outdoors(party.outdoor_corner.x + 1,party.outdoor_corner.y + 1,1,1,0,0,NULL);
	
	build_outdoors();

	// reload graphics -- who knows what we added
	load_area_graphics();		
	make_cursor_sword();

}


void position_party(short out_x,short out_y,short pc_pos_x,short pc_pos_y) ////
{
	short i,j;

	if ((pc_pos_x != minmax(0,47,pc_pos_x)) || (pc_pos_y != minmax(0,47,pc_pos_y)) ||
		(out_x != minmax(0,scenario.out_width - 1,out_x)) || (out_y != minmax(0,scenario.out_height - 1,out_y))) {
			give_error("The scenario has tried to place you in an out of bounds outdoor location.","",0);
			return;
			}

	save_outdoor_maps();
	party.p_loc.x = pc_pos_x;
	party.p_loc.y = pc_pos_y;
	party.loc_in_sec = global_to_local(party.p_loc);
	
	if ((party.outdoor_corner.x != out_x) || (party.outdoor_corner.y != out_y)) {
		party.outdoor_corner.x = out_x;
		party.outdoor_corner.y = out_y;	
		load_outdoors(party.outdoor_corner.x + 1,party.outdoor_corner.y + 1,1,1,0,0,NULL);
		load_outdoors(party.outdoor_corner.x,party.outdoor_corner.y + 1,0,1,0,0,NULL);
		load_outdoors(party.outdoor_corner.x + 1,party.outdoor_corner.y,1,0,0,0,NULL);
		load_outdoors(party.outdoor_corner.x,party.outdoor_corner.y,0,0,0,0,NULL);
		}
	party.i_w_c.x = (party.p_loc.x > 47) ? 1 : 0;
	party.i_w_c.y = (party.p_loc.y > 47) ? 1 : 0;
	for (i = 0; i < 10; i++)
		party.out_c[i].exists = FALSE;
	for (i = 0; i < 96; i++)
		for (j = 0; j < 96; j++) 	
			out_e[i][j] = 0;
	build_outdoors();	
}


void build_outdoors()
{
	short i,j;
		for (i = 0; i < 48; i++)
			for (j = 0; j < 48; j++) {
				out[i][j] = outdoors[0][0].terrain[i][j];
				out[48 + i][j] = outdoors[1][0].terrain[i][j];
				out[i][48 + j] = outdoors[0][1].terrain[i][j];
				out[48 + i][48 + j] = outdoors[1][1].terrain[i][j];	
			}
			
	fix_boats();
	add_outdoor_maps(); 
	make_out_trim();
	if (in_startup_mode == FALSE)
		erase_out_specials();
	
for (i = 0; i < 10; i++)
		if (party.out_c[i].exists == TRUE)
			if ((party.out_c[i].m_loc.x < 0) || (party.out_c[i].m_loc.y < 0) ||
				(party.out_c[i].m_loc.x > 95) || (party.out_c[i].m_loc.y > 95))
				party.out_c[i].exists = FALSE;
	
}

short onm(char x_sector,char y_sector)
{
	short i;

	i = y_sector * scenario.out_width + x_sector;
	return i;
}



// This adds the current outdoor map info to the saved outdoor map info
void save_outdoor_maps()
{
	short i,j;
	
	for (i = 0; i < 48; i++)
		for (j = 0; j < 48; j++) {
			if (out_e[i][j] > 0) 
				o_maps.outdoor_maps[onm(party.outdoor_corner.x,party.outdoor_corner.y)][i / 8][j] =
				 o_maps.outdoor_maps[onm(party.outdoor_corner.x,party.outdoor_corner.y)][i / 8][j] |
				 (char) (s_pow(2,i % 8));	
			if (party.outdoor_corner.x + 1 < scenario.out_width) {
				if (out_e[i + 48][j] > 0) 
					o_maps.outdoor_maps[onm(party.outdoor_corner.x + 1,party.outdoor_corner.y)][i / 8][j] =
					 o_maps.outdoor_maps[onm(party.outdoor_corner.x + 1,party.outdoor_corner.y)][i / 8][j] |
					 (char) (s_pow(2,i % 8));
				}
			if (party.outdoor_corner.y + 1 < scenario.out_height) {
				if (out_e[i][j + 48] > 0) 
					o_maps.outdoor_maps[onm(party.outdoor_corner.x,party.outdoor_corner.y + 1)][i / 8][j] =
					 o_maps.outdoor_maps[onm(party.outdoor_corner.x,party.outdoor_corner.y + 1)][i / 8][j] |
					 (char) (s_pow(2,i % 8));
				}
			if ((party.outdoor_corner.y + 1 < scenario.out_height) &&
				(party.outdoor_corner.x + 1 < scenario.out_width)) {
				if (out_e[i + 48][j + 48] > 0) 
					o_maps.outdoor_maps[onm(party.outdoor_corner.x + 1,party.outdoor_corner.y + 1)][i / 8][j] =
					 o_maps.outdoor_maps[onm(party.outdoor_corner.x + 1,party.outdoor_corner.y + 1)][i / 8][j] |
					 (char) (s_pow(2,i % 8));
				}
			}
}

void add_outdoor_maps()  // This takes the existing outdoor map info and supplements it
						// with the saved map info
{
	short i,j;
	
	for (i = 0; i < 48; i++)
		for (j = 0; j < 48; j++) {
			if ((out_e[i][j] == 0) && 
			 ((o_maps.outdoor_maps[onm(party.outdoor_corner.x,party.outdoor_corner.y)][i / 8][j] &
			  (char) (s_pow(2,i % 8))) != 0))
			 	out_e[i][j] = 1;
			if (party.outdoor_corner.x + 1 < scenario.out_width) {
				if ((out_e[i + 48][j] == 0) && 
				 ((o_maps.outdoor_maps[onm(party.outdoor_corner.x + 1,party.outdoor_corner.y)][i / 8][j] &
				  (char) (s_pow(2,i % 8))) != 0))
				 	out_e[i + 48][j] = 1;		
			 		}	 	
			if (party.outdoor_corner.y + 1 < scenario.out_height) {
				if ((out_e[i][j + 48] == 0) && 
				 ((o_maps.outdoor_maps[onm(party.outdoor_corner.x,party.outdoor_corner.y + 1)][i / 8][j] &
				  (char) (s_pow(2,i % 8))) != 0))
				 	out_e[i][j + 48] = 1;
				}
			if ((party.outdoor_corner.y + 1 < scenario.out_height) &&
				(party.outdoor_corner.x + 1 < scenario.out_width)) {
				if ((out_e[i + 48][j + 48] == 0) && 
				 ((o_maps.outdoor_maps[onm(party.outdoor_corner.x + 1,party.outdoor_corner.y + 1)][i / 8][j] &
				  (char) (s_pow(2,i % 8))) != 0))
				 	out_e[i + 48][j + 48] = 1;
			 	}
			 }
} 



void fix_boats()
{
	short i;

	for (i = 0; i < 30; i++)
		if ((party.boats[i].exists == TRUE) && (party.boats[i].which_town == 200)) {
			if (party.boats[i].boat_sector.x == party.outdoor_corner.x)
				party.boats[i].boat_loc.x = party.boats[i].boat_loc_in_sec.x;
				else if (party.boats[i].boat_sector.x == party.outdoor_corner.x + 1)
				party.boats[i].boat_loc.x = party.boats[i].boat_loc_in_sec.x + 48;
					else party.boats[i].boat_loc.x = 500;
			if (party.boats[i].boat_sector.y == party.outdoor_corner.y) 
				party.boats[i].boat_loc.y = party.boats[i].boat_loc_in_sec.y;
				else if (party.boats[i].boat_sector.y == party.outdoor_corner.y + 1)
				party.boats[i].boat_loc.y = party.boats[i].boat_loc_in_sec.y + 48;
					else party.boats[i].boat_loc.y = 500;
			}
	for (i = 0; i < 30; i++)
		if ((party.horses[i].exists == TRUE) && (party.horses[i].which_town == 200)) {
			if (party.horses[i].horse_sector.x == party.outdoor_corner.x)
				party.horses[i].horse_loc.x = party.horses[i].horse_loc_in_sec.x;
				else if (party.horses[i].horse_sector.x == party.outdoor_corner.x + 1)
				party.horses[i].horse_loc.x = party.horses[i].horse_loc_in_sec.x + 48;
					else party.horses[i].horse_loc.x = 500;
			if (party.horses[i].horse_sector.y == party.outdoor_corner.y) 
				party.horses[i].horse_loc.y = party.horses[i].horse_loc_in_sec.y;
				else if (party.horses[i].horse_sector.y == party.outdoor_corner.y + 1)
				party.horses[i].horse_loc.y = party.horses[i].horse_loc_in_sec.y + 48;
					else party.horses[i].horse_loc.y = 500;
			}
}


void load_outdoors(short to_create_x, short to_create_y, short targ_x, short targ_y,
	short mode,short extra,char *str)
//short	to_create_x, to_create_y; // actual sector being loaded
//short 	targ_x, targ_y; // 0 or 1
// mode 0 - whole out, 1 - just string number extra
{
	short file_id;
	short i,j;
	long len;
	Str255 file_name;
	short out_sec_num;
	long len_to_jump = 0,store = 0;
	OSErr error;
	
	if ((to_create_x != minmax(0,scenario.out_width - 1,to_create_x)) ||
		(to_create_y != minmax(0,scenario.out_height - 1,to_create_y))) { // not exist
			for (i = 0; i < 48; i++)
				for (j = 0; j < 48; j++)
					outdoors[targ_x][targ_y].terrain[i][j] = 5;
			for (i = 0; i < 18; i++) {
				outdoors[targ_x][targ_y].special_id[i] = -1;
				outdoors[targ_x][targ_y].special_locs[i].x = 100;
				}
			return;
			}

	build_scen_file_name(file_name);
	
	error = HOpen(start_volume,start_dir,file_name,3,&file_id);	
	
	out_sec_num = scenario.out_width * to_create_y + to_create_x;
	
	len_to_jump = sizeof(scenario_data_type);
	len_to_jump += sizeof(scen_item_data_type);
	for (i = 0; i < 300; i++)
		len_to_jump += (long) scenario.scen_str_len[i];
	store = 0;
	for (i = 0; i < out_sec_num; i++)
		for (j = 0; j < 2; j++)
			store += (long) (scenario.out_data_size[i][j]);
	len_to_jump += store;
	
	error = SetFPos (file_id, 1, len_to_jump);	
	if (error != 0) {FSClose(file_id);oops_error(32);}

	len = sizeof(outdoor_record_type);
	if (mode == 0) {
		error = FSRead(file_id, &len, (char *) &outdoors[targ_x][targ_y]);
		port_out(&outdoors[targ_x][targ_y]);
		if (error != 0) {FSClose(file_id);oops_error(33);}
		}
		else error = FSRead(file_id, &len, (char *) &dummy_out);
		
	if (mode == 0) {
		for (i = 0; i < 9; i++) {
			len = (long) (outdoors[targ_x][targ_y].strlens[i]);
			FSRead(file_id, &len, (char *) &(outdoor_text[targ_x][targ_y].out_strs[i]));
			outdoor_text[targ_x][targ_y].out_strs[i][len] = 0;
			}
		}
	if (mode == 1) {
		for (i = 0; i < 120; i++) {
			len = (long) (dummy_out.strlens[i]);
			if (i == extra) {
				FSRead(file_id, &len, (char *) str);
				str[len] = 0;
				}
				else SetFPos (file_id, 3, len);	
			}
	
		}

	error = FSClose(file_id);
	if (error != 0) {FSClose(file_id);oops_error(33);}
}





void start_data_dump()
{
	long i;
	char get_text[280];
	
	OSErr error;
	long len;


	error = HOpen(start_volume,start_dir,"\pData dump",3,&data_dump_file_id);

	if (error != 0) {
		HCreate(start_volume,start_dir,"\pData dump",'ttxt','TEXT');
		error = HOpen(start_volume,start_dir,"\pData dump",3,&data_dump_file_id);
		}


	SetFPos (data_dump_file_id, 2, 0);

	sprintf((char *)get_text,"Begin data dump:\r");
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);
	sprintf((char *)get_text,"  Overall mode  %d\r",overall_mode);
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);
	sprintf((char *)get_text,"  Outdoor loc  %d %d  Ploc %d %d\r",party.outdoor_corner.x,party.outdoor_corner.y,
		party.p_loc.x,party.p_loc.y);
	len = (long) (strlen((char *)get_text));
	FSWrite(data_dump_file_id, &len, (char *) get_text);
	if ((is_town()) || (is_combat())) {
		sprintf((char *)get_text,"  Town num %d  Town loc  %d %d \r",c_town.town_num,
			c_town.p_loc.x,c_town.p_loc.y);
		len = (long) (strlen((char *)get_text));
		FSWrite(data_dump_file_id, &len, (char *) get_text);
		if (is_combat()) {
			sprintf((char *)get_text,"  Combat type %d \r",which_combat_type);
			len = (long) (strlen((char *)get_text));
			FSWrite(data_dump_file_id, &len, (char *) get_text);
			}

		for (i = 0; i < T_M; i++) {
			sprintf((char *)get_text,"  Monster %d   Status %d  Loc %d %d  Number %d  Att %d  Tf %d\r",
				(short) i,(short) c_town.monst.dudes[i].active,(short) c_town.monst.dudes[i].m_loc.x,
				(short) c_town.monst.dudes[i].m_loc.y,(short) c_town.monst.dudes[i].number,
				(short) c_town.monst.dudes[i].attitude,(short) c_town.monst.dudes[i].monst_start.time_flag);
			len = (long) (strlen((char *)get_text));
			FSWrite(data_dump_file_id, &len, (char *) get_text);	
			}
		}
	
	
	
}	

void end_data_dump()
{
	FSClose(data_dump_file_id);
	play_sound(1); // formerly force_play_sound
}

// expecting party record to contain name of proper scenario to load
Boolean load_scenario()
{

	short i,file_id;
	Boolean file_ok = FALSE;
	OSErr error;
	long len;
	Str255 file_name;
	
	build_scen_file_name(file_name);
	
	error = HOpen(start_volume,start_dir,file_name,3,&file_id);	
	if (error != 0) {
		oops_error(10000 + error);
		SysBeep(2);	return FALSE;
		}	
		
	len = (long) sizeof(scenario_data_type);
	if ((error = FSRead(file_id, &len, (char *) &scenario)) != 0){
		FSClose(file_id); oops_error(29); return FALSE;
		}
	if ((scenario.flag1 == 10) && (scenario.flag2 == 20)
	 && (scenario.flag3 == 30)
	  && (scenario.flag4 == 40)) {
	  	file_ok = TRUE;
	  	cur_scen_is_mac = TRUE;
	  	}
	if ((scenario.flag1 == 20) && (scenario.flag2 == 40)
	 && (scenario.flag3 == 60)
	  && (scenario.flag4 == 80)) {
	  	file_ok = TRUE;
	  	cur_scen_is_mac = FALSE;
	  	port_scenario();
	  	}
	 if (file_ok == FALSE) {
		FSClose(file_id); 
		give_error("This is not a legitimate Blades of Exile scenario.","",0);
		return FALSE;	 
	 	}
	len = sizeof(scen_item_data_type); // item data
	if ((error = FSRead(file_id, &len, (char *) &(data_store->scen_item_list))) != 0){
		FSClose(file_id); oops_error(30); return FALSE;
		}
	port_item_list();
	for (i = 0; i < 270; i++) {
		len = (long) (scenario.scen_str_len[i]);
		FSRead(file_id, &len, (char *) &(data_store->scen_strs[i]));
		data_store->scen_strs[i][len] = 0;
		}
	
	FSClose(file_id);
	load_spec_graphics();
	set_up_ter_pics();
	return TRUE;
}

void set_up_ter_pics()
{
	short i;
	
	set_terrain_blocked();
	for (i = 0; i < 256; i++)
		terrain_pic[i] = scenario.ter_types[i].picture;
}
void oops_error(short error)
{
	Str255 error_str;
	
		SysBeep(50);
		SysBeep(50);
		SysBeep(50);
	sprintf((char *) error_str,"Giving the scenario editor more memory might also help. Be sure to back your scenario up often. Error number: %d.",error);
	give_error("The program encountered an error while loading/saving/creating the scenario. To prevent future problems, the program will now terminate. Trying again may solve the problem.",(char *) error_str,0);
	//ExitToShell();
}

/*

typedef struct {
	unsigned char flag1, flag2, flag3, flag4;
	unsigned char ver[3],min_run_ver,prog_make_ver[3],num_towns;
	unsigned char out_width,out_height,difficulty,intro_pic,default_ground;
	} scen_header_type;
*/
void build_scen_headers()
{
	short i,index = 1,last_colon;
	short cur_entry = 0;
	Str255 scen_name;
	OSErr err;
	string scenDir = progPath;
	scenDir.erase(scenDir.find_last_of("/"));
	scenDir += "/Blades of Exile Scenarios";
	printf("%s\n%s\n",progPath.c_str(),scenDir.c_str());
	scen_headers.clear();
//	for (i = 0; i < 25; i++)
//		scen_headers[i].flag1 = 0;
	FSRef ref;
	FSIterator iter;
	err = FSPathMakeRef((UInt8*)scenDir.c_str(),&ref,NULL);
	if(err != noErr){
		printf("Directory not found!\n");
		return;
	}
	/**TESTING**/
	UniChar x[] = {0x0024, 0x0041, 0x0020, 0x0075, 0x0073, 0x0065, 0x006c, 0x0065, 0x0073, 0x0073, 0x0020, 0x0066, 0x0069, 0x006c, 0x0065};
	err = FSCreateFileUnicode (&ref, 15, x, NULL, NULL, NULL, NULL);
	if(err != noErr)printf("Error creating file.");
	/**END TESTING**/
	err = FSOpenIterator(&ref, kFSIterateFlat, &iter);
	if(err != noErr){
		printf("Error opening iterator!\n");
		return;
	}
	ItemCount numScens = 0;
	FSRef fileRefs[50];
	FSSpec files[50];
	
	//myCPB.dirInfo.ioCompletion = NULL;
	//myCPB.dirInfo.ioNamePtr = scen_name;
	//myCPB.dirInfo.ioVRefNum = start_volume;
	
	do {
		//myCPB.hFileInfo.ioFDirIndex = index;
		//myCPB.hFileInfo.ioDirID = scen_dir;
		//err = PBGetCatalogInfoSync(&myCPB);
		if(cur_entry == numScens){
			err = FSGetCatalogInfoBulk(iter, 50, &numScens, NULL, kFSCatInfoNone, NULL, fileRefs, files, NULL);
			err = FSGetCatalogInfoBulk(iter, 50, &numScens, NULL, kFSCatInfoNone, NULL, fileRefs, files, NULL);
			if(err == errFSNoMoreItems && cur_entry == 0){
				printf("No scenarios were found!");
				printf("\nnumScens = %d");
			}else if(err != noErr){
				printf("Error getting catalog list!");
			}
			cur_entry = 0;
		}
		if (err == noErr) {
			if ( /*(myCPB.hFileInfo.ioFlAttrib & ioDirMask) != 0*/ false) {
			
			} // folder, so do nothing
			else {
				if (load_scenario_header(fileRefs[cur_entry]) == TRUE) {
					// now we need to store the file name, first stripping any path that occurs
					// before it
					last_colon = -1;
					//p2cstr(scen_name);
//					for (i = 0; i < strlen((char *) scen_name); i++)
//						if (scen_name[i] == ':')
//							last_colon = i;
//					for (i = last_colon + 1; i < strlen((char *) scen_name); i++)
//						data_store->scen_names[cur_entry][i - last_colon - 1] = scen_name[i];
//					data_store->scen_names[cur_entry][strlen((char *) scen_name) - last_colon - 1] = 0;
//					cur_entry++;
					std::string curScenarioName((char*) files[cur_entry].name);
					for(i = 0; i < curScenarioName.length(); i++)
						if(curScenarioName[i] == ':')
							last_colon = i;
					curScenarioName.erase(0,last_colon + 1);
					//if(curScenario != "valleydy.exs" &&
					//   curScenario != "stealth.exs" &&
					//   curScenario != "zakhazi.exs")
						data_store->scen_names.push_back(curScenarioName);
				}
				cur_entry++;
				//entries_so_far++;
			}
		}
		index++;
	}while (err == noErr);
	if (data_store->scen_names.size() == 0) { // no scens present
	}
	FSCloseIterator(iter);
}

// This is only called at startup, when bringing headers of active scenarios.
// This wipes out the scenario record, so be sure not to call it while in an active scenario.
bool load_scenario_header(FSRef file/*,short header_entry*/)
{

	short i, file_id;
	short store;
	Boolean file_ok = FALSE;
	OSErr error;
	long len;
	Str255 load_str;
	Boolean mac_header = TRUE;
		
	//error = HOpen(start_volume,scen_dir,filename,3,&file_id);
	error = FSOpenFork(&file, 0, NULL, fsRdPerm, &file_id);
	if (error != 0) {
		return FALSE;
	}
	scen_header_type curScen;
	len = (long) sizeof(scen_header_type);
	if ((error = FSRead(file_id, &len, (char *) &curScen)) != 0){
		FSClose(file_id); return FALSE;
	}
	if ((curScen.flag1 == 10) && (curScen.flag2 == 20)
	 && (curScen.flag3 == 30)
	  && (curScen.flag4 == 40)) {
	  	file_ok = TRUE;
	  	mac_header = TRUE;
	}
	if ((curScen.flag1 == 20) && (curScen.flag2 == 40)
	 && (curScen.flag3 == 60)
	  && (curScen.flag4 == 80)) {
	  	file_ok = TRUE;
	  	mac_header = FALSE;
	}
	if (file_ok == FALSE) {
		//scen_headers[header_entry].flag1 = 0;
		//scen_headers.pop_back();
		FSClose(file_id); 
		return FALSE;
	}

	// So file is OK, so load in string data and close it.
	SetFPos(file_id,1,0);
	len = (long) sizeof(scenario_data_type);
	error = FSRead(file_id, &len, (char *) &scenario);
	if (error != 0){
		FSClose(file_id);
		oops_error(29);
		return FALSE;
	}
	store = scenario.rating;
	if (mac_header == FALSE)
		flip_short(&store);
	curScen.default_ground = store;
	
	len = sizeof(scen_item_data_type);
	if (SetFPos(file_id,3,len) != 0){
		FSClose(file_id);
		return FALSE;
	}
	
	scen_headers.push_back(curScen);
	string* scen_strs = new string[3];
	data_store->scen_header_strs.push_back(scen_strs);
	for (i = 0; i < 3; i++) {
		store = (short) scenario.scen_str_len[i];
		len = (long) (store);
		FSRead(file_id, &len, (char *) load_str);
		load_str[len] = 0;
		if (i == 0)
			load_str[29] = 0;
		else load_str[59] = 0;
		scen_strs[i] = (char*)load_str;
	}
	
	FSClose(file_id);

	return TRUE;
}

GWorldPtr load_bmp_from_file(Str255 filename)
{
	short file_id;
	long length;
	if (HOpen(start_volume,start_dir,filename,1,&file_id) != 0) return NULL;
	GetEOF(file_id,&length);
	if (length < 54) {
		FSClose(file_id);
		return NULL;
	}
	unsigned char * data = new unsigned char[length];
	FSRead(file_id,&length,data);
	FSClose(file_id);
	GWorldPtr ret = load_bmp(data,length);
	delete data;
	return ret;
}

//extern GWorldPtr spec_scen_g;
void load_spec_graphics()
{
	short i,file_num;
	Str255 file_name;
	
	if (spec_scen_g != NULL) {
		DisposeGWorld(spec_scen_g);
		spec_scen_g = NULL;
		}
	build_scen_file_name(file_name);
	for (i = 0; i < 250; i++) {
		if (file_name[i] == '.') {
			file_name[i + 1] = 'm';
			file_name[i + 2] = 'e';
			file_name[i + 3] = 'g';
			i = 250;
			}
		}
	file_num = HOpenResFile(start_volume,start_dir,file_name,1);
	if (file_num < 0)
	{
		for (i = 0; i < 250; i++) {
			if (file_name[i] == '.') {
				file_name[i + 1] = 'b';
				file_name[i + 2] = 'm';
				file_name[i + 3] = 'p';
				i = 250;
			}
		}
		spec_scen_g = load_bmp_from_file(file_name);
		return;
	}
	spec_scen_g = load_pict(1);
	CloseResFile(file_num);
}


short init_data(short flag)
{
	long k = 0;
		
	k = (long) flag;
	k = k * k;
	jl = jl * jl + 84 + k;
	k = k + 51;
		jl = jl * 2 + 1234 + k;
k = k % 3000;
	jl = jl * 54;
	jl = jl * 2 + 1234 + k;
	k = k * 82;
	k = k % 10000;
	k = k + 10000;
	
	return (short) k;
}

short town_s(short flag)
{
	long k = 0;
		
	k = (long) flag;
	k = k * k * k;
	jl = jl * 54;
	jl = jl * 2 + 1234 + k;
	k = k + 51;
	k = k % 3000;
	jl = jl * 2 + 1234 + k;
	k = k * scenario.num_towns;
	k = k % 10000;
	jl = jl * jl + 84 + k;
	k = k + 10000;
	
	return (short) k;
}

short out_s(short flag)
{
	long k = 0;
		
	k = (long) flag;
	k = k * k * k;
	jl = jl * jl + 84 + k;
	k = k + scenario.out_data_size[0][1];
	k = k % 3000;
	k = k * 4;
	jl = jl * 2 + 1234 + k;
	jl = jl * 54;
	jl = jl * 2 + 1234 + k;
	k = k % 10000;
	k = k + 4;
	
	return (short) k;
}

short str_size_1(short flag)
{
	long k = 0;
		
	k = (long) flag;
	k = k * k;
	jl = jl * 2 + 1234 + k;
	jl = jl * jl + 84 + k;
	k = k + scenario.scen_str_len[0] + scenario.scen_str_len[1] + scenario.scen_str_len[2];
	jl = jl * 2 + 1234 + k;
	k = k % 3000;
	jl = jl * 54;
	jl = jl * jl + 84 + k;
	k = k * 4;
	k = k % 5000;
	k = k - 9099;
	
	return (short) k;
}

short str_size_2(short flag)
{
	long k = 0;
		
	k = (long) flag;
	jl = jl * jl + 84 + k;
	k = k * k * k * k;
	jl = jl * 54;
	k = k + 80;
	k = k % 3000;
	jl = jl * 2 + 1234 + k;
	k = k * scenario.out_width * scenario.out_height;
	jl = jl * jl + 84 + k;
	k = k % 3124;
	k = k - 5426;
	
	return (short) k;
}

short str_size_3(short flag)
{
	long k = 0;
		
	k = (long) flag;
	k = k * (scenario.town_data_size[0][0] +  scenario.town_data_size[0][1] +  scenario.town_data_size[0][2] +  scenario.town_data_size[0][3]);
	k = k + 80;
	jl = jl * jl + 84 + k;
	k = k % 3000;
	jl = jl * 2 + 1234 + k;
	k = k * 45;
	jl = jl * 54;
	jl = jl * jl + 84 + k;
	k = k % 887;
	k = k + 9452;
	
	return (short) k;
}

short get_buf_ptr(short flag)
{
	long k = 0;
		
	k = (long) flag;
	jl = jl * jl + 84 + k;
	k = k * (scenario.out_width +  scenario.out_width +  scenario.out_height +  scenario.town_data_size[0][3]);
	k = k + 80;
	jl = jl * jl + 84 + k;
	k = k % 2443;
	jl = jl * 54;
	k = k * 322;
	jl = jl * 2 + 1234 + k;
	k = k % 2542;
	jl = jl * jl + 84 + k;
	k = k + 234;
	
	return (short) k;
}

Boolean check_p (short pword)
{
	if (scenario.flag_b != town_s(pword))
		return FALSE;
	if (scenario.flag_c != out_s(pword))
		return FALSE;
	if (scenario.flag_e != str_size_1(pword))
		return FALSE;
	if (scenario.flag_f != str_size_2(pword))
		return FALSE;
	if (scenario.flag_h != str_size_3(pword))
		return FALSE;
	if (scenario.flag_d != init_data(pword))
		return FALSE;

	return TRUE;
}

void port_talk_nodes()
{
	short i;
	
	if (cur_scen_is_mac == TRUE)
		return;
	for (i = 0; i < 60; i++) {
		flip_short(&talking.talk_nodes[i].personality);
		flip_short(&talking.talk_nodes[i].type);
		flip_short(&talking.talk_nodes[i].extras[0]);
		flip_short(&talking.talk_nodes[i].extras[1]);
		flip_short(&talking.talk_nodes[i].extras[2]);
		flip_short(&talking.talk_nodes[i].extras[3]);
		}
}

void port_town()
{
	short i;

	if (cur_scen_is_mac == TRUE)
		return;
	flip_short(&c_town.town.town_chop_time);
	flip_short(&c_town.town.town_chop_key);
	flip_short(&c_town.town.lighting);
	for (i =0 ; i < 4; i++)
		flip_short(&c_town.town.exit_specs[i]);
	flip_rect(&c_town.town.in_town_rect);
	for (i =0 ; i < 64; i++) {
		flip_short(&c_town.town.preset_items[i].item_code);
		flip_short(&c_town.town.preset_items[i].ability);
		}
	for (i =0 ; i < 50; i++) {
		flip_short(&c_town.town.preset_fields[i].field_type);
		}
	flip_short(&c_town.town.max_num_monst);
	flip_short(&c_town.town.spec_on_entry);
	flip_short(&c_town.town.spec_on_entry_if_dead);
	for (i =0 ; i < 8; i++) 
		flip_short(&c_town.town.timer_spec_times[i]);
	for (i =0 ; i < 8; i++) 
		flip_short(&c_town.town.timer_specs[i]);
	flip_short(&c_town.town.difficulty);
	for (i =0 ; i < 100; i++) 
		flip_spec_node(&c_town.town.specials[i]);
	
}

void port_t_d()
{
	short i;
	if (cur_scen_is_mac == TRUE)
		return;

	for (i =0 ; i < 16; i++) 
		flip_rect(&t_d.room_rect[i]);
	for (i =0 ; i < 60; i++) {
		flip_short(&t_d.creatures[i].spec1);
		flip_short(&t_d.creatures[i].spec2);
		flip_short(&t_d.creatures[i].monster_time);
		flip_short(&t_d.creatures[i].personality);
		flip_short(&t_d.creatures[i].special_on_kill);
		flip_short(&t_d.creatures[i].facial_pic);
	
		} 
}

void port_scenario()
{
	short i,j;
	
	if (cur_scen_is_mac == TRUE)
		return;
	flip_short(&scenario.flag_a);
	flip_short(&scenario.flag_b);
	flip_short(&scenario.flag_c);
	flip_short(&scenario.flag_d);
	flip_short(&scenario.flag_e);
	flip_short(&scenario.flag_f);
	flip_short(&scenario.flag_g);
	flip_short(&scenario.flag_h);
	flip_short(&scenario.flag_i);
	flip_short(&scenario.intro_mess_pic);
	flip_short(&scenario.intro_mess_len);
	flip_short(&scenario.which_town_start);
	for (i = 0; i < 200; i++)
		for (j = 0; j < 5; j++)
			flip_short(&scenario.town_data_size[i][j]);
	for (i = 0; i < 10; i++)
		flip_short(&scenario.town_to_add_to[i]);
	for (i = 0; i < 10; i++)
		for (j = 0; j < 2; j++)
			flip_short(&scenario.flag_to_add_to_town[i][j]);
	for (i = 0; i < 100; i++)
		for (j = 0; j < 2; j++)
			flip_short(&scenario.out_data_size[i][j]);
	for (i = 0; i < 3; i++)
		flip_rect(&scenario.store_item_rects[i]);
	for (i = 0; i < 3; i++)
		flip_short(&scenario.store_item_towns[i]);
	for (i = 0; i < 50; i++)
		flip_short(&scenario.special_items[i]);
	for (i = 0; i < 50; i++)
		flip_short(&scenario.special_item_special[i]);
	flip_short(&scenario.rating);
	flip_short(&scenario.uses_custom_graphics);
	for (i = 0; i < 256; i++) {
		flip_short(&scenario.scen_monsters[i].health);
		flip_short(&scenario.scen_monsters[i].m_health);
		flip_short(&scenario.scen_monsters[i].max_mp);
		flip_short(&scenario.scen_monsters[i].mp);
		flip_short(&scenario.scen_monsters[i].a[1]);
		flip_short(&scenario.scen_monsters[i].a[0]);
		flip_short(&scenario.scen_monsters[i].a[2]);
		flip_short(&scenario.scen_monsters[i].morale);
		flip_short(&scenario.scen_monsters[i].m_morale);
		flip_short(&scenario.scen_monsters[i].corpse_item);
		flip_short(&scenario.scen_monsters[i].corpse_item_chance);
		flip_short(&scenario.scen_monsters[i].picture_num);
		}

	for (i = 0; i < 256; i++) {
		flip_short(&scenario.ter_types[i].picture);
		}
	for (i = 0; i < 30; i++) {
		flip_short(&scenario.scen_boats[i].which_town);
		}
	for (i = 0; i < 30; i++) {
		flip_short(&scenario.scen_horses[i].which_town);
		}
	for (i = 0; i < 20; i++) 
		flip_short(&scenario.scenario_timer_times[i]);
	for (i = 0; i < 20; i++) 
		flip_short(&scenario.scenario_timer_specs[i]);
	for (i = 0; i < 256; i++) {
		flip_spec_node(&scenario.scen_specials[i]);
		}
	for (i = 0; i < 10; i++)  {
		flip_short(&scenario.storage_shortcuts[i].ter_type);
		flip_short(&scenario.storage_shortcuts[i].property);
		for (j = 0; j < 10; j++)  {
			flip_short(&scenario.storage_shortcuts[i].item_num[j]);
			flip_short(&scenario.storage_shortcuts[i].item_odds[j]);
			}
		}
	flip_short(&scenario.last_town_edited);
}


void port_item_list()
{
	short i;
	
	if (cur_scen_is_mac == TRUE)
		return;
	
	for (i = 0; i < 400; i++) {
		flip_short(&(data_store->scen_item_list.scen_items[i].variety));
		flip_short(&(data_store->scen_item_list.scen_items[i].item_level));
		flip_short(&(data_store->scen_item_list.scen_items[i].value));
		}
}

void port_out(outdoor_record_type *out)
{
	short i;
	
	if (cur_scen_is_mac == TRUE)
		return;
		
	for (i = 0; i < 4; i++) {
		flip_short(&(out->wandering[i].spec_on_meet));
		flip_short(&(out->wandering[i].spec_on_win));
		flip_short(&(out->wandering[i].spec_on_flee));
		flip_short(&(out->wandering[i].cant_flee));
		flip_short(&(out->wandering[i].end_spec1));
		flip_short(&(out->wandering[i].end_spec2));
		flip_short(&(out->special_enc[i].spec_on_meet));
		flip_short(&(out->special_enc[i].spec_on_win));
		flip_short(&(out->special_enc[i].spec_on_flee));
		flip_short(&(out->special_enc[i].cant_flee));
		flip_short(&(out->special_enc[i].end_spec1));
		flip_short(&(out->special_enc[i].end_spec2));	
		}
	for (i = 0; i < 8; i++) 
		flip_rect(&(out->info_rect[i]));
	for (i = 0; i < 60; i++) 
		flip_spec_node(&(out->specials[i]));
}

void flip_spec_node(special_node_type *spec)
{
	flip_short(&(spec->type));
	flip_short(&(spec->sd1));
	flip_short(&(spec->sd2));
	flip_short(&(spec->pic));
	flip_short(&(spec->m1));
	flip_short(&(spec->m2));
	flip_short(&(spec->ex1a));
	flip_short(&(spec->ex1b));
	flip_short(&(spec->ex2a));
	flip_short(&(spec->ex2b));
	flip_short(&(spec->jumpto));
}

void flip_short(short *s)
{
	char store,*s1, *s2;

	s1 = (char *) s;
	s2 = s1 + 1;
	store = *s1;
	*s1 = *s2;
	*s2 = store;

}
void alter_rect(Rect *r)
{
	short a;

	a = r->top;
	r->top = r->left;
	r->left = a;
	a = r->bottom;
	r->bottom = r->right;
	r->right = a;
}

void flip_rect(Rect *s)
{
	flip_short(&(s->top));
	flip_short(&(s->bottom));
	flip_short(&(s->left));
	flip_short(&(s->right));
	alter_rect(s);
}
