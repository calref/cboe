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

#include "location.h"
#include "simpletypes.h"

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
	int magic_use_type;
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
	bool ident : 1;
	bool property : 1;
	bool magic : 1;
	bool contained : 1;
	bool cursed : 1;
	bool concealed : 1;
	bool enchanted : 1;
	bool unsellable : 1;
public:
	std::string desc;
	unsigned char rec_treas_class() const;
	short item_weight() const;
	
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

class cSpecItem {
public:
	short flags;
	short special;
	std::string name;
	std::string descr;
};

#endif
