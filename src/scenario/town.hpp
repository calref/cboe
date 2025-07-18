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
#include <boost/dynamic_bitset.hpp>
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
	friend class cScenario; // so the read/save functions can access item_taken directly
protected:
	cScenario* scenario;
public:
	class cWandering { // formerly wandering_type
	public:
		std::array<mon_num_t,4> monst;
		
		bool isNull() const;
		void import_legacy(legacy::wandering_type old);
	};
	class cItem { // formerly preset_item_type
	public:
		location loc;
		item_num_t code;
		eEnchant ability;
		int charges = -1;
		bool always_there = false, property = false, contained = false;
		// For detecting actual changes to town items in the editor
		bool operator==(const cItem& other);
		bool operator!=(const cItem& other) { return !(*this == other); }
		
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
	size_t count_hostiles() const;
	long max_num_monst;
	std::vector<cField> preset_fields;
	short spec_on_entry,spec_on_entry_if_dead;
	short spec_on_hostile;
	std::array<cTimer,8> timers;
	bool strong_barriers, defy_mapping, defy_scrying;
	// Whether the town STARTS OUT hidden.
	bool is_hidden;
	bool has_tavern;
	short difficulty;
	// Using std::array here so we can have .size()
	// This'll make the transition smoother once it becomes a vector.
	std::array<std::string,3> comment;
	std::vector<std::string> spec_strs;
	cSpeech talking;
private:
	// Persistent data for saved games
	boost::dynamic_bitset<> item_taken;
public:
	std::vector<location> door_unlocked;
	// Whether the town IS CURRENTLY visible.
	bool can_find;
	long m_killed = 0;
	
	template<typename T>
	void import_legacy(T& old, int town_num);
	void init_start();
	void set_up_lights();
	short light_obscurity(short x,short y) const; // Obscurity function used for calculating lighting
	bool is_cleaned_out() const;
	bool any_preset_items() const;
	
	explicit cTown(cScenario& scenario, size_t dim);
	void import_legacy(legacy::town_record_type& old);
	void reattach(cScenario& to);
	// Work with the item_taken bitset
	bool is_item_taken(size_t i) const;
	void clear_items_taken();
	void set_item_taken(size_t i, bool val = true);
};

std::ostream& operator<< (std::ostream& out, eLighting light);
std::istream& operator>> (std::istream& in, eLighting& light);

// Store a version of the town details for undo history.
// This could be made a struct that cTown contains, and that would eliminate the next 2 functions, but it would
// require changing every reference to these detail values in the game and fileio code, making them more verbose. I don't know
// if that's worth it.
struct town_details_t {
	std::string name;
	short town_chop_time;
	short town_chop_key;
	long max_num_monst;
	short difficulty;
	eLighting lighting_type;
	std::array<std::string,3> comment;

	bool operator==(const town_details_t& other) const {
		CHECK_EQ(other,name);
		CHECK_EQ(other,town_chop_key);
		CHECK_EQ(other,max_num_monst);
		CHECK_EQ(other,difficulty);
		CHECK_EQ(other,lighting_type);
		for(int i = 0; i < comment.size(); ++i){
			if(other.comment[i] != comment[i]) return false;
		}
		return true;
	}
	bool operator!=(const town_details_t& other) const { return !(*this == other); }
};

inline town_details_t details_from_town(cTown& town) {
	return {
		town.name,
		town.town_chop_time,
		town.town_chop_key,
		town.max_num_monst,
		town.difficulty,
		town.lighting_type,
		town.comment
	};
}

inline void town_set_details(cTown& town, const town_details_t& details) {
	town.name = details.name;
	town.town_chop_time = details.town_chop_time;
	town.town_chop_key = details.town_chop_key;
	town.max_num_monst = details.max_num_monst;
	town.difficulty = details.difficulty;
	town.lighting_type = details.lighting_type;
	town.comment = details.comment;
}

// Store a version of the advanced town details for undo history.
// This could be made a struct that cTown contains, and that would eliminate the next 2 functions, but it would
// require changing every reference to these detail values in the game and fileio code, making them more verbose. I don't know
// if that's worth it.
struct town_advanced_t {
	std::array<spec_loc_t, 4> exits;
	short spec_on_entry;
	short spec_on_entry_if_dead;
	short spec_on_hostile;
	int bg_town;
	int bg_fight;
	bool is_hidden;
	bool defy_mapping;
	bool defy_scrying;
	bool strong_barriers;
	bool has_tavern;
	boost::optional<rectangle> store_item_rect;

	bool operator==(const town_advanced_t& other) const {
		for(int i = 0; i < exits.size(); ++i){
			if(other.exits[i].spec != exits[i].spec) return false;
			if(other.exits[i] != exits[i]) return false;
		}
		CHECK_EQ(other,spec_on_entry);
		CHECK_EQ(other,spec_on_entry_if_dead);
		CHECK_EQ(other,spec_on_hostile);
		CHECK_EQ(other,bg_town);
		CHECK_EQ(other,bg_fight);
		CHECK_EQ(other,is_hidden);
		CHECK_EQ(other,defy_mapping);
		CHECK_EQ(other,defy_scrying);
		CHECK_EQ(other,strong_barriers);
		CHECK_EQ(other,has_tavern);
		CHECK_EQ(other,store_item_rect);
		return true;
	}
	bool operator!=(const town_advanced_t& other) const { return !(*this == other); }
};

town_advanced_t advanced_from_town(size_t which, cTown& town, cScenario& scenario);
void town_set_advanced(size_t which, cTown& town, cScenario& scenario, const town_advanced_t& details);

#endif
