/*
 *  item.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#ifndef BOE_DATA_ITEM_H
#define BOE_DATA_ITEM_H

#include <string>
#include <iosfwd>

#include "location.hpp"
#include "item_abilities.hpp"
#include "item_variety.hpp"
#include "skills_traits.hpp"
#include "alchemy.hpp"
#include "enchant.hpp"

namespace legacy { struct item_record_type; };

class cTagFile_Page;

enum eItemPreset {
	ITEM_KNIFE,
	ITEM_BUCKLER,
	ITEM_BOW,
	ITEM_ARROW,
	ITEM_POLEARM,
	ITEM_HELM,
	ITEM_ROBE,
	ITEM_RAZORDISK,
	ITEM_FOOD,
	ITEM_SPELL,
	ITEM_POTION,
	ITEM_DEBUG_HEAVY,
	ITEM_SPECIAL,
	ITEM_SHOP,
};

class cItem {
public:
	eItemType variety;
	short item_level;
	int awkward;
	int bonus;
	int protection;
	int charges, max_charges;
	eSkill weap_type;
	eItemUse magic_use_type;
	unsigned short graphic_num;
	eItemAbil ability;
	unsigned int abil_strength;
	uItemAbilData abil_data;
	unsigned short type_flag;
	unsigned int is_special;
	short value;
	unsigned int weight;
	unsigned int special_class;
	miss_num_t missile;
	location item_loc;
	std::string full_name;
	std::string name;
	unsigned int treas_class;
	bool ident, property, magic, contained, held, cursed, concealed, enchanted, unsellable, rechargeable;
	std::string desc;
	unsigned char rec_treas_class() const;
	short item_weight() const;
	std::string interesting_string() const;
	
	std::string getAbilName() const;
	void enchant_weapon(eEnchant enchant_type);
	bool abil_harms() const;
	bool abil_group() const;
	bool can_use() const;
	bool use_in_combat() const;
	bool use_in_town() const;
	bool use_outdoors() const;
	bool use_magic() const;
	
	cItem();
	explicit cItem(eItemPreset preset);
	explicit cItem(eAlchemy recipe);
	void import_legacy(legacy::item_record_type& old);
	void writeTo(cTagFile_Page& file) const;
	void readFrom(const cTagFile_Page& sin);
};

class cSpecItem {
public:
	short flags = 0;
	short special = -1;
	std::string name;
	std::string descr;
};

#endif
