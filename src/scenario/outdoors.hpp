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
#include <bitset>

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
class cTagFile_Page;

enum eAmbientSound {
	AMBIENT_NONE,
	AMBIENT_DRIP,
	AMBIENT_BIRD,
	AMBIENT_CUSTOM,
};

template<size_t x, size_t y>
using bitmap = std::array<std::bitset<y>, x>;

class cOutdoors : public cArea {
	cScenario* scenario;
public:
	// Definition of an outdoor combat encounter
	class cWandering { // formerly out_wandering_type
	public:
		// max 7 types of monster per encounter
		std::array<mon_num_t,7> monst;
		// max 3 types of friendly npc per encounter
		std::array<mon_num_t,3> friendly;
		short spec_on_meet,spec_on_win,spec_on_flee;
		short end_spec1,end_spec2;
		bool cant_flee, forced;
		
		bool isNull() const;
		void import_legacy(legacy::out_wandering_type old);
		void writeTo(cTagFile_Page& page) const;
		void readFrom(const cTagFile_Page& page);
		cWandering();
	};
	// Instantiation of an outdoor wandering monster on the map
	class cCreature { // formerly outdoor_creature_type
	public:
		bool exists = false;
		short direction;
		cWandering what_monst;
		location which_sector,m_loc,home_sector; // home_sector is the sector it was spawned in
		
		void import_legacy(legacy::outdoor_creature_type old);
		void writeTo(cTagFile_Page& page) const;
		void readFrom(const cTagFile_Page& page);
	};
	short x,y; // Used while loading legacy scenarios.
	std::vector<spec_loc_t> city_locs;
	std::array<cWandering,4> wandering, special_enc;
	std::array<location,4> wandering_locs;
	std::string comment;
	std::vector<std::string> spec_strs;
	bitmap<48, 48> special_spot;
	bitmap<48, 48> roads;
	eAmbientSound ambient_sound = AMBIENT_NONE;
	snd_num_t out_sound;
	int bg_out, bg_fight, bg_town, bg_dungeon;
	
	explicit cOutdoors(cScenario& scenario);
	void import_legacy(legacy::outdoor_record_type& old);
	void reattach(cScenario& to);
};

// Store a version of the outdoor details for undo history.
// This could be made a struct that cTown contains, and that would eliminate the next 2 functions, but it would
// require changing every reference to these detail values in the game and fileio code, making them more verbose. I don't know
// if that's worth it.
struct out_details_t {
	std::string name;
	eAmbientSound ambient_sound;
	snd_num_t out_sound;
	int bg_out;
	int bg_fight;
	int bg_town;
	int bg_dungeon;
	std::string comment;

	bool operator==(const out_details_t& other) const {
		CHECK_EQ(other,name);
		CHECK_EQ(other,ambient_sound);
		CHECK_EQ(other,out_sound);
		CHECK_EQ(other,bg_out);
		CHECK_EQ(other,bg_fight);
		CHECK_EQ(other,bg_town);
		CHECK_EQ(other,bg_dungeon);
		CHECK_EQ(other,comment);
		return true;
	}
	bool operator!=(const out_details_t& other) const { return !(*this == other); }
};

inline out_details_t details_from_out(cOutdoors& out) {
	return {
		out.name,
		out.ambient_sound,
		out.out_sound,
		out.bg_out,
		out.bg_fight,
		out.bg_town,
		out.bg_dungeon,
		out.comment
	};
}

inline void out_set_details(cOutdoors& out, const out_details_t& details) {
	out.name = details.name;
	out.ambient_sound = details.ambient_sound;
	out.out_sound = details.out_sound;
	out.bg_out = details.bg_out;
	out.bg_fight = details.bg_fight;
	out.bg_town = details.bg_town;
	out.bg_dungeon = details.bg_dungeon;
	out.comment = details.comment;
}

#endif
