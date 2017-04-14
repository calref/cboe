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
void rect_draw_some_item(sf::RenderTarget& targ_gworld,rectangle targ_rect);
void rect_draw_some_item(const sf::Texture& src_gworld,rectangle src_rect,sf::RenderTarget& targ_gworld,rectangle targ_rect,sf::BlendMode mode = sf::BlendNone);
void rect_draw_some_item(const sf::Texture& src_gworld,rectangle src_rect,rectangle targ_rect,location offset,sf::BlendMode mode = sf::BlendNone);
void rect_draw_some_item(const sf::Texture& src_gworld,rectangle src_rect,const sf::Texture& mask_gworld,sf::RenderTarget& targ_gworld,rectangle targ_rect);
void draw_splash(const sf::Texture& splash, sf::RenderWindow& targ, rectangle dest_rect);

void setActiveRenderTarget(sf::RenderTarget& where);

#endif
