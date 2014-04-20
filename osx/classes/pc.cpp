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
#include <sstream>


#include "classes.h"
#include "oldstructs.h"

cPlayer& cPlayer::operator = (legacy::pc_record_type old){
	int i;
	main_status = (eMainStatus) old.main_status;
	name = old.name;
	for(i = 0; i < 20; i++)
		skills[i] = old.skills[i];
	max_health = old.max_health;
	cur_health = old.cur_health;
	max_sp = old.max_sp;
	cur_sp = old.cur_sp;
	experience = old.experience;
	skill_pts = old.skill_pts;
	level = old.level;
	for(i = 0; i < 15; i++){
		status[i] = old.status[i];
		//advan[i] = old.advan[i];
		traits[i] = old.traits[i];
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
	short tnl = 100,i,store_per = 100;
	static const short rp[3] = {0,12,20};
	static const short ap[15] = {10,20,8,10,4, 6,10,7,12,15, -10,-8,-8,-20,-8};
	
	tnl = (tnl * (100 + rp[race])) / 100;
	for (i = 0; i < 15; i++)
		if (traits[i]) 
			store_per = store_per + ap[i];
	
	tnl = (tnl * store_per) / 100;	
	
	return tnl;
}

cPlayer::cPlayer(){
	short i;
	main_status = MAIN_STATUS_ABSENT;
	name = "\n";
	
	for (i = 0; i < 30; i++)
		skills[i] = (i < 3) ? 1 : 0;
	cur_health = 6; 
	max_health = 6; 
 	cur_sp = 0; 
 	max_sp = 0; 
 	experience = 0; 
	skill_pts = 60; 
	level = 1;
 	for (i = 0; i < 15; i++)
 		status[i] = 0;
	for (i = 0; i < 24; i++)
		items[i] = cItemRec();
	for (i = 0; i < 24; i++)
		equip[i] = false;
	
 	for (i = 0; i < 62; i++) {
		priest_spells[i] = (i < 30) ? true : false;
		mage_spells[i] = (i < 30) ? true : false;
	}
	which_graphic = 0;
	weap_poisoned = 24;
	
	for (i = 0; i < 15; i++) {
		//advan[i] = false;
		traits[i] = false;	
	}		
	race = RACE_HUMAN;
	//exp_adj = 100;
	direction = 0;
}

cPlayer::cPlayer(long key,short slot){
	short i;
	main_status = MAIN_STATUS_ALIVE;
	if(key == 'dbug'){
		switch (slot) {
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
		for (i = 0; i < 30; i++)
			skills[i] = (i < 3) ? 20 : 8;
		cur_health = 60; 
		max_health = 60; 
		cur_sp = 90; 
		max_sp = 90; 
		experience = 0; 
		skill_pts = 60; 
		level = 1;
		for (i = 0; i < 15; i++)
			status[i] = 0;
		for (i = 0; i < 24; i++)
			items[i] = cItemRec();
		for (i = 0; i < 24; i++)
			equip[i] = false;
		
		for (i = 0; i < 62; i++) {
			priest_spells[i] = true;
			mage_spells[i] = true;
		}
		//which_graphic = num * 3 + 1;	// 1, 4, 7, 10, 13, 16
		which_graphic = slot + 4;		// 4, 5, 6, 7,  8,  9
		weap_poisoned = 24; // was 16, as an E2 relic
		
		for (i = 0; i < 15; i++) {
			//advan[i] = false;
			traits[i] = false;	
		}		
		
		race = RACE_HUMAN;
		//exp_adj = 100;
		direction = 0;
	}else if(key == 'dflt'){
		static const short pc_stats[6][19] = {
			{8,6,2,  6,0,0,0,0,0,  0,0,0,0,1,  0,0,2,0,0},
			{8,7,2,  0,0,6,3,0,3,  0,0,0,0,0,  0,0,0,2,0},
			{8,6,2,  3,3,0,0,2,0,  0,0,0,0,0,  4,4,0,2,1},
			{3,2,6,  2,0,0,2,0,0,  3,0,3,0,1,  0,0,0,0,0},
			{2,2,6,  3,0,0,2,0,0,  2,1,4,0,0,  0,0,0,0,1},
			{2,2,6,  0,2,0,2,0,1,  0,3,3,2,0,  0,0,0,0,0}
		};
		static const short pc_health[6] = {22,24,24,16,16,18};
		static const short pc_sp[6] = {0,0,0,20,20,21};
		static const short pc_graphics[6] = {3,32,29,16,23,14};
		static const short pc_race[6] = {0,2,1,0,0,0};
		static const short pc_t[6][15] = {
			{0,0,1,0,0, 0,1,0,0,0, 0,1,0,0,0},		
			{1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0},	
			{0,0,0,1,0, 0,0,0,0,0, 0,0,1,0,0},	
			{0,1,0,0,0, 0,0,0,0,0, 0,0,0,0,0},	
			{0,0,0,0,1, 0,1,1,0,0, 0,0,0,0,1},	
			{0,1,0,0,0, 0,0,0,0,0, 0,0,0,0,0}
		};
		
		main_status = MAIN_STATUS_ALIVE;
		switch (slot) {
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
		
		for (i = 0; i < 19; i++)
			skills[i] = pc_stats[slot][i];
		cur_health = pc_health[slot]; 
		max_health = pc_health[slot]; 
		experience = 0; 
		skill_pts = 0; 
		level = 1;
		
		for (i = 0; i < 15; i++)
			status[i] = 0;
		for (i = 0; i < 24; i++)
			items[i] = cItemRec();
		for (i = 0; i < 24; i++)
			equip[i] = false;
		cur_sp = pc_sp[slot]; 
		max_sp = pc_sp[slot]; 
		for (i = 0; i < 62; i++) {
			priest_spells[i] = (i < 30) ? true : false;
			mage_spells[i] = (i < 30) ? true : false;
		}
		for (i = 0; i < 15; i++) {
			traits[i] = pc_t[slot][i];
			//advan[i] = false;
		}
		
		race = (eRace) pc_race[slot];
		//exp_adj = 100;
		direction = 0;
		
		which_graphic = pc_graphics[slot];
	}
}

void operator += (eMainStatus& stat, eMainStatus othr){
	if(othr == MAIN_STATUS_SPLIT)
		stat = (eMainStatus) (10 + stat);
}

void operator -= (eMainStatus& stat, eMainStatus othr){
	if(othr == MAIN_STATUS_SPLIT)
		stat = (eMainStatus) (-10 + stat);
}

void cPlayer::writeTo(std::ostream& file){
	file << "STATUS main " << main_status << '\n';
	file << "NAME " << name << '\n';
	file << "SKILL hp " << max_health << '\n';
	file << "SKILL sp " << max_sp << '\n';
	for(int i = 0; i < 30; i++)
		if(skills[i] > 0)
			file << "SKILL " << i << ' ' << skills[i] << '\n';
	file << "HEALTH " << cur_health << '\n';
	file << "MANA " << cur_sp << '\n';
	file << "EXPERIENCE " << experience << '\n';
	file << "SKILLPTS " << skill_pts << '\n';
	file << "LEVEL " << level << '\n';
	for(int i = 0; i < 15; i++)
		if(status[i] != 0)
			file << "STATUS " << i << ' ' << status[i] << '\n';
	for(int i = 0; i < 24; i++)
		if(equip[i])
			file << "EQUIP " << i << '\n';
	for(int i = 0; i < 62; i++)
		if(mage_spells[i])
			file << "MAGE " << i << '\n';
	for(int i = 0; i < 62; i++)
		if(priest_spells[i])
			file << "PRIEST " << i << '\n';
	for(int i = 0; i < 62; i++)
		if(traits[i])
			file << "TRAIT " << i << '\n';
	file << "ICON " <<  which_graphic << '\n';
	file << "RACE " << race << '\n';
	file << "DIRECTION " << direction << '\n';
	file << "POISON " << weap_poisoned << '\n';
	file << '\f';
	for(int i; i < 24; i++)
		if(items[i].variety > ITEM_TYPE_NO_ITEM){
			file << "ITEM " << i << '\n';
			items[i].writeTo(file);
		}
}

void cPlayer::readFrom(std::istream& file){
	std::istringstream bin, sin;
	std::string cur;
	getline(file, cur, '\f');
	bin.str(cur);
	while(bin) { // continue as long as no error, such as eof, occurs
		getline(bin, cur);
		sin.str(cur);
		sin >> cur;
		if(cur == "STATUS"){
			int i;
			sin >> i;
			if(i < 0) sin >> main_status;
			else sin >> status[i];
		}else if(cur == "NAME")
			sin >> name;
		else if(cur == "SKILL"){
			int i;
			sin >> i;
			switch(i){
				case -1:
					sin >> max_sp;
					break;
				case -2:
					sin >> max_health;
					break;
				default:
					sin >> skills[i];
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
			int i;
			sin >> i;
			sin >> status[i];
		}else if(cur == "ITEM"){
			int i;
			sin >> i >> cur;
			items[i].readAttrFrom(cur, sin);
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
			traits[i] = true;
		}else if(cur == "ICON")
			sin >> which_graphic;
		else if(cur == "DIRECTION")
			sin >> direction;
		else if(cur == "RACE")
			sin >> race;
		else if(cur == "POISON")
			sin >> weap_poisoned;
	}
}

std::ostream& operator << (std::ostream& out, eMainStatus& e){
	return out << (int) e;
}

std::istream& operator >> (std::istream& in, eMainStatus& e){
	int i;
	in >> i;
	if(i > 0 && i < 18 && i !=8 && i != 9)
		e = (eMainStatus) i;
	else e = MAIN_STATUS_ABSENT;
	return in;
}
