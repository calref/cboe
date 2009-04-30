/*
 *  fileio.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

void init_fileio();

Boolean scen_file_filter(AEDesc* item, void* info, void * dummy, NavFilterModes filterMode);
Boolean party_file_filter(AEDesc* item, void* info, void * dummy, NavFilterModes filterMode);
FSSpecPtr nav_get_scenario();
FSSpecPtr nav_put_scenario();
bool load_scenario(FSSpec file_to_load);
bool load_town(short which_town, cTown*& the_town);
bool load_town(short which_town, cSpeech& the_talk);
bool load_town_str(short which_town, short which_str, char* str);
bool load_town_str(short which_town, cTown*& the_town);
bool load_outdoors(location which_out,cOutdoors& the_out);
bool load_outdoors(location which_out, short mode, unsigned char borders[4][50]);
bool load_outdoor_str(location which_out, short which_str, char* str);
void load_spec_graphics();

FSSpecPtr nav_get_party();
FSSpecPtr nav_put_party();
bool load_party(FSSpec file_to_load);
void save_party(FSSpec dest_file);
