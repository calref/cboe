/*
 *  talking.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include "talking.hpp"

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "oldstructs.hpp"

void cSpeech::append(legacy::talking_record_type& old, std::vector<shop_info_t>& shops){
	int i,j;
	talk_nodes.resize(60);
	for(i = 0; i < 60; i++){
		talk_nodes[i].personality = old.talk_nodes[i].personality;
		for(j = 0; j < 4; j++){
			talk_nodes[i].link1[j] = old.talk_nodes[i].link1[j];
			talk_nodes[i].link2[j] = old.talk_nodes[i].link2[j];
			talk_nodes[i].extras[j] = old.talk_nodes[i].extras[j];
		}
		// Now, convert data if necessary
		switch(old.talk_nodes[i].type) {
			case 0: // normal speech
				talk_nodes[i].type = eTalkNode::REGULAR;
				break;
			case 1: // depend on SDF
				talk_nodes[i].type = eTalkNode::DEP_ON_SDF;
				break;
			case 2: // set SDF
				talk_nodes[i].type = eTalkNode::SET_SDF;
				break;
			case 3: // inn
				talk_nodes[i].type = eTalkNode::INN;
				break;
			case 4: // depend on time
				talk_nodes[i].type = eTalkNode::DEP_ON_TIME;
				break;
			case 5: // depend on event
				talk_nodes[i].type = eTalkNode::DEP_ON_TIME_AND_EVENT;
				break;
			case 6: // depend on town
				talk_nodes[i].type = eTalkNode::DEP_ON_TOWN;
				break;
			case 7: // Item shop
				talk_nodes[i].type = eTalkNode::SHOP;
				shops.push_back({eShopItemType::ITEM, talk_nodes[i].extras[1], talk_nodes[i].extras[2], talk_nodes[i].str1});
				talk_nodes[i].extras[1] = shops.size() + 5;
				talk_nodes[i].extras[2] = 0;
				break;
			case 8: // Training
				talk_nodes[i].type = eTalkNode::TRAINING;
				break;
			case 9: // Spell shops
			case 10:
				talk_nodes[i].type = eTalkNode::SHOP;
				shops.push_back({
					old.talk_nodes[i].type == 9 ? eShopItemType::MAGE_SPELL : eShopItemType::PRIEST_SPELL,
					talk_nodes[i].extras[1] + 30,
					talk_nodes[i].extras[2],
					talk_nodes[i].str1
				});
				talk_nodes[i].extras[1] = shops.size() + 5;
				talk_nodes[i].extras[2] = 0;
				break;
			case 11: // Alchemy shop
				talk_nodes[i].type = eTalkNode::SHOP;
				shops.push_back({eShopItemType::ALCHEMY, talk_nodes[i].extras[1], talk_nodes[i].extras[2], talk_nodes[i].str1});
				talk_nodes[i].extras[1] = shops.size() + 5;
				talk_nodes[i].extras[2] = 0;
				break;
			case 12: // Healer
				talk_nodes[i].type = eTalkNode::SHOP;
				talk_nodes[i].extras[1] = 5;
				talk_nodes[i].extras[2] = 0;
				break;
			case 13: // Sell weapons
				talk_nodes[i].type = eTalkNode::SELL_WEAPONS;
				break;
			case 14: // Sell armor
				talk_nodes[i].type = eTalkNode::SELL_ARMOR;
				break;
			case 15: // Sell any
				talk_nodes[i].type = eTalkNode::SELL_ITEMS;
				break;
			case 16: // Identification
				talk_nodes[i].type = eTalkNode::IDENTIFY;
				break;
			case 17: // Augmentation
				talk_nodes[i].type = eTalkNode::ENCHANT;
				break;
			case 18: // buy response
				talk_nodes[i].type = eTalkNode::BUY_INFO;
				break;
			case 19: // buy SDF
				talk_nodes[i].type = eTalkNode::BUY_SDF;
				break;
			case 20: // buy boat
				talk_nodes[i].type = eTalkNode::BUY_SHIP;
				break;
			case 21: // buy horse
				talk_nodes[i].type = eTalkNode::BUY_HORSE;
				break;
			case 22: // buy special item
				talk_nodes[i].type = eTalkNode::BUY_SPEC_ITEM;
				break;
			case 23: // Junk shop
				talk_nodes[i].type = eTalkNode::SHOP;
				talk_nodes[i].extras[2] = 0;
				break;
			case 24: // buy town location
				talk_nodes[i].type = eTalkNode::BUY_TOWN_LOC;
				break;
			case 25: // force conversation end
				talk_nodes[i].type = eTalkNode::END_FORCE;
				break;
			case 26: // end and fight
				talk_nodes[i].type = eTalkNode::END_FIGHT;
				break;
			case 27: // end and raise alarm
				talk_nodes[i].type = eTalkNode::END_ALARM;
				break;
			case 28: // end and die
				talk_nodes[i].type = eTalkNode::END_DIE;
				break;
			case 29: // call town special
				talk_nodes[i].type = eTalkNode::CALL_TOWN_SPEC;
				break;
			case 30: // call scenario special
				talk_nodes[i].type = eTalkNode::CALL_SCEN_SPEC;
				break;
		}
	}
}
