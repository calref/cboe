/*
 *  restypes.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 10-08-25.
 *
 */

#ifndef BOE_RES_CURSOR_HPP
#define BOE_RES_CURSOR_HPP

#include <fstream>
#include <boost/filesystem/path.hpp>
#include <SFML/Graphics.hpp>
#include "resmgr.hpp"
#include "cursors.hpp"

using CursorRsrc = Cursor;

// Redeclare this instead of including "fileio.h"
extern std::ostream& std_fmterr(std::ostream& out);

namespace ResMgr {
	/// Load a cursor from a GIF file.
	/// The cursor's hotspot location is stored in a GIF comment, with the following syntax (case-sensitive):
	/// "Hotspot(x,y)"
	template<> inline CursorRsrc* resLoader<CursorRsrc>::operator() (fs::path fpath) {
		if(!fs::exists(fpath))
			throw xResMgrErr("Failed to load GIF cursor: " + fpath.string());
		int x = 0, y = 0, f_sz;
		std::ifstream fin(fpath.string().c_str(), std::ios::binary);
		fin.seekg(0, std::ios::end);
		f_sz = fin.tellg();
		fin.clear();
		fin.seekg(0, std::ios::beg);
		bool found_hotspot = false;
		while(fin && !found_hotspot) {
			unsigned char c = fin.get();
			if(c != 0x21) continue;
			c = fin.get();
			if(c != 0xfe) continue;
			// If we get here, we've probably found a GIF comment
			std::string str;
			int count;
			found_hotspot = true;
			do {
				count = fin.get();
				if(count + fin.tellg() >= f_sz) {
					found_hotspot = false;
					break;
				}
				std::copy_n(std::istream_iterator<std::string::value_type>(fin), count, std::back_inserter(str));
			} while(count > 0);
			if(found_hotspot) {
				if(str.substr(0,7) == "Hotspot") {
					size_t open_paren = str.find_first_of('('), comma = str.find_first_of(','), close_paren = str.find_first_of(')');
					std::string x_str = str.substr(open_paren + 1, comma - open_paren - 1);
					std::string y_str = str.substr(comma + 1, close_paren - comma - 1);
					x = std::stoi(x_str);
					y = std::stoi(y_str);
				} else found_hotspot = false;
			}
		}
		if(!found_hotspot)
			std::cerr << "Cursor hotspot missing: " << fpath.string() << std::endl;
		// TODO: Handle errors?
		CursorRsrc* cur = new Cursor(fpath.string(),x,y);
		return cur;
	}
}

#endif
