/*
 *  creatlist.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#ifndef CREATLIST_H
#define CREATLIST_H

#include "monster.h"

namespace legacy {
	struct creature_list_type;
	struct creature_data_type;
};

class cPopulation { // formerly creature_list_type
//	class cCreature { // formerly creature_data_type
//	public:
//		short active, attitude;
//		unsigned char number;
//		location m_loc;
//		cMonster m_d;
//		bool mobile;
//		short summoned;
//		cTown::cCreature monst_start;
//		
//		cCreature& operator = (legacy::creature_data_type old);
//	};
	cCreature dudes[60];
public:
	short which_town;
	short friendly;
	
	cPopulation& operator= (legacy::creature_list_type old);
	cCreature& operator[](size_t n);
};

#endif