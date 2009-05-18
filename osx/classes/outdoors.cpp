/*
 *  outdoors.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>
using namespace std;

#include "classes.h"
#include "oldstructs.h"

extern cScenario scenario;

__attribute__((deprecated))
cOutdoors& cOutdoors::operator = (legacy::outdoor_record_type& old){
	int i,j;
	for(i = 0; i < 48; i++)
		for(j = 0; j < 48; j++){
			terrain[i][j] = old.terrain[i][j];
			if(scenario.ter_types[terrain[i][j]].i == 3000) // marker to indicate it used to be a special spot
				special_spot[i][j] = true;
			else special_spot[i][j] = false;
		}
	for(i = 0; i < 18; i++){
		special_locs[i].x = old.special_locs[i].x;
		special_locs[i].y = old.special_locs[i].y;
		special_id[i] = old.special_id[i];
	}
	for(i = 0; i < 8; i++){
		exit_locs[i].x = old.exit_locs[i].x;
		exit_locs[i].y = old.exit_locs[i].y;
		exit_dests[i] = old.exit_dests[i];
		sign_locs[i].x = old.sign_locs[i].x;
		sign_locs[i].y = old.sign_locs[i].y;
		info_rect[i].top = old.info_rect[i].top;
		info_rect[i].left = old.info_rect[i].left;
		info_rect[i].bottom = old.info_rect[i].bottom;
		info_rect[i].right = old.info_rect[i].right;
	}
	for(i = 0; i < 4; i++){
//		for(j = 0; j < 7; j++){
//			wandering[i].monst[j] = old.wandering[i].monst[j];
//			special_enc[i].monst[j] = old.special_enc[i].monst[j];
//		}
//		for(j = 0; j < 3; j++){
//			wandering[i].friendly[j] = old.wandering[i].friendly[j];
//			special_enc[i].friendly[j] = old.special_enc[i].friendly[j];
//		}
//		wandering[i].spec_on_meet = old.wandering[i].spec_on_meet;
//		special_enc[i].spec_on_meet = old.special_enc[i].spec_on_meet;
//		wandering[i].spec_on_win = old.wandering[i].spec_on_win;
//		special_enc[i].spec_on_win = old.special_enc[i].spec_on_win;
//		wandering[i].spec_on_flee = old.wandering[i].spec_on_flee;
//		special_enc[i].spec_on_flee = old.special_enc[i].spec_on_flee;
//		wandering[i].cant_flee = old.wandering[i].cant_flee;
//		special_enc[i].cant_flee = old.special_enc[i].cant_flee;
//		wandering[i].end_spec1 = old.wandering[i].end_spec1;
//		special_enc[i].end_spec1 = old.special_enc[i].end_spec1;
//		wandering[i].end_spec2 = old.wandering[i].end_spec2;
//		special_enc[i].end_spec2 = old.special_enc[i].end_spec2;
		wandering[i] = old.wandering[i];
		special_enc[i] = old.special_enc[i];
		wandering_locs[i].x = old.wandering_locs[i].x;
		wandering_locs[i].y = old.wandering_locs[i].y;
	}
	for(i = 0; i < 180; i++)
		strlens[i] = old.strlens[i];
	for(i = 0; i < 60; i++)
		specials[i] = old.specials[i];
	return *this;
}

cOutdoors::cOutdoors(){
	short i,j;
	location d_loc(100,0);
	cOutdoors::cWandering d_monst = {{0,0,0,0,0,0,0},{0,0,0},-1,-1,-1,0,-1,-1};
	rectangle d_rect;
	location locs[4] = {loc(8,8),loc(32,8),loc(8,32),loc(32,32)};
	
	for (i = 0; i < 48; i++)
		for (j = 0; j < 48; j++) {
			terrain[i][j] = 5; // formerly 0
		}
	
	for (i = 0; i < 18; i++) {
		special_locs[i] = d_loc;
		special_id[i] = 0;
	}
	for (i = 0; i < 8; i++) {
		exit_locs[i] = d_loc;
		exit_dests[i] = 0;
		sign_locs[i] = d_loc;
		sign_locs[i].x = 100;
		info_rect[i] = d_rect;
	}
	for (i = 0; i < 4; i++) {
		wandering[i] = d_monst;
		wandering_locs[i] = locs[i];
		special_enc[i] = d_monst;
	}
	for (i = 0; i < 60; i++) {
		specials[i] = cSpecial();
	}
	
}

__attribute__((deprecated))
char(& cOutdoors::out_strs(short i))[256]{
	if(i == 0) return out_name;
	if(i >= 1 && i < 9) return rect_names[i - 1];
	if(i == 9) return comment;
	if(i >= 10 && i < 100) return spec_strs[i - 10];
	if(i >= 100 && i < 108) return sign_strs[i - 100];
	return comment;
}

cOutdoors::cWandering& cOutdoors::cWandering::operator = (legacy::out_wandering_type old){
	for(int i = 0; i < 7; i++)
		monst[i] = old.monst[i];
	for(int j = 0; j < 3; j++)
		friendly[j] = old.friendly[j];
	spec_on_meet = old.spec_on_meet;
	spec_on_win = old.spec_on_win;
	spec_on_flee = old.spec_on_flee;
	cant_flee = old.cant_flee;
	end_spec1 = old.end_spec1;
	end_spec2 = old.end_spec2;
	return *this;
}

cOutdoors::cCreature& cOutdoors::cCreature::operator = (legacy::outdoor_creature_type old){
	exists = old.exists;
	direction = old.direction;
	what_monst = old.what_monst;
	which_sector.x = old.which_sector.x;
	which_sector.y = old.which_sector.y;
	m_loc.x = old.m_loc.x;
	m_loc.y = old.m_loc.y;
	return *this;
}

void cOutdoors::writeTo(ostream& file){
//	for(int i = 0; i < 48; i++){
//		file << expl[i][0];
//		for(int j = 1; j < 48; j++){
//			file << '\t' << expl[i][j];
//		}
//		file << endl;
//	}
}

void cOutdoors::cWandering::writeTo(ostream& file, string prefix){
	for(int i = 0; i < 7; i++)
		file << prefix << "HOSTILE " << i << ' ' << monst[i] << endl;
	for(int i = 0; i < 3; i++)
		file << prefix << "FRIEND " << i << ' ' << friendly[i] << endl;
	file << prefix << "MEET " << spec_on_meet << endl;
	file << prefix << "WIN " << spec_on_win << endl;
	file << prefix << "FLEE " << spec_on_flee << endl;
	file << prefix << "FLAGS " << cant_flee << endl;
	file << prefix << "SDF " << end_spec1 << ' ' << end_spec2 << endl;
}

void cOutdoors::cWandering::readAttrFrom(string cur, istream& sin){
	if(cur == "HOSTILE"){
		int i;
		sin >> i;
		sin >> monst[i];
	}else if(cur == "FRIEND"){
		int i;
		sin >> i;
		sin >> friendly[i];
	}else if(cur == "MEET")
		sin >> spec_on_meet;
	else if(cur == "WIN")
		sin >> spec_on_win;
	else if(cur == "FLEE")
		sin >> spec_on_flee;
	else if(cur == "FLAGS")
		sin >> cant_flee;
	else if(cur == "SDF")
		sin >> end_spec1 >> end_spec2;
}
