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
#include "monster.hpp"
#include "area.hpp"

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

class cOutdoors : public cArea {
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
		void import_legacy(legacy::out_wandering_type const &old);
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
		
		void import_legacy(legacy::outdoor_creature_type const &old);
	};
	short x,y; // Used while loading legacy scenarios.
	std::vector<spec_loc_t> city_locs;
	std::array<cWandering,4> wandering, special_enc;
	std::array<location,4> wandering_locs;
	std::string comment;
	std::vector<std::string> spec_strs;
	bool special_spot[48][48];
	bool roads[48][48];
	eAmbientSound ambient_sound = AMBIENT_NONE;
	snd_num_t out_sound;
	int bg_out, bg_fight, bg_town, bg_dungeon;
	
	explicit cOutdoors(cScenario& scenario);
	void import_legacy(legacy::outdoor_record_type const &old);
	void reattach(cScenario& to);

	std::string &get_special_string(int id);
	std::string const &get_special_string(int id) const;
};

#endif
