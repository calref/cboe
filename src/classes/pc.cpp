/*
 *  pc.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <iostream>

#include "classes.h"
#include "oldstructs.h"
#include "mathutil.hpp"

void cPlayer::append(legacy::pc_record_type old){
	int i;
	main_status = (eMainStatus) old.main_status;
	name = old.name;
	for(i = 0; i < 19; i++) {
		eSkill skill = eSkill(i);
		skills[skill] = old.skills[i];
	}
	max_health = old.max_health;
	cur_health = old.cur_health;
	max_sp = old.max_sp;
	cur_sp = old.cur_sp;
	experience = old.experience;
	skill_pts = old.skill_pts;
	level = old.level;
	// TODO: Why are advan and exp_adj commented out?
	for(i = 0; i < 15; i++){
		status[(eStatus) i] = old.status[i];
		eTrait trait = eTrait(i);
		traits[trait] = old.traits[i];
	}
	for(i = 0; i < 24; i++){
		items[i].append(old.items[i]);
		equip[i] = old.equip[i];
	}
	for(i = 0; i < 62; i++){
		priest_spells[i] = old.priest_spells[i];
		mage_spells[i] = old.mage_spells[i];
	}
	which_graphic = old.which_graphic;
	weap_poisoned = old.weap_poisoned;
	race = (eRace) old.race;
	direction = old.direction;
}

short cPlayer::get_tnl(){
	short tnl = 100,store_per = 100;
	// Omitting a race from this list gives it a value of 0, thanks to the defaulting implementation of operator[]
	// TODO: Vahnatai, pacifist
	static std::map<const eRace, const int> rp = {{eRace::NEPHIL,12},{eRace::SLITH,20}};
	static std::map<const eTrait, const short> ap = {
		{eTrait::TOUGHNESS,10}, {eTrait::MAGICALLY_APT,20}, {eTrait::AMBIDEXTROUS,8}, {eTrait::NIMBLE,10}, {eTrait::CAVE_LORE,4},
		{eTrait::WOODSMAN,6}, {eTrait::GOOD_CONST,10}, {eTrait::HIGHLY_ALERT,7}, {eTrait::STRENGTH,12}, {eTrait::RECUPERATION,15},
		{eTrait::SLUGGISH,-10}, {eTrait::MAGICALLY_INEPT,-8}, {eTrait::FRAIL,-8}, {eTrait::CHRONIC_DISEASE,-20}, {eTrait::BAD_BACK,-8},
		{eTrait::PACIFIST,-40},
	};
	
	tnl = (tnl * (100 + rp[race])) / 100;
	for(int i = 0; i < 15; i++) {
		eTrait trait = eTrait(i);
		if(traits[trait])
			store_per = store_per + ap[trait];
	}
	
	tnl = (tnl * store_per) / 100;	
	
	return tnl;
}

void cPlayer::apply_status(eStatus which, int how_much) {
	static const std::set<eStatus> allow_negative = {
		// The obvious ones:
		eStatus::BLESS_CURSE, eStatus::HASTE_SLOW,
		// The ones that BoE previously allowed:
		eStatus::POISONED_WEAPON, eStatus::POISON, eStatus::ASLEEP,
		// (Note: Negative levels of sleep can be obtained from the Hyperactivity spell. The other two never go negative.)
		// The additional ones that make sense in the negative:
		eStatus::MAGIC_RESISTANCE, eStatus::DUMB,
	};
	
	// TODO: Apply STATUS_PROTECTION item abilities; the challenge is to determine whether to apply to positive or negative how_much
	// TODO: I'd like to merge poison_pc, web_pc, acid_pc etc into this function.
	
	if(main_status != eMainStatus::ALIVE)
		return;
	status[which] = minmax(-8,8,status[which] + how_much);
	if(!allow_negative.count(which))
		status[which] = max(status[which],0);
}

void cPlayer::avatar() {
	status[eStatus::BLESS_CURSE] = 8;
	status[eStatus::HASTE_SLOW] = 8;
	status[eStatus::INVULNERABLE] = 3;
	status[eStatus::MAGIC_RESISTANCE] = 8;
	status[eStatus::WEBS] = 0;
	status[eStatus::DISEASE] = 0;
	status[eStatus::DUMB] = 0;
	status[eStatus::MARTYRS_SHIELD] = 8;
}

void cPlayer::sort_items() {
	using it = eItemType;
	static std::map<eItemType, const short> item_priority = {
		{it::NO_ITEM, 20}, {it::ONE_HANDED, 8}, {it::TWO_HANDED, 8}, {it::GOLD, 20}, {it::BOW, 9},
		{it::ARROW, 9}, {it::THROWN_MISSILE, 3}, {it::POTION, 2}, {it::SCROLL, 1}, {it::WAND, 0},
		{it::TOOL, 7}, {it::FOOD, 20}, {it::SHIELD, 10}, {it::ARMOR, 10}, {it::HELM, 10},
		{it::GLOVES, 10}, {it::SHIELD_2, 10}, {it::BOOTS, 10}, {it::RING, 5}, {it::NECKLACE, 6},
		{it::WEAPON_POISON, 4}, {it::NON_USE_OBJECT, 11}, {it::PANTS, 12}, {it::CROSSBOW, 9}, {it::BOLTS, 9},
		{it::MISSILE_NO_AMMO, 9}, {it::UNUSED1, 20}, {it::UNUSED2, 20}
	};
	bool no_swaps = false;
	
	while(!no_swaps) {
		no_swaps = true;
		for(int i = 0; i < 23; i++)
			if(item_priority[items[i + 1].variety] <
			   item_priority[items[i].variety]) {
			  	no_swaps = false;
				std::swap(items[i + 1], items[i]);
				std::swap(equip[i + 1], equip[i]);
				if(weap_poisoned == i + 1)
					weap_poisoned--;
				else if(weap_poisoned == i)
					weap_poisoned++;
			}
	}
}

bool cPlayer::give_item(cItem item, bool do_print, bool allow_overload) {
	short free_space;
	
	if(item.variety == eItemType::NO_ITEM)
		return true;
	if(item.variety == eItemType::GOLD) {
		party.gold += item.item_level;
		if(do_print && print_result)
			print_result("You get some gold.");
		return true;
	}
	if(item.variety == eItemType::FOOD) {
		party.food += item.item_level;
		if(do_print && print_result)
			print_result("You get some food.");
		return true;
	}
	if(!allow_overload && item.item_weight() > free_weight()) {
	  	if(do_print && print_result) {
			//beep(); // TODO: This is a game event, so it should have a game sound, not a system alert.
			print_result("Item too heavy to carry.");
		}
		return false;
	}
	free_space = has_space();
	if(free_space == 24 || main_status != eMainStatus::ALIVE)
		return false;
	else {
		item.property = false;
		item.contained = false;
		items[free_space] = item;
		
		if(do_print && print_result) {
			std::ostringstream announce;
			announce << "  " << name << " gets ";
			if(!item.ident)
				announce << item.name;
			else announce << item.full_name;
			announce << '.';
			print_result(announce.str());
		}
		
		combine_things();
		sort_items();
		return true;
	}
	return false;
}

short cPlayer::max_weight() {
	return 100 + (15 * min(skills[eSkill::STRENGTH],20)) + (traits[eTrait::STRENGTH] * 30)
		+ (traits[eTrait::BAD_BACK] * -50);
}

short cPlayer::cur_weight() {
	short weight = 0;
	bool airy = false,heavy = false;
	
	for(int i = 0; i < 24; i++)
		if(items[i].variety != eItemType::NO_ITEM) {
			weight += items[i].item_weight();
			if(items[i].ability == eItemAbil::LIGHTER_OBJECT)
				airy = true;
			if(items[i].ability == eItemAbil::HEAVIER_OBJECT)
				heavy = true;
		}
	if(airy)
		weight -= 30;
	if(heavy)
		weight += 30;
	if(weight < 0)
		weight = 0;
	return weight;
}

short cPlayer::free_weight() {
	return max_weight() - cur_weight();
}

short cPlayer::has_space() {
	for(int i = 0; i < 24; i++) {
		if(items[i].variety == eItemType::NO_ITEM)
			return i;
	}
	return 24;
}

void cPlayer::combine_things() {
	for(int i = 0; i < 24; i++) {
		if(items[i].variety != eItemType::NO_ITEM && items[i].type_flag > 0 && items[i].ident) {
			for(int j = i + 1; j < 24; j++)
				if(items[j].variety != eItemType::NO_ITEM && items[j].type_flag == items[i].type_flag && items[j].ident) {
					if(print_result) print_result("(items combined)");
					short test = items[i].charges + items[j].charges;
					if(test > 125) {
						items[i].charges = 125;
						if(print_result) print_result("(Can have at most 125 of any item.");
					}
					else items[i].charges += items[j].charges;
				 	if(equip[j]) {
				 		equip[i] = true;
				 		equip[j] = false;
					}
					take_item(j);
				}
		}
		if(items[i].variety != eItemType::NO_ITEM && items[i].charges < 0)
			items[i].charges = 1;
	}
}

short cPlayer::get_prot_level(eItemAbil abil, short dat) {
	int sum = 0;
	for(int i = 0; i < 24; i++) {
		if(items[i].variety == eItemType::NO_ITEM) continue;
		if(items[i].ability != abil) continue;
		if(!equip[i]) continue;
		if(dat >= 0 && dat != items[i].abil_data[1]) continue;
		sum += items[i].abil_data[1];
	}
	return sum; // TODO: Should we return -1 if the sum is 0?
	
}

short cPlayer::has_abil_equip(eItemAbil abil,short dat) {
	for(short i = 0; i < 24; i++) {
		if(items[i].variety == eItemType::NO_ITEM) continue;
		if(items[i].ability != abil) continue;
		if(!equip[i]) continue;
		if(dat >= 0 && dat != items[i].abil_data[1]) continue;
		return i;
	}
	return 24;
}

short cPlayer::has_abil(eItemAbil abil,short dat) {
	for(short i = 0; i < 24; i++) {
		if(items[i].variety == eItemType::NO_ITEM) continue;
		if(items[i].ability != abil) continue;
		if(dat >= 0 && dat != items[i].abil_data[1]) continue;
		return i;
	}
	return 24;
}

eBuyStatus cPlayer::ok_to_buy(short cost,cItem item) {
	if(item.variety != eItemType::GOLD && item.variety != eItemType::FOOD) {
		for(int i = 0; i < 24; i++)
			if(items[i].variety != eItemType::NO_ITEM && items[i].type_flag == item.type_flag && items[i].charges > 123)
				return eBuyStatus::HAVE_LOTS;
		
		if(has_space() == 24)
			return eBuyStatus::NO_SPACE;
		if(item.item_weight() > free_weight()) {
	  		return eBuyStatus::TOO_HEAVY;
		}
	}
	if(cost > party.gold)
		return eBuyStatus::NEED_GOLD;
	return eBuyStatus::OK;
}

void cPlayer::take_item(int which_item) {
	if(weap_poisoned == which_item && status[eStatus::POISONED_WEAPON] > 0) {
		if(print_result) print_result("  Poison lost.");
		status[eStatus::POISONED_WEAPON] = 0;
	}
	if(weap_poisoned > which_item && status[eStatus::POISONED_WEAPON] > 0)
		weap_poisoned--;
	
	for(int i = which_item; i < 23; i++) {
		items[i] = items[i + 1];
		equip[i] = equip[i + 1];
	}
	items[23] = cItem();
	equip[23] = false;
}

void cPlayer::remove_charge(int which_item) {
	if(items[which_item].charges > 0) {
		items[which_item].charges--;
		if(items[which_item].charges == 0) {
			take_item(which_item);
		}
	}
}

void cPlayer::finish_create() {
	// Start items
	switch(race) {
		case eRace::HUMAN:
			items[0] = cItem('nife');
			items[1] = cItem('buck');
			break;
		case eRace::NEPHIL:
			items[0] = cItem('bow ');
			items[1] = cItem('arrw');
			break;
		case eRace::SLITH:
			items[0] = cItem('pole');
			items[1] = cItem('helm');
			break;
		case eRace::VAHNATAI:
			items[0] = cItem('nife');
			items[1] = cItem('rdsk');
			break;
	}
	equip[0] = true;
	equip[1] = true;
	// Racial stat adjustments
	if(race == eRace::NEPHIL)
		skills[eSkill::DEXTERITY] += 2;
	if(race == eRace::SLITH) {
		skills[eSkill::STRENGTH] += 2;
		skills[eSkill::INTELLIGENCE] += 1;
	}
	// TODO: Vahnatai
	// Bonus spell points for spellcasters
	max_sp += skills[eSkill::MAGE_SPELLS] * 3 + skills[eSkill::PRIEST_SPELLS] * 3;
	cur_sp = max_sp;
}

cPlayer::cPlayer(cParty& party) : party(party) {
	short i;
	main_status = eMainStatus::ABSENT;
	name = "\n";
	
	skills[eSkill::STRENGTH] = 1;
	skills[eSkill::DEXTERITY] = 1;
	skills[eSkill::INTELLIGENCE] = 1;
	cur_health = 6; 
	max_health = 6; 
 	cur_sp = 0; 
 	max_sp = 0; 
 	experience = 0; 
	skill_pts = 60; 
	level = 1;
	for(i = 0; i < 24; i++)
		items[i] = cItem();
	for(i = 0; i < 24; i++)
		equip[i] = false;
	
 	for(i = 0; i < 62; i++) {
		priest_spells[i] = (i < 30) ? true : false;
		mage_spells[i] = (i < 30) ? true : false;
	}
	which_graphic = 0;
	weap_poisoned = 24;
		
	race = eRace::HUMAN;
	direction = 0;
}

cPlayer::cPlayer(cParty& party,long key,short slot) : party(party) {
	short i;
	main_status = eMainStatus::ALIVE;
	if(key == 'dbug'){
		switch(slot) {
			case 0:
				name = "Gunther";
				break;
			case 1:
				name = "Yanni";
				break;
			case 2:
				name = "Mandolin";
				break;
			case 3:
				name = "Pete";
				break;
			case 4:
				name = "Vraiment";
				break;
			case 5:
				name = "Goo";
				break;
		}
		skills[eSkill::STRENGTH] = 20;
		skills[eSkill::DEXTERITY] = 20;
		skills[eSkill::INTELLIGENCE] = 20;
		for(i = 3; i < 19; i++) {
			eSkill skill = eSkill(i);
			skills[skill] = 8;
		}
		cur_health = 60;
		max_health = 60; 
		cur_sp = 90; 
		max_sp = 90; 
		experience = 0; 
		skill_pts = 60; 
		level = 1;
		for(i = 0; i < 24; i++)
			items[i] = cItem();
		for(i = 0; i < 24; i++)
			equip[i] = false;
		
		for(i = 0; i < 62; i++) {
			priest_spells[i] = true;
			mage_spells[i] = true;
		}
		//which_graphic = num * 3 + 1;	// 1, 4, 7, 10, 13, 16
		which_graphic = slot + 4;		// 4, 5, 6, 7,  8,  9
		weap_poisoned = 24; // was 16, as an E2 relic
		
		for(i = 0; i < 10; i++) {
			eTrait trait = eTrait(i);
			traits[trait] = true;
		}		
		
		race = eRace::HUMAN;
		direction = 0;
	}else if(key == 'dflt'){
		// TODO: The duplication of std::map<eSkill,short> shouldn't be needed here
		static std::map<eSkill, short> pc_stats[6] = {
			std::map<eSkill,short>{
				{eSkill::STRENGTH,8}, {eSkill::DEXTERITY,6}, {eSkill::INTELLIGENCE,2},
				{eSkill::EDGED_WEAPONS,6}, {eSkill::ITEM_LORE,1}, {eSkill::ASSASSINATION,2},
			}, std::map<eSkill,short>{
				{eSkill::STRENGTH,8}, {eSkill::DEXTERITY,7}, {eSkill::INTELLIGENCE,2},
				{eSkill::POLE_WEAPONS,6}, {eSkill::THROWN_MISSILES,3}, {eSkill::DEFENSE,3},
				{eSkill::POISON,2},
			}, std::map<eSkill,short>{
				{eSkill::STRENGTH,8}, {eSkill::DEXTERITY,6}, {eSkill::INTELLIGENCE,2},
				{eSkill::EDGED_WEAPONS,3}, {eSkill::BASHING_WEAPONS,3}, {eSkill::ARCHERY,2},
				{eSkill::DISARM_TRAPS,4}, {eSkill::LOCKPICKING,4}, {eSkill::POISON,2}, {eSkill::LUCK,1},
			}, std::map<eSkill,short>{
				{eSkill::STRENGTH,3}, {eSkill::DEXTERITY,2}, {eSkill::INTELLIGENCE,6},
				{eSkill::EDGED_WEAPONS,2}, {eSkill::THROWN_MISSILES,2},
				{eSkill::MAGE_SPELLS,3}, {eSkill::MAGE_LORE,3}, {eSkill::ITEM_LORE,1},
			}, std::map<eSkill,short>{
				{eSkill::STRENGTH,2}, {eSkill::DEXTERITY,2}, {eSkill::INTELLIGENCE,6},
				{eSkill::EDGED_WEAPONS,3}, {eSkill::THROWN_MISSILES,2},
				{eSkill::MAGE_SPELLS,2}, {eSkill::PRIEST_SPELLS,1}, {eSkill::MAGE_LORE,4},
				{eSkill::LUCK,1},
			}, std::map<eSkill,short>{
				{eSkill::STRENGTH,2}, {eSkill::DEXTERITY,2}, {eSkill::INTELLIGENCE,6},
				{eSkill::BASHING_WEAPONS,2}, {eSkill::THROWN_MISSILES,2}, {eSkill::DEFENSE,1},
				{eSkill::PRIEST_SPELLS,3}, {eSkill::MAGE_LORE,3}, {eSkill::ALCHEMY,2},
			},
		};
		static const short pc_health[6] = {22,24,24,16,16,18};
		static const short pc_sp[6] = {0,0,0,20,20,21};
		static const short pc_graphics[6] = {3,32,29,16,23,14};
		static const short pc_race[6] = {0,2,1,0,0,0};
		// TODO: The duplication of std::set<eTrait> should not be necessary here
		static const std::set<eTrait> pc_t[6] = {
			std::set<eTrait>{eTrait::AMBIDEXTROUS, eTrait::GOOD_CONST, eTrait::MAGICALLY_INEPT},
			std::set<eTrait>{eTrait::TOUGHNESS, eTrait::WOODSMAN, eTrait::SLUGGISH},
			std::set<eTrait>{eTrait::NIMBLE, eTrait::FRAIL},
			std::set<eTrait>{eTrait::MAGICALLY_APT},
			std::set<eTrait>{eTrait::CAVE_LORE, eTrait::GOOD_CONST, eTrait::HIGHLY_ALERT, eTrait::BAD_BACK},
			std::set<eTrait>{eTrait::MAGICALLY_APT},
		};
		
		main_status = eMainStatus::ALIVE;
		switch(slot) {
			case 0:
				name = "Jenneke";
				break;
			case 1:
				name = "Thissa";
				break;
			case 2:
				name = "Frrrrrr";
				break;
			case 3:
				name = "Adrianna";
				break;
			case 4:
				name = "Feodoric";
				break;
			case 5:
				name = "Michael";
				break;
				
		}
		
		for(i = 0; i < 19; i++) {
			eSkill skill = eSkill(i);
			skills[skill] = pc_stats[slot][skill];
		}
		cur_health = pc_health[slot];
		max_health = pc_health[slot]; 
		experience = 0; 
		skill_pts = 0; 
		level = 1;
		
		for(i = 0; i < 24; i++)
			items[i] = cItem();
		for(i = 0; i < 24; i++)
			equip[i] = false;
		cur_sp = pc_sp[slot]; 
		max_sp = pc_sp[slot]; 
		for(i = 0; i < 62; i++) {
			priest_spells[i] = (i < 30) ? true : false;
			mage_spells[i] = (i < 30) ? true : false;
		}
		for(i = 0; i < 15; i++) {
			eTrait trait = eTrait(i);
			traits[trait] = pc_t[slot].count(trait);
		}
		
		race = (eRace) pc_race[slot];
		direction = 0;
		
		which_graphic = pc_graphics[slot];
	}
}

void operator += (eMainStatus& stat, eMainStatus othr){
	if(othr == eMainStatus::SPLIT)
		stat = (eMainStatus) (10 + (int)stat);
	else if(stat == eMainStatus::SPLIT)
		stat = (eMainStatus) (10 + (int)othr);
}

void operator -= (eMainStatus& stat, eMainStatus othr){
	if(othr == eMainStatus::SPLIT)
		stat = (eMainStatus) (-10 + (int)stat);
	else if(stat == eMainStatus::SPLIT)
		stat = (eMainStatus) (-10 + (int)othr);
}

void cPlayer::writeTo(std::ostream& file) const {
	file << "STATUS -1 " << main_status << '\n';
	file << "NAME " << name << '\n';
	file << "SKILL 19 " << max_health << '\n';
	file << "SKILL 20 " << max_sp << '\n';
	for(int i = 0; i < 19; i++) {
		eSkill skill = eSkill(i);
		if(skills.at(skill) > 0)
			file << "SKILL " << i << ' ' << skills.at(skill) << '\n';
	}
	file << "HEALTH " << cur_health << '\n';
	file << "MANA " << cur_sp << '\n';
	file << "EXPERIENCE " << experience << '\n';
	file << "SKILLPTS " << skill_pts << '\n';
	file << "LEVEL " << level << '\n';
	auto status = this->status;
	for(int i = 0; i < 15; i++) {
		eStatus stat = (eStatus) i;
		if(status[stat] != 0)
			file << "STATUS " << i << ' ' << status.at(stat) << '\n';
	}
	for(int i = 0; i < 24; i++)
		if(equip[i])
			file << "EQUIP " << i << '\n';
	for(int i = 0; i < 62; i++)
		if(mage_spells[i])
			file << "MAGE " << i << '\n';
	for(int i = 0; i < 62; i++)
		if(priest_spells[i])
			file << "PRIEST " << i << '\n';
	auto traits = this->traits;
	for(int i = 0; i < 62; i++) {
		eTrait trait = eTrait(i);
		if(traits[trait])
			file << "TRAIT " << i << '\n';
	}
	file << "ICON " <<  which_graphic << '\n';
	file << "RACE " << race << '\n';
	file << "DIRECTION " << direction << '\n';
	file << "POISON " << weap_poisoned << '\n';
	file << '\f';
	for(int i = 0; i < 24; i++)
		if(items[i].variety != eItemType::NO_ITEM){
			file << "ITEM " << i << '\n';
			items[i].writeTo(file);
			file << '\f';
		}
}

void cPlayer::readFrom(std::istream& file){
	std::istringstream bin, sin;
	std::string cur;
	getline(file, cur, '\f');
	bin.str(cur);
	std::fill(equip.begin(), equip.end(), false);
	while(bin) { // continue as long as no error, such as eof, occurs
		getline(bin, cur);
		std::cout << "Parsing line in pcN.txt: " << cur << std::endl;
		sin.str(cur);
		sin >> cur;
		if(cur == "STATUS"){
			eStatus i;
			sin >> i;
			if(i == eStatus::MAIN) sin >> main_status;
			else sin >> status[i];
		}else if(cur == "NAME")
			sin >> name;
		else if(cur == "SKILL"){
			int i;
			sin >> i;
			switch(i){
				case -1:
				case 20:
					sin >> max_sp;
					break;
				case -2:
				case 19:
					sin >> max_health;
					break;
				default:
					if(i < 0 || i >= 19) break;
					eSkill skill = eSkill(i);
					sin >> skills[skill];
			}
		}else if(cur == "HEALTH")
			sin >> cur_health;
		else if(cur == "MANA")
			sin >> cur_sp;
		else if(cur == "EXPERIENCE")
			sin >> experience;
		else if(cur == "SKILLPTS")
			sin >> skill_pts;
		else if(cur == "LEVEL")
			sin >> level;
		else if(cur == "STATUS"){
			eStatus i;
			sin >> i;
			sin >> status[i];
		}else if(cur == "EQUIP"){
			int i;
			sin >> i;
			equip[i] = true;
		}else if(cur == "MAGE"){
			int i;
			sin >> i;
			mage_spells[i] = true;
		}else if(cur == "PRIEST"){
			int i;
			sin >> i;
			priest_spells[i] = true;
		}else if(cur == "TRAIT"){
			int i;
			sin >> i;
			if(i < 0 || i > 15) continue;
			eTrait trait = eTrait(i);
			traits[trait] = true;
		}else if(cur == "ICON")
			sin >> which_graphic;
		else if(cur == "DIRECTION")
			sin >> direction;
		else if(cur == "RACE")
			sin >> race;
		else if(cur == "POISON")
			sin >> weap_poisoned;
		sin.clear();
	}
	bin.clear();
	while(file) {
		getline(file, cur, '\f');
		bin.str(cur);
		bin >> cur;
		if(cur == "ITEM") {
			int i;
			bin >> i;
			items[i].readFrom(bin);
		}
		bin.clear();
	}
}

std::ostream& operator << (std::ostream& out, eMainStatus e){
	return out << (int) e;
}

// TODO: This should probably understand symbolic names as well as the numbers?
std::istream& operator >> (std::istream& in, eMainStatus& e){
	int i;
	in >> i;
	if(i > 0 && i < 18 && i !=8 && i != 9)
		e = (eMainStatus) i;
	else e = eMainStatus::ABSENT;
	return in;
}

void(* cPlayer::print_result)(std::string) = nullptr;
