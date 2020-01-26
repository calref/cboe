/*
 *  restypes.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 10-08-25.
 *
 */

#include "res_strings.hpp"
#include <iostream>

// Redeclare this instead of including "fileio.h"
extern std::ostream& std_fmterr(std::ostream& out);

class StringsLoader : public ResMgr::cLoader<StringList> {
	/// Load a list of strings from a TXT file.
	/// Each line in the file becomes one string in the resulting list.
	/// (Empty lines are included too.)
	StringList* operator() (const fs::path& fpath) const override {
		std::ifstream fin(fpath.string().c_str());
		if(fin.fail()) {
			std::cerr << std_fmterr << ": Error opening file";
			throw ResMgr::xError(ResMgr::ERR_LOAD, "Failed to load string list: " + fpath.string());
		}
		std::string next;
		StringList* strlist = new StringList;
		while(!fin.eof()) {
			getline(fin,next);
			strlist->push_back(next);
		}
		return strlist;
	}

	ResourceList expand(const std::string& name) const override {
		return {name + ".txt"};
	}

	std::string typeName() const override {
		return "string list";
	}
};

// TODO: What's a good max strings count?
StringsLoader loader;
ResMgr::cPool<StringList> ResMgr::strings(loader, 100);
