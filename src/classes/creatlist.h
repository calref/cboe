/*
 *  creatlist.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#ifndef BOE_DATA_CREATLIST_H
#define BOE_DATA_CREATLIST_H

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
	
	void append(legacy::creature_list_type old);
	void assign(size_t n, const cTownperson& other, const cMonster& base, bool easy, int difficulty_adjust);
	cCreature& operator[](size_t n);
	const cCreature& operator[](size_t n) const;
	cPopulation() : which_town(200) {}
	// Apparently Visual Studio needs this to work
	cPopulation& operator=(const cPopulation& other) = default;
};

#endif
