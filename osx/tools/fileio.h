/*
 *  fileio.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include <string>
#include <vector>
#include <sstream>
#include <boost/filesystem/path.hpp>

#include "town.h"
#include "outdoors.h"

namespace fs = boost::filesystem; // TODO: Centralize this alias!

bool load_scenario(fs::path file_to_load, bool skip_strings = false);
bool load_town(short which_town, cTown*& the_town);
bool load_town_talk(short which_town);
bool load_town_str(short which_town, short which_str, char* str);
bool load_town_str(short which_town, cTown*& the_town);
bool load_outdoors(location which_out,cOutdoors& the_out);
bool load_outdoors(location which_out, short mode, ter_num_t borders[4][50]);
bool load_outdoor_str(location which_out, short which_str, char* str);
void load_spec_graphics();

bool load_party(fs::path file_to_load);
bool save_party(fs::path dest_file);

void init_directories(const char* exec_path);

std::string read_maybe_quoted_string(std::istream& from);
std::string maybe_quote_string(std::string which);

template<typename T, int D>
void writeArray(std::ostream& to, const T(* array)[D], int width, int height) {
	using int_type = decltype(T() + 1);
	for(int i = 0; i < width; i++) {
		to << array[i][0];
		for(int j = 1; j < height; j++)
			to << '\t' << int_type(array[i][j]);
		to << '\n';
	}
	to << '\f';
}

template<typename T, int D>
void readArray(std::istream& from, T(* array)[D], int width, int height) {
	using int_type = decltype(T() + 1);
	std::string arrayContents;
	getline(from, arrayContents, '\f');
	std::istringstream arrayIn(arrayContents);
	for(int i = 0; i < width; i++) {
		std::string line;
		getline(arrayIn, line);
		std::istringstream lineIn(line);
		for(int j = 0; j < height; j++)
			lineIn >> array[i][j];
		if(!arrayIn) break;
	}
}
