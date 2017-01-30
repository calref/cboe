/*
 *  restypes.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 10-08-25.
 *
 */

#ifndef BOE_RESTYPES_H
#define BOE_RESTYPES_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "resmgr.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <boost/filesystem/path.hpp>
#include "cursors.hpp"
#include "location.hpp"

using ImageRsrc = sf::Texture;
using CursorRsrc = cCursor;
using FontRsrc = sf::Font;
using StringRsrc = std::vector<std::string>;
using SoundRsrc = sf::SoundBuffer;

// Redeclare this instead of including "fileio.h"
extern std::ostream& std_fmterr(std::ostream& out);

namespace ResMgr {
	/// Load an image from a PNG file.
	template<> inline ImageRsrc* resLoader<ImageRsrc>::operator() (fs::path fpath) {
		ImageRsrc* img = new ImageRsrc();
		if(img->loadFromFile(fpath.string())) return img;
		delete img;
		throw xResMgrErr("Failed to load PNG image: " + fpath.string());
	}
	
	/// Load a cursor from a GIF file.
	/// The cursor's hotspot location is stored in a GIF comment, with the following syntax (case-sensitive):
	/// "Hotspot(x,y)"
	template<> inline CursorRsrc* resLoader<CursorRsrc>::operator() (fs::path fpath) {
		if(!fs::exists(fpath))
			throw xResMgrErr("Failed to load GIF cursor: " + fpath.string());
		int x = 0, y = 0, f_sz;
		std::ifstream fin(fpath.c_str(), std::ios::binary);
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
		CursorRsrc* cur = new cCursor(fpath.string(),x,y);
		return cur;
	}
	
	/// Load a font from a TTF file.
	template<> inline FontRsrc* resLoader<FontRsrc>::operator() (fs::path fpath) {
		FontRsrc* theFont = new FontRsrc;
		if(theFont->loadFromFile(fpath.string())) return theFont;
		delete theFont;
		throw xResMgrErr("Failed to find font: " + fpath.string());
	}
	
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
	
	/// Load a sound from a WAV file.
	template<> inline SoundRsrc* resLoader<SoundRsrc>::operator() (fs::path fpath) {
		SoundRsrc* snd = new SoundRsrc;
		if(snd->loadFromFile(fpath.string())) return snd;
		delete snd;
		throw xResMgrErr("Failed to load WAV sound: " + fpath.string());
	}
}

#endif
