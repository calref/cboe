/*
 *  fileio.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include <zlib.h>
//#include "scen.global.h"
#include <fstream>
#include "classes.h"
//#include "scen.fileio.h"
//#include "scen.keydlgs.h"
#include "graphtool.h"
//#include "scen.core.h"
//#include "soundtool.h"
#include "mathutil.h"
#include "fileio.h"
#include "porting.h"
#include "dlogutil.h"
#include "restypes.hpp"

extern bool cur_scen_is_mac, mac_is_intel;
extern cScenario scenario;
extern sf::Texture items_gworld,tiny_obj_gworld,fields_gworld,roads_gworld,boom_gworld,missiles_gworld;
extern sf::Texture dlogpics_gworld,monst_gworld[],terrain_gworld[],anim_gworld,talkfaces_gworld,pc_gworld;
extern sf::Texture status_gworld, vehicle_gworld, small_ter_gworld;
extern cCustomGraphics spec_scen_g;
extern cUniverse univ;
//extern unsigned char borders[4][50];
//extern cOutdoors current_terrain;
//extern cTown* town;
extern fs::path progDir;
//extern short overall_mode;

struct header_posix_ustar {
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char checksum[8];
	char typeflag[1];
	char linkname[100];
	char magic[6];
	char version[2];
	char uname[32];
	char gname[32];
	char devmajor[8];
	char devminor[8];
	char prefix[155];
	char pad[12];
};

#include <stdexcept>

static header_posix_ustar generateTarHeader(const std::string& fileName, unsigned long long fileSize,bool directory=false){
	if(fileSize>077777777777LL)
		throw std::length_error("Specified file size >= 8 GB");
	if(fileName.length()>=100)
		throw std::length_error("Specified file name longer than 99 characters.");
	header_posix_ustar header;
	char* init = (char*) &header;
	for(unsigned int i = 0; i < sizeof(header); i++) init[i] = 0;
	
	sprintf(header.name,"%s",fileName.c_str());
	sprintf(header.mode,"%07o",0600);
	//leave uid filled with NULs
	//leave gid filled with NULs
	sprintf(header.size,"%011llo",fileSize);
	sprintf(header.mtime,"%011lo",time(NULL));
	memset(header.checksum,' ',8);
	header.typeflag[0]=directory?'5':'0';
	//leave linkname filled with NULs
	sprintf(header.magic,"ustar  "); //overwrites header with " \0"
	//leave uname filled with NULs
	//leave gname filled with NULs
	//leave devmajor filled with NULs
	//leave devminor filled with NULs
	//leave prefix filled with NULs
	//leave pad filled with NULs
	
	unsigned int sum=0;
	unsigned char* ptr=reinterpret_cast<unsigned char*>(&header);
	for(unsigned int i=0; i<sizeof(header); i++){
		sum+=ptr[i];
	}
	if(sum>0777777)
		throw std::runtime_error("Checksum overflow");
	sprintf(header.checksum,"%o",sum);
	return(header);
}

bool load_scenario(fs::path file_to_load){
	short i,n;
	bool file_ok = false;
	long len;
	legacy::scenario_data_type *temp_scenario = new legacy::scenario_data_type;
	legacy::scen_item_data_type *item_data = new legacy::scen_item_data_type;
	// TODO: Convert this (and all the others in this file) to use C++ streams
	FILE* file_id = fopen(file_to_load.c_str(),"rb");
	if(file_id == NULL) {
		// TODO: The third parameter to oopsError is supposed to specify whether we're in the scenario editor or the game, but I don't think this code knows that.
		// TODO: Alternatively, nuke oopsError and just use giveError. It's more informative, anyway.
		oopsError(10, 0, 0);
		return false;
	}
	
	len = (long) sizeof(scenario_header_flags);
	
	if (fread(&scenario.format, len, 1, file_id) < 1){
		fclose(file_id);
		oopsError(11, 0, 0);
		return false;
	}
	
	if ((scenario.format.flag1 == 10) && (scenario.format.flag2 == 20) &&
		(scenario.format.flag3 == 30) && (scenario.format.flag4 == 40)) {
	  	cur_scen_is_mac = true;
	  	file_ok = true;
	}
	else if ((scenario.format.flag1 == 20) && (scenario.format.flag2 == 40) &&
		(scenario.format.flag3 == 60) && (scenario.format.flag4 == 80)) {
	  	cur_scen_is_mac = false;
	  	file_ok = true;
	}
	if (file_ok == false) {
		fclose(file_id);
		giveError("This is not a legitimate Blades of Exile scenario.");
		return false;
	}
	
	len = (long) sizeof(legacy::scenario_data_type);
	n = fread(temp_scenario, len, 1, file_id);
	if(n < 1){
		fclose(file_id);
		oopsError(12, 0, 0);
		return false;
	}
	port_scenario(temp_scenario);
	len = sizeof(legacy::scen_item_data_type); // item data
	n = fread(item_data, len, 1, file_id);
	if(n < 1){
		fclose(file_id);
		oopsError(13, 0, 0);
		return false;
	}
	port_item_list(item_data);
	scenario = *temp_scenario;
	scenario.append(*item_data);
	// TODO: Consider skipping the fread and assignment when len is 0
	for (i = 0; i < 270; i++) {
		len = (long) (scenario.scen_str_len[i]);
		n = fread(&(scenario.scen_strs(i)), len, 1, file_id);
		scenario.scen_strs(i)[len] = 0;
	}
	
	fclose(file_id);
	
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
	scenario.scen_file = file_to_load;
	load_spec_graphics();
	return true;
}

static long get_town_offset(short which_town){
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
	short i,n;
	long len,len_to_jump = 0;
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
		giveError("The scenario tried to place you into a non-existant town.");
		return false;
	}
	
	FILE* file_id = fopen(scenario.scen_file.c_str(), "rb");
	if(file_id == NULL) {
		oopsError(14, 0, 0);
		return false;
	}
	
	len_to_jump = get_town_offset(which_town);
	n = fseek(file_id, len_to_jump, SEEK_SET);
	if(n != 0) {
		fclose(file_id);
		oopsError(15, 0, 0);
		return false;
	}
	
	len = sizeof(legacy::town_record_type);
	n = fread(&store_town, len, 1, file_id);
	if(n < 1) {
		fclose(file_id);
		oopsError(16, 0, 0);
		return false;
	}
	port_town(&store_town);
	
	if(the_town != NULL) delete the_town;
	switch (scenario.town_size[which_town]) {
		case 0:
			len = sizeof(legacy::big_tr_type);
			n = fread(&t_d, len, 1, file_id);
			port_t_d(&t_d);
			the_town = new cBigTown;
			*the_town = store_town;
			the_town->append(t_d);
			break;
			
		case 1:
			len = sizeof(legacy::ave_tr_type);
			n = fread(&ave_t, len, 1, file_id);
			port_ave_t(&ave_t);
			the_town = new cMedTown;
			*the_town = store_town;
			the_town->append(ave_t);
			break;
			
		case 2:
			len = sizeof(legacy::tiny_tr_type);
			n = fread(&tiny_t, len, 1, file_id);
			port_tiny_t(&tiny_t);
			the_town = new cTinyTown;
			*the_town = store_town;
			the_town->append(tiny_t);
			break;
	}
	
	for (i = 0; i < 140; i++) {
		len = (long) (the_town->strlens[i]);
		n = fread(&(the_town->town_strs(i)), len, 1, file_id);
		the_town->town_strs(i)[len] = 0;
	}
	
	len = sizeof(legacy::talking_record_type);
	n = fread(&store_talk, len, 1, file_id);
	if(n < 1) {
		fclose(file_id);
		oopsError(17, 0, 0);
		return false;
	}
	port_talk_nodes(&store_talk);
	the_town->talking = store_talk;
	
	for (i = 0; i < 170; i++) {
		len = (long) (the_town->talking.strlens[i]);
		n = fread(&(the_town->talking.talk_strs[i]), len, 1, file_id);
		the_town->talking.talk_strs[i][len] = 0;
	}
	
	//	town_type = scenario.town_size[which_town];
	n = fclose(file_id);
	if(n != 0) {
		oopsError(18, 0, 0);
	}
	
	return true;
}

bool load_town(short which_town, cSpeech*& the_talk){
	if(which_town == univ.town.cur_talk_loaded) return true;
	if(univ.town.num != univ.town.cur_talk_loaded) delete the_talk;
	univ.town.cur_talk_loaded = which_town;
	if(which_town == univ.town.num) {
		the_talk = &univ.town->talking;
		return true;
	}
	the_talk = new cSpeech;
	
	short i,n;
	long len,len_to_jump = 0;
	legacy::town_record_type store_town;
	legacy::talking_record_type store_talk;
	
	//	if (overall_mode == 61)
	//		return;
	
	if (which_town != minmax(0,scenario.num_towns - 1,which_town)) {
		// This should never be reached from the scenario editor,
		// because it does its own range checking before calling load_town.
		giveError("The scenario tried to place you into a non-existant town.");
		return false;
	}
	
	FILE* file_id = fopen(scenario.scen_file.c_str(), "rb");
	if(file_id == NULL) {
		oopsError(19, 0, 0);
		return false;
	}
	
	len_to_jump = get_town_offset(which_town);
	n = fseek(file_id, len_to_jump, SEEK_SET);
	if(n != 0) {
		fclose(file_id);
		oopsError(20, 0, 0);
		return false;
	}
	
	len = sizeof(legacy::town_record_type);
	n = fread(&store_town, len, 1, file_id);
	if(n < 1) {
		fclose(file_id);
		oopsError(21, 0, 0);
		return false;
	}
	port_town(&store_town);
	
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
	
	n = fseek(file_id, len, SEEK_CUR);
	for (i = 0; i < 140; i++) {
		len = (long) (store_town.strlens[i]);
		fseek(file_id, len, SEEK_CUR);
	}
	
	len = sizeof(legacy::talking_record_type);
	n = fread(&store_talk, len, 1, file_id);
	if(n < 1) {
		fclose(file_id);
		oopsError(22, 0, 0);
		return false;
	}
	port_talk_nodes(&store_talk);
	*the_talk = store_talk;
	
	for (i = 0; i < 170; i++) {
		len = (long) (the_talk->strlens[i]);
		n = fread(&(the_talk->talk_strs[i]), len, 1, file_id);
		the_talk->talk_strs[i][len] = 0;
	}
	
	//	town_type = scenario.town_size[which_town];
	n = fclose(file_id);
	if(n != 0) {
		oopsError(23, 0, 0);
	}
	
	return true;
}

bool load_town_str(short which_town, short which_str, char* str){
	short i,n;
	long len,len_to_jump = 0;
	legacy::town_record_type store_town;
	
	//	if (overall_mode == 61)
	//		return;

	FILE* file_id = fopen(scenario.scen_file.c_str(), "rb");
	if(file_id == NULL) {
		oopsError(24, 0, 0);
		return false;
	}
	
	len_to_jump = get_town_offset(which_town);
	n = fseek(file_id, len_to_jump, SEEK_SET);
	if(n < 1) {
		fclose(file_id);
		oopsError(25, 0, 0);
		return false;
	}
	
	len = sizeof(legacy::town_record_type);
	
	n = fread(&store_town, len, 1, file_id);
	if(len < 1) {
		fclose(file_id);
		oopsError(26, 0, 0);
		return false;
	}
	port_town(&store_town);
	
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
	
	n = fseek(file_id, len, SEEK_CUR);
	for (i = 0; i < 140; i++) {
		len = (long) (univ.town->strlens[i]);
		if(i == which_str){
			n = fread(str, len, 1, file_id);
			str[len] = 0;
		}
		else fseek(file_id, len, SEEK_CUR);
	}
	
	//	town_type = scenario.town_size[which_town];
	n = fclose(file_id);
	if(n != 0) {
		oopsError(27, 0, 0);
	}
	
	return true;
}

bool load_town_str(short which_town, cTown*& t){
	short i,n;
	long len,len_to_jump = 0;
	legacy::town_record_type store_town;
	
	//	if (overall_mode == 61)
	//		return;
	
	FILE* file_id = fopen(scenario.scen_file.c_str(), "rb");
	if(file_id == NULL) {
		oopsError(28, 0, 0);
		return false;
	}
	
	len_to_jump = get_town_offset(which_town);
	n = fseek(file_id, len_to_jump, SEEK_SET);
	if(n != 0) {
		fclose(file_id);
		oopsError(29, 0, 0);
		return false;
	}
	
	len = sizeof(legacy::town_record_type);
	n = fread(&store_town, len, 1, file_id);
	if(n < 1) {
		fclose(file_id);
		oopsError(30, 0, 0);
		return false;
	}
	port_town(&store_town);
	
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
	
	n = fseek(file_id, len, SEEK_CUR);
	for (i = 0; i < 140; i++) {
		len = (long) (t->strlens[i]);
		n = fread(&(t->town_strs(i)), len, 1, file_id);
		t->town_strs(i)[len] = 0;
	}
	
	//	town_type = scenario.town_size[which_town];
	n = fclose(file_id);
	if(n != 0) {
		oopsError(31, 0, 0);
	}
	
	return true;
}

static long get_outdoors_offset(location& which_out){
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
	short i,n;
	long len,len_to_jump;
	legacy::outdoor_record_type store_out;
	
//	if (overall_mode == 61)
//		return;
	
	if ((which_out.x != minmax(0,scenario.out_width - 1,which_out.x)) ||
		(which_out.y != minmax(0,scenario.out_height - 1,which_out.y))) {
		the_out = cOutdoors();
		return true;
	}
	
	FILE* file_id = fopen(scenario.scen_file.c_str(), "rb");
	if(file_id == NULL) {
		oopsError(32, 0, 0);
		return false;
	}
	
	len_to_jump = get_outdoors_offset(which_out);
	n = fseek(file_id, len_to_jump, SEEK_SET);
	if(n != 0) {
		fclose(file_id);
		oopsError(33, 0, 0);
		return false;
	}
	
	len = sizeof(legacy::outdoor_record_type);
	n = fread(&store_out, len, 1, file_id);
	if(n < 1) {
		fclose(file_id);
		oopsError(34, 0, 0);
		return false;
	}
	
	port_out(&store_out);
	the_out = store_out;
	for (i = 0; i < 108; i++) {
		len = (long) (the_out.strlens[i]);
		n = fread(&(the_out.out_strs(i)), len, 1, file_id);
		the_out.out_strs(i)[len] = 0;
	}
	
	n = fclose(file_id);
	if(n != 0) {
		oopsError(35, 0, 0);
	}
	return true;
}

bool load_outdoors(location which_out, short mode, ter_num_t borders[4][50]){
	short j,n;
	long len,len_to_jump;
	legacy::outdoor_record_type store_out;
	
	FILE* file_id = fopen(scenario.scen_file.c_str(), "rb");
	if(file_id == NULL) {
		oopsError(36, 0, 0);
		return false;
	}
	
	len_to_jump = get_outdoors_offset(which_out);
	
	n = fseek(file_id, len_to_jump, SEEK_SET);
	if(n != 0) {
		fclose(file_id);
		oopsError(37, 0, 0);
		return false;
	}
	
	len = sizeof(legacy::outdoor_record_type);
	n = fread(&store_out, len, 1, file_id);
	if(n < 1) {
		fclose(file_id);
		oopsError(38, 0, 0);
		return false;
	}
	
	switch(mode) {
	case 1:
		for (j = 0; j < 48; j++) {
			borders[0][j] = store_out.terrain[j][47];
		}
		break;
	case 2:
		for (j = 0; j < 48; j++) {
			borders[1][j] = store_out.terrain[0][j];
		}
		break;
	case 3:
		for (j = 0; j < 48; j++) {
			borders[2][j] = store_out.terrain[j][0];
		}
		break;
	case 4:
		for (j = 0; j < 48; j++) {
			borders[3][j] = store_out.terrain[47][j];
		}
		break;
	}
	
	n = fclose(file_id);
	if(n != 0) {
		oopsError(39, 0, 0);
	}
	return true;
}

bool load_outdoor_str(location which_out, short which_str, char* str){
	short i,n;
	long len,len_to_jump;
	legacy::outdoor_record_type store_out;
	
	FILE* file_id = fopen(scenario.scen_file.c_str(), "rb");
	if(file_id == NULL) {
		oopsError(40, 0, 0);
		return false;
	}
	
	len_to_jump = get_outdoors_offset(which_out);
	n = fseek(file_id, len_to_jump, SEEK_SET);
	if(n != 0) {
		fclose(file_id);
		oopsError(41, 0, 0);
		return false;
	}
	
	n = fread(&store_out, len, 1, file_id);
	for (i = 0; i < 120; i++) {
		len = (long) (store_out.strlens[i]);
		if (i == which_str) {
			n = fread(str, len, 1, file_id);
			str[len] = 0;
		}
		else fseek(file_id, len, SEEK_CUR);
	}
	
	n = fclose(file_id);
	if(n != 0) {
		oopsError(42, 0, 0);
	}
	return true;
}

#ifdef __APPLE__
bool tryLoadPictFromResourceFile(fs::path& gpath, sf::Image& graphics_store);
#endif

void load_spec_graphics()
{
	static const char*const noGraphics = "The game will still work without the custom graphics, but some things will not look right.";
	short i;
	fs::path path(scenario.scen_file);
	printf("Loading scenario graphics... (%s)\n",path.c_str());
	// Tried path.replace_extension, but that only deleted the extension, so I have to do it manually
	std::string filename = path.stem().string();
	path = path.parent_path();
	if(spec_scen_g) spec_scen_g.clear();
	// TODO: Load new-style sheets
	{
		static sf::Image graphics_store;
		bool foundGraphics = false;
#ifdef __APPLE__
		fs::path gpath = path/(filename + ".meg");
		if(fs::exists(gpath))
			foundGraphics = tryLoadPictFromResourceFile(gpath, graphics_store);
#endif
		if(!foundGraphics) {
			fs::path gpath = path/(filename + ".bmp");
			if(fs::exists(gpath)) {
				if(graphics_store.loadFromFile(gpath.string()))
					foundGraphics = true;
				else giveError("An old-style .bmp graphics file was found, but there was an error reading from the file.",noGraphics);
			}
		}
		if(foundGraphics) {
			// If we're here, we found old-style graphics.
			// This means they need an alpha channel
			graphics_store.createMaskFromColor(sf::Color::White);
			spec_scen_g.is_old = true;
			spec_scen_g.sheets = new sf::Texture[1];
			spec_scen_g.sheets[0].loadFromImage(graphics_store);
		} else {
			// Check for new-style graphics
		}
	}//else{}
	
	// Now load regular graphics
	items_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("objects"));
	tiny_obj_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("tinyobj"));
	fields_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("fields"));
	roads_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("trim"));
	boom_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("booms"));
	missiles_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("missiles"));
	dlogpics_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("dlogpics"));
	status_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("staticons"));
	
	for (i = 0; i < NUM_MONST_SHEETS; i++){
		std::ostringstream sout;
		sout << "monst" << i + 1;
		monst_gworld[i].loadFromImage(*ResMgr::get<ImageRsrc>(sout.str()));
	}
	for (i = 0; i < NUM_TER_SHEETS; i++){
		std::ostringstream sout;
		sout << "ter" << i + 1;
		terrain_gworld[i].loadFromImage(*ResMgr::get<ImageRsrc>(sout.str()));
	}
	anim_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("teranim"));
	talkfaces_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("talkportraits"));
	pc_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("pcs"));
	vehicle_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("vehicle"));
	small_ter_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("mapOLD")); // TODO: Should use the new map graphics instead
	// TODO: Scenario icons ...
}


bool load_party_v1(fs::path file_to_load, bool town_restore, bool in_scen, bool maps_there, bool must_port);
bool load_party_v2(fs::path file_to_load, bool town_restore, bool in_scen, bool maps_there);
bool load_party(fs::path file_to_load){
	bool town_restore = false;
	bool maps_there = false;
	bool in_scen = false;
	enum {old_mac, old_win, new_oboe, unknown} format;
	
	//char flag_data[8];
	long len;
//	flag_type flag;
//	flag_type *flag_ptr;
	short vers,n;
	struct __attribute__((packed)) {ushort a; ushort b; ushort c; ushort d; ushort e;} flags;
	
	// TODO: Putting these flags in hex would make some things a bit clearer
	static const unsigned short mac_flags[3][2] = {
		{5790,1342}, // slot 0 ... 5790 - out,         1342 - town
		{100,200},   // slot 1 ... 100  - in scenario, 200  - not in
		{3422,5567}  // slot 2 ... 3422 - no maps,     5567 - maps
	};
	static const unsigned short win_flags[3][2] = {
		{40470,15877}, // slot 0 ... 40470 - out,         15877 - town
		{25600,51200}, // slot 1 ... 25600 - in scenario, 51200 - not in
		{24077,48917}  // slot 2 ... 24077 - no maps,     48917 - maps
	};
	// but if the first flag is 0x0B0E, we have a new-format save
	// the three flags still follow that.
	FILE* file_id = fopen(file_to_load.c_str(), "rb");
	if(file_id == NULL) {
		cChoiceDlog("load-game-fail.xml").show();
		return false;
	}		
	
	
	//file_size = sizeof(legacy::party_record_type); // 45368
	
	len = sizeof(flags); // 10
	
	//	sprintf((char *) debug, "  Len %d               ", (short) len);
	//	add_string_to_buf((char *) debug);
	
	n = fread(&flags, len, 1, file_id);
	if(n < 1) {
		fclose(file_id);
		cChoiceDlog("not-save-game.xml").show();
		return false;
	}
	
	if(mac_is_intel && flags.a == 0x0B0E){ // new format
		format = new_oboe;
		if(flags.b == mac_flags[0][1]) town_restore = true;
		else if(flags.b != mac_flags[0][0]) format = unknown;
		if(flags.c == mac_flags[1][0]) in_scen = true;
		else if(flags.c != mac_flags[1][1]) format = unknown;
		if(flags.d == mac_flags[2][1]) maps_there = true;
		else if(flags.d != mac_flags[2][0]) format = unknown;
		vers = flags.e;
	}else if(!mac_is_intel && flags.a == 0x0E0B){ // new format
		format = new_oboe;
		flip_short((short*)&flags.b);
		flip_short((short*)&flags.c);
		flip_short((short*)&flags.d);
		flip_short((short*)&flags.e);
		if(flags.b == mac_flags[0][1]) town_restore = true;
		else if(flags.b != mac_flags[0][0]) format = unknown;
		if(flags.c == mac_flags[1][0]) in_scen = true;
		else if(flags.c != mac_flags[1][1]) format = unknown;
		if(flags.d == mac_flags[2][1]) maps_there = true;
		else if(flags.d != mac_flags[2][0]) format = unknown;
		vers = flags.e;
	}else if(flags.a == mac_flags[0][0] || flags.a == mac_flags[0][1]){ // old format
		if(mac_is_intel){ // it's actually a windows save
			flip_short((short*)&flags.a);
			flip_short((short*)&flags.b);
			flip_short((short*)&flags.c);
			format = old_win;
			if(flags.a == win_flags[0][1]) town_restore = true;
			else if(flags.a != win_flags[0][0]) format = unknown;
			if(flags.b == win_flags[1][0]) in_scen = true;
			else if(flags.b != win_flags[1][1]) format = unknown;
			if(flags.c == win_flags[2][1]) maps_there = true;
			else if(flags.c != win_flags[2][0]) format = unknown;
		}else{ // mac save
			format = old_mac;
			if(flags.a == mac_flags[0][1]) town_restore = true;
			else if(flags.a != mac_flags[0][0]) format = unknown;
			if(flags.b == mac_flags[1][0]) in_scen = true;
			else if(flags.b != mac_flags[1][1]) format = unknown;
			if(flags.c == mac_flags[2][1]) maps_there = true;
			else if(flags.c != mac_flags[2][0]) format = unknown;
		}
	}else if(flags.a == win_flags[0][0] || flags.a == win_flags[0][1]){ // old format
		if(mac_is_intel){ // it's actually a macintosh save
			flip_short((short*)&flags.a);
			flip_short((short*)&flags.b);
			flip_short((short*)&flags.c);
			format = old_mac;
			if(flags.a == mac_flags[0][1]) town_restore = true;
			else if(flags.a != mac_flags[0][0]) format = unknown;
			if(flags.b == mac_flags[1][0]) in_scen = true;
			else if(flags.b != mac_flags[1][1]) format = unknown;
			if(flags.c == mac_flags[2][1]) maps_there = true;
			else if(flags.c != mac_flags[2][0]) format = unknown;
		}else{ // win save
			format = old_win;
			if(flags.a == win_flags[0][1]) town_restore = true;
			else if(flags.a != win_flags[0][0]) format = unknown;
			if(flags.b == win_flags[1][0]) in_scen = true;
			else if(flags.b != win_flags[1][1]) format = unknown;
			if(flags.c == win_flags[2][1]) maps_there = true;
			else if(flags.c != win_flags[2][0]) format = unknown;
		}
	}else format = unknown;
	
	fclose(file_id);
	switch(format){
		case old_mac:
			return load_party_v1(file_to_load, town_restore, in_scen, maps_there, mac_is_intel);
		case old_win:
			return load_party_v1(file_to_load, town_restore, in_scen, maps_there, !mac_is_intel);
		case new_oboe:
			return load_party_v2(file_to_load, town_restore, in_scen, maps_there);
		case unknown:
			cChoiceDlog("not-save-game.xml").show();
			return false;
	}
	
//	for (i = 0; i < 3; i++) {
//		error = FSRead(file_id, &len, (char *) &flag);
//		if (error != 0) {
//			FSClose(file_id);
//			FCD(1064,0);
//			return false;
//		}
////		flag_ptr = (flag_type *) flag_data;
////		flag = *flag_ptr;
//		if (flag == 0x0B0E) { // new format save
//			new_format = true;
//			i--;
//			continue;
//		}
//		if(mac_is_intel && !new_format) flip_short(&flag); // because the old format save was made for non-intel macs
//		if ((flag != flags[i][0]) && (flag != flags[i][1])) { // OK Exile II save file?
//			FSClose(file_id);
//			FCD(1063,0);
//			return false;
//		} 
//		
//		if ((i == 0) && (flag == flags[i][1]))
//			town_restore = true;
//		if ((i == 1) && (flag == flags[i][0])) {
//			in_scen = true;
//		}
//		if ((i == 2) && (flag == flags[i][1]))
//			maps_there = true;
//	}
	return true;
}

bool load_party_v1(fs::path file_to_load, bool town_restore, bool in_scen, bool maps_there, bool must_port){
	std::ifstream fin(file_to_load.c_str(), std::ios_base::binary);
	fin.seekg(3*sizeof(short),std::ios_base::beg); // skip the header, which is 6 bytes in the old format
	//short flags[3];
	//fin.read((char*)flags,3*sizeof(short));
//	OSErr error = FSpOpenDF(&file_to_load,1,&file_id);
//	if (error != 0) {
//		FCD(1064,0);
//		SysBeep(2);
//		return false;
//	}
	
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
	char *party_ptr;
	char *pc_ptr;
	long len,store_len,count;
	
	// LOAD PARTY	
	len = (long) sizeof(legacy::party_record_type); // 45368
	store_len = len;
	fin.read((char*)&store_party, len);
//	error = FSRead(file_id, &len, (char *) &store_party)
//	if (error != 0){
//		FSClose(file_id);
//		SysBeep(2);
//		FCD(1064,0);
//		return false;
//	}
	if(must_port) port_party(&store_party);
	party_ptr = (char*) &store_party;
	for (count = 0; count < store_len; count++)
		party_ptr[count] ^= 0x5C;	
	
	// LOAD SETUP
	len = (long) sizeof(legacy::setup_save_type);
	fin.read((char*)&store_setup, len);
//	error = FSRead(file_id, &len, (char *) &store_setup)
//	if (error != 0){
//		FSClose(file_id);
//		SysBeep(2);
//		FCD(1064,0);
//		return false;
//	}
	
	// LOAD PCS
	store_len = (long) sizeof(legacy::pc_record_type);
	for (int i = 0; i < 6; i++) {
		len = store_len;
		fin.read((char*)&store_pc[i], len);
//		error = FSRead(file_id, &len, (char *) &store_pc[i]);
//		if (error  != 0){
//			FSClose(file_id);
//			SysBeep(2);
//			FCD(1064,0);
//			return false;
//		}
		if(must_port) port_pc(&store_pc[i]);
		pc_ptr = (char*) &store_pc[i];
		for (count = 0; count < store_len; count++)
			pc_ptr[count] ^= 0x6B;	
	}
	
	if (in_scen) {
		
		// LOAD OUTDOOR MAP
		len = (long) sizeof(legacy::out_info_type);
		fin.read((char*)&store_out_info, len);
//		error = FSRead(file_id, &len, (char *) &store_out_info)
//		if (error != 0){
//			FSClose(file_id);
//			SysBeep(2);
//			FCD(1064,0);
//			return false;
//		}
		
		if(univ.town.loaded()) univ.town.unload();
		
		// LOAD TOWN 
		if (town_restore) {
			len = (long) sizeof(legacy::current_town_type);
			fin.read((char*)&store_c_town, len);
//			error = FSRead(file_id, &len, (char *) &store_c_town)
//			if (error != 0){
//				FSClose(file_id);
//				SysBeep(2);
//				FCD(1064,0);
//				return false;
//			}
			if(must_port) port_c_town(&store_c_town);
			
			len = (long) sizeof(legacy::big_tr_type);
			fin.read((char*)&t_d, len);
//			error = FSRead(file_id, &len, (char *) &t_d)
//			if (error != 0){
//				FSClose(file_id);
//				SysBeep(2);
//				FCD(1064,0);
//				return false;
//			}
			if(must_port) port_t_d(&t_d);
			
			len = (long) sizeof(legacy::town_item_list);
			fin.read((char*)&t_i, len);
//			error = FSRead(file_id, &len, (char *) &t_i)
//			if (error != 0){
//				FSClose(file_id);
//				SysBeep(2);
//				FCD(1064,0);
//				return false;
//			}
			
		}else univ.town.num = 200;
		
		// LOAD STORED ITEMS
		for (int i = 0; i < 3; i++) {
			len = (long) sizeof(legacy::stored_items_list_type);
			fin.read((char*)&stored_items[i], len);
//			error = FSRead(file_id, &len, (char *) &stored_items[i])
//			if (error  != 0){
//				FSClose(file_id);
//				SysBeep(2);
//				FCD(1064,0);
//				return false;
//			}
		}
		
		// LOAD SAVED MAPS
		if (maps_there) {
			len = (long) sizeof(legacy::stored_town_maps_type);
			fin.read((char*)&town_maps, len);
//			error = FSRead(file_id, &len, (char *) &(town_maps))
//			if (error != 0){
//				FSClose(file_id);
//				SysBeep(2);
//				FCD(1064,0);
//				return false;
//			}
			
			len = (long) sizeof(legacy::stored_outdoor_maps_type);
			fin.read((char*)&o_maps, len);
//			error = FSRead(file_id, &len, (char *) &o_maps)
//			if (error != 0) {
//				FSClose(file_id);
//				SysBeep(2);
//				FCD(1064,0);
//				return false;
//			}	
		}
		
		// LOAD SFX & MISC_I
		len = (long) (64 * 64);
		fin.read((char*)sfx, len);
//		error = FSRead(file_id, &len, (char *) sfx)
//		if (error != 0){
//			FSClose(file_id);
//			SysBeep(2);
//			FCD(1064,0);
//			return false;
//		}
		fin.read((char*)misc_i, len);
//		error = FSRead(file_id, &len, (char *) misc_i)
//		if (error != 0){
//			FSClose(file_id);
//			SysBeep(2);
//			FCD(1064,0);
//			return false;
//		}	
		
	} // end if_scen
	
	fin.close();
//	error = FSClose(file_id)
//	if (error != 0){
//		give_error("Load: Can't close file.","",0);
//		SysBeep(2);
//		return false;
//	}
	
	univ.party = store_party;
	univ.party.append(store_setup);
	univ.party.void_pcs();
	for(int i = 0; i < 6; i++)
		univ.party.add_pc(store_pc[i]);
	if(in_scen){
		univ.out.append(store_out_info);
		if(town_restore){
			univ.town.append(store_c_town,scenario.town_size[univ.town.num]);
			univ.town.append(t_d);
			univ.town.append(t_i);
		}
		for(int i = 0; i < 3; i++)
			univ.party.append(stored_items[i],i);
		univ.append(town_maps);
		univ.append(o_maps);
		univ.town.append(sfx, misc_i);
	}
	if(in_scen){
		fs::path path;
		path = progDir/"Blades of Exile Scenarios"/univ.party.scen_name;
		//std::cout<<path<<'\n';
		if(!load_scenario(path))
			return false;
		univ.file = path;
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

bool load_party_v2(fs::path file_to_load, bool town_restore, bool in_scen, bool maps_there){
	char* buf;
	FILE* f = fopen(file_to_load.c_str(),"rb");
	
	// TODO: Write the parsers and stuff
	/*   Skip header   */
	fseek(f, sizeof(short) * 5, SEEK_SET);
	gzFile party_file = gzdopen(fileno(f), "rb");
	
	/*   Initialize various variables   */
	header_posix_ustar header;
	// TODO: Don't call this sin; that's a trig function
	std::istringstream sin;
	std::string tar_name;
	std::string tar_entry;
	size_t tar_size;
	std::map<std::string,std::string> party_archive;
	
	/*   Split the tar archive into its component files   */
	while(!gzeof(party_file)){
		gzread(party_file, &header, sizeof(header));
		sscanf(header.size, "%lo", &tar_size);
		tar_name = header.name;
		buf = new char[tar_size];
		gzread(party_file, buf, tar_size);
		tar_entry = buf;
		delete buf;
		party_archive[tar_name] = tar_entry;
		gzseek(party_file, sizeof(header) * 2, SEEK_CUR);
	}
	
	/*   Read the data from each file   */
	if(party_archive.find("save/party.txt") == party_archive.end()){
		cChoiceDlog("load-game-fail.xml").show();
		return false;
	}else{
		sin.str(party_archive["save/party.txt"]);
		univ.party.readFrom(sin);
	}
	for(int i = 0; i < 6; i++){
		buf = new char[20];
		sprintf(buf,"save/pc%i.txt",i);
		std::string f_name = buf;
		delete buf;
		if(party_archive.find(f_name) == party_archive.end()){
			cChoiceDlog("load-game-fail.xml").show();
			return false;
		}else{
			sin.str(party_archive[f_name]);
			univ.party[i].readFrom(sin);
		}
	}
	if(in_scen){
		if(town_restore){
			if(party_archive.find("save/town.txt") == party_archive.end()){
				cChoiceDlog("load-game-fail.xml").show();
				return false;
			}else{
				sin.str(party_archive["save/town.txt"]);
				univ.town.readFrom(sin);
			}
			if(party_archive.find("save/townmaps.dat") == party_archive.end()){
				cChoiceDlog("load-game-fail.xml").show();
				return false;
			}else{
				sin.str(party_archive["save/townmaps.dat"]);
				for(int i = 0; i < 200; i++)
					for(int j = 0; j < 8; j++)
						for(int k = 0; k < 64; k++)
							univ.town_maps[i][j][k] = sin.get();
			}
		}
		if(party_archive.find("save/out.txt") == party_archive.end()){
			cChoiceDlog("load-game-fail.xml").show();
			return false;
		}else{
			sin.str(party_archive["save/out.txt"]);
			univ.out.readFrom(sin);
		}
		if(party_archive.find("save/outmaps.dat") == party_archive.end()){
			cChoiceDlog("load-game-fail.xml").show();
			return false;
		}else{
			sin.str(party_archive["save/outmaps.dat"]);
			for(int i = 0; i < 100; i++)
				for(int j = 0; j < 6; j++)
					for(int k = 0; k < 48; k++)
						univ.out_maps[i][j][k] = sin.get();
		}
	}
	return true;
}

bool save_party(fs::path dest_file)
//mode;  // 0 - normal  1 - save as
{
	//printf("Saving is currently disabled.\n");
	bool in_scen = univ.party.scen_name != "";
	bool in_town = univ.town.num < 200;
	bool save_maps = !univ.party.stuff_done[306][0];
	
	// TODO: Strip off the header and save the remainder to a temporary file, then use gzip on that; this should fix the issue of the header being interpreted as compressed.
	/*   Open the GZip file and write the (uncompressed) header   */
	std::ofstream fout(dest_file.c_str(), std::ios_base::binary);
	short flags[] = {
		0x0B0E, // to indicate new format
		static_cast<short>(in_town ? 1342 : 5790), // is the party in town?
		static_cast<short>(in_scen ? 100 : 200), // is the party in a scenario?
		static_cast<short>(save_maps ? 5567 : 3422), // is the save maps feature enabled?
		0x0100, // current version number, major and minor revisions only
		// Version 1 indicates a beta format that won't be supported in the final release
	};
	if(!mac_is_intel) // must flip all the flags to little-endian
		for(int i = 0; i < 5; i++)
			flip_short(&flags[i]);
	//fwrite(flags, sizeof(short), 5, f);
	//gzFile party_file = gzdopen(fileno(f),"wb0"); // 0 indicates no compression; this is temporary
	//gzwrite(party_file, flags, sizeof(short)*5);
	//int q = gzflush(party_file, Z_FULL_FLUSH); // required in order to avoid the header getting compressed
	//gzsetparams(party_file, Z_DEFAULT_COMPRESSION, Z_DEFAULT_STRATEGY);
	fout.write((char*) flags, 5*sizeof(short));
	
	/*   Initialize buffer and other variables   */
	header_posix_ustar header;
	static char footer[2*sizeof(header_posix_ustar)] = {0};
	std::ostringstream sout("");
	std::ostringstream bout("",std::ios_base::binary);
	std::string tar_entry;
	size_t tar_size, y;
	int x;
	sout << x;
	/*   Write main party data to a buffer, and then to the file   */
	univ.party.writeTo(sout);
	//write the footer to end the file
//	for(unsigned int j=0; j<2*sizeof(header_posix_ustar); j++)
//		sout.put('\0');
	tar_entry = sout.str();
	y = tar_size = tar_entry.length();
	while(y % 512){
		y++;
		tar_entry += '\0';
	}
	header = generateTarHeader("save/party.txt",tar_size);
	bout.write((char*) &header, sizeof(header));
	bout.write(tar_entry.c_str(), y);
//	x = gzwrite(party_file, (char*) &header, sizeof(header));
//	x = gzwrite(party_file, tar_entry.c_str(), y);
//	//x = gzwrite(party_file, footer, 2*sizeof(header_posix_ustar));
//	if(x == 0) { // error
//		gzerror(party_file, &x);
//		oops_error((x == Z_ERRNO) ? Z_ERRNO + 1000 : 1, (x == Z_ERRNO) ? errno : x, 1);
//		gzclose(party_file);
//		return false;
//	}
	sout.str(""); // clear the stream (I think)
	
	/*   Write player character data to a buffer, and then to the file   */
	for(int i = 0; i < 6; i++){
		char f_name[20];
		univ.party[i].writeTo(sout);
		//write the footer to end the file
//		for(unsigned int j=0; j<2*sizeof(header_posix_ustar); j++)
//			sout.put('\0');
		tar_entry = sout.str();
		y = tar_size = tar_entry.length();
		while(y % 512){
			y++;
			tar_entry += '\0';
		}
		sprintf(f_name,"save/pc%i.txt",i+1);
		header = generateTarHeader(f_name,tar_size);
		bout.write((char*) &header, sizeof(header));
		bout.write(tar_entry.c_str(), y);
//		x = gzwrite(party_file, (char*) &header, sizeof(header));
//		x = gzwrite(party_file, tar_entry.c_str(), y);
//		//x = gzwrite(party_file, footer, 2*sizeof(header_posix_ustar));
//		if(x == 0) { // error
//			gzerror(party_file, &x);
//			oops_error((x == Z_ERRNO) ? Z_ERRNO + 1000 : 1, (x == Z_ERRNO) ? errno : x, 1);
//			gzclose(party_file);
//			return false;
//		}
		sout.str(""); // clear the stream (I think)
	}
	
	if(in_scen){
		if(in_town){
			/*   Write current town data to a buffer, and then to the file   */
			univ.town.writeTo(sout);
			//write the footer to end the file
//			for(unsigned int j=0; j<2*sizeof(header_posix_ustar); j++)
//				sout.put('\0');
			tar_entry = sout.str();
			y = tar_size = tar_entry.length();
			while(y % 512){
				y++;
				tar_entry += '\0';
			}
			header = generateTarHeader("save/town.txt",tar_size);
			bout.write((char*) &header, sizeof(header));
			bout.write(tar_entry.c_str(), y);
//			x = gzwrite(party_file, (char*) &header, sizeof(header));
//			x = gzwrite(party_file, tar_entry.c_str(), y);
//			//x = gzwrite(party_file, footer, 2*sizeof(header_posix_ustar));
//			if(x == 0) { // error
//				gzerror(party_file, &x);
//				oops_error((x == Z_ERRNO) ? Z_ERRNO + 1000 : 1, (x == Z_ERRNO) ? errno : x, 1);
//				gzclose(party_file);
//				return false;
//			}
			sout.str(""); // clear the stream (I think)
			
			if(save_maps){
				/*   Write town maps data to a buffer, and then to the file   */
				for(int i = 0; i < 200; i++)
					for(int j = 0; j < 8; j++)
						for(int k = 0; k < 64; k++)
							sout.put(univ.town_maps[i][j][k]);
				//write the footer to end the file
//				for(unsigned int j=0; j<2*sizeof(header_posix_ustar); j++)
//					sout.put('\0');
				tar_entry = sout.str();
				y = tar_size = tar_entry.length();
				while(y % 512){
					y++;
					tar_entry += '\0';
				}
				header = generateTarHeader("save/townmaps.dat",tar_size);
				bout.write((char*) &header, sizeof(header));
				bout.write(tar_entry.c_str(), y);
//				x = gzwrite(party_file, (char*) &header, sizeof(header));
//				x = gzwrite(party_file, tar_entry.c_str(), y);
//				//x = gzwrite(party_file, footer, 2*sizeof(header_posix_ustar));
//				if(x == 0) { // error
//					gzerror(party_file, &x);
//					oops_error((x == Z_ERRNO) ? Z_ERRNO + 1000 : 1, (x == Z_ERRNO) ? errno : x, 1);
//					gzclose(party_file);
//					return false;
//				}
				sout.str(""); // clear the stream (I think)
			}
		}
		
		/*   Write current outdoors data to a buffer, and then to the file   */
		univ.out.writeTo(sout);
		//write the footer to end the file
//		for(unsigned int j=0; j<2*sizeof(header_posix_ustar); j++)
//			sout.put('\0');
		tar_entry = sout.str();
		y = tar_size = tar_entry.length();
		while(y % 512){
			y++;
			tar_entry += '\0';
		}
		header = generateTarHeader("save/out.txt",tar_size);
		bout.write((char*) &header, sizeof(header));
		bout.write(tar_entry.c_str(), y);
//		x = gzwrite(party_file, (char*) &header, sizeof(header));
//		x = gzwrite(party_file, tar_entry.c_str(), y);
//		//x = gzwrite(party_file, footer, 2*sizeof(header_posix_ustar));
//		if(x == 0) { // error
//			gzerror(party_file, &x);
//			oops_error((x == Z_ERRNO) ? Z_ERRNO + 1000 : 1, (x == Z_ERRNO) ? errno : x, 1);
//			gzclose(party_file);
//			return false;
//		}
		sout.str(""); // clear the stream (I think)
		
		if(save_maps){
			/*   Write outdoor maps data to a buffer, and then to the file   */
			for(int i = 0; i < 100; i++)
				for(int j = 0; j < 6; j++)
					for(int k = 0; k < 48; k++)
						sout.put(univ.out_maps[i][j][k]);
			//write the footer to end the file
//			for(unsigned int j=0; j<2*sizeof(header_posix_ustar); j++)
//				sout.put('\0');
			tar_entry = sout.str();
			y = tar_size = tar_entry.length();
			while(y % 512){
				y++;
				tar_entry += '\0';
			}
			header = generateTarHeader("save/outmaps.dat",tar_size);
			bout.write((char*) &header, sizeof(header));
			bout.write(tar_entry.c_str(), y);
//			x = gzwrite(party_file, (char*) &header, sizeof(header));
//			x = gzwrite(party_file, tar_entry.c_str(), y);
//			//x = gzwrite(party_file, footer, 2*sizeof(header_posix_ustar));
//			if(x == 0) { // error
//				gzerror(party_file, &x);
//				oops_error((x == Z_ERRNO) ? Z_ERRNO + 1000 : 1, (x == Z_ERRNO) ? errno : x, 1);
//				gzclose(party_file);
//				return false;
//			}
//			sout.str(""); // clear the stream (I think)
		}
	}
	//x = gzwrite(party_file, footer, 2*sizeof(header_posix_ustar));
	//gzclose(party_file);
	bout.write(footer, 2*sizeof(header_posix_ustar));
	tar_entry = bout.str();
	tar_size = tar_entry.length();
	unsigned char* buf = new unsigned char[tar_size];
	compress(buf, &tar_size, (unsigned char*) tar_entry.data(), tar_size);
	fout.write((char*)buf, tar_size);
	delete buf;
	fout.close();
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
	return true;
}

std::vector<std::string> load_strings(std::string which){
	// TODO: Fix the bundle-ness
	// TODO: Is this even used anywhere?
#if 0
	UniChar* strListName = new UniChar[which.length()];
	for(int i = 0; i < which.length(); i++) strListName[i] = which[i];
	char stringsPath[512];
	std::string path;
	CFBundleRef mainBundle=CFBundleGetMainBundle();
	CFURLRef stringsURL = CFBundleCopyResourceURL(
		mainBundle,
		CFStringCreateWithCharacters(NULL, strListName, which.length()),
		CFSTR(""), CFSTR("strings")
	);
	delete strListName;
	if(stringsURL == NULL) return std::vector<std::string>(); // if not found, return an empty list
	CFStringGetCString(CFURLCopyFileSystemPath(stringsURL, kCFURLPOSIXPathStyle), stringsPath, 512, kCFStringEncodingUTF8);
#endif
	fs::path path = progDir/which;
	printf("Loading strings from: %s\n",path.c_str());
	std::ifstream fin(path.c_str());
	std::string s;
	std::vector<std::string> v;
	while(getline(fin,s))
		v.push_back(s);
	return v;
}
