/*
 *  outdoors.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#ifndef BOE_DATA_OUTDOORS_H
#define BOE_DATA_OUTDOORS_H

#include <string>
#include <iosfwd>
#include <array>

#include "location.h"
#include "special.h"
#include "simpletypes.h"
#include "monster.h"

namespace legacy {
	struct out_wandering_type;
	struct outdoor_record_type;
	struct outdoor_creature_type;
};

class cScenario;

enum eAmbientSound {
	AMBIENT_NONE,
	AMBIENT_DRIP,
	AMBIENT_BIRD,
	AMBIENT_CUSTOM,
};

class cOutdoors {
	cScenario& scenario;
public:
	class cWandering { // formerly out_wandering_type
	public:
		std::array<mon_num_t,7> monst;
		std::array<mon_num_t,3> friendly;
		short spec_on_meet,spec_on_win,spec_on_flee,cant_flee;
		short end_spec1,end_spec2;
		
		bool isNull();
		void append(legacy::out_wandering_type old);
		void writeTo(std::ostream& file, std::string prefix = "") const;
		void readFrom(std::istream& sin);
		cWandering();
	};
	class cCreature { // formerly outdoor_creature_type
	public:
		bool exists;
		short direction;
		cWandering what_monst;
		location which_sector,m_loc,home_sector; // home_sector is the sector it was spawned in
		
		void append(legacy::outdoor_creature_type old);
	};
	short x,y; // Used while loading legacy scenarios.
	ter_num_t terrain[48][48];
	std::array<spec_loc_t,18> special_locs;
	location exit_locs[8];
	short exit_dests[8];
	location sign_locs[8];
	std::array<cWandering,4> wandering, special_enc;
	location wandering_locs[4];
	std::array<cSpecial,60> specials;
	std::string out_name;
	// Using std::array here so we can have .size()
	// This'll make the transition smoother once it becomes a vector.
	std::array<info_rect_t,8> info_rect;
	std::string comment;
	std::array<std::string,90> spec_strs;
	std::array<std::string,8> sign_strs;
	bool special_spot[48][48];
	eAmbientSound ambient_sound;
	snd_num_t out_sound;
	int bg_out, bg_fight, bg_town, bg_dungeon;
	
	explicit cOutdoors(cScenario& scenario, bool init_strings = false);
	void append(legacy::outdoor_record_type& old);
};

#endif
