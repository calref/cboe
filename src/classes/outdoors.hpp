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

#include "location.hpp"
#include "special.hpp"
#include "simpletypes.hpp"
#include "monster.hpp"

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
	cScenario* scenario;
public:
	class cWandering { // formerly out_wandering_type
	public:
		std::array<mon_num_t,7> monst;
		std::array<mon_num_t,3> friendly;
		short spec_on_meet,spec_on_win,spec_on_flee;
		short end_spec1,end_spec2;
		bool cant_flee, forced;
		
		bool isNull();
		void import_legacy(legacy::out_wandering_type old);
		void writeTo(std::ostream& file, std::string prefix = "") const;
		void readFrom(std::istream& sin);
		cWandering();
	};
	class cCreature { // formerly outdoor_creature_type
	public:
		bool exists = false;
		short direction;
		cWandering what_monst;
		location which_sector,m_loc,home_sector; // home_sector is the sector it was spawned in
		
		void import_legacy(legacy::outdoor_creature_type old);
	};
	short x,y; // Used while loading legacy scenarios.
	ter_num_t terrain[48][48];
	std::vector<spec_loc_t> special_locs;
	std::vector<spec_loc_t> city_locs;
	std::vector<sign_loc_t> sign_locs;
	std::array<cWandering,4> wandering, special_enc;
	std::array<location,4> wandering_locs;
	std::vector<cSpecial> specials;
	std::string out_name;
	// Using std::array here so we can have .size()
	// This'll make the transition smoother once it becomes a vector.
	std::vector<info_rect_t> info_rect;
	std::string comment;
	std::vector<std::string> spec_strs;
	bool special_spot[48][48];
	bool roads[48][48];
	eAmbientSound ambient_sound;
	snd_num_t out_sound;
	int bg_out, bg_fight, bg_town, bg_dungeon;
	// Persistent data for saved games
	std::array<std::bitset<48>, 48> maps;
	
	explicit cOutdoors(cScenario& scenario);
	void import_legacy(legacy::outdoor_record_type& old);
	void reattach(cScenario& to);
};

#endif
