/*
 *  restypes.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 10-08-25.
 *
 */

#ifndef BOE_RES_STRINGS_HPP
#define BOE_RES_STRINGS_HPP

#include <fstream>
#include <boost/filesystem/path.hpp>
#include "resmgr.hpp"

using StringRsrc = std::vector<std::string>;

// Redeclare this instead of including "fileio.h"
extern std::ostream& std_fmterr(std::ostream& out);

namespace ResMgr {
	/// Load a list of strings from a TXT file.
	/// Each line in the file becomes one string in the resulting list.
	/// (Empty lines are included too.)
	template<> inline StringRsrc* resLoader<StringRsrc>::operator() (fs::path fpath) {
		std::ifstream fin(fpath.c_str());
		if(fin.fail()) {
			std::cerr << std_fmterr << ": Error opening file";
			throw xResMgrErr("Failed to load string list: " + fpath.string());
		}
		std::string next;
		StringRsrc* strlist = new StringRsrc;
		while(!fin.eof()) {
			getline(fin,next);
			strlist->push_back(next);
		}
		return strlist;
	}
}

#endif
