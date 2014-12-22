/*
 *  special.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "dlogutil.hpp"
#include "classes.h"
#include "oldstructs.h"

cSpecial::cSpecial(){
	type = eSpecType::NONE;
	sd1 = -1;
	sd2 = -1;
	pic = -1;
	pictype = 4;
	m1 = -1;
	m2 = -1;
	ex1a = -1;
	ex1b = -1;
	ex2a = -1;
	ex2b = -1;
	jumpto = -1;
}

void cSpecial::append(legacy::special_node_type& old){
	type = (eSpecType)old.type;
	sd1 = old.sd1;
	sd2 = old.sd2;
	pic = old.pic;
	m1 = old.m1;
	m2 = old.m2;
	ex1a = old.ex1a;
	ex1b = old.ex1b;
	ex2a = old.ex2a;
	ex2b = old.ex2b;
	jumpto = old.jumpto;
	// Now apply any needed conversions.
	switch(old.type) {
		case 55: case 58: case 189: // Large dialogs with 36x36 dialog graphics
			pic -= 700;
			break;
		case 57: case 60: // Large dialogs with monster graphics
			pic -= 400;
			break;
			// TODO: Originally the block nodes supported messages; the new version doesn't.
			// (Will probably need to make special nodes a dynamic vector before fixing this.)
		case 7: case 8: case 9: case 10: // out, town, combat, look block
			type = eSpecType::IF_CONTEXT;
			ex1b = ex1a;
			if(old.type == 7) ex1a = (int) eSpecCtx::OUT_MOVE;
			if(old.type == 8) ex1a = (int) eSpecCtx::TOWN_MOVE;
			if(old.type == 9) ex1a = (int) eSpecCtx::COMBAT_MOVE;
			if(old.type == 10) ex1a = 100;
			break;
		case 24: // ritual of sanctification
			type = eSpecType::IF_CONTEXT;
			ex1c = jumpto;
			jumpto = ex1b;
			ex1a = (int) eSpecCtx::TARGET;
			ex1b = 108; // Spell ID for ritual of sanctification, as seen in cast_town_spell()
			break;
		case 99: case 100: // Add mage/priest spell TODO: Merge these by adding 100 if it's a priest spell
			ex1a += 30;
			ex1b = 1; // Meaning give spell, not take
			break;
		case 148: case 149: // if barrels or crates
			type = eSpecType::IF_OBJECTS;
			ex1a = old.type - 148;
			break;
		case 151: case 152: // if has cave lore or woodsman
			type = eSpecType::IF_TRAIT;
			ex1a = old.type - 147;
			break;
		case 153: // if enough mage lore
			type = eSpecType::IF_STATISTIC;
			if(ex2a >= 0) { // Windows version added "if statistic" much earlier, but it still needs a little conversion.
				switch(ex2a) {
					case 20: ex2a = 19; break; // Max HP
					case 22: ex2a = 20; break; // Max SP
					case 19: ex2a = 100; break; // Current HP
					case 21: ex2a = 101; break; // Current SP
					case 23: ex2a = 102; break; // Experience
					case 24: ex2a = 103; break; // Skill points
					case 25: ex2a = 104; break; // Level
				}
			} else ex2a = 11;
			ex2b = 0;
			break;
		case 229: // Outdoor store - fix spell IDs
			if(ex1b == 1 || ex1b == 2)
				ex1a += 30;
			break;
			// These are ones that were added in the Windows version but only recently added to the Mac version.
		case 28:
			type = eSpecType::DISPLAY_PICTURE;
			giveError("Warning: This scenario contains a Display Picture special node created by the 'Classic Windows' version of the game. Although this version of the game also supports a Display Picture node, the format is incompatible, and automatic conversion is impossible.", "If this is not your scenario, consider contacting the scenario designer to get this fixed.");
			ex1a = 0;
			break;
		case 29:
			type = eSpecType::SDF_RANDOM;
			break;
		case 156:
			type = eSpecType::IF_SPECIES;
			break;
		case 196:
			type = eSpecType::TOWN_CHANGE_LIGHTING;
			break;
		case 197:
			type = eSpecType::TOWN_SET_ATTITUDE;
			break;
	}
}

std::ostream& operator << (std::ostream& out, eSpecType& e) {
	return out << (int) e;
}

// TODO: This should probably understand symbolic names as well?
std::istream& operator >> (std::istream& in, eSpecType& e) {
	int i;
	in >> i;
	e = (eSpecType) i;
	if(getNodeCategory(e) == eSpecCat::INVALID)
		e = eSpecType::ERROR;
	return in;
}

// This is sort of a workaround for VS2013 not supporting C99 designated initializers in C++.
namespace node_types {
	using np_populator = std::function<void(node_properties_t)>;
	template<typename T> struct np_populator_builder {
		T node_properties_t::*const prop;
		np_populator_builder(T node_properties_t::* p) : prop(p) {}
		void setVal(node_properties_t props, T val) const {
			(props.*prop) = val;
		}
		template<typename T2> np_populator operator= (T2 val) const {
			return std::bind(&np_populator_builder<T>::setVal, this, std::placeholders::_1, val);
		}
	};
	namespace keys {
		template<typename T> using npb = const np_populator_builder<T>;
		using npt = node_properties_t;
		// Needs to be one of these for every member variable in node_properties_t.
		npb<bool> ex1a_ch(&npt::ex1a_choose), ex1b_ch(&npt::ex1b_choose), ex1c_ch(&npt::ex1c_choose);
		npb<bool> ex2a_ch(&npt::ex1a_choose), ex2b_ch(&npt::ex2b_choose), ex2c_ch(&npt::ex2c_choose);
		npb<short> sdf_lbl(&npt::sdf_label), msg_lbl(&npt::sdf_label);
		npb<short> pic_lbl(&npt::sdf_label), jmp_lbl(&npt::sdf_label);
	}
}

node_properties_t::node_properties_t(std::initializer_list<node_types::np_populator> vals) {
	for(node_types::np_populator val : vals) val(*this);
}

using namespace node_types::keys;

// This is the database of information on the special nodes, used by the scenario editor to decide how to set up the edit node dialog.
// Keys ending in _ch indicate whether a "Choose" or "Create/Edit" button should be present, and must be a boolean.
//    (Whether it's a Choose or Create/Edit button depends on the field.)
// Keys ending in _lbl indicate the set of labels applied to that set of fields, and are also used to determine the effect of the buttons.
// The extra fields curently have their label associations hard-coded rather than listed here.

const std::map<eSpecType, node_properties_t> allNodeProps = {
	{eSpecType::NONE, {}},
	{eSpecType::SET_SDF, {sdf_lbl = 1,msg_lbl = 1}},
	{eSpecType::INC_SDF, {sdf_lbl = 1,msg_lbl = 1}},
	{eSpecType::DISPLAY_MSG, {msg_lbl = 1}},
	{eSpecType::SECRET_PASSAGE, {msg_lbl = 1}},
	{eSpecType::DISPLAY_SM_MSG, {msg_lbl = 1}},
	{eSpecType::FLIP_SDF, {sdf_lbl = 1,msg_lbl = 1}},
	// TODO: XXX_BLOCK were here and had jmp_lbl = 1; what to do about that?
	{eSpecType::CANT_ENTER, {msg_lbl = 1}},
	{eSpecType::CHANGE_TIME, {msg_lbl = 1}},
	{eSpecType::SCEN_TIMER_START, {ex1b_ch = true}},
	{eSpecType::PLAY_SOUND, {}},
	{eSpecType::CHANGE_HORSE_OWNER, {}},
	{eSpecType::CHANGE_BOAT_OWNER, {}},
	{eSpecType::SET_TOWN_VISIBILITY, {msg_lbl = 1}},
	{eSpecType::MAJOR_EVENT_OCCURRED, {msg_lbl = 1}},
	{eSpecType::FORCED_GIVE, {ex1a_ch = true,ex2b_ch = true,msg_lbl = 1}},
	{eSpecType::BUY_ITEMS_OF_TYPE, {ex1b_ch = true,msg_lbl = 1}},
	{eSpecType::CALL_GLOBAL, {}},
	{eSpecType::SET_SDF_ROW, {sdf_lbl = 1}},
	{eSpecType::COPY_SDF, {sdf_lbl = 1}},
	// TODO: Sanctify was here, and had ex1b_ch = true; what to do about that?
	{eSpecType::REST, {msg_lbl = 1}},
	{eSpecType::WANDERING_WILL_FIGHT, {}},
	{eSpecType::END_SCENARIO, {}},
	{eSpecType::ONCE_GIVE_ITEM, {ex1a_ch = true,ex2b_ch = true,sdf_lbl = 1,msg_lbl = 1}},
	{eSpecType::ONCE_GIVE_SPEC_ITEM, {sdf_lbl = 1,msg_lbl = 1}},
	{eSpecType::ONCE_NULL, {sdf_lbl = 1}},
	{eSpecType::ONCE_SET_SDF, {sdf_lbl = 1}},
	{eSpecType::ONCE_DISPLAY_MSG, {sdf_lbl = 1,msg_lbl = 1}},
	{eSpecType::ONCE_DIALOG, {ex1a_ch = true,ex2a_ch = true,ex1b_ch = true,ex2b_ch = true,
		sdf_lbl = 1,msg_lbl = 4,pic_lbl = 1,jmp_lbl = 4}},
	{eSpecType::ONCE_DIALOG_TERRAIN, {ex1a_ch = true,ex2a_ch = true,ex1b_ch = true,ex2b_ch = true,
		sdf_lbl = 1,msg_lbl = 4,pic_lbl = 2,jmp_lbl = 4}},
	{eSpecType::ONCE_DIALOG_MONSTER, {ex1a_ch = true,ex2a_ch = true,ex1b_ch = true,ex2b_ch = true,
		sdf_lbl = 1,msg_lbl = 4,pic_lbl = 3,jmp_lbl = 4}},
	{eSpecType::ONCE_GIVE_ITEM_DIALOG, {ex1a_ch = true,ex2b_ch = true,sdf_lbl = 1,msg_lbl = 5,pic_lbl = 1}},
	{eSpecType::ONCE_GIVE_ITEM_TERRAIN, {ex1a_ch = true,ex2b_ch = true,sdf_lbl = 1,msg_lbl = 5,pic_lbl = 2}},
	{eSpecType::ONCE_GIVE_ITEM_MONSTER, {ex1a_ch = true,ex2b_ch = true,sdf_lbl = 1,msg_lbl = 5,pic_lbl = 3}},
	{eSpecType::ONCE_OUT_ENCOUNTER, {sdf_lbl = 1,msg_lbl = 1}},
	{eSpecType::ONCE_TOWN_ENCOUNTER, {sdf_lbl = 1,msg_lbl = 1}},
	{eSpecType::ONCE_TRAP, {sdf_lbl = 1,msg_lbl = 1,jmp_lbl = 2}},
	{eSpecType::SELECT_PC, {ex1b_ch = true,msg_lbl = 1}},
	{eSpecType::DAMAGE, {msg_lbl = 1}},
	{eSpecType::AFFECT_HP, {msg_lbl = 1}},
	{eSpecType::AFFECT_SP, {msg_lbl = 1}},
	{eSpecType::AFFECT_XP, {msg_lbl = 1}},
	{eSpecType::AFFECT_SKILL_PTS, {msg_lbl = 1}},
	{eSpecType::AFFECT_DEADNESS, {msg_lbl = 1}},
	{eSpecType::AFFECT_POISON, {msg_lbl = 1}},
	{eSpecType::AFFECT_SPEED, {msg_lbl = 1}},
	{eSpecType::AFFECT_INVULN, {msg_lbl = 1}},
	{eSpecType::AFFECT_MAGIC_RES, {msg_lbl = 1}},
	{eSpecType::AFFECT_WEBS, {msg_lbl = 1}},
	{eSpecType::AFFECT_DISEASE, {msg_lbl = 1}},
	{eSpecType::AFFECT_SANCTUARY, {msg_lbl = 1}},
	{eSpecType::AFFECT_CURSE_BLESS, {msg_lbl = 1}},
	{eSpecType::AFFECT_DUMBFOUND, {msg_lbl = 1}},
	{eSpecType::AFFECT_SLEEP, {msg_lbl = 1}},
	{eSpecType::AFFECT_PARALYSIS, {msg_lbl = 1}},
	{eSpecType::AFFECT_STAT, {msg_lbl = 1,pic_lbl = 4}},
	{eSpecType::AFFECT_MAGE_SPELL, {msg_lbl = 1}},
	{eSpecType::AFFECT_PRIEST_SPELL, {msg_lbl = 1}},
	{eSpecType::AFFECT_GOLD, {msg_lbl = 1}},
	{eSpecType::AFFECT_FOOD, {msg_lbl = 1}},
	{eSpecType::AFFECT_ALCHEMY, {msg_lbl = 1}},
	{eSpecType::AFFECT_STEALTH, {msg_lbl = 1}},
	{eSpecType::AFFECT_FIREWALK, {msg_lbl = 1}},
	{eSpecType::AFFECT_FLIGHT, {msg_lbl = 1}},
	{eSpecType::IF_SDF, {ex1b_ch = true,ex2b_ch = true,sdf_lbl = 1,jmp_lbl = 3}},
	{eSpecType::IF_TOWN_NUM, {ex1b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_RANDOM, {ex1b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_HAVE_SPECIAL_ITEM, {ex1b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_SDF_COMPARE, {ex2b_ch = true,sdf_lbl = 1,jmp_lbl = 3}},
	{eSpecType::IF_TOWN_TER_TYPE, {ex2a_ch = true,ex2b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_OUT_TER_TYPE, {ex2a_ch = true,ex2b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_HAS_GOLD, {ex1b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_HAS_FOOD, {ex1b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_ITEM_CLASS_ON_SPACE, {ex2b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_HAVE_ITEM_CLASS, {ex1b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_EQUIP_ITEM_CLASS, {ex1b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_HAS_GOLD_AND_TAKE, {ex1b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_HAS_FOOD_AND_TAKE, {ex1b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_ITEM_CLASS_ON_SPACE_AND_TAKE, {ex2b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_HAVE_ITEM_CLASS_AND_TAKE, {ex1b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_EQUIP_ITEM_CLASS_AND_TAKE, {ex1b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_DAY_REACHED, {ex1b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_OBJECTS, {ex1b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_EVENT_OCCURRED, {ex1b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_SPECIES, {ex1b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_TRAIT, {ex1b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_STATISTIC, {ex1b_ch = true,jmp_lbl = 3}},
	{eSpecType::IF_TEXT_RESPONSE, {ex1b_ch = true,ex2b_ch = true,pic_lbl = 5,jmp_lbl = 3}},
	{eSpecType::IF_SDF_EQ, {ex1b_ch = true,sdf_lbl = 1,jmp_lbl = 3}},
	{eSpecType::IF_CONTEXT, {}},
	{eSpecType::MAKE_TOWN_HOSTILE, {msg_lbl = 1}},
	{eSpecType::TOWN_CHANGE_TER, {ex2a_ch = true,msg_lbl = 1}},
	{eSpecType::TOWN_SWAP_TER, {ex2a_ch = true,msg_lbl = 1}},
	{eSpecType::TOWN_TRANS_TER, {msg_lbl = 1}},
	{eSpecType::TOWN_MOVE_PARTY, {msg_lbl = 1}},
	{eSpecType::TOWN_HIT_SPACE, {msg_lbl = 1}},
	{eSpecType::TOWN_EXPLODE_SPACE, {msg_lbl = 1,pic_lbl = 6}},
	{eSpecType::TOWN_LOCK_SPACE, {msg_lbl = 1}},
	{eSpecType::TOWN_UNLOCK_SPACE, {msg_lbl = 1}},
	{eSpecType::TOWN_SFX_BURST, {msg_lbl = 1}},
	{eSpecType::TOWN_CREATE_WANDERING, {msg_lbl = 1}},
	{eSpecType::TOWN_PLACE_MONST, {ex2a_ch = true,msg_lbl = 1}},
	{eSpecType::TOWN_DESTROY_MONST, {ex1a_ch = true,msg_lbl = 1}},
	{eSpecType::TOWN_NUKE_MONSTS, {msg_lbl = 1}},
	{eSpecType::TOWN_GENERIC_LEVER, {ex1b_ch = true}},
	{eSpecType::TOWN_GENERIC_PORTAL, {}},
	{eSpecType::TOWN_GENERIC_BUTTON, {ex1b_ch = true}},
	{eSpecType::TOWN_GENERIC_STAIR, {}},
	{eSpecType::TOWN_LEVER, {ex1b_ch = true,msg_lbl = 2,pic_lbl = 2}},
	{eSpecType::TOWN_PORTAL, {msg_lbl = 2,pic_lbl = 1}},
	{eSpecType::TOWN_STAIR, {msg_lbl = 2}},
	{eSpecType::TOWN_RELOCATE, {msg_lbl = 1}},
	{eSpecType::TOWN_PLACE_ITEM, {ex2a_ch = true,msg_lbl = 1}},
	{eSpecType::TOWN_SPLIT_PARTY, {msg_lbl = 1}},
	{eSpecType::TOWN_REUNITE_PARTY, {msg_lbl = 1}},
	{eSpecType::TOWN_TIMER_START, {ex1b_ch = true,msg_lbl = 1}},
	{eSpecType::RECT_PLACE_FIRE, {sdf_lbl = 2,msg_lbl = 1}},
	{eSpecType::RECT_PLACE_FORCE, {sdf_lbl = 2,msg_lbl = 1}},
	{eSpecType::RECT_PLACE_ICE, {sdf_lbl = 2,msg_lbl = 1}},
	{eSpecType::RECT_PLACE_BLADE, {sdf_lbl = 2,msg_lbl = 1}},
	{eSpecType::RECT_PLACE_SCLOUD, {sdf_lbl = 2,msg_lbl = 1}},
	{eSpecType::RECT_PLACE_SLEEP, {sdf_lbl = 2,msg_lbl = 1}},
	{eSpecType::RECT_PLACE_QUICKFIRE, {sdf_lbl = 2,msg_lbl = 1}},
	{eSpecType::RECT_PLACE_FIRE_BARR, {sdf_lbl = 2,msg_lbl = 1}},
	{eSpecType::RECT_PLACE_FORCE_BARR, {sdf_lbl = 2,msg_lbl = 1}},
	{eSpecType::RECT_CLEANSE, {sdf_lbl = 2,msg_lbl = 1}},
	{eSpecType::RECT_PLACE_SFX, {sdf_lbl = 7,msg_lbl = 1}},
	{eSpecType::RECT_PLACE_OBJECT, {sdf_lbl = 8,msg_lbl = 1}},
	{eSpecType::RECT_MOVE_ITEMS, {sdf_lbl = 4,msg_lbl = 1}},
	{eSpecType::RECT_DESTROY_ITEMS, {msg_lbl = 1}},
	{eSpecType::RECT_CHANGE_TER, {sdf_lbl = 5,msg_lbl = 1}},
	{eSpecType::RECT_SWAP_TER, {sdf_lbl = 6,msg_lbl = 1}},
	// TODO: Is it correct for some RECT specials to not have sdf_lbl = something?
	{eSpecType::RECT_TRANS_TER, {msg_lbl = 1}},
	{eSpecType::RECT_LOCK, {msg_lbl = 1}},
	{eSpecType::RECT_UNLOCK, {msg_lbl = 1}},
	{eSpecType::OUT_MAKE_WANDER, {}},
	{eSpecType::OUT_CHANGE_TER, {ex2a_ch = true,msg_lbl = 1}},
	{eSpecType::OUT_PLACE_ENCOUNTER, {msg_lbl = 1}},
	{eSpecType::OUT_MOVE_PARTY, {msg_lbl = 1}},
	{eSpecType::OUT_STORE, {ex1a_ch = true,msg_lbl = 3}},
};

const node_properties_t& operator* (eSpecType t) {
	node_properties_t p = allNodeProps.at(t);
	return p;
}
