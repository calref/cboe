/*
 *  restypes.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 10-08-25.
 *
 */

#include "res_image.hpp"

class ImageLoader : public ResMgr::cLoader<sf::Texture> {
	/// Load an image from a PNG file.
	sf::Texture* operator() (const fs::path& fpath) const override {
		sf::Texture* img = new sf::Texture();
		if(img->loadFromFile(fpath.string())) return img;
		delete img;
		throw ResMgr::xError(ResMgr::ERR_LOAD, "Failed to load PNG image: " + fpath.string());
	}

	ResourceList expand(const std::string& name) const override {
		return {name + ".png", name + ".bmp"};
	}

	std::string typeName() const override {
		return "image";
	}
};

// TODO: What's a good max texture count?
static ImageLoader loader;
ResMgr::cPool<sf::Texture> ResMgr::graphics(loader, 50);
