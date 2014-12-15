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

#include "classes.h"
#include "oldstructs.h"

cPlayer& cPlayer::operator = (legacy::pc_record_type old){
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
		//advan[i] = old.advan[i];
		eTrait trait = eTrait(i);
		traits[trait] = old.traits[i];
	}
	for(i = 0; i < 24; i++){
		items[i] = old.items[i];
		equip[i] = old.equip[i];
	}
	for(i = 0; i < 62; i++){
		priest_spells[i] = old.priest_spells[i];
		mage_spells[i] = old.mage_spells[i];
	}
	which_graphic = old.which_graphic;
	weap_poisoned = old.weap_poisoned;
	race = (eRace) old.race;
	//exp_adj = old.exp_adj;
	direction = old.direction;
	return *this;
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

cPlayer::cPlayer(){
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
		items[i] = cItemRec();
	for(i = 0; i < 24; i++)
		equip[i] = false;
	
 	for(i = 0; i < 62; i++) {
		priest_spells[i] = (i < 30) ? true : false;
		mage_spells[i] = (i < 30) ? true : false;
	}
	which_graphic = 0;
	weap_poisoned = 24;
		
	race = eRace::HUMAN;
	//exp_adj = 100;
	direction = 0;
}

cPlayer::cPlayer(long key,short slot){
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
			items[i] = cItemRec();
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
		//exp_adj = 100;
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
			items[i] = cItemRec();
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
			//advan[i] = false;
		}
		
		race = (eRace) pc_race[slot];
		//exp_adj = 100;
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

void cPlayer::writeTo(std::ostream& file){
	file << "STATUS -1 " << main_status << '\n';
	file << "NAME " << name << '\n';
	file << "SKILL 19 " << max_health << '\n';
	file << "SKILL 20 " << max_sp << '\n';
	for(int i = 0; i < 19; i++) {
		eSkill skill = eSkill(i);
		if(skills[skill] > 0)
			file << "SKILL " << i << ' ' << skills[skill] << '\n';
	}
	file << "HEALTH " << cur_health << '\n';
	file << "MANA " << cur_sp << '\n';
	file << "EXPERIENCE " << experience << '\n';
	file << "SKILLPTS " << skill_pts << '\n';
	file << "LEVEL " << level << '\n';
	for(int i = 0; i < 15; i++) {
		eStatus stat = (eStatus) i;
		if(status[stat] != 0)
			file << "STATUS " << i << ' ' << status[stat] << '\n';
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
	for(int i; i < 24; i++)
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
	while(bin) { // continue as long as no error, such as eof, occurs
		getline(bin, cur);
		printf("Parsing line in pcN.txt: %s\n", cur.c_str());
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

std::ostream& operator << (std::ostream& out, eMainStatus& e){
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
