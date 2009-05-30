/*
 *  item.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>


#include "classes.h"
#include "boe.consts.h"
#include "oldstructs.h"

cItemRec k;
unsigned char cItemRec::rec_treas_class() const {
	short tmp = value;
	if(charges > 0) tmp *= charges;
	if(tmp >=   15) return 1;
	if(tmp >=  100) return 2;
	if(tmp >=  900) return 3;
	if(tmp >= 2400) return 4;
	return 0;
}

bool cItemRec::is_ident() const {
	return item_properties & 1;
}

bool cItemRec::is_property() const {
	return item_properties & 2;
}

bool cItemRec::is_magic() const {
	return item_properties & 4;
}

bool cItemRec::is_contained() const {
	return item_properties & 8;
}

bool cItemRec::is_cursed() const {
	return item_properties & 16;
}

bool cItemRec::is_concealed() const {
	return item_properties & 32;
}

bool cItemRec::is_enchanted() const {
	return item_properties & 64;
}

// Set bit to 1: a |= b
// Set bit to 0: a &= ~b
// Toggle bit:   a ^= b

void cItemRec::set_ident(bool b) {
	if(b) item_properties |=  1;
	else  item_properties &= ~1;
}

void cItemRec::set_property(bool b) {
	if(b) item_properties |=  2;
	else  item_properties &= ~2;
}

void cItemRec::set_magic(bool b) {
	if(b) item_properties |=  4;
	else  item_properties &= ~4;
}

void cItemRec::set_contained(bool b) {
	if(b) item_properties |=  8;
	else  item_properties &= ~8;
}

void cItemRec::set_cursed(bool b) {
	if(b) item_properties |=  16;
	else  item_properties &= ~16;
}

void cItemRec::set_concealed(bool b) {
	if(b) item_properties |=  32;
	else  item_properties &= ~32;
}

void cItemRec::set_enchanted(bool b) {
	if(b) item_properties |=  64;
	else  item_properties &= ~64;
}

short cItemRec::item_weight() const {
	short n = charges, w = weight;
	if (variety == 0)
		return 0;
	if(charges > 0){
		if((variety == 5) || (variety == 6) || (variety == 7) || (variety == 24))
			return n * w;
	}
	return w;
}

cItemRec::cItemRec(){
	variety = ITEM_TYPE_NO_ITEM;
	item_level = 0;
	awkward = 0;
	bonus = 0;
	protection = 0;
	charges = 0;
	type = ITEM_NOT_MELEE;
	magic_use_type = 0;
	graphic_num = 0;
	ability = ITEM_NO_ABILITY;
	ability_strength = 0;
	type_flag = 0;
	is_special = 0;
	value = 0;
	weight = 0;
	special_class = 0;
	item_loc.x = 0;
	item_loc.y = 0;
	full_name = "";
	name = "";
	treas_class = 0;
	item_properties = 0;
	reserved1 = 0;
	reserved2 = 0;
}

cItemRec::cItemRec(long preset){
	ability = ITEM_NO_ABILITY;
	ability_strength = 0;
	type_flag = 0;
	is_special = 0;
	special_class = 0;
	item_loc.x = 0;
	item_loc.y = 0;
	treas_class = 0;
	reserved1 = 0;
	reserved2 = 0;
	switch(preset){
		case 'nife':
			variety = ITEM_TYPE_ONE_HANDED;
			item_level = 4;
			awkward = 0;
			bonus = 1;
			protection = 0;
			charges = 0;
			type = ITEM_EDGED;
			magic_use_type = 0;
			graphic_num = 45;
			value = 2;
			weight = 7;
			full_name = "Bronze Knife";
			name = "Knife";
			set_ident(true);
			break;
		case 'buck':
			variety = ITEM_TYPE_SHIELD;
			item_level = 1;
			awkward = 1;
			bonus = 0;
			protection = 0;
			charges = 0;
			type = ITEM_NOT_MELEE;
			magic_use_type = 0;
			graphic_num = 65;
			value = 2;
			weight = 20;
			full_name = "Crude Buckler";
			name = "Buckler";
			set_ident(true);
			break;
		case 'bow ':
			variety = ITEM_TYPE_BOW;
			item_level = 0;
			awkward = 0;
			bonus = 0;
			protection = 0;
			charges = 0;
			type = ITEM_NOT_MELEE;
			magic_use_type = 0;
			graphic_num = 10;
			value = 15;
			weight = 20;
			full_name = "Cavewood Bow";
			name = "Bow";
			set_ident(true);
			break;
		case 'arrw':
			variety = ITEM_TYPE_ARROW;
			item_level = 12;
			awkward = 0;
			bonus = 0;
			protection = 0;
			charges = 12;
			type = ITEM_NOT_MELEE;
			magic_use_type = 0;
			graphic_num = 47;
			value = 1;
			weight = 1;
			full_name = "Arrows";
			name = "Arrows";
			set_ident(true);
			break;
		case 'pole':
			variety = ITEM_TYPE_TWO_HANDED;
			item_level = 9;
			awkward = 0;
			bonus = 0;
			protection = 0;
			charges = 0;
			type = ITEM_POLE;
			magic_use_type = 0;
			graphic_num = 4;
			value = 10;
			weight = 20;
			full_name = "Stone Spear";
			name = "Spear";
			set_ident(true);
			break;
		case 'helm':
			variety = ITEM_TYPE_HELM;
			item_level = 1;
			awkward = 0;
			bonus = 0;
			protection = 0;
			charges = 0;
			type = ITEM_NOT_MELEE;
			magic_use_type = 0;
			graphic_num = 66;
			value = 6;
			weight = 15;
			full_name = "Leather Helm";
			name = "Helm";
			set_ident(true);
			break;
		case 'food':
			variety = ITEM_TYPE_FOOD;
			item_level = 0;
			awkward = 0;
			bonus = 0;
			protection = 0;
			charges = 0;
			type = ITEM_NOT_MELEE;
			magic_use_type = 0;
			graphic_num = 62;
			value = 0;
			weight = 0;
			full_name = "Food";
			name = "Food";
			set_ident(false);
			break;
		case 'spel':
			variety = ITEM_TYPE_NON_USE_OBJECT;
			item_level = 0;
			awkward = 0;
			bonus = 0;
			protection = 0;
			charges = 0;
			type = ITEM_NOT_MELEE;
			magic_use_type = 0;
			graphic_num = 53;
			value = 0;
			weight = 0;
			full_name = "";
			name = "";
			set_ident(false);
			break;
		case 'alch':
			variety = ITEM_TYPE_POTION;
			item_level = 0;
			awkward = 0;
			bonus = 0;
			protection = 0;
			charges = 1;
			type = ITEM_NOT_MELEE;
			magic_use_type = 0;
			graphic_num = 50;
			value = 0;
			weight = 8;
			full_name = "Potion";
			name = "Potion";
			set_ident(false);
			set_magic(true);
			break;
	}
}

cItemRec& cItemRec::operator = (legacy::item_record_type& old){
	variety = (eItemType) old.variety;
	item_level = old.item_level;
	awkward = old.awkward;
	bonus = old.bonus;
	protection = old.protection;
	charges = old.charges;
	type = (eWeapType) old.type;
	magic_use_type = old.magic_use_type;
	graphic_num = old.graphic_num;
	if(graphic_num >= 150) // custom item graphic
		graphic_num += 850;
	else if(graphic_num == 59) // duplicate mushroom graphic
		graphic_num = 64;
	ability = (eItemAbil) old.ability;
	ability_strength = old.ability_strength;
	type_flag = old.type_flag;
	is_special = old.is_special;
	value = old.value;
	weight = old.weight;
	special_class = old.special_class;
	item_loc.x = old.item_loc.x;
	item_loc.y = old.item_loc.y;
	full_name = std::string(old.full_name);
	name = old.name;
	treas_class = old.treas_class;
	item_properties = old.item_properties;
	reserved1 = old.reserved1;
	reserved2 = old.reserved2;
	return *this;
}

void cItemRec::writeTo(std::ostream& file, std::string prefix){
	file << prefix << "VARIETY " << variety << std::endl;
	file << prefix << "LEVEL " << item_level << std::endl;
	file << prefix << "AWKWARD " << awkward << std::endl;
	file << prefix << "BONUS " << bonus << std::endl;
	file << prefix << "PROT " << protection << std::endl;
	file << prefix << "CHARGES " << charges << std::endl;
	file << prefix << "WEAPON " << type << std::endl;
	file << prefix << "USE " << magic_use_type << std::endl;
	file << prefix << "ICON " << graphic_num << std::endl;
	file << prefix << "ABILITY " << ability << std::endl;
	file << prefix << "ABILSTR " << ability_strength << std::endl;
	file << prefix << "TYPE " << type_flag << std::endl;
	file << prefix << "ISSPEC " << is_special << std::endl;
	file << prefix << "VALUE " << value << std::endl;
	file << prefix << "WEIGHT " << weight << std::endl;
	file << prefix << "SPEC " << special_class << std::endl;
	file << prefix << "AT " << item_loc.x << ' ' << item_loc.y << std::endl;
	file << prefix << "FULLNAME " << full_name << std::endl;
	file << prefix << "NAME " << name << std::endl;
	file << prefix << "TREASURE " << treas_class << std::endl;
	if(is_ident()) file << prefix << "IDENTIFIED" << std::endl;
	if(is_property()) file << prefix << "PROPERTY" << std::endl;
	if(is_magic()) file << prefix << "MAGIC" << std::endl;
	if(is_contained()) file << prefix << "CONTAINED" << std::endl;
	if(is_cursed()) file << prefix << "CURSED" << std::endl;
	if(is_concealed()) file << prefix << "CONCEALED" << std::endl;
	if(is_enchanted()) file << prefix << "ENCHANTED" << std::endl;
}

void cItemRec::readAttrFrom(std::string cur, std::istream& sin){
	if(cur == "VARIETY") sin >> variety;
	else if(cur == "LEVEL") sin >> item_level;
	else if(cur == "AWKWARD") sin >> awkward;
	else if(cur == "BONUS") sin >> bonus;
	else if(cur == "PROT") sin >> protection;
	else if(cur == "CHARGES") sin >> charges;
	else if(cur == "WEAPON") sin >> type;
	else if(cur == "USE") sin >> magic_use_type;
	else if(cur == "ICON") sin >> graphic_num;
	else if(cur == "ABILITY") sin >> ability;
	else if(cur == "ABILSTR") sin >> ability_strength;
	else if(cur == "TYPE") sin >> type_flag;
	else if(cur == "ISSPEC") sin >> is_special;
	else if(cur == "VALUE") sin >> value;
	else if(cur == "WEIGHT") sin >> weight;
	else if(cur == "SPEC") sin >> special_class;
	else if(cur == "AT") sin >> item_loc.x >> item_loc.y;
	else if(cur == "FULLNAME"){
		getline(sin,cur);
		full_name = cur;
	}else if(cur == "NAME"){
		getline(sin,cur);
		name = cur;
	}
	else if(cur == "TREASURE") sin >> treas_class;
	else if(cur == "IDENTIFIED") set_ident(true);
	else if(cur == "PROPERTY") set_property(true);
	else if(cur == "MAGIC") set_magic(true);
	else if(cur == "CONTAINED") set_contained(true);
	else if(cur == "CURSED") set_cursed(true);
	else if(cur == "CONCEALED") set_concealed(true);
	else if(cur == "ENCHANTED") set_enchanted(true);
}

std::ostream& operator << (std::ostream& out, eWeapType& e){
	return out << (int) e;
}

std::ostream& operator << (std::ostream& out, eItemType& e){
	return out << (int) e;
}

std::ostream& operator << (std::ostream& out, eItemAbil& e){
	return out << (int) e;
}

std::istream& operator >> (std::istream& in, eWeapType& e){
	int i;
	in >> i;
	if(i > 0 && i < 4)
		e = (eWeapType) i;
	else e = ITEM_NOT_MELEE;
	return in;
}

std::istream& operator >> (std::istream& in, eItemType& e){
	int i;
	in >> i;
	if(i > 0 && i < 28)
		e = (eItemType) i;
	else e = ITEM_TYPE_NO_ITEM;
	return in;
}

std::istream& operator >> (std::istream& in, eItemAbil& e){
	int i;
	in >> i;
	if((i > 0 && i < 15) || (i > 29 && i < 63) ||
	   (i > 69 && i < 95) || (i > 109 && i < 136) ||
	   (i > 149 && i < 162) || (i > 169 && i < 177))
		e = (eItemAbil) i;
	else e = ITEM_NO_ABILITY;
	return in;
}
