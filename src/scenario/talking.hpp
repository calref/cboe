/*
 *  talking.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#ifndef BOE_DATA_TALKING_H
#define BOE_DATA_TALKING_H

#include <iosfwd>
#include <string>
#include "shop.hpp"

namespace legacy {
	struct talking_record_type;
	struct talking_node_type;
};

class cPersonality {
public:
	std::string title;
	std::string look, name, job, dunno;
};

// This is used solely for porting old shops
struct shop_info_t {
	eShopItemType type;
	int first, count;
	std::string name;
};

enum class eTalkNode {
	REGULAR = 0,
	DEP_ON_SDF = 1,
	SET_SDF = 2,
	INN = 3,
	DEP_ON_TIME = 4,
	DEP_ON_TIME_AND_EVENT = 5,
	DEP_ON_TOWN = 6,
	SHOP = 7,
	TRAINING = 8,
	JOB_BANK = 9,
	RECHARGE = 12,
	SELL_WEAPONS = 13,
	SELL_ARMOR = 14,
	SELL_ITEMS = 15,
	IDENTIFY = 16,
	ENCHANT = 17,
	BUY_INFO = 18,
	BUY_SDF = 19,
	BUY_SHIP = 20,
	BUY_HORSE = 21,
	BUY_SPEC_ITEM = 22,
	RECEIVE_QUEST = 23,
	BUY_TOWN_LOC = 24,
	END_FORCE = 25,
	END_FIGHT = 26,
	END_ALARM = 27, // Town hostile
	END_DIE = 28,
	CALL_TOWN_SPEC = 29,
	CALL_SCEN_SPEC = 30,
};

class cSpeech { // formerly talking_record_type
public:
	class cNode { // formerly talking_node_type
	public:
		short personality = -1;
		eTalkNode type = eTalkNode::REGULAR;
		char link1[4],link2[4];
		short extras[4];
		std::string str1, str2;
		cNode() {
			std::fill(extras, extras + 4, -1);
			std::fill(link1, link1 + 4, ' ');
			std::fill(link2, link2 + 4, ' ');
		}
	};
	std::array<cPersonality, 10> people;
	std::vector<cNode> talk_nodes;
	
	void import_legacy(legacy::talking_record_type& old, std::vector<shop_info_t>& shops);
	void writeTo(std::ostream& file) const;
};

std::ostream& operator<< (std::ostream& out, eTalkNode node);
std::istream& operator>> (std::istream& in, eTalkNode& node);

#endif
