/*
 *  fileio.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include "fileio.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <boost/filesystem/operations.hpp>

#include "porting.hpp"
#include "fileio/resmgr/res_image.hpp"
#include "fileio/resmgr/res_cursor.hpp"
#include "fileio/resmgr/res_font.hpp"
#include "fileio/resmgr/res_strings.hpp"
#include "fileio/resmgr/res_sound.hpp"

fs::path progDir, tempDir, scenDir;

// This is here to avoid unnecessarily duplicating it in platform-specific files.
cursor_type Cursor::current = sword_curs;

std::filebuf logfile;

#if !defined(_WIN32) && !defined(_WIN64) && !defined(__APPLE__)
static fs::path get_posix_tempdir();
#endif

static void add_resmgr_paths(const fs::path& basePath) {
	ResMgr::textures.pushPath(basePath/"graphics");
	ResMgr::cursors.pushPath(basePath/"cursors");
	ResMgr::fonts.pushPath(basePath/"fonts");
	ResMgr::strings.pushPath(basePath/"strings");
	ResMgr::sounds.pushPath(basePath/"sounds");
}

void init_directories(const char* exec_path) {
	progDir = fs::canonical(exec_path);
#ifdef __APPLE__
	// Need to back up out of the application package
	// We're pointing at .app/Contents/MacOS/exec_name, so back out three steps
	progDir = progDir.parent_path().parent_path().parent_path();
#endif
	progDir = progDir.parent_path();
	// Initialize the resource manager paths
	add_resmgr_paths(progDir/"data");
	
	// We need a location for temporary files, primarily for loading and saving operations
	// The scenario editor may also use this location as "scratch space"
#if defined(_WIN32) || defined(_WIN64)
	tempDir = getenv("APPDATA");
	tempDir /= "Blades of Exile";
#elif defined(__APPLE__)
	tempDir = getenv("HOME");
	tempDir /= "Library/Application Support/Blades of Exile";
#else
	tempDir = get_posix_tempdir();
#endif
	scenDir = tempDir/"Scenarios";
	fs::create_directories(scenDir);
	add_resmgr_paths(tempDir/"data");
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
	// Now print all the paths
	std::cout << "Working directory: " << fs::current_path() << std::endl;
	std::cout << "Program directory: " << progDir << std::endl;
	std::cout << "Scenario directory: " << scenDir << std::endl;
	std::cout << "Temporary directory: " << tempDir << std::endl;
}

#if !defined(_WIN32) && !defined(_WIN64) && !defined(__APPLE__)
fs::path get_posix_tempdir() {

	fs::path tempdir;

	const char* xdg_config_dir = std::getenv("XDG_CONFIG_HOME");

	if(xdg_config_dir != nullptr) {
		tempdir = xdg_config_dir;
	} else {
		// Default to $HOME/.config
		const char* home = std::getenv("HOME");

		if(home == nullptr)
			throw std::runtime_error { "HOME and XDG_CONFIG_HOME env variables not set!" };

		tempdir = home;
		tempdir /= ".config";
	}

	tempdir /= "openboe/blades";

	return tempdir;
}
#endif

void check_for_intel() {
	porting::check_endian();
}

std::string read_maybe_quoted_string(std::istream& from) {
	std::string result;
	from >> std::ws;
	if(from.peek() == '"' || from.peek() == '\'') {
		char delim = from.get();
		getline(from, result, delim);
		if(result.empty()) return result;
		while(result[result.length() - 1] == '\\') {
			result[result.length() - 1] = delim;
			std::string nextPart;
			getline(from, nextPart, delim);
			// Collapse any double backslashes; remove any single backslashes
			for(std::string::iterator iter = nextPart.begin(); iter != nextPart.end(); iter++) {
				if(iter[0] == '\\' && iter + 1 != nextPart.end() && iter[1] != '\\') {
					iter = nextPart.erase(iter);
					// After this, iter points to the second of the two backslashes, so
					// when incremented by the loop, it'll point to the character after the backslashes.
				}
			}
			// Note that this does not support escaping the single quotes in strings delimited by double quotes, and vice versa.
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
