/*
 *  restypes.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 10-08-25.
 *
 */

#include "res_image.hpp"

#include "gfx/texture.hpp"

class TextureLoader : public ResMgr::cLoader<Texture> {
	/// Load an image from a PNG file.
	Texture* operator() (const fs::path& fpath) const override {
		auto img=std::make_shared<sf::Texture>();
		if(img->loadFromFile(fpath.string())) {
			Texture *texture=new Texture;
			texture->texture=img;
			texture->dimension=Texture::getApplicationDimension(fpath.filename().string(),sf::Vector2u(img->getSize()));
			return texture;
		}
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
static TextureLoader texturesLoader;
ResMgr::cPool<Texture> ResMgr::textures(texturesLoader, 50);

