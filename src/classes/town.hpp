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
#include "simpletypes.hpp"
#include "location.hpp"
#include "special.hpp"
#include "monster.hpp"
#include "talking.hpp"
#include "item.hpp"

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

class cTown { // formerly town_record_type
protected:
	cScenario& scenario;
public:
	class cWandering { // formerly wandering_type
	public:
		mon_num_t monst[4];
		
		bool isNull();
		void append(legacy::wandering_type old);
	};
	class cItem { // formerly preset_item_type
	public:
		location loc;
		short code,ability;
		int charges = -1;
		bool always_there = false, property = false, contained = false;
		
		void append(legacy::preset_item_type old);
		cItem();
		cItem(location loc, short num, ::cItem& item);
	};
	class cField { // formerly preset_field_type
	public:
		location loc;
		eFieldType type;
		
		void append(legacy::preset_field_type old);
		cField() : type(FIELD_DISPEL) {}
	};
	short town_chop_time,town_chop_key;
	int bg_town, bg_fight;
	std::array<cWandering,4> wandering;
	location wandering_locs[4];
	std::vector<spec_loc_t> special_locs;
	std::vector<sign_loc_t> sign_locs;
	eLighting lighting_type;
	location start_locs[4];
	location exit_locs[4];
	short exit_specs[4];
	rectangle in_town_rect;
	std::vector<cItem> preset_items;
	std::vector<cTownperson> creatures;
	short max_num_monst;
	std::vector<cField> preset_fields;
	short spec_on_entry,spec_on_entry_if_dead;
	short spec_on_hostile;
	std::array<cTimer,8> timers;
	std::vector<cSpecial> specials;
	bool strong_barriers, defy_mapping, defy_scrying;
	bool is_hidden, has_tavern;
	short difficulty;
	std::string town_name;
	// Using std::array here so we can have .size()
	// This'll make the transition smoother once it becomes a vector.
	std::vector<info_rect_t> room_rect;
	std::array<std::string,3> comment;
	std::vector<std::string> spec_strs;
	cSpeech talking;
	
	virtual ~cTown(){}
	virtual void append(legacy::big_tr_type& old, int town_num);
	virtual void append(legacy::ave_tr_type& old, int town_num);
	virtual void append(legacy::tiny_tr_type& old, int town_num);
	virtual ter_num_t& terrain(size_t x, size_t y) = 0;
	virtual unsigned char& lighting(size_t i, size_t r) = 0;
	virtual size_t max_dim() const = 0;
	virtual bool is_templated() const {return false;}
	void init_start();
	void set_up_lights();
	short light_obscurity(short x,short y); // Obscurity function used for calculating lighting
	
	explicit cTown(cScenario& scenario);
	void append(legacy::town_record_type& old);
	virtual void writeTerrainTo(std::ostream& file) = 0;
	virtual void readTerrainFrom(std::istream& file) = 0;
};

std::ostream& operator<< (std::ostream& out, eLighting light);
std::istream& operator>> (std::istream& in, eLighting& light);

#endif
