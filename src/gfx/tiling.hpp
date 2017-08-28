//
//  tiling.hpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-14.
//
//

#ifndef BoE_TILING_HPP
#define BoE_TILING_HPP

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include "location.hpp"

struct tessel_ref_t {
	int key;
};

bool operator==(const tessel_ref_t& a, const tessel_ref_t& b);

void init_tiling();
tessel_ref_t prepareForTiling(sf::Texture& srcImg, rectangle srcRect);
void tileImage(sf::RenderTarget& target, rectangle area, tessel_ref_t tessel, sf::BlendMode mode = sf::BlendNone);
void tileImage(sf::RenderWindow& target, class Region& rgn, tessel_ref_t tessel, sf::BlendMode mode = sf::BlendNone);

extern tessel_ref_t bg[];

#endif
