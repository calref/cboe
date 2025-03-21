/*
 *  fileio.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#ifndef BOE_FILEIO_HPP
#define BOE_FILEIO_HPP

#include <ctime>
#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <SFML/System/InputStream.hpp>
#include <boost/filesystem/path.hpp>

class cScenario;
class cUniverse;

extern std::vector<fs::path> extra_scen_dirs;

enum class eLoadScenario {
	ONLY_HEADER,
	SAVE_PREVIEW,
	FULL
};
std::vector<fs::path> all_scen_dirs();
fs::path locate_scenario(std::string scen_name, bool allow_unpacked = false);
bool load_scenario(fs::path file_to_load, cScenario& scenario, eLoadScenario load_type = eLoadScenario::FULL);

fs::path nav_get_or_decode_party();
fs::path nav_put_or_temp_party(fs::path def = "");

fs::path os_file_picker(bool saving);
// The game implements a fancy file picker, the editors just call the OS picker.
extern fs::path run_file_picker(bool saving);

const std::set<fs::path> save_extensions = {".exg", ".boe", ".SAV", ".mac"};
// Return a directory's files sorted by last modified time
std::vector<std::pair<fs::path, std::time_t>> sorted_file_mtimes(fs::path dir, std::set<fs::path> valid_extensions = save_extensions);

bool load_party(fs::path file_to_load, cUniverse& univ, bool preview = false);
bool save_party(cUniverse& univ, bool save_as = false);
bool save_party_force(cUniverse& univ, fs::path file);

void init_directories(const char* exec_path);

std::string read_maybe_quoted_string(std::istream& from);
std::string maybe_quote_string(std::string which, bool force = false);

template<typename T>
struct array_value_type{
	using type = typename T::value_type;
};

template<typename T, int D1, int D2>
struct array_value_type<T[D1][D2]> {
	using type = T;
};

template<typename T, size_t D1, size_t D2>
struct array_value_type<std::array<std::array<T, D2>, D1>> {
	using type = T;
};

template<typename T>
void writeArray(std::ostream& to, const T& array, int width, int height) {
	using int_type = decltype(typename array_value_type<T>::type() + 1);
	for(int y = 0; y < height; y++) {
		to << int_type(array[0][y]);
		for(int x = 1; x < width; x++)
			to << '\t' << int_type(array[x][y]);
		to << '\n';
	}
	to << '\f';
}

template<typename T>
void readArray(std::istream& from, T& array, int width, int height) {
	using int_type = decltype(typename array_value_type<T>::type() + 1);
	from >> std::ws;
	std::string arrayContents;
	getline(from, arrayContents, '\f');
	std::istringstream arrayIn(arrayContents);
	for(int y = 0; y < height; y++) {
		std::string line;
		getline(arrayIn, line);
		std::istringstream lineIn(line);
		lineIn.flags(from.flags());
		int_type temp;
		for(int x = 0; x < width; x++)
			lineIn >> temp, array[x][y] = temp;
		if(!arrayIn) break;
	}
}

// Manipulator to write an error code to a C++ string, similar to how perror() does
std::ostream& std_fmterr(std::ostream& out);

// SFML doesn't support standard C++ streams, so I need to do this in order to load images from a stream.
class StdInputStream : public sf::InputStream {
	std::istream& stream;
public:
	StdInputStream(std::istream& stream) : stream(stream) {}
	virtual ~StdInputStream() {}
	// TODO: All four of these functions should return -1 on failure, but I'm not quite sure which conditions should be counted as failure
	virtual sf::Int64 read(void *data, sf::Int64 size) {
		stream.read((char*)data, size);
		return stream.gcount();
	}
	virtual sf::Int64 seek(sf::Int64 position) {
		stream.seekg(position);
		return tell();
	}
	virtual sf::Int64 tell() {
		return stream.tellg();
	}
	virtual sf::Int64 getSize() {
		auto was_at = stream.tellg();
		stream.seekg(0, std::ios::end);
		auto pos = stream.tellg();
		stream.seekg(was_at);
		return pos;
	}
};

#endif
