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
#include <map>
#include <boost/filesystem/path.hpp>
#include "cursors.h"
#include "location.h"

using ImageRsrc = sf::Image;
using CursorRsrc = Cursor;
using FontRsrc = sf::Font;
using StringRsrc = std::vector<std::string>;
using SoundRsrc = sf::SoundBuffer;

namespace ResMgr {
	/// Load an image from a PNG file.
	template<> inline ImageRsrc* resLoader<ImageRsrc>::operator() (std::string fname) {
		fs::path fpath = resPool<ImageRsrc>::rel2abs(fname + ".png");
		ImageRsrc* img = new ImageRsrc();
		if(img->loadFromFile(fpath.string())) return img;
		delete img;
		throw xResMgrErr("Failed to load PNG image: " + fpath.string());
	}
	
	/// Load a cursor from a GIF file.
	/// The cursor's hotspot location is stored in a .hot file, as plaintext:
	/// simply the X value followed by the Y value, separated by whitespace.
	template<> inline CursorRsrc* resLoader<CursorRsrc>::operator() (std::string fname) {
		// TODO: Store the hotspots on disk instead of hardcoded here
		static const std::map<std::string,location> cursor_hs = {
			{"wand", {1, 4}}, {"eyedropper", {14, 1}}, {"brush", {13, 5}}, {"spraycan", {8, 8}},
			{"eraser", {8, 8}}, {"topleft", {8, 8}}, {"bottomright", {8, 8}}, {"hand", {0, 14}},
			{"NW", {3, 12}}, {"N", {7, 13}}, {"NE", {3, 12}},
			{"W", {3, 9}}, {"wait", {8, 8}}, {"E", {8, 3}},
			{"SW", {3, 12}}, {"S", {7, 13}}, {"SE", {3, 12}},
			{"sword", {1, 1}}, {"boot", {7, 3}}, {"drop", {0, 14}}, {"target", {8, 8}},
			{"talk", {6, 7}}, {"key", {3, 2}}, {"look", {7, 6}}, {"watch", {7,8}},
		};
		fs::path fpath = resPool<CursorRsrc>::rel2abs(fname + ".gif");
		fs::path hotpath = resPool<CursorRsrc>::rel2abs(fname + ".hot");
		int x = 0, y = 0;
		if(fs::exists(hotpath)) {
			std::ifstream fin(hotpath.c_str());
			fin >> x >> y;
			fin.close();
		} else {
			auto entry = cursor_hs.find(fname);
			if(entry == cursor_hs.end())
				fprintf(stderr,"Cursor hotspot missing: %s\n",fname.c_str());
			else {
				fprintf(stderr,"Cursor hotspot missing (using fallback value): %s\n",fname.c_str());
				location hs = entry->second;
				x = hs.x; y = hs.y;
			}
		}
		// TODO: Handle errors?
		CursorRsrc* cur = new Cursor(fpath.c_str(),x,y);
		return cur;
		throw xResMgrErr("Failed to load GIF cursor: " + fname);
	}
	
	/// Load a font form a TTF file.
	template<> inline FontRsrc* resLoader<FontRsrc>::operator() (std::string fname) {
		fs::path fpath = resPool<FontRsrc>::rel2abs(fname + ".ttf");
		FontRsrc* theFont = new FontRsrc;
		if(theFont->loadFromFile(fpath.string())) return theFont;
		fpath = resPool<FontRsrc>::rel2abs(fname + ".otf");
		if(theFont->loadFromFile(fpath.string())) return theFont;
		delete theFont;
		throw xResMgrErr("Failed to find font: " + fpath.string());
	}
	
	/// Load a list of strings from a TXT file.
	/// Each line in the file becomes one string in the resulting list.
	/// (Empty lines are included too.)
	template<> inline StringRsrc* resLoader<StringRsrc>::operator() (std::string fname) {
		fs::path fpath = resPool<StringRsrc>::rel2abs(fname + ".txt");
		std::ifstream fin(fpath.c_str());
		if(fin.fail()) {
			perror("Error opening file");
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
	template<> inline SoundRsrc* resLoader<SoundRsrc>::operator() (std::string fname) {
		fs::path fpath = resPool<SoundRsrc>::rel2abs(fname + ".wav");
		SoundRsrc* snd = new SoundRsrc;
		if(snd->loadFromFile(fpath.string())) return snd;
		delete snd;
		throw xResMgrErr("Failed to load WAV sound: " + fpath.string());
	}
}

#endif
