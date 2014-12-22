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

class cScenario;
class cUniverse;

namespace fs = boost::filesystem; // TODO: Centralize this alias!

bool load_scenario(fs::path file_to_load, cScenario& scenario);

bool load_party(fs::path file_to_load, cUniverse& univ);
bool save_party(fs::path dest_file, const cUniverse& univ);

void init_directories(const char* exec_path);
void check_for_intel();

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
