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
#include <boost/filesystem/path.hpp>

struct cCursor {
	sf::Image img;
	int xHot, yHot;
};

using ImageRsrc = sf::Image;
using CursorRsrc = cCursor;
using FontRsrc = sf::Font;
using StringRsrc = std::vector<std::string>;
using SoundRsrc = sf::SoundBuffer;

namespace ResMgr {
	template<> inline ImageRsrc* resLoader<ImageRsrc>::operator() (std::string fname) {
//		static bool inited = false;
//		if(!inited) {
//			wxImage::AddHandler(new wxPNGHandler());
//		}
		fs::path fpath = resPool<ImageRsrc>::rel2abs(fname + ".png");
		ImageRsrc* img = new ImageRsrc();
		if(img->loadFromFile(fpath.string())) return img;
		delete img;
		throw xResMgrErr("Failed to load PNG image: " + fpath.string());
	}
	
	template<> inline CursorRsrc* resLoader<CursorRsrc>::operator() (std::string fname) {
//		static bool inited = false;
//		if(!inited) {
//			wxImage::AddHandler(new wxGIFHandler());
//		}
		fs::path fpath = resPool<CursorRsrc>::rel2abs(fname + ".gif");
		fs::path hotpath = resPool<CursorRsrc>::rel2abs(fname + ".hot");
		int x = 0, y = 0;
		if(fs::exists(hotpath)) {
			std::ifstream fin(hotpath.c_str());
			fin >> x >> y;
			fin.close();
		} else fprintf(stderr,"Cursor hotspot missing: %s",fname.c_str());
		sf::Image img;
		if(img.loadFromFile(fpath.string())) {
			CursorRsrc* cur = new CursorRsrc{img,x,y};
			return cur;
		}
		throw xResMgrErr("Failed to load GIF cursor: " + fname);
	}
	
	template<> inline FontRsrc* resLoader<FontRsrc>::operator() (std::string fname) {
		fs::path fpath = resPool<FontRsrc>::rel2abs(fname + ".ttf");
		FontRsrc* theFont = new FontRsrc;
		if(theFont->loadFromFile(fpath.string())) return theFont;
		fpath = resPool<FontRsrc>::rel2abs(fname + ".otf");
		if(theFont->loadFromFile(fpath.string())) return theFont;
		delete theFont;
		throw xResMgrErr("Failed to find font: " + fpath.string());
	}
	
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
	
	template<> inline SoundRsrc* resLoader<SoundRsrc>::operator() (std::string fname) {
		fs::path fpath = resPool<SoundRsrc>::rel2abs(fname + ".wav");
		SoundRsrc* snd = new SoundRsrc;
		if(snd->loadFromFile(fpath.string())) return snd;
		delete snd;
		throw xResMgrErr("Failed to load WAV sound: " + fpath.string());
	}
}

#endif
