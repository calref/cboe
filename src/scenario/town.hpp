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
	bool any_items() const;
	
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

#endif
