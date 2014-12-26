/*
 *  special.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include "special.h"
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <boost/lexical_cast.hpp>

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
			type = eSpecType::IF_FIELDS;
			ex1a = old.type == 148 ? OBJECT_BARREL : OBJECT_CRATE;
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
			// Place fields (twelve individual node types were collapsed into one)
		case 200:
			type = eSpecType::RECT_PLACE_FIELD;
			sd2 = WALL_FIRE;
			break;
		case 201:
			type = eSpecType::RECT_PLACE_FIELD;
			sd2 = WALL_FORCE;
			break;
		case 202:
			type = eSpecType::RECT_PLACE_FIELD;
			sd2 = WALL_ICE;
			break;
		case 203:
			type = eSpecType::RECT_PLACE_FIELD;
			sd2 = WALL_BLADES;
			break;
		case 204:
			type = eSpecType::RECT_PLACE_FIELD;
			sd2 = CLOUD_STINK;
			break;
		case 205:
			type = eSpecType::RECT_PLACE_FIELD;
			sd2 = CLOUD_SLEEP;
			break;
		case 206:
			type = eSpecType::RECT_PLACE_FIELD;
			sd2 = FIELD_QUICKFIRE;
			break;
		case 207:
			type = eSpecType::RECT_PLACE_FIELD;
			sd2 = BARRIER_FIRE;
			break;
		case 208:
			type = eSpecType::RECT_PLACE_FIELD;
			sd2 = BARRIER_FORCE;
			break;
		case 209:
			type = eSpecType::RECT_PLACE_FIELD;
			sd2 = FIELD_DISPEL;
			break;
		case 210:
			type = eSpecType::RECT_PLACE_FIELD;
			sd2 += SFX_SMALL_BLOOD;
			break;
		case 211:
			type = eSpecType::RECT_PLACE_FIELD;
			switch(old.sd2) {
				case 0: sd2 = FIELD_WEB; break;
				case 1: sd2 = OBJECT_BARREL; break;
				case 2: sd2 = OBJECT_CRATE; break;
			}
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

// Key:
// space - no button
// m - Create/Edit button to edit message pair (covers msg1 and msg2 together)
// M - Create/Edit button to edit single message
// $ - As above, but always a scenario message
// d - Create/Edit button to edit dialog message sequence
// b - Choose button to select a button label
// p - Choose button to select a picture (uses pictype for type)
// ? - Choose button to select a picture type
// s - Create/Edit button to edit special node
// S - As above, but always a scenario node
// x - Choose button to select a sound
// X - Choose button to select a trap type
// T - Choose button to select a town
// i - Choose button to select an item
// I - Choose button to select a special item
// # - Choose button to select store item (uses ex1b for type)
// t - Choose button to select a terrain type
// c - Choose button to select a monster type
// a - Choose button to select an alchemy recipe
// A - Choose button to select a mage spell
// P - Choose button to select a priest spell
// k - Choose button to select a skill
// K - As above, but add the special pseudo-skills for the if-statistic node
// f - Choose button to select a field type
// F - As above, but also include Dispel and Smash
// q - Choose button to select a trait
// Q - Choose button to select a species
// = - Choose button to select a comparison method (<=, <, =, >, >=)
// + - Choose button to select stat cumulation mode
// * - Choose button to select a special node context
// @ - Choose button to select a monster attitude
// D - Choose button to select a damage type
// ! - Choose button to select an explosion animation type
// / - Choose button to select generic stairway text
// : - Choose stairway trigger conditions
// L - Choose button to select a town lighting type
// & - Choose button to select shop type
// % - Choose button to select shop cost adjustment
static const char*const button_dict[7][11] = {
	{ // general nodes
		" mmmmmmmmm mmm mmmmmm   Mmm  $ mmm", // msg1
		"                                  ", // msg2
		"                                  ", // msg3
		"                                  ", // pic
		"                                  ", // pictype
		"              x  T i              ", // ex1a
		"             S     ss             ", // ex1b
		"                                  ", // ex1c
		"                                  ", // ex2a
		"                                  ", // ex2b
		"                                  ", // ex2c
	}, { // one-shot nodes
		"mm  mddddddmmm", // msg1
		"        III   ", // msg2
		"              ", // msg3
		"     pppppp   ", // pic
		"              ", // pictype
		"iI   bbbiii  X", // ex1a
		"     sss      ", // ex1b
		"              ", // ex1c
		"     bbb      ", // ex2a
		"s    ssssss   ", // ex2b
		"              ", // ex2c
	}, { // affect pc nodes
		"mmmmmmmmmmmmmmmmmmmmmmmmmmm", // msg1
		"                           ", // msg2
		"                           ", // msg3
		"                           ", // pic
		"                           ", // pictype
		"                   AP  a   ", // ex1a
		"                           ", // ex1b
		"                           ", // ex1c
		"                  K        ", // ex2a
		" D                         ", // ex2b
		"                           ", // ex2c
	}, { // if-then nodes
		"                        $  $", // msg1
		"                            ", // msg2
		"                            ", // msg3
		"                            ", // pic
		"                            ", // pictype
		"                  f  Qq $ * ", // ex1a
		"ssss   ss ssss sssss sssss =", // ex1b
		"                          ss", // ex1c
		"                       K$   ", // ex2a
		"s   sss  s    s     s==+s  =", // ex2b
		"                           s", // ex2c
	}, { // town nodes
		"mmmmmmmmmmmmmmm   dddmmmmmmm", // msg1
		"                            ", // msg2
		"                            ", // msg3
		"                  pp        ", // pic
		"                            ", // pictype
		"            c             L ", // ex1a
		"              s s s      s @", // ex1b
		"                            ", // ex1c
		"@tt      ! c     T  T i     ", // ex2a
		"  t  DD          /          ", // ex2b
		"                 :  :       ", // ex2c
	}, { // rectangle nodes
		"m           mmmmmmm", // msg1
		"                   ", // msg2
		"                   ", // msg3
		"                   ", // pic
		"                   ", // pictype
		"              tt   ", // sdf1
		"                   ", // unused
		"                   ", // ex1c
		"F              t   ", // sdf2
		"                   ", // unused
		"                   ", // ex2c
	}, { // outdoors nodes
		" mmmM", // msg1
		"     ", // msg2
		"     ", // msg3
		"     ", // pic
		"     ", // pictype
		"    #", // ex1a
		"    &", // ex1b
		"     ", // ex1c
		" t   ", // ex2a
		"    %", // ex2b
		"     ", // ex2c
	}
};

static int offsets[] = {
	int(eSpecType::NONE),
	int(eSpecType::ONCE_GIVE_ITEM),
	int(eSpecType::SELECT_PC),
	int(eSpecType::IF_SDF),
	int(eSpecType::MAKE_TOWN_HOSTILE),
	int(eSpecType::RECT_PLACE_FIELD),
	int(eSpecType::OUT_MAKE_WANDER),
};

static std::map<eSpecType, node_properties_t> loadProps() {
	std::map<eSpecType, node_properties_t> allNodeProps;
	// There's really no need to check all the way to the max of the underlying type.
	// It's unlikely we'd go above 255, so unsigned char would be fine, but just in case,
	// let's use unsigned short.
	// Could change the actual enum's underlying type instead though?
	using underlying = unsigned short;//std::underlying_type<eSpecType>::type;
	for(underlying i = 0; i < std::numeric_limits<underlying>::max(); i++) {
		eSpecType check = (eSpecType) i;
		eSpecCat category = getNodeCategory(check);
		if(category == eSpecCat::INVALID) continue;
		node_properties_t props;
		props.self = check;
		int j = int(category), k = i - offsets[j];
		props.m1_btn = button_dict[j][0][k];
		props.m2_btn = button_dict[j][1][k];
		props.m3_btn = button_dict[j][2][k];
		props.p_btn = button_dict[j][3][k];
		props.pt_btn = button_dict[j][4][k];
		if(category != eSpecCat::RECT) {
			props.x1a_btn = button_dict[j][5][k];
			props.x1b_btn = button_dict[j][6][k];
		}
		props.x1c_btn = button_dict[j][7][k];
		if(category != eSpecCat::RECT) {
			props.x2a_btn = button_dict[j][8][k];
			props.x2b_btn = button_dict[j][9][k];
		}
		props.x2c_btn = button_dict[j][10][k];
		if(category == eSpecCat::RECT) {
			props.x1a_btn = props.x2a_btn = ' ';
			props.x1b_btn = props.x2b_btn = ' ';
		}
		allNodeProps[check] = props;
	}
	return allNodeProps;
}

const node_properties_t& operator* (eSpecType t) {
	static std::map<eSpecType, node_properties_t> allNodeProps = loadProps();
	return allNodeProps.at(t);
}

std::string node_properties_t::opcode() const {
	return get_str("specials-opcodes", int(self) + 1);
}

static std::string get_node_string(std::string base, eSpecType type, int which) {
	eSpecCat cat = getNodeCategory(type);
	int i = int(cat), j = int(type);
	int strnum = (j - offsets[i]) * 16 + which + 1;
	switch(cat) {
		case eSpecCat::GENERAL:
			return get_str(base + "-general", strnum);
		case eSpecCat::ONCE:
			return get_str(base + "-once", strnum);
		case eSpecCat::AFFECT:
			return get_str(base + "-affect", strnum);
		case eSpecCat::IF_THEN:
			return get_str(base + "-ifthen", strnum);
		case eSpecCat::TOWN:
			return get_str(base + "-town", strnum);
		case eSpecCat::RECT:
			return get_str(base + "-rect", strnum);
		case eSpecCat::OUTDOOR:
			return get_str(base + "-outdoor", strnum);
	}
	return "";
}

std::string node_properties_t::name() const {
	return get_node_string("specials-text", self, 0);
}

std::string node_properties_t::sdf1_lbl() const {
	return get_node_string("specials-text", self, 1);
}

std::string node_properties_t::sdf2_lbl() const {
	return get_node_string("specials-text", self, 2);
}

std::string node_properties_t::msg1_lbl() const {
	return get_node_string("specials-text", self, 3);
}

std::string node_properties_t::msg2_lbl() const {
	return get_node_string("specials-text", self, 4);
}

std::string node_properties_t::msg3_lbl() const {
	return get_node_string("specials-text", self, 5);
}

std::string node_properties_t::pic_lbl() const {
	return get_node_string("specials-text", self, 6);
}

std::string node_properties_t::pt_lbl() const {
	return get_node_string("specials-text", self, 7);
}

std::string node_properties_t::ex1a_lbl() const {
	return get_node_string("specials-text", self, 8);
}

std::string node_properties_t::ex1b_lbl() const {
	return get_node_string("specials-text", self, 9);
}

std::string node_properties_t::ex1c_lbl() const {
	return get_node_string("specials-text", self, 10);
}

std::string node_properties_t::ex2a_lbl() const {
	return get_node_string("specials-text", self, 11);
}

std::string node_properties_t::ex2b_lbl() const {
	return get_node_string("specials-text", self, 12);
}

std::string node_properties_t::ex2c_lbl() const {
	return get_node_string("specials-text", self, 13);
}

std::string node_properties_t::jmp_lbl() const {
	return get_node_string("specials-text", self, 14);
}
