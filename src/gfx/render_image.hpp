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
#include "dialogxml/widgets/pictypes.hpp"

void init_shaders();
void rect_draw_some_item(sf::RenderTarget& targ_gworld,rectangle targ_rect);
void rect_draw_some_item(const sf::Texture& src_gworld,rectangle src_rect,sf::RenderTarget& targ_gworld,rectangle targ_rect,sf::BlendMode mode = sf::BlendNone);
void rect_draw_some_item(const sf::Texture& src_gworld,rectangle src_rect,sf::RenderTarget& targ_gworld,rectangle targ_rect,rectangle in_frame,sf::BlendMode mode = sf::BlendNone);
void rect_draw_some_item(sf::RenderTexture& src_render_gworld,rectangle src_rect,sf::RenderTarget& targ_gworld,rectangle targ_rect,sf::BlendMode mode = sf::BlendNone);
void rect_draw_some_item(const sf::Texture& src_gworld,rectangle src_rect,const sf::Texture& mask_gworld,sf::RenderTarget& targ_gworld,rectangle targ_rect);
void rect_draw_some_item(sf::RenderTexture& src_render_gworld,rectangle src_rect,const sf::Texture& mask_gworld,sf::RenderTarget& targ_gworld,rectangle targ_rect);
void draw_splash(const sf::Texture& splash, sf::RenderWindow& targ, rectangle dest_rect);

// NEVER call setActive() directly.
void enableGL(sf::RenderTarget& targ, std::string name, fs::path file, int line);
void disableGL(sf::RenderTarget& targ, std::string name);
#define ENABLEGL(targ) enableGL(targ, #targ, __FILE__, __LINE__)
#define DISABLEGL(targ) disableGL(targ, #targ)

#endif

