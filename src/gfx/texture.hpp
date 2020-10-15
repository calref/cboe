//
//  texture.hpp
//  Common Data Files
//
//  Created by alonso on 15/10/2020.
//

#ifndef BoE_TEXTURE_HPP
#define BoE_TEXTURE_HPP

#include <memory>
#include <string>

#include <SFML/Graphics.hpp>

#include "location.hpp"

struct Texture {
    operator bool() const {
        return bool(texture);
    }
    sf::Texture const &operator*() const {
        return *texture;
    }
    sf::Texture const *operator->() const {
        return texture.get();
    }
    sf::Texture *operator->() {
        return texture.get();
    }
    operator rectangle() const {
        return rectangle(0, 0, dimension.y, dimension.x);
    }
    static sf::Vector2u getApplicationDimension(std::string const &name) {
        static std::map<std::string, sf::Vector2u> nameToDimensions = {
             { "buttons.png", {192,115} },
             { "spidlogo.png", {350,350} },
             { "inventory.png", {271,144} },
             { "pcedbuttons", {114,57} },
             { "startanim.png", {280,590} },
             { "startbut.png", {301,48} },
             { "startsplash.png", {640,480} },
             { "startup.png", {602, 322} },
             { "statarea.png", {271,116} },
             { "terscreen.png", {278,350} },
             { "textbar.png", {279,22} },
             { "transcript.png", {256,138} },
         };
         auto const &it=nameToDimensions.find(name);
         if (it!=nameToDimensions.end())
             return it->second;
         else
             return {0,0};
    }
    std::shared_ptr<sf::Texture> texture;
    sf::Vector2u dimension;
};

#endif /* texture_h */
