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

sf::Shader maskShader;
extern fs::path progDir;
// TODO: Shouldn't need this
extern sf::RenderWindow mainPtr;

void init_shaders() {
	fs::path shaderPath = progDir/"data"/"shaders";
	fs::path fragPath = shaderPath/"mask.frag", vertPath = shaderPath/"mask.vert";
	
	do {
		std::ifstream fin;
		fin.open(fragPath.string().c_str());
		if(!fin.good()) {
			std::cerr << std_fmterr << ": Error loading fragment shader" << std::endl;
			break;
		}
		fin.seekg(0, std::ios::end);
		int size = fin.tellg();
		fin.seekg(0);
		char* fbuf = new char[size + 1];
		fbuf[size] = 0;
		fin.read(fbuf, size);
		fbuf[fin.gcount()] = 0;
		fin.close();

		fin.open(vertPath.string().c_str());
		if(!fin.good()) {
			std::cerr << std_fmterr << ": Error loading vertex shader" << std::endl;
			delete[] fbuf;
			break;
		}
		fin.seekg(0, std::ios::end);
		size = fin.tellg();
		fin.seekg(0);
		char* vbuf = new char[size + 1];
		vbuf[size] = 0;
		fin.read(vbuf, size);
		vbuf[fin.gcount()] = 0;
	
		if(!maskShader.loadFromMemory(vbuf, fbuf)) {
			std::cerr << "Error: Failed to load shaders from " << shaderPath << "\nVertex:\n" << vbuf << "\nFragment:\n" << fbuf << std::endl;
		}
		delete[] fbuf;
		delete[] vbuf;
	} while(false);
}

void draw_splash(const sf::Texture& splash, sf::RenderWindow& targ, rectangle dest_rect) {
	rectangle from_rect = rectangle(splash);
	targ.clear(sf::Color::Black);
	rect_draw_some_item(splash, from_rect, targ, dest_rect);
	targ.display();
}

static void rect_draw_some_item(const sf::Texture& src_gworld,rectangle src_rect,sf::RenderTarget& targ_gworld,rectangle targ_rect,sf::RenderStates mode);

void rect_draw_some_item(sf::RenderTarget& targ_gworld,rectangle targ_rect) {
	fill_rect(targ_gworld, targ_rect, sf::Color::Black);
}

void rect_draw_some_item(const sf::Texture& src_gworld,rectangle src_rect,sf::RenderTarget& targ_gworld,rectangle targ_rect,sf::BlendMode mode){
	rect_draw_some_item(src_gworld, src_rect, targ_gworld, targ_rect, sf::RenderStates(mode));
}

void rect_draw_some_item(const sf::Texture& src_gworld,rectangle src_rect,sf::RenderTarget& targ_gworld,rectangle targ_rect,sf::RenderStates mode) {
	setActiveRenderTarget(targ_gworld);
	sf::Sprite tile(src_gworld, src_rect);
	tile.setPosition(targ_rect.left, targ_rect.top);
	double xScale = targ_rect.width(), yScale = targ_rect.height();
	xScale /= src_rect.width();
	yScale /= src_rect.height();
	tile.setScale(xScale, yScale);
	targ_gworld.draw(tile, mode);
}

void rect_draw_some_item(const sf::Texture& src_gworld,rectangle src_rect,rectangle targ_rect,location offset, sf::BlendMode mode) {
	targ_rect.offset(offset);
	rect_draw_some_item(src_gworld,src_rect,mainPtr,targ_rect,mode);
}

void rect_draw_some_item(const sf::Texture& src_gworld,rectangle src_rect,const sf::Texture& mask_gworld,sf::RenderTarget& targ_gworld,rectangle targ_rect) {
	static sf::RenderTexture src;
	static bool inited = false;
	if(!inited || src_rect.width() != src.getSize().x || src_rect.height() != src.getSize().y) {
		src.create(src_rect.width(), src_rect.height());
		inited =  true;
	}
	rectangle dest_rect = src_rect;
	dest_rect.offset(-dest_rect.left,-dest_rect.top);
	rect_draw_some_item(src_gworld, src_rect, src, dest_rect);
	src.display();
	
	maskShader.setParameter("texture", sf::Shader::CurrentTexture);
	maskShader.setParameter("mask", mask_gworld);
	rect_draw_some_item(src.getTexture(), dest_rect, targ_gworld, targ_rect, &maskShader);
}

void setActiveRenderTarget(sf::RenderTarget& where) {
	const std::type_info& type = typeid(where);
	if(type == typeid(sf::RenderWindow&))
		dynamic_cast<sf::RenderWindow&>(where).setActive();
	else if(type == typeid(sf::RenderTexture&))
		dynamic_cast<sf::RenderTexture&>(where).setActive();
	else throw std::bad_cast();
}
