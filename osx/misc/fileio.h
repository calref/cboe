/*
 *  fileio.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

Boolean scen_file_filter(AEDesc* item, void* info, void * dummy, NavFilterModes filterMode);
Boolean party_file_filter(AEDesc* item, void* info, void * dummy, NavFilterModes filterMode);
void init_fileio();
FSRef nav_get_scenario();
FSRef nav_put_scenario();
FSRef nav_get_party();
FSRef nav_put_party();
bool load_scenario(FSSpec file_to_load);
bool load_town(short which_town);
bool load_outdoors(location which_out,short mode);
void load_spec_graphics();