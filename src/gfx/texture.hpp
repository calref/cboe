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
#include <utility>

#include <SFML/Graphics.hpp>

#include "location.hpp"

struct Texture {
	Texture()
	: texture()
	, dimension(0,0)
	{
	}
	Texture(Texture const &)=default;
	Texture(sf::Texture const &tex)
	: texture(std::make_shared<sf::Texture>(tex))
	, dimension(0,0)
	{
		dimension={tex.getSize().x, tex.getSize().y};
	}
	Texture(std::shared_ptr<const sf::Texture> const &tex)
	: texture(tex)
	, dimension(0,0)
	{
		if (tex)
			dimension={tex->getSize().x, tex->getSize().y};
	}
	Texture &operator=(Texture const &)=default;

	operator bool() const {
		return bool(texture);
	}
	sf::Texture const &operator*() const {
		return *texture;
	}
	sf::Texture const *operator->() const {
		return texture.get();
	}
	bool operator==(Texture const &orig) const {
		return texture.get()==orig.texture.get() && dimension==orig.dimension;
	}
	bool operator!=(Texture const &orig) const {
		return !(*this==orig);
	}
	operator rectangle() const {
		return rectangle(0, 0, dimension.y, dimension.x);
	}
	static sf::Vector2u getApplicationDimension(std::string const &name) {
		if (name.size()<5 || name.substr(name.size()-4)!=".png") return {0,0};
		std::string const base=name.substr(0,name.size()-4);
		static std::map<std::string, sf::Vector2u> nameToDimensions = {
			 { "actionhelp", {275,100} },
			 { "bigscenpics", {256,64} },
			 { "blank", {28,36} },
			 { "booms", {224,252} },
			 { "buttons", {192,115} },
			 { "bwpats", {48,8} },
			 { "dlgbtnred", {60,30} },
			 { "dlogbtnhelp", {32,13} },
			 { "dlogbtnled", {56,26} },
			 { "dlogbtnlg", {204,23} },
			 { "dlogbtnmed", {126,138} },
			 { "dlogbtnsm", {46,23} },
			 { "dlogbtntall", {126,80} },
			 { "dlogpics", {144,360} },
			 { "dlogscrollled", {56,48} },
			 { "dlogscrollwh", {64,64} },
			 { "edbuttons", {251,164} },
			 { "edsplash", {640,480} },
			 { "fields", {224,144} },
			 { "fighthelp", {320,125} },
			 { "icon", {38,38} },
			 { "invenbtns", {130,80} },
			 { "invenhelp", {138,112} },
			 { "inventory", {271,144} },
			 { "missiles", {144,288} },
			 { "objects", {140,396} },
			 { "outhelp", {320,125} },
			 { "pcedbuttons", {114,57} },
			 { "pcedtitle", {310,70} },
			 { "pcs", {280,576} },
			 { "pixpats", {320, 256} },
			 { "scenpics", {160,224} },
			 { "spidlogo", {350,350} },
			 { "startanim", {280,590} },
			 { "startbut", {301,48} },
			 { "startsplash", {640,480} },
			 { "startup", {602,322} },
			 { "statarea", {271,116} },
			 { "stathelp", {106,29} },
			 { "staticons", {36,120} },
			 { "talkportraits", {320,288} },
			 { "teranim", {672,180} },
			 { "termap", {252,576} },
			 { "terscreen", {278,350} },
			 { "textbar", {279,22} },
			 { "tinyobj", {180,252} },
			 { "townhelp", {320,125} },
			 { "transcript", {256,138} },
			 { "trim", {224,72} },
			 { "vehicle", {112,108} },
		 };
		 auto const &it=nameToDimensions.find(base);
		 if (it!=nameToDimensions.end())
			 return it->second;
		// now check the special case terXXX.png and monstXXX.png
		if (base.size()>=4 && base.substr(0,3)=="ter" && base.substr(3).find_first_not_of( "0123456789" ) == std::string::npos)
			return {280,180};
		 if (base.size()>=6 && base.substr(0,5)=="monst" && base.substr(5).find_first_not_of( "0123456789" ) == std::string::npos)
			 return {224,360};
		 return {0,0};
	}
	std::shared_ptr<const sf::Texture> texture;
	sf::Vector2u dimension;
};

typedef std::pair<Texture,rectangle> Texture_pos;
#endif /* texture_h */
