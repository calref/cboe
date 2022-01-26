/*
 *  graphtool.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#ifndef GRAPHTOOL_H
#define GRAPHTOOL_H

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <boost/filesystem/path.hpp>
#include <SFML/Graphics.hpp>
#include "location.hpp"
#include "pictypes.hpp"

void init_shaders();

struct RenderState {
	RenderState() = default;
	RenderState(sf::BlendMode blendMode, unsigned long colorMod=0)
	: blend_mode(blendMode)
	, color_mod(colorMod) {
	}
	RenderState(RenderState const &)=default;
	RenderState &operator=(RenderState const &)=default;
	void set_mask(const sf::Texture& mask_gworld) {
		mask=&mask_gworld;
	}
	sf::BlendMode blend_mode = sf::BlendNone;
	unsigned long color_mod=0;
	sf::Texture const *mask=nullptr;
};

struct Texture;
void rect_draw_some_item(const Texture & src_gworld,rectangle src_rect,sf::RenderTarget& targ_gworld,rectangle targ_rect,
						 RenderState const &mode=RenderState());
void draw_splash(const Texture & splash, sf::RenderWindow& targ, rectangle dest_rect);

void setActiveRenderTarget(sf::RenderTarget& where);

#endif
