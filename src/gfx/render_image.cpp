/*
 *  graphtool.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#include "render_image.hpp"

#include <iostream>
#include <fstream>
#include <boost/filesystem/path.hpp>

#include "fileio.hpp"
#include "render_shapes.hpp"
#include "res_image.hpp"

static sf::Shader maskShader, tintShader;
extern fs::path progDir;

void init_shaders() {
	fs::path shaderPath = progDir/"data"/"shaders";
	fs::path fragPath = shaderPath/"mask.frag", vertPath = shaderPath/"mask.vert", tintPath = shaderPath/"tint.frag";
	
	do {
		if(!maskShader.loadFromFile(vertPath.string(), fragPath.string())) {
			std::cerr << "Error: Failed to load shaders from " << shaderPath << "\nVertex:\n" << vertPath.string() << "\nFragment:\n" << fragPath.string()  << std::endl;
		}
		if(!tintShader.loadFromFile(tintPath.string(), sf::Shader::Fragment)) {
			std::cerr << "Error: Failed to load shaders from " << shaderPath << "\nFragment:\n" << tintPath.string()  << std::endl;
		}
	} while(false);
}

void draw_splash(const Texture& splash, sf::RenderWindow& targ, rectangle dest_rect) {
	targ.clear(sf::Color::Black);
	rect_draw_some_item(splash, rectangle(splash), targ, dest_rect);
	targ.display();
}

static void rect_draw_some_item(const Texture& src_gworld,rectangle src_rect,sf::RenderTarget& targ_gworld,rectangle targ_rect,
				sf::RenderStates const &mode,  sf::Uint32 colorMod) {
	setActiveRenderTarget(targ_gworld);
	src_rect=src_rect.rescale(src_gworld.dimension, src_gworld->getSize());
	sf::Sprite tile(*src_gworld, src_rect);
	tile.setPosition(targ_rect.left, targ_rect.top);
	double xScale = targ_rect.width(), yScale = targ_rect.height();
	xScale /= src_rect.width();
	yScale /= src_rect.height();
	tile.setScale(xScale, yScale);
	if(colorMod != 0) {
		sf::Color color(colorMod);
		tintShader.setUniform("tintColor", sf::Glsl::Vec4(float(color.r)/255, float(color.g)/255, float(color.b)/255, float(color.a)/255));
		targ_gworld.draw(tile, &tintShader);
	}
	else
		targ_gworld.draw(tile, mode);
}

void rect_draw_some_item(const Texture& src_gworld,rectangle src_rect,sf::RenderTarget& targ_gworld,rectangle targ_rect,
						 RenderState const &mode) {
	if (mode.mask==nullptr) {
		rect_draw_some_item(src_gworld, src_rect, targ_gworld, targ_rect, sf::RenderStates(mode.blend_mode), mode.color_mod);
		return;
	}
	
	rectangle real_src_rect=src_rect.rescale(src_gworld.dimension, src_gworld->getSize());
	static sf::RenderTexture src;
	static bool inited = false;
	if(!inited || real_src_rect.width() != src.getSize().x || real_src_rect.height() != src.getSize().y) {
			src.create(real_src_rect.width(), real_src_rect.height());
			inited =  true;
	}
	rectangle dest_rect = real_src_rect;
	dest_rect.offset(-dest_rect.left,-dest_rect.top);
	rect_draw_some_item(src_gworld, src_rect, src, dest_rect, sf::RenderStates(sf::BlendNone), 0);
	src.display();

	maskShader.setParameter("texture", sf::Shader::CurrentTexture);
	maskShader.setParameter("mask", *mode.mask);
	rect_draw_some_item(Texture(src.getTexture()), dest_rect, targ_gworld, targ_rect, &maskShader, mode.color_mod);
}

void setActiveRenderTarget(sf::RenderTarget& where) {
	const std::type_info& type = typeid(where);
	if(type == typeid(sf::RenderWindow&))
		dynamic_cast<sf::RenderWindow&>(where).setActive();
	else if(type == typeid(sf::RenderTexture&))
		dynamic_cast<sf::RenderTexture&>(where).setActive();
	else throw std::bad_cast();
}
