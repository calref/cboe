/*
 *  restypes.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 10-08-25.
 *
 */

#include "res_font.hpp"

class FontLoader : public ResMgr::cLoader<sf::Font> {
	/// Load a font from a TTF or BDF file.
	sf::Font* operator() (const fs::path& fpath) const override {
		sf::Font* theFont = new sf::Font;
		if(theFont->loadFromFile(fpath.string())) return theFont;
		delete theFont;
		throw ResMgr::xError(ResMgr::ERR_LOAD, "Failed to find font: " + fpath.string());
	}

	ResourceList expand(const std::string& name) const override {
		return {name + ".ttf", name + ".bdf"};
	}

	std::string typeName() const override {
		return "font";
	}
};

// We'll allow all fonts to be loaded simultaneously (and leave some leeway in case a few more fonts are added)
FontLoader loader;
ResMgr::cPool<sf::Font> ResMgr::fonts(loader, 10);
