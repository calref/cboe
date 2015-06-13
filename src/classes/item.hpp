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
#include "simpletypes.hpp"

namespace legacy { struct item_record_type; };

class cItem {
public:
	eItemType variety;
	short item_level;
	int awkward;
	int bonus;
	int protection;
	int charges;
	eSkill weap_type;
	eItemUse magic_use_type;
	unsigned short graphic_num;
	eItemAbil ability;
	unsigned int abil_data[2];
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
	bool ident, property, magic, contained, held, cursed, concealed, enchanted, unsellable;
	std::string desc;
	unsigned char rec_treas_class() const;
	short item_weight() const;
	
	std::string getAbilName() const;
	void enchant_weapon(eEnchant enchant_type, short new_val);
	bool abil_harms() const;
	bool abil_group() const;
	
	cItem();
	explicit cItem(long preset);
	explicit cItem(eAlchemy recipe);
	void append(legacy::item_record_type& old);
	void writeTo(std::ostream& file, std::string prefix = "") const;
	void readFrom(std::istream& sin);
};

std::ostream& operator << (std::ostream& out, eItemType e);
std::ostream& operator << (std::ostream& out, eItemAbil e);
std::istream& operator >> (std::istream& in, eItemType& e);
std::istream& operator >> (std::istream& in, eItemAbil& e);
std::ostream& operator << (std::ostream& out, eSkill e);
std::istream& operator >> (std::istream& in, eSkill& e);
std::ostream& operator << (std::ostream& out, eItemUse e);
std::istream& operator >> (std::istream& in, eItemUse& e);

class cSpecItem {
public:
	short flags;
	short special;
	std::string name;
	std::string descr;
};

#endif
