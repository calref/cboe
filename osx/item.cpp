/*
 *  item.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

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
	variety = 0;
	item_level = 0;
	awkward = 0;
	bonus = 0;
	protection = 0;
	charges = 0;
	type = 0;
	magic_use_type = 0;
	graphic_num = 0;
	ability = 0;
	ability_strength = 0;
	type_flag = 0;
	is_special = 0;
	value = 0;
	weight = 0;
	special_class = 0;
	item_loc.x = 0;
	item_loc.y = 0;
	strcpy(full_name,"");
	strcpy(name,"");
	treas_class = 0;
	item_properties = 0;
	reserved1 = 0;
	reserved2 = 0;
}

cItemRec::cItemRec(long preset){
	ability = 0;
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
			strcpy(full_name,"Bronze Knife");
			strcpy(name,"Knife");
			set_ident(true);
			break;
		case 'buck':
			variety = ITEM_TYPE_SHIELD;
			item_level = 1;
			awkward = 1;
			bonus = 0;
			protection = 0;
			charges = 0;
			type = 0;
			magic_use_type = 0;
			graphic_num = 65;
			value = 2;
			weight = 20;
			strcpy(full_name,"Crude Buckler");
			strcpy(name,"Buckler");
			set_ident(true);
			break;
		case 'bow ':
			variety = ITEM_TYPE_BOW;
			item_level = 0;
			awkward = 0;
			bonus = 0;
			protection = 0;
			charges = 0;
			type = 0;
			magic_use_type = 0;
			graphic_num = 10;
			value = 15;
			weight = 20;
			strcpy(full_name,"Cavewood Bow");
			strcpy(name,"Bow");
			set_ident(true);
			break;
		case 'arrw':
			variety = ITEM_TYPE_ARROW;
			item_level = 12;
			awkward = 0;
			bonus = 0;
			protection = 0;
			charges = 12;
			type = 0;
			magic_use_type = 0;
			graphic_num = 47;
			value = 1;
			weight = 1;
			strcpy(full_name,"Arrows");
			strcpy(name,"Arrows");
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
			strcpy(full_name,"Stone Spear");
			strcpy(name,"Spear");
			set_ident(true);
			break;
		case 'helm':
			variety = ITEM_TYPE_HELM;
			item_level = 1;
			awkward = 0;
			bonus = 0;
			protection = 0;
			charges = 0;
			type = 0;
			magic_use_type = 0;
			graphic_num = 66;
			value = 6;
			weight = 15;
			strcpy(full_name,"Leather Helm");
			strcpy(name,"Helm");
			set_ident(true);
			break;
		case 'food':
			variety = ITEM_TYPE_FOOD;
			item_level = 0;
			awkward = 0;
			bonus = 0;
			protection = 0;
			charges = 0;
			type = 0;
			magic_use_type = 0;
			graphic_num = 62;
			value = 0;
			weight = 0;
			strcpy(full_name,"Food");
			strcpy(name,"Food");
			set_ident(false);
			break;
		case 'spel':
			variety = ITEM_TYPE_NON_USE_OBJECT;
			item_level = 0;
			awkward = 0;
			bonus = 0;
			protection = 0;
			charges = 0;
			type = 0;
			magic_use_type = 0;
			graphic_num = 53;
			value = 0;
			weight = 0;
			strcpy(full_name,"");
			strcpy(name,"");
			set_ident(false);
			break;
		case 'alch':
			variety = ITEM_TYPE_POTION;
			item_level = 0;
			awkward = 0;
			bonus = 0;
			protection = 0;
			charges = 1;
			type = 0;
			magic_use_type = 0;
			graphic_num = 50;
			value = 0;
			weight = 8;
			strcpy(full_name,"Potion");
			strcpy(name,"Potion");
			set_ident(false);
			set_magic(true);
			break;
	}
}

__attribute__((deprecated))
cItemRec& cItemRec::operator = (legacy::item_record_type& old){
	variety = old.variety;
	item_level = old.item_level;
	awkward = old.awkward;
	bonus = old.bonus;
	protection = old.protection;
	charges = old.charges;
	type = old.type;
	magic_use_type = old.magic_use_type;
	graphic_num = old.graphic_num;
	ability = old.ability;
	ability_strength = old.ability_strength;
	type_flag = old.type_flag;
	is_special = old.is_special;
	value = old.value;
	weight = old.weight;
	special_class = old.special_class;
	item_loc.x = old.item_loc.x;
	item_loc.y = old.item_loc.y;
	strcpy(full_name,old.full_name);
	strcpy(name,old.name);
	treas_class = old.treas_class;
	item_properties = old.item_properties;
	reserved1 = old.reserved1;
	reserved2 = old.reserved2;
	return *this;
}