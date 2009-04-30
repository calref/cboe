/*
 *  creatlist.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include "classes.h"
#include "oldstructs.h"

__attribute__((deprecated))
cPopulation::cCreature& cPopulation::cCreature::operator = (legacy::creature_data_type old){
	active = old.active;
	attitude = old.attitude;
	number = old.number;
	m_loc.x = old.m_loc.x;
	m_loc.y = old.m_loc.y;
	m_d = old.m_d;
	mobile = old.mobile;
	summoned = old.summoned;
	monst_start = old.monst_start;
	return *this;
}

__attribute__((deprecated))
cPopulation& cPopulation::operator = (legacy::creature_list_type old){
	for(int i = 0; i < 60; i++)
		dudes[i] = old.dudes[i];
	which_town = old.which_town;
	friendly = old.friendly;
	return *this;
}
