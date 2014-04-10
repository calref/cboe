/*
 *  fileio.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include <string>
#include <vector>
#include <boost/filesystem/path.hpp>
#include "town.h"
#include "outdoors.h"

namespace fs = boost::filesystem; // TODO: Centralize this alias!

bool load_scenario(fs::path file_to_load);
bool load_town(short which_town, cTown*& the_town);
bool load_town(short which_town, cSpeech& the_talk);
bool load_town_str(short which_town, short which_str, char* str);
bool load_town_str(short which_town, cTown*& the_town);
bool load_outdoors(location which_out,cOutdoors& the_out);
bool load_outdoors(location which_out, short mode, ter_num_t borders[4][50]);
bool load_outdoor_str(location which_out, short which_str, char* str);
void load_spec_graphics();
std::vector<std::string> load_strings(std::string which);

bool load_party(fs::path file_to_load);
bool save_party(fs::path dest_file);
