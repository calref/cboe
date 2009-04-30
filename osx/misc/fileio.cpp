/*
 *  fileio.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include <string.h>
#include <string>
//#include "scen.global.h"
#include <iostream>
#include "classes.h"
//#include "scen.fileio.h"
//#include "scen.keydlgs.h"
#include "graphtool.h"
//#include "scen.core.h"
//#include "soundtool.h"
#include "mathutil.h"
#include "fileio.h"
#include "porting.h"
#include "dlgutil.h"

extern bool cur_scen_is_mac, mac_is_intel;
extern cScenario scenario;
extern GWorldPtr spec_scen_g;
extern cUniverse univ;
//extern unsigned char borders[4][50];
//extern cOutdoors current_terrain;
//extern cTown* town;
extern std::string progDir;
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
		NULL,//CFSTR("scenname.exs"),
		NULL,//CFSTR("Select a location to save the scenario:"),
		0,
		NULL,
		kWindowModalityAppModal,
		NULL
	};
	err = NavCreatePutFileDialog (&opts, 'BlEd', 'BETM', NULL, NULL, &dlg_put_scen);
	err = NavCreatePutFileDialog (&opts, 'blx!', 'beSV', NULL, NULL, &dlg_put_game);
	err = NavCreateGetFileDialog (&opts, NULL, NULL, NULL, scen_file_filter, NULL, &dlg_get_scen);
	err = NavCreateGetFileDialog (&opts, NULL, NULL, NULL, party_file_filter, NULL, &dlg_get_game);
}

FSSpecPtr nav_get_scenario(){
	NavReplyRecord s_reply;
	AEKeyword keyword;
	DescType descType;
	Size actualSize;
	FSSpecPtr file_to_load;
	
	NavGetFile(NULL,&s_reply,NULL,NULL,NULL,NULL,NULL,NULL);
	if (s_reply.validRecord == false)
		return NULL;
	AEGetNthPtr(&s_reply.selection,1,typeFSS,&keyword,&descType,file_to_load,sizeof(FSSpec),&actualSize);
	
	return file_to_load;
}

FSSpecPtr nav_put_scenario(){
	
}

FSSpecPtr nav_get_party(){
	NavReplyRecord reply;
	NavTypeList type_list; // To be able to have more than one type, this wouldn't do.
	FSSpecPtr file_to_load;
	
	type_list.componentSignature = kNavGenericSignature;
	type_list.osTypeCount = 1;
	type_list.osType[0] = 'beSV';
	
	NavTypeListPtr type_list_p = &type_list;
	
	NavChooseFile(NULL,&reply,NULL,NULL,NULL,NULL,&type_list_p,NULL);
	
	if (!reply.validRecord)
		return NULL;
	
	AEKeyword keyword;
	DescType descType;
	Size actualSize;
	
	AEGetNthPtr(&reply.selection,1,typeFSS,&keyword,&descType,file_to_load,sizeof(FSSpec),&actualSize);
	
	return file_to_load;
}
FSSpecPtr nav_put_party(){
	
}

bool load_scenario(FSSpec file_to_load){
	short i,j,k,l,file_id;
	bool file_ok = false;
	OSErr error;
	long len;
//	FSSpec file_to_load;
	legacy::scenario_data_type *temp_scenario = new legacy::scenario_data_type;
	legacy::scen_item_data_type *item_data = new legacy::scen_item_data_type;
	error = FSpOpenDF(&file_to_load,1,&file_id);
	if (error != 0) {
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
	  	file_ok = true;
	}
	else if ((scenario.format.flag1 == 20) && (scenario.format.flag2 == 40) &&
		(scenario.format.flag3 == 60) && (scenario.format.flag4 == 80)) {
	  	SysBeep(20);
	  	cur_scen_is_mac = false;
	  	file_ok = true;
	}
	if (file_ok == false) {
		FSClose(file_id); 
		give_error("This is not a legitimate Blades of Exile scenario.","",0);
		return false;
	}
	
	len = (long) sizeof(legacy::scenario_data_type);
	temp_scenario = new legacy::scenario_data_type;
	error = FSRead(file_id, &len, (char *) temp_scenario);
	if (error != 0){
		FSClose(file_id);
		oops_error(29);
		return false;
	}
	port_scenario(temp_scenario);
	len = sizeof(legacy::scen_item_data_type); // item data
	error = FSRead(file_id, &len, (char *) item_data);
	if (error != 0){
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
	 if (check_p(i) == true) {
	 user_given_password = i;
	 i = 30000;
	 }
	 
	 }
	 */
	
	/* Now check password
	 
	 if (check_p(0) == false) {
	 user_given_password = enter_password();
	 if (check_p(user_given_password) == false) {
	 fancy_choice_dialog(868,0);
	 if (overall_mode != 61) {
	 user_given_password = enter_password();
	 if (check_p(user_given_password) == false) 
	 ExitToShell();
	 }
	 else return;
	 }
	 }
	 else user_given_password = 0; /**/
	
	
	//given_password = user_given_password;
	
	scenario.ter_types[23].fly_over = false;
	delete temp_scenario;
	delete item_data;
	
	//store_file_reply = file_to_load;
//	overall_mode = 60;
//	change_made = false;
//	if(!load_town(scenario.last_town_edited)) return false;
//	//load_town(0);
//	if(!load_outdoors(scenario.last_out_edited,0)) return false;
//	augment_terrain(scenario.last_out_edited);
	load_spec_graphics();
	scenario.scen_file = file_to_load;
	return true;
}

long get_town_offset(short which_town){
	int i,j;
	long len_to_jump,store;
	
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
	
	return len_to_jump;
}

bool load_town(short which_town, cTown*& the_town){
	short i,j,k,file_id;
	long len,len_to_jump = 0,store;
	OSErr error;
	legacy::town_record_type store_town;
	legacy::talking_record_type store_talk;
	legacy::big_tr_type t_d;
	legacy::ave_tr_type ave_t;
	legacy::tiny_tr_type tiny_t;
	
	//	if (overall_mode == 61)
	//		return;
	
	if (which_town != minmax(0,scenario.num_towns - 1,which_town)) {
		// This should never be reached from the scenario editor,
		// because it does its own range checking before calling load_town.
		give_error("The scenario tried to place you into a non-existant town.","",0);
		return false;
	}
	
	len_to_jump = get_town_offset(which_town);
	
	if ((error = FSpOpenDF(&scenario.scen_file,1,&file_id)) != 0) {
		oops_error(34);
		return false;
	}
	
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
	
	if(the_town != NULL) delete the_town;
	switch (scenario.town_size[which_town]) {
		case 0:
			len =  sizeof(legacy::big_tr_type);
			FSRead(file_id, &len, (char *) &t_d);
			port_t_d(&t_d);
			the_town = new cBigTown;
			*the_town = store_town;
			the_town->append(t_d);
			break;
			
		case 1:
			len = sizeof(legacy::ave_tr_type);
			FSRead(file_id, &len, (char *) &ave_t);
			port_ave_t(&ave_t);
			the_town = new cMedTown;
			*the_town = store_town;
			the_town->append(ave_t);
			break;
			
		case 2:
			len = sizeof(legacy::tiny_tr_type);
			FSRead(file_id,&len , (char *) &tiny_t);
			port_tiny_t(&tiny_t);
			the_town = new cTinyTown;
			*the_town = store_town;
			the_town->append(tiny_t);
			break;
	}
	
	for (i = 0; i < 140; i++) {
		len = (long) (the_town->strlens[i]);
		FSRead(file_id, &len, (char *) &(the_town->town_strs(i)));
		the_town->town_strs(i)[len] = 0;
	}
	
	len = sizeof(legacy::talking_record_type);
	error = FSRead(file_id, &len , (char *) &store_talk);
	if (error != 0) {
		FSClose(file_id);
		oops_error(37);
		return false;
	}
	port_talk_nodes(&store_talk);
	the_town->talking = store_talk;
	
	for (i = 0; i < 170; i++) {
		len = (long) (the_town->talking.strlens[i]);
		FSRead(file_id, &len, (char *) &(the_town->talking.talk_strs[i]));
		the_town->talking.talk_strs[i][len] = 0;
	}
	
	//	town_type = scenario.town_size[which_town];
	error = FSClose(file_id);
	if (error != 0) {
		FSClose(file_id);
		oops_error(38);
	}
	
	return true;
}

bool load_town(short which_town, cSpeech& the_talk){
	short i,j,k,file_id;
	long len,len_to_jump = 0,store;
	OSErr error;
	legacy::town_record_type store_town;
	legacy::talking_record_type store_talk;
	legacy::big_tr_type t_d;
	legacy::ave_tr_type ave_t;
	legacy::tiny_tr_type tiny_t;
	
	//	if (overall_mode == 61)
	//		return;
	
	if (which_town != minmax(0,scenario.num_towns - 1,which_town)) {
		// This should never be reached from the scenario editor,
		// because it does its own range checking before calling load_town.
		give_error("The scenario tried to place you into a non-existant town.","",0);
		return false;
	}
	
	len_to_jump = get_town_offset(which_town);
	
	if ((error = FSpOpenDF(&scenario.scen_file,1,&file_id)) != 0) {
		oops_error(34);
		return false;
	}
	
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
			break;
		case 1:
			len = sizeof(legacy::ave_tr_type);
			break;
		case 2:
			len = sizeof(legacy::tiny_tr_type);
			break;
	}
	error = SetFPos (file_id, 3, len);
	
	for (i = 0; i < 140; i++) {
		len = (long) (store_town.strlens[i]);
		SetFPos (file_id, 3, len);
	}
	
	len = sizeof(legacy::talking_record_type);
	error = FSRead(file_id, &len , (char *) &store_talk);
	if (error != 0) {
		FSClose(file_id);
		oops_error(37);
		return false;
	}
	port_talk_nodes(&store_talk);
	the_talk = store_talk;
	
	for (i = 0; i < 170; i++) {
		len = (long) (the_talk.strlens[i]);
		FSRead(file_id, &len, (char *) &(the_talk.talk_strs[i]));
		the_talk.talk_strs[i][len] = 0;
	}
	
	//	town_type = scenario.town_size[which_town];
	error = FSClose(file_id);
	if (error != 0) {
		FSClose(file_id);
		oops_error(38);
	}
	
	return true;
}

bool load_town_str(short which_town, short which_str, char* str){
	short i,j,k,file_id;
	long len,len_to_jump = 0,store;
	OSErr error;
	legacy::town_record_type store_town;
	legacy::talking_record_type store_talk;
	legacy::big_tr_type t_d;
	legacy::ave_tr_type ave_t;
	legacy::tiny_tr_type tiny_t;
	
	//	if (overall_mode == 61)
	//		return;
	
	len_to_jump = get_town_offset(which_town);
	
	if ((error = FSpOpenDF(&scenario.scen_file,1,&file_id)) != 0) {
		oops_error(34);
		return false;
	}
	
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
			break;
		case 1:
			len = sizeof(legacy::ave_tr_type);
			break;
		case 2:
			len = sizeof(legacy::tiny_tr_type);
			break;
	}
	error = SetFPos (file_id, 3, len);
	
	for (i = 0; i < 140; i++) {
		len = (long) (univ.town->strlens[i]);
		if(i == which_str){
			FSRead(file_id, &len, (char *) str);
			str[len] = 0;
		}
		else SetFPos (file_id, 3, len);
	}
	
	//	town_type = scenario.town_size[which_town];
	error = FSClose(file_id);
	if (error != 0) {
		FSClose(file_id);
		oops_error(38);
	}
	
	return true;
}

bool load_town_str(short which_town, cTown*& t){
	short i,j,k,file_id;
	long len,len_to_jump = 0,store;
	OSErr error;
	legacy::town_record_type store_town;
	legacy::talking_record_type store_talk;
	legacy::big_tr_type t_d;
	legacy::ave_tr_type ave_t;
	legacy::tiny_tr_type tiny_t;
	
	//	if (overall_mode == 61)
	//		return;
	
	len_to_jump = get_town_offset(which_town);
	
	if ((error = FSpOpenDF(&scenario.scen_file,1,&file_id)) != 0) {
		oops_error(34);
		return false;
	}
	
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
			break;
		case 1:
			len = sizeof(legacy::ave_tr_type);
			break;
		case 2:
			len = sizeof(legacy::tiny_tr_type);
			break;
	}
	error = SetFPos (file_id, 3, len);
	
	for (i = 0; i < 140; i++) {
		len = (long) (t->strlens[i]);
		FSRead(file_id, &len, (char *) &(t->town_strs(i)));
		t->town_strs(i)[len] = 0;
	}
	
	//	town_type = scenario.town_size[which_town];
	error = FSClose(file_id);
	if (error != 0) {
		FSClose(file_id);
		oops_error(38);
	}
	
	return true;
}

long get_outdoors_offset(location& which_out){
	int i,j,out_sec_num;
	long len_to_jump,store;
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
	
	return len_to_jump;
}

//mode -> 0 - primary load  1 - add to top  2 - right  3 - bottom  4 - left
bool load_outdoors(location which_out,cOutdoors& the_out){
	short i,j,k,file_id;
	long len,len_to_jump,store;
	short out_sec_num;
	legacy::outdoor_record_type store_out;
	OSErr error;
	
//	if (overall_mode == 61)
//		return;
	
	if ((which_out.x != minmax(0,scenario.out_width - 1,which_out.x)) ||
		(which_out.y != minmax(0,scenario.out_height - 1,which_out.y))) {
		the_out = cOutdoors();
		return true;
	}
	
	error = FSpOpenDF(&scenario.scen_file,1,&file_id);
	if (error != 0) {
		oops_error(31);
		return false;
	}
	
	len_to_jump = get_outdoors_offset(which_out);
	
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
	
	port_out(&store_out);
	the_out = store_out;
	for (i = 0; i < 108; i++) {
		len = (long) (the_out.strlens[i]);
		FSRead(file_id, &len, (char *) &(the_out.out_strs(i)));
		the_out.out_strs(i)[len] = 0;
	}
	
	error = FSClose(file_id);
	if (error != 0) {
		FSClose(file_id);
		oops_error(33);
	}
	return true;
}

bool load_outdoors(location which_out, short mode, unsigned char borders[4][50]){
	short i,j,k,file_id;
	long len,len_to_jump,store;
	short out_sec_num;
	legacy::outdoor_record_type store_out;
	OSErr error;
	
	error = FSpOpenDF(&scenario.scen_file,1,&file_id);
	if (error != 0) {
		oops_error(31);
		return false;
	}
	
	len_to_jump = get_outdoors_offset(which_out);
	
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

bool load_outdoor_str(location which_out, short which_str, char* str){
	short i,j,k,file_id;
	long len,len_to_jump,store;
	short out_sec_num;
	legacy::outdoor_record_type store_out;
	OSErr error;
	
	error = FSpOpenDF(&scenario.scen_file,1,&file_id);
	if (error != 0) {
		oops_error(31);
		return false;
	}
	
	len_to_jump = get_outdoors_offset(which_out);
	
	error = SetFPos (file_id, 1, len_to_jump);
	if (error != 0) {
		FSClose(file_id);
		oops_error(32);
		return false;
	}
	
	error = FSRead(file_id, &len, (char *) &store_out);
	
	for (i = 0; i < 120; i++) {
		len = (long) (store_out.strlens[i]);
		if (i == which_str) {
			FSRead(file_id, &len, (char *) str);
			str[len] = 0;
		}
		else SetFPos (file_id, 3, len);	
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

bool load_party(FSSpec file_to_load){
	long file_size;
	OSErr error;
	short file_id,i,j,k;
	bool town_restore = false;
	bool maps_there = false;
	bool in_scen = false;
	bool new_format = false;
	
	char flag_data[8];
	
	long len,store_len,count;
	char *party_ptr;
	char *pc_ptr;
//	flag_type flag;
//	flag_type *flag_ptr;
	short flag;
	legacy::party_record_type store_party;
	legacy::setup_save_type store_setup;
	legacy::pc_record_type store_pc[6];
	legacy::out_info_type store_out_info;
	legacy::current_town_type store_c_town;
	legacy::big_tr_type t_d;
	legacy::town_item_list t_i;
	legacy::stored_items_list_type stored_items[3];
	legacy::stored_town_maps_type town_maps;
	legacy::stored_outdoor_maps_type o_maps;
	unsigned char misc_i[64][64], sfx[64][64];
	
	static const short flags[3][2] = {
		{5790,1342}, // slot 0 ... 5790 - out,         1342 - town
		{100,200},   // slot 1 ... 100  - in scenario, 200  - not in
		{3422,5567}  // slot 2 ... 3422 - no maps,     5567 - maps
	};
	// but if the first flag is 0x0B0E, we have a new-format save
	// the three flags still follow that.
	error = FSpOpenDF(&file_to_load,1,&file_id);
	if (error != 0) {
		FCD(1064,0);
		SysBeep(2);
		return false;
	}		
	
	
	file_size = sizeof(legacy::party_record_type); // 45368
	
	len = sizeof(short); // 2
	
	//	sprintf((char *) debug, "  Len %d               ", (short) len);
	//	add_string_to_buf((char *) debug);
	
	for (i = 0; i < 3; i++) {
		error = FSRead(file_id, &len, (char *) &flag);
		if (error != 0) {
			FSClose(file_id);
			FCD(1064,0);
			return false;
		}
//		flag_ptr = (flag_type *) flag_data;
//		flag = *flag_ptr;
		if (flag == 0x0B0E) { // new format save
			new_format = true;
			i--;
			continue;
		}
		if(mac_is_intel && !new_format) flip_short(&flag); // because the old format save was made for non-intel macs
		if ((flag != flags[i][0]) && (flag != flags[i][1])) { // OK Exile II save file?
			FSClose(file_id);
			FCD(1063,0);
			return false;
		} 
		
		if ((i == 0) && (flag == flags[i][1]))
			town_restore = true;
		if ((i == 1) && (flag == flags[i][0])) {
			in_scen = true;
		}
		if ((i == 2) && (flag == flags[i][1]))
			maps_there = true;
	}
	
	// LOAD PARTY	
	len = (long) sizeof(legacy::party_record_type); // 45368
	store_len = len;
	if ((error = FSRead(file_id, &len, (char *) &store_party)) != 0){
		FSClose(file_id);
		SysBeep(2);
		FCD(1064,0);
		return false;
	}
	port_party(&store_party);
	party_ptr = (char*) &store_party;
	for (count = 0; count < store_len; count++)
		party_ptr[count] ^= 0x5C;	
	
	// LOAD SETUP
	len = (long) sizeof(legacy::setup_save_type);
	if ((error = FSRead(file_id, &len, (char *) &store_setup)) != 0){
		FSClose(file_id);
		SysBeep(2);
		FCD(1064,0);
		return false;
	}
	
	// LOAD PCS
	store_len = (long) sizeof(legacy::pc_record_type);
	for (i = 0; i < 6; i++) {
		len = store_len;
		error = FSRead(file_id, &len, (char *) &store_pc[i]);
		if (error  != 0){
			FSClose(file_id);
			SysBeep(2);
			FCD(1064,0);
			return false;
		}
		port_pc(&store_pc[i]);
		pc_ptr = (char*) &store_pc[i];
		for (count = 0; count < store_len; count++)
			pc_ptr[count] ^= 0x6B;	
	}
	
	if (in_scen) {
		
		// LOAD OUTDOOR MAP
		len = (long) sizeof(legacy::out_info_type);
		if ((error = FSRead(file_id, &len, (char *) &store_out_info)) != 0){
			FSClose(file_id);
			SysBeep(2);
			FCD(1064,0);
			return false;
		}
		
		if(univ.town.loaded()) univ.town.unload();
		
		// LOAD TOWN 
		if (town_restore) {
			len = (long) sizeof(legacy::current_town_type);
			if ((error = FSRead(file_id, &len, (char *) &store_c_town)) != 0){
				FSClose(file_id);
				SysBeep(2);
				FCD(1064,0);
				return false;
			}
			port_c_town(&store_c_town);
			
			len = (long) sizeof(legacy::big_tr_type);
			if ((error = FSRead(file_id, &len, (char *) &t_d)) != 0){
				FSClose(file_id);
				SysBeep(2);
				FCD(1064,0);
				return false;
			}
			port_t_d(&t_d);
			
			len = (long) sizeof(legacy::town_item_list);
			if ((error = FSRead(file_id, &len, (char *) &t_i))  != 0){
				FSClose(file_id);
				SysBeep(2);
				FCD(1064,0);
				return false;
			}
			
		}else univ.town.num = 200;
		
		// LOAD STORED ITEMS
		for (i = 0; i < 3; i++) {
			len = (long) sizeof(legacy::stored_items_list_type);
			if ((error = FSRead(file_id, &len, (char *) &stored_items[i]))  != 0){
				FSClose(file_id);
				SysBeep(2);
				FCD(1064,0);
				return false;
			}
		}
		
		// LOAD SAVED MAPS
		if (maps_there) {
			len = (long) sizeof(legacy::stored_town_maps_type);
			if ((error = FSRead(file_id, &len, (char *) &(town_maps)))  != 0){
				FSClose(file_id);
				SysBeep(2);
				FCD(1064,0);
				return false;
			}
			
			len = (long) sizeof(legacy::stored_outdoor_maps_type);
			if ((error = FSRead(file_id, &len, (char *) &o_maps)) != 0) {
				FSClose(file_id);
				SysBeep(2);
				FCD(1064,0);
				return false;
			}	
		}
		
		// LOAD SFX & MISC_I
		len = (long) (64 * 64);
		if ((error = FSRead(file_id, &len, (char *) sfx))  != 0){
			FSClose(file_id);
			SysBeep(2);
			FCD(1064,0);
			return false;
		}	
		if ((error = FSRead(file_id, &len, (char *) misc_i))  != 0){
			FSClose(file_id);
			SysBeep(2);
			FCD(1064,0);
			return false;
		}	
		
	} // end if_scen
	
	if ((error = FSClose(file_id))  != 0){
		give_error("Load: Can't close file.","",0);
		SysBeep(2);
		return false;
	}
	
	univ.party = store_party;
	univ.party.append(store_setup);
	for(i = 0; i < 6; i++)
		univ.party.add_pc(store_pc[i]);
	if(in_scen){
		univ.out.append(store_out_info);
		if(town_restore){
			univ.town.append(store_c_town,scenario.town_size[univ.town.num]);
			univ.town.append(t_d);
			univ.town.append(t_i);
		}
		for(i = 0; i < 3; i++)
			univ.party.append(stored_items[i],i);
		univ.append(town_maps);
		univ.append(o_maps);
		univ.town.append(sfx, misc_i);
	}
	if(in_scen){
		FSRef file_ref;
		FSSpec file_spec;
		std::string path;
		path = progDir + "/Blades of Exile Scenarios/" + univ.party.scen_name;
		std::cout<<path<<'/n';
		FSPathMakeRef((UInt8*) path.c_str(), &file_ref, NULL);
		FSGetCatalogInfo(&file_ref, kFSCatInfoNone, NULL, NULL, &file_spec, NULL);
		if (!load_scenario(file_spec))
			return false;
		univ.file = file_spec;
	}else{
		univ.party.scen_name = "";
	}
	
	// Compatibility flags
	if(in_scen && scenario.format.prog_make_ver[0] < 2){
		univ.party.stuff_done[305][8] = 1;
	} else {
		univ.party.stuff_done[305][8] = 0;
	}
	
	return true;
}

void save_party(FSSpec dest_file)
//mode;  // 0 - normal  1 - save as
{
	printf("Saving is currently disabled.\n");
//	NavReplyRecord reply;
//	OSErr error;
//	short file_id;
//	bool town_save = false;
//	Str63 store_name;
//	FSSpec to_load;
//	
//	short i;
//	
//	long len,store_len,count;
//	flag_type flag;
//	flag_type *store;
//	party_record_type *party_ptr;
//	setup_save_type	*setup_ptr;
//	pc_record_type *pc_ptr;
//	//	out_info_type store_explored;
//	current_town_type *town_ptr;
//	big_tr_type *town_data_ptr;
//	town_item_list *item_ptr;
//	stored_items_list_type *items_ptr;
//	stored_town_maps_type *maps_ptr; 
//	stored_outdoor_maps_type *o_maps_ptr; 
//	
//	char *party_encryptor;	
//	
//	if ((in_startup_mode == false) && (is_town()))
//		town_save = true;
//	
//	strcpy ((char *) store_name, (char *) last_load_file);
//	
//	
//	if ((mode == 1) || (loaded_yet == false)) {
//		NavPutFile(NULL,&reply,NULL,NULL,'beSV','blx!',NULL);
//		if (reply.validRecord == false)
//			return;
//		
//		AEKeyword keyword;
//		DescType descType;
//		Size actualSize;
//		
//		AEGetNthPtr(&reply.selection,1,typeFSS,&keyword,&descType,&to_load,sizeof(FSSpec),&actualSize);
//		loaded_yet = true;
//	}
//	else to_load = store_file_reply;
//	
//	error = FSpCreate(&to_load,'blx!','beSV',reply.keyScript);
//	if ((error != 0) && (error != dupFNErr)) {
//		add_string_to_buf("Save: Couldn't create file.         ");
//		SysBeep(2);
//		return;
//	}
//	if ((error = FSpOpenDF(&to_load,3,&file_id)) != 0) {
//		add_string_to_buf("Save: Couldn't open file.         ");
//		SysBeep(2);
//		return;
//	}			
//	strcpy ((char *) last_load_file, (char *) to_load.name);
//	store_file_reply = to_load;
//	
//	
//	store = &flag;	
//	
//	len = sizeof(flag_type);
//	
//	flag.i = (town_save == true) ? 1342 : 5790;
//	if ((error = FSWrite(file_id, &len, (char *) store))  != 0){
//		add_string_to_buf("Save: Couldn't write to file.         ");
//		FSClose(file_id);
//		SysBeep(2);
//	}
//	flag.i = (in_startup_mode == false) ? 100 : 200;
//	if ((error = FSWrite(file_id, &len, (char *) store)) != 0) {
//		add_string_to_buf("Save: Couldn't write to file.         ");
//		FSClose(file_id);
//		SysBeep(2);
//		return;
//	}
//	flag.i = (save_maps == true) ? 5567 : 3422;
//	if ((error = FSWrite(file_id, &len, (char *) store))  != 0){
//		add_string_to_buf("Save: Couldn't write to file.         ");
//		FSClose(file_id);
//		SysBeep(2); 
//		return;
//	}
//	
//	// SAVE PARTY
//	party_ptr = &party;	
//	len = sizeof(party_record_type);
//	
//	store_len = len;
//	party_encryptor = (char *) party_ptr;
//	for (count = 0; count < store_len; count++)
//		party_encryptor[count] ^= 0x5C;
//	if ((error = FSWrite(file_id, &len, (char *) party_ptr)) != 0) {
//		add_string_to_buf("Save: Couldn't write to file.         ");
//		FSClose(file_id);
//		for (count = 0; count < store_len; count++)
//			party_encryptor[count] ^= 0x5C;
//		SysBeep(2); 
//		return;
//	}
//	for (count = 0; count < store_len; count++)
//		party_encryptor[count] ^= 0x5C;
//	
//	// SAVE SETUP
//	setup_ptr = &setup_save;	
//	len = sizeof(setup_save_type);
//	if ((error = FSWrite(file_id, &len, (char *) setup_ptr))  != 0){
//		add_string_to_buf("Save: Couldn't write to file.         ");
//		FSClose(file_id);
//		SysBeep(2); 
//		return;
//	}
//	
//	// SAVE PCS	
//	store_len = sizeof(pc_record_type);
//	for (i = 0; i < 6; i++) {
//		pc_ptr = &adven[i];	
//		
//		len = store_len;
//		party_encryptor = (char *) pc_ptr;
//		for (count = 0; count < store_len; count++)
//			party_encryptor[count] ^= 0x6B;
//		if ((error = FSWrite(file_id, &len, (char *) pc_ptr))  != 0){
//			add_string_to_buf("Save: Couldn't write to file.         ");
//			FSClose(file_id);
//			for (count = 0; count < store_len; count++)
//				party_encryptor[count] ^= 0x6B;
//			SysBeep(2); 
//			return;
//		}
//		for (count = 0; count < store_len; count++)
//			party_encryptor[count] ^= 0x6B;
//	}
//	
//	if (in_startup_mode == false) {
//		
//		// SAVE OUT DATA
//		len = sizeof(out_info_type);
//		if ((error = FSWrite(file_id, &len, (char *) out_e)) != 0) {
//			add_string_to_buf("Save: Couldn't write to file.         ");
//			FSClose(file_id);
//			SysBeep(2); 
//			return;
//		}
//		
//		if (town_save == true) {	
//			town_ptr = &c_town;	
//			len = sizeof(current_town_type);
//			if ((error = FSWrite(file_id, &len, (char *) town_ptr)) != 0) {
//				add_string_to_buf("Save: Couldn't write to file.         ");
//				FSClose(file_id);
//				SysBeep(2); 
//				return;
//			}
//			town_data_ptr = &t_d;	
//			len = sizeof(big_tr_type);
//			if ((error = FSWrite(file_id, &len, (char *) town_data_ptr)) != 0) {
//				add_string_to_buf("Save: Couldn't write to file.         ");
//				FSClose(file_id);
//				SysBeep(2); 
//				return;
//			}
//			item_ptr = &t_i;	
//			len = sizeof(town_item_list);
//			if ((error = FSWrite(file_id, &len, (char *) item_ptr)) != 0) {
//				add_string_to_buf("Save: Couldn't write to file.         ");
//				FSClose(file_id);
//				SysBeep(2); 
//				return;
//			}	
//		}
//		
//		// Save stored items 
//		for (i = 0; i < 3; i++) {
//			items_ptr = &stored_items[i];
//			len = (long) sizeof(stored_items_list_type);
//			if ((error = FSWrite(file_id, &len, (char *) items_ptr))  != 0){
//				add_string_to_buf("Save: Couldn't write to file.         ");
//				FSClose(file_id);
//				SysBeep(2); 
//				return;
//			}
//		}
//		
//		// If saving maps, save maps
//		if (save_maps == true) {
//			maps_ptr = &(town_maps);
//			len = (long) sizeof(stored_town_maps_type);
//			if ((error = FSWrite(file_id, &len, (char *) maps_ptr))  != 0){
//				add_string_to_buf("Save: Couldn't write to file.         ");
//				FSClose(file_id);
//				SysBeep(2); 
//				return;
//			}	
//			
//			o_maps_ptr = &o_maps;
//			len = (long) sizeof(stored_outdoor_maps_type);
//			if ((error = FSWrite(file_id, &len, (char *) o_maps_ptr)) != 0) {
//				add_string_to_buf("Save: Couldn't write to file.         ");
//				FSClose(file_id);
//				SysBeep(2); 
//				return;
//			}
//		} 
//		
//		// SAVE SFX and MISC_I
//		len = (long) (64 * 64);
//		if ((error = FSWrite(file_id, &len, (char *) sfx))  != 0){
//			add_string_to_buf("Save: Couldn't write to file.         ");
//			FSClose(file_id);
//			SysBeep(2); 
//			return;
//		}
//		if ((error = FSWrite(file_id, &len, (char *) misc_i))  != 0){
//			add_string_to_buf("Save: Couldn't write to file.         ");
//			FSClose(file_id);
//			SysBeep(2); 
//			return;
//		}
//		
//		
//		
//	}
//	
//	if ((error = FSClose(file_id)) != 0) {
//		add_string_to_buf("Save: Couldn't close file.         ");
//		SysBeep(2);
//		return;
//	}
//	if (in_startup_mode == false)
//		add_string_to_buf("Save: Game saved.              ");
}