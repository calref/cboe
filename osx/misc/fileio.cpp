/*
 *  fileio.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include <string.h>
//#include "scen.global.h"
#include <stdio.h>
#include "scenario.h"
//#include "scen.fileio.h"
//#include "scen.keydlgs.h"
#include "graphtool.h"
//#include "scen.core.h"
//#include "soundtool.h"
//#include "mathutil.h"
#include "fileio.h"
#include "porting.h"
#include "dlgutil.h"

extern bool cur_scen_is_mac, mac_is_intel;
extern cScenario scenario;
extern unsigned char borders[4][50];
extern cOutdoors current_terrain;
extern cTown* town;
extern GWorldPtr spec_scen_g;
//extern short overall_mode;

NavDialogRef dlg_get_scen,dlg_get_game,dlg_put_scen,dlg_put_game;

Boolean scen_file_filter(AEDesc* item, void* info, void * dummy, NavFilterModes filterMode){
	if(item->descriptorType == typeFSRef){
		OSErr err;
		FSRef the_file;
		FSCatalogInfo cat_info;
		FSSpec file_spec;
		AEGetDescData(item,&the_file,AEGetDescDataSize(item));
		err = FSGetCatalogInfo (&the_file,kFSCatInfoFinderInfo,&cat_info,NULL,&file_spec,NULL);
		if(err != noErr) return false;
		FileInfo* file_info = (FileInfo*) &cat_info.finderInfo;
		if(file_info->fileType == 'BETM') return true;
		for(int i = 0; i < 64; i++){
			if(file_spec.name[i] == '.')
				if(file_spec.name[i + 1] == 'e' || file_spec.name[i + 1] == 'E')
					if(file_spec.name[i + 1] == 'x' || file_spec.name[i + 1] == 'X')
						if(file_spec.name[i + 1] == 's' || file_spec.name[i + 1] == 'S')
							return true;
		}
	}
	return false;
}

Boolean party_file_filter(AEDesc* item, void* info, void * dummy, NavFilterModes filterMode){
	if(item->descriptorType == typeFSRef){
		OSErr err;
		FSRef the_file;
		FSCatalogInfo cat_info;
		FSSpec file_spec;
		AEGetDescData(item,&the_file,AEGetDescDataSize(item));
		err = FSGetCatalogInfo (&the_file,kFSCatInfoFinderInfo,&cat_info,NULL,&file_spec,NULL);
		if(err != noErr) return false;
		FileInfo* file_info = (FileInfo*) &cat_info.finderInfo;
		if(file_info->fileType == 'beSV') return true;
		for(int i = 0; i < 64; i++){
			if(file_spec.name[i] == '.')
				if(file_spec.name[i + 1] == 'e' || file_spec.name[i + 1] == 'E')
					if(file_spec.name[i + 1] == 'x' || file_spec.name[i + 1] == 'X')
						if(file_spec.name[i + 1] == 'g' || file_spec.name[i + 1] == 'G')
							return true;
		}
	}
	return false;
}

void init_fileio(){
	OSErr err;
	NavDialogCreationOptions opts = {
		kNavDialogCreationOptionsVersion,
		kNavNoTypePopup,
		{-2,-2},
		CFSTR("com.spidweb.boesceneditor"),
		CFSTR("BoE Scenario Editor"),
		NULL,
		NULL,
		CFSTR("scenname.exs"),
		CFSTR("Select a location to save the scenario:"),
		0,
		NULL,
		kWindowModalityAppModal,
		NULL
	};
	err = NavCreateGetFileDialog (&opts, NULL, NULL, NULL, scen_file_filter, NULL, &dlg_get_scen);
	err = NavCreateGetFileDialog (&opts, NULL, NULL, NULL, party_file_filter, NULL, &dlg_get_game);
	err = NavCreatePutFileDialog (&opts, 'BlEd', 'BETM', NULL, NULL, &dlg_put_scen);
	err = NavCreatePutFileDialog (&opts, 'blx!', 'beSV', NULL, NULL, &dlg_put_game);
}

FSRef nav_get_scenario(){}
FSRef nav_put_scenario(){}
FSRef nav_get_party(){}
FSRef nav_put_party(){}

bool load_scenario(FSSpec file_to_load){
	short i,j,k,l,file_id;
	Boolean file_ok = FALSE;
	OSErr error;
	long len;
//	FSSpec file_to_load;
	legacy::scenario_data_type *temp_scenario;
	legacy::scen_item_data_type *item_data;
	
	if ((error = FSpOpenDF(&file_to_load,1,&file_id)) != 0) {
		oops_error(28);
		SysBeep(2);
		return false;
	}
	
	len = (long) sizeof(scenario_header_flags);
	
	if ((error = FSRead(file_id, &len, (char *) &scenario.format)) != 0){
		FSClose(file_id);
		oops_error(29);
		return false;
	}
	
	if ((scenario.format.flag1 == 10) && (scenario.format.flag2 == 20) &&
		(scenario.format.flag3 == 30) && (scenario.format.flag4 == 40)) {
	  	cur_scen_is_mac = true;
	  	file_ok = TRUE;
	}
	if ((scenario.format.flag1 == 20) && (scenario.format.flag2 == 40) &&
		(scenario.format.flag3 == 60) && (scenario.format.flag4 == 80)) {
	  	SysBeep(20);
	  	cur_scen_is_mac = FALSE;
	  	file_ok = TRUE;
	}
	if (file_ok == FALSE) {
		FSClose(file_id); 
		give_error("This is not a legitimate Blades of Exile scenario.","",0);
		return false;
	}
	
	len = (long) sizeof(legacy::scenario_data_type);
	temp_scenario = new legacy::scenario_data_type;
	
	if ((error = FSRead(file_id, &len, (char *) temp_scenario)) != 0){
		FSClose(file_id);
		oops_error(29);
		return false;
	}
	port_scenario(temp_scenario);
	len = sizeof(legacy::scen_item_data_type); // item data
	if ((error = FSRead(file_id, &len, (char *) item_data)) != 0){
		FSClose(file_id);
		oops_error(30);
		return false;
	}
	port_item_list(item_data);
	scenario = *temp_scenario;
	scenario.append(*item_data);
	for (i = 0; i < 270; i++) {
		len = (long) (scenario.scen_str_len[i]);
		FSRead(file_id, &len, (char *) &(scenario.scen_strs(i)));
		scenario.scen_strs(i)[len] = 0;
	}
	
	FSClose(file_id);
	
	// code for master
	/*
	 for (i = 0; i < 30000; i++) {
	 if (check_p(i) == TRUE) {
	 user_given_password = i;
	 i = 30000;
	 }
	 
	 }
	 */
	
	/* Now check password
	 
	 if (check_p(0) == FALSE) {
	 user_given_password = enter_password();
	 if (check_p(user_given_password) == FALSE) {
	 fancy_choice_dialog(868,0);
	 if (overall_mode != 61) {
	 user_given_password = enter_password();
	 if (check_p(user_given_password) == FALSE) 
	 ExitToShell();
	 }
	 else return;
	 }
	 }
	 else user_given_password = 0; /**/
	
	
	//given_password = user_given_password;
	
	scenario.ter_types[23].fly_over = FALSE;
	delete temp_scenario;
	
	//store_file_reply = file_to_load;
//	overall_mode = 60;
//	change_made = FALSE;
//	if(!load_town(scenario.last_town_edited)) return false;
//	//load_town(0);
//	if(!load_outdoors(scenario.last_out_edited,0)) return false;
//	augment_terrain(scenario.last_out_edited);
	load_spec_graphics();
	scenario.scen_file = file_to_load;
	return true;
}

bool load_town(short which_town)
{
	short i,j,k,file_id;
	long len,len_to_jump = 0,store;
	OSErr error;
	legacy::town_record_type store_town;
	legacy::talking_record_type store_talk;
	legacy::big_tr_type t_d;
	legacy::ave_tr_type ave_t;
	legacy::tiny_tr_type tiny_t;
	
	if(town != NULL) delete town;
	
//	if (overall_mode == 61)
//		return;
	
	if ((error = FSpOpenDF(&scenario.scen_file,1,&file_id)) != 0) {
		oops_error(34);
		return false;
	}	
	
	len_to_jump = sizeof(scenario_header_flags);
	len_to_jump += sizeof(legacy::scenario_data_type);
	len_to_jump += sizeof(legacy::scen_item_data_type);
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
	if (error != 0) {
		FSClose(file_id);
		oops_error(35);
		return false;
	}
	
	len = sizeof(legacy::town_record_type);
	
	error = FSRead(file_id, &len , (char *) &store_town);
	port_town(&store_town);
	if (error != 0) {
		FSClose(file_id);
		oops_error(36);
		return false;
	}
	
	switch (scenario.town_size[which_town]) {
		case 0:
			len =  sizeof(legacy::big_tr_type);
			FSRead(file_id, &len, (char *) &t_d);
			town = new cBigTown;
			*town = store_town;
			port_t_d(&t_d);
			town->append(t_d);
			break;
			
		case 1:
			len = sizeof(legacy::ave_tr_type);
			FSRead(file_id, &len, (char *) &ave_t);
			town = new cMedTown;
			*town = store_town;
			port_ave_t(&ave_t);
			town->append(ave_t);
			//				for (i = 0; i < 48; i++)
			//					for (j = 0; j < 48; j++) {
			//						town->terrain(i,j) = ave_t.terrain[i][j];
			//						town->lighting(i / 8,j) = ave_t.lighting[i / 8][j];					
			//						}
			//				for (i = 0; i < 16; i++) {
			//					town->room_rect(i) = ave_t.room_rect[i];
			//					}
			//				for (i = 0; i < 40; i++) {
			//					town->creatures(i) = ave_t.creatures[i];
			//					}
			//				for (i = 40; i < 60; i++) {
			//					town->creatures(i).number = 0;
			//					}
			break;
			
		case 2:
			len = sizeof(legacy::tiny_tr_type);
			FSRead(file_id,&len , (char *) &tiny_t);
			town = new cTinyTown;
			*town = store_town;
			port_tiny_t(&tiny_t);
			town->append(tiny_t);
			//			for (i = 0; i < 32; i++)
			//				for (j = 0; j < 32; j++) {
			//					town->terrain(i,j) = tiny_t.terrain[i][j];
			//					town->lighting(i / 8,j) = tiny_t.lighting[i / 8][j];					
			//					}
			//			for (i = 0; i < 16; i++) {
			//				town->room_rect(i) = tiny_t.room_rect[i];
			//				}
			//			for (i = 0; i < 30; i++) {
			//				town->creatures(i) = tiny_t.creatures[i];
			//				}
			//			for (i = 30; i < 60; i++) {
			//				town->creatures(i).number = 0;
			//				}
			break;
	}
	
	for (i = 0; i < 140; i++) {
		len = (long) (town->strlens[i]);
		FSRead(file_id, &len, (char *) &(town->town_strs(i)));
		town->town_strs(i)[len] = 0;
	}
	
	len = sizeof(legacy::talking_record_type);
	error = FSRead(file_id, &len , (char *) &store_talk);
	if (error != 0) {
		FSClose(file_id);
		oops_error(37);
		return false;
	}
	port_talk_nodes(&store_talk);
	town->talking = store_talk;
	
	for (i = 0; i < 170; i++) {
		len = (long) (town->talking.strlens[i]);
		FSRead(file_id, &len, (char *) &(town->talk_strs[i]));
		town->talk_strs[i][len] = 0;
	}
	
//	town_type = scenario.town_size[which_town];
	error = FSClose(file_id);
	if (error != 0) {
		FSClose(file_id);
		oops_error(38);
	}
	return true;
}

//mode -> 0 - primary load  1 - add to top  2 - right  3 - bottom  4 - left
bool load_outdoors(location which_out,short mode)
{
	short i,j,k,file_id;
	long len,len_to_jump,store;
	short out_sec_num;
	legacy::outdoor_record_type store_out;
	OSErr error;
	
//	if (overall_mode == 61)
//		return;
	
	if ((error = FSpOpenDF(&scenario.scen_file,1,&file_id)) != 0) {
		oops_error(31);
		return false;
	}
	
	out_sec_num = scenario.out_width * which_out.y + which_out.x;
	
	len_to_jump = sizeof(scenario_header_flags);
	len_to_jump += sizeof(legacy::scenario_data_type);
	len_to_jump += sizeof(legacy::scen_item_data_type);
	for (i = 0; i < 300; i++)
		len_to_jump += (long) scenario.scen_str_len[i];
	store = 0;
	for (i = 0; i < out_sec_num; i++)
		for (j = 0; j < 2; j++)
			store += (long) (scenario.out_data_size[i][j]);
	len_to_jump += store;
	
	error = SetFPos (file_id, 1, len_to_jump);
	if (error != 0) {
		FSClose(file_id);
		oops_error(32);
		return false;
	}
	
	len = sizeof(legacy::outdoor_record_type);
	error = FSRead(file_id, &len, (char *) &store_out);
	if (error != 0) {
		FSClose(file_id);
		oops_error(33);
		return false;
	}
	
	if (mode == 0) {
		port_out(&store_out);
		current_terrain = store_out;
		for (i = 0; i < 120; i++) {
			len = (long) (current_terrain.strlens[i]);
			FSRead(file_id, &len, (char *) &(current_terrain.out_strs(i)));
			current_terrain.out_strs(i)[len] = 0;
		}
		
	}
	
	
	if (mode == 1) 
		for (j = 0; j < 48; j++) {
			borders[0][j] = store_out.terrain[j][47];
		}
	if (mode == 2) 
		for (j = 0; j < 48; j++) {
			borders[1][j] = store_out.terrain[0][j];
		}
	if (mode == 3) 
		for (j = 0; j < 48; j++) {
			borders[2][j] = store_out.terrain[j][0];
		}
	if (mode == 4) 
		for (j = 0; j < 48; j++) {
			borders[3][j] = store_out.terrain[47][j];
		}
	
	
	error = FSClose(file_id);
	if (error != 0) {
		FSClose(file_id);
		oops_error(33);
	}
	return true;
}

void load_spec_graphics()
{
	short i,file_num;
	Str255 file_name;
	UInt8 path[256];
	FSRef file;
	ResFileRefNum custRef;
	OSErr err;
	char *whatever;
	if (spec_scen_g != NULL) {
		DisposeGWorld(spec_scen_g);
		spec_scen_g = NULL;
	}
	//build_scen_file_name(file_name);
	err = FSpMakeFSRef(&scenario.scen_file, &file);
	err = FSRefMakePath(&file, path, 255);
	printf("Loading scenario graphics... (%s)\n",(char*)path);
	//	for (i = 0; i < 63; i++) 
	//		file_name[i] = whatever[i];
	for (i = 0; i < 250; i++) {
		if (path[i] == '.') {
			path[i + 1] = 'm';
			path[i + 2] = 'e';
			path[i + 3] = 'g';
			//path[i + 4] = 0;
			break;
		}
	}
	err = FSPathMakeRef(path, &file, NULL);
	err = FSOpenResourceFile(&file, 0, NULL, fsRdPerm /*fsRdWrPerm*/, &custRef);
	//file_num = HOpen(file_to_load.vRefNum,file_to_load.parID,file_name,1);
	//if (file_num < 0){
	if(err != noErr){
		//whatever = (char *) file_to_load.name;
		printf("First attempt failed... (%s)\n",(char*)path);
		for (i = 0; i < 250; i++) {
			if (path[i] == '.') {
				path[i + 1] = 'b';
				path[i + 2] = 'm';
				path[i + 3] = 'p';
				//path[i + 4] = 0;
				break;
			}
		}
		err = FSPathMakeRef(path, &file, NULL);
		FSSpec spec;
		FSGetCatalogInfo(&file, kFSCatInfoNone, NULL, NULL, &spec, NULL);
		err = FSpOpenDF(&spec, fsRdPerm, &custRef);
		//spec_scen_g = load_bmp_from_file(file_name);
		spec_scen_g = importPictureFileToGWorld(&spec);
		if(spec_scen_g == NULL)printf("Scenario graphics not found (%s).\n",file_name);
		return;
	}
	
	spec_scen_g = load_pict(1);
	//CloseResFile(file_num);
	CloseResFile(custRef);
}