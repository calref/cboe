/*
 *  town.h
 *  BoE
 *
 *  Created by Celtic Minsrel on 22/04/09.
 *
 */

#ifndef BOE_DATA_TOWN_H
#define BOE_DATA_TOWN_H

#include <vector>
#include <iosfwd>
#include <array>
#include <bitset>
#include "location.hpp"
#include "special.hpp"
#include "monster.hpp"
#include "talking.hpp"
#include "item.hpp"
#include "area.hpp"

namespace legacy {
	struct town_record_type;
	struct big_tr_type;
	struct ave_tr_type;
	struct tiny_tr_type;
	struct creature_start_type;
	struct wandering_type;
	struct preset_item_type;
	struct preset_field_type;
};

enum eLighting {
	LIGHT_NORMAL = 0,
	LIGHT_DARK = 1,
	LIGHT_DRAINS = 2,
	LIGHT_NONE = 3,
};

class cScenario;

class cTown : public cArea { // formerly town_record_type
protected:
	cScenario* scenario;
public:
	class cWandering { // formerly wandering_type
	public:
		std::array<mon_num_t,4> monst;
		
		bool isNull();
		void import_legacy(legacy::wandering_type old);
	};
	class cItem { // formerly preset_item_type
	public:
		location loc;
		short code,ability;
		int charges = -1;
		bool always_there = false, property = false, contained = false;
		
		void import_legacy(legacy::preset_item_type old);
		cItem();
		cItem(location loc, short num, ::cItem& item);
	};
	class cField { // formerly preset_field_type
	public:
		location loc;
		eFieldType type;
		
		void import_legacy(legacy::preset_field_type old);
		cField() : type(FIELD_DISPEL) {}
		cField(location l, eFieldType t) : loc(l), type(t) {}
	};
	short town_chop_time,town_chop_key;
	int bg_town, bg_fight;
	std::array<cWandering,4> wandering;
	std::array<location, 4> wandering_locs;
	eLighting lighting_type;
	std::vector<boost::dynamic_bitset<>> lighting;
	std::array<location, 4> start_locs;
	std::array<spec_loc_t, 4> exits;
	rectangle in_town_rect;
	std::vector<cItem> preset_items;
	std::vector<cTownperson> creatures;
	long max_num_monst;
	std::vector<cField> preset_fields;
	short spec_on_entry,spec_on_entry_if_dead;
	short spec_on_hostile;
	std::array<cTimer,8> timers;
	std::vector<cSpecial> specials;
	bool strong_barriers, defy_mapping, defy_scrying;
	bool is_hidden, has_tavern;
	short difficulty;
	// Using std::array here so we can have .size()
	// This'll make the transition smoother once it becomes a vector.
	std::array<std::string,3> comment;
	std::vector<std::string> spec_strs;
	cSpeech talking;
	// Persistent data for saved games
	std::bitset<64> item_taken;
	bool can_find;
	long m_killed;
	
	template<typename T>
	void import_legacy(T& old, int town_num);
	void init_start();
	void set_up_lights();
	short light_obscurity(short x,short y); // Obscurity function used for calculating lighting
	bool is_cleaned_out();
	
	explicit cTown(cScenario& scenario, size_t dim);
	void import_legacy(legacy::town_record_type& old);
	void reattach(cScenario& to);
	void writeTerrainTo(std::ostream& file);
	void readTerrainFrom(std::istream& file);
};

std::ostream& operator<< (std::ostream& out, eLighting light);
std::istream& operator>> (std::istream& in, eLighting& light);

#endif
