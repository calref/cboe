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
#include "simpletypes.hpp"
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
			std::fill(link1, link1 + 4, 'x');
			std::fill(link2, link2 + 4, 'x');
		}
	};
	cPersonality people[10];
	std::vector<cNode> talk_nodes;
	
	void append(legacy::talking_record_type& old, std::vector<shop_info_t>& shops);
	void writeTo(std::ostream& file) const;
};

std::ostream& operator<< (std::ostream& out, eTalkNode node);
std::istream& operator>> (std::istream& in, eTalkNode& node);

#endif
