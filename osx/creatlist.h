/*
 *  creatlist.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

namespace legacy {
	struct creature_list_type;
	struct creature_data_type;
};

class cPopulation { // formerly creature_list_type
public:
	class cCreature { // formerly creature_data_type
	public:
		short active, attitude;
		unsigned char number;
		location m_loc;
		cMonster m_d;
		bool mobile;
		short summoned;
		cTown::cCreature monst_start;
		
		cCreature& operator = (legacy::creature_data_type old);
	};
	cCreature dudes[60];
	short which_town;
	short friendly;
	
	cPopulation& operator = (legacy::creature_list_type old);
};
