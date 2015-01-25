/*
 *  fileio.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include "fileio.hpp"

#include <fstream>
#include <boost/filesystem/operations.hpp>

#include "restypes.hpp"
#include "cursors.hpp"

bool mac_is_intel;
fs::path progDir, tempDir;

// Cursors included here so that they needn't be unnecessarily duplicated in platform-specific files
cursor_type current_cursor = sword_curs;
cursor_type arrow_curs[3][3] = {
	{NW_curs, N_curs, NE_curs},
	{W_curs, wait_curs, E_curs},
	{SW_curs, S_curs, SE_curs},
};

#include <stdexcept>
std::filebuf logfile;

void init_directories(const char* exec_path) {
	progDir = fs::canonical(exec_path);
#ifdef __APPLE__
	// Need to back up out of the application package
	// We're pointing at .app/Contents/MacOS/exec_name, so back out three steps
	progDir = progDir.parent_path().parent_path().parent_path();
#endif
	progDir = progDir.parent_path();
	if(progDir.filename() == "Scenario Editor") progDir = progDir.parent_path();
	// Initialize the resource manager paths
	ResMgr::pushPath<ImageRsrc>(progDir/"Scenario Editor"/"graphics.exd"/"mac");
	ResMgr::pushPath<CursorRsrc>(progDir/"Scenario Editor"/"graphics.exd"/"mac"/"cursors");
	ResMgr::pushPath<FontRsrc>(progDir/"data"/"fonts");
	ResMgr::pushPath<StringRsrc>(progDir/"data"/"strings");
	ResMgr::pushPath<SoundRsrc>(progDir/"Scenario Editor"/"sounds.exa");
	
	// We need a location for temporary files, primarily for loading and saving operations
	// The scenario editor may also use this location as "scratch space"
#if defined(_WIN32) || defined(_WIN64)
	tempDir = getenv("APPDATA");
	tempDir /= "Blades of Exile";
#else
	tempDir = getenv("HOME");
#ifdef __APPLE__
	tempDir /= "Library/Application Support/Blades of Exile";
#else
	tempDir /= ".oboe/blades";
#endif // __APPLE__
#endif // _Win32||_Win64
	tempDir /= "Temporary Files";
	
	// Depending on the build environment, we may need to redirect stdout and stderr.
#ifdef _MSC_VER
#ifdef DEBUG
	void set_debug_buffers();
	set_debug_buffers();
#else
	std::string logpath = (tempDir.parent_path()/"bladeslog.txt").string();
	logfile.open(logpath.c_str(), std::ios::out);
	std::cout.rdbuf(&logfile);
	std::cerr.rdbuf(&logfile);
#endif
	std::cout << "Testing cout" << std::endl;
	std::cerr << "Testing cerr" << std::endl;
	sf::err().rdbuf(std::cerr.rdbuf());
#endif
	std::cout << progDir << std::endl;
}

void check_for_intel() {
	union {uint16_t x; uint8_t c;} endian;
	endian.x = 1;
	mac_is_intel = endian.c;
}

std::string read_maybe_quoted_string(std::istream& from) {
	std::string result;
	from >> std::ws;
	if(from.peek() == '"' ||  from.peek() == '\'') {
		char delim = from.get();
		getline(from, result, delim);
		if(result.empty()) return result;
		while(result[result.length() - 1] == '\\') {
			result += delim;
			std::string nextPart;
			getline(from, nextPart, delim);
			result += nextPart;
		}
	} else from >> result;
	return result;
}

std::string maybe_quote_string(std::string which) {
	if(which.find_first_of(' ') != std::string::npos || which[0] == '"' || which[0] == '\'') {
		// The string contains spaces or starts with a quote, so quote it.
		// We may have to escape quotes or backslashes.
		int apos = 0, quot = 0, bslash = 0;
		std::for_each(which.begin(), which.end(), [&apos,&quot,&bslash](char c) {
			if(c == '\'') apos++;
			if(c == '"') quot++;
			if(c == '\\') bslash++;
		});
		char quote_c;
		// Surround it in whichever quote character appears fewer times.
		if(quot < apos) quote_c = '"';
		else quote_c = '\'';
		// Let's create this string to initially have the required size.
		std::string temp;
		size_t quoted_len = which.length() + std::min(quot,apos) + bslash + 2;
		temp.reserve(quoted_len);
		temp += quote_c;
		for(size_t i = 0; i < which.length(); i++) {
			if(which[i] == quote_c) {
				temp += '\\';
				temp += quote_c;
			} else if(which[i] == '\\')
				temp += R"(\\)";
			else temp += which[i];
		}
		temp += quote_c;
		which.swap(temp);
	}
	return which;
}

std::ostream& std_fmterr(std::ostream& out) {
	return out << strerror(errno);
}
