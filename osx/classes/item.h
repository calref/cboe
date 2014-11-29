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

class cItemRec {
public:
	eItemType variety;
	short item_level;
	int awkward;
	int bonus;
	int protection;
	int charges;
	eWeapType type;
	int magic_use_type;
	unsigned short graphic_num;
	eItemAbil ability;
	unsigned int ability_strength;
	unsigned short type_flag;
	unsigned int is_special;
	short value;
	unsigned int weight;
	unsigned int special_class;
	location item_loc;
	std::string full_name;
	std::string name;
	unsigned int treas_class;
	//unsigned char item_properties;
	bool ident : 1;
	bool property : 1;
	bool magic : 1;
	bool contained : 1;
	bool cursed : 1;
	bool concealed : 1;
	bool enchanted : 1;
	bool unsellable : 1;
private:
	unsigned int reserved1;
	unsigned int reserved2;
public:
	//string desc; // for future use
	unsigned char rec_treas_class() const;
//	bool is_ident() const;
//	bool is_property() const;
//	bool is_magic() const;
//	bool is_contained() const;
//	bool is_cursed() const;
//	bool is_concealed() const;
//	bool is_enchanted() const;
//	void set_ident(bool b);
//	void set_property(bool b);
//	void set_magic(bool b);
//	void set_contained(bool b);
//	void set_cursed(bool b);
//	void set_concealed(bool b);
//	void set_enchanted(bool b);
	short item_weight() const;
	
	cItemRec();
	cItemRec(long preset);
	cItemRec& operator = (legacy::item_record_type& old);
	void writeTo(std::ostream& file, std::string prefix = "");
	void readFrom(std::istream& sin);
};

std::ostream& operator << (std::ostream& out, eWeapType& e);
std::ostream& operator << (std::ostream& out, eItemType& e);
std::ostream& operator << (std::ostream& out, eItemAbil& e);
std::istream& operator >> (std::istream& in, eWeapType& e);
std::istream& operator >> (std::istream& in, eItemType& e);
std::istream& operator >> (std::istream& in, eItemAbil& e);

class cSpecItem {
public:
	short flags;
	short special;
	// TODO: Change to std::string
	char name[256];
	char descr[256];
};

/*
 typedef struct {
 short variety, item_level;
 char awkward, bonus, protection, charges, type;
 unsigned char graphic_num,ability, type_flag, is_special;
 short value;
 bool identified, magic;
 unsigned char weight, description_flag;
 char full_name[25], name[15];
 unsigned char reserved1,reserved2;
 unsigned char magic_use_type, ability_strength, treas_class, real_abil;
 } short_item_record_type;
 */

#endif