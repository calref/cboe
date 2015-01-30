/*
 *  special.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include "special.h"

#include "special.h"
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <boost/lexical_cast.hpp>

#include "dlogutil.hpp"
#include "oldstructs.h"

cSpecial::cSpecial(){
	type = eSpecType::NONE;
	sd1 = -1;
	sd2 = -1;
	pic = -1;
	pictype = 4;
	m1 = -1;
	m2 = -1;
	m3 = -1;
	ex1a = -1;
	ex1b = -1;
	ex2a = -1;
	ex2b = -1;
	jumpto = -1;
}

void cSpecial::writeTo(std::ostream& file, int n) const {
	file << '@' << (*type).opcode() << " = " << n << '\n';
	file << "\tsdf " << sd1 << ", " << sd2 << '\n';
	file << "\tmsg " << m1 << ", " << m2 << ", " << m3 << '\n';
	file << "\tpic " << pic << ", " << pictype << '\n';
	file << "\tex1 " << ex1a << ", " << ex1b << ", " << ex1c << '\n';
	file << "\tex2 " << ex2a << ", " << ex2b << ", " << ex2c << '\n';
	file << "\tgoto " << jumpto << '\n';
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
			pictype = PIC_DLOG;
			m3 = m2;
			m2 = -1;
			break;
		case 57: case 60: // Large dialogs with monster graphics
			pic -= 400;
			pictype = PIC_MONST;
			m3 = m2;
			m2 = -1;
			if(old.type == 57) type = eSpecType::ONCE_DIALOG;
			else if(old.type == 60) type = eSpecType::ONCE_GIVE_ITEM_DIALOG;
			break;
		case 56: case 59: case 188: // Large dialogs with terrain graphics
			pictype = PIC_TER;
			m3 = m2;
			m2 = -1;
			if(old.type == 56) type = eSpecType::ONCE_DIALOG;
			else if(old.type == 59) type = eSpecType::ONCE_GIVE_ITEM_DIALOG;
			break;
		case 190: // Large stairway dialog
			pic = 19;
			pictype = PIC_DLOG;
			m3 = m2;
			m2 = -1;
			break;
			// TODO: Originally the block nodes supported messages; the new version doesn't.
			// (Will probably need to make special nodes a dynamic vector before fixing this.)
		case 7: case 8: case 9: case 10: // out, town, combat, look block
			type = eSpecType::IF_CONTEXT;
			ex1b = ex1a;
			if(old.type == 7) ex1a = (int) eSpecCtx::OUT_MOVE;
			if(old.type == 8) ex1a = (int) eSpecCtx::TOWN_MOVE;
			if(old.type == 9) ex1a = (int) eSpecCtx::COMBAT_MOVE;
			if(old.type == 10) type = eSpecType::IF_LOOKING;
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
			ex1b = 0; // Meaning give spell, not take
			break;
		case 148: case 149: // if barrels or crates
			type = eSpecType::IF_FIELDS;
			m1 = old.type == 148 ? OBJECT_BARREL : OBJECT_CRATE;
			m2 = ex1b;
			ex1a = ex1b = 0;
			ex2a = ex2b = 64;
			sd1 = 1;
			sd2 = std::numeric_limits<short>::max();
			break;
		case 151: case 152: // if has cave lore or woodsman
			type = eSpecType::IF_TRAIT;
			ex1a = old.type - 147;
			break;
		case 63: // Trap used to force a specific picture
			type = eSpecType::ONCE_TRAP;
			pic = 27;
			pictype = PIC_DLOG;
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
		case 154: // text response
			ex1a -= 160;
			break;
		case 229: // Outdoor store - fix spell IDs
			type = eSpecType::ENTER_SHOP;
			if(ex1b == 1 || ex1b == 2)
				ex1a += 30;
			break;
		case 4: // Secret passage
			type = eSpecType::CANT_ENTER;
			ex1a = 0;
			ex2a = 1;
			break;
		case 171: case 226: // Change terrain (town/outdoor)
			type = eSpecType::CHANGE_TER;
			break;
		case 172: // Swap terrain
			type = eSpecType::SWAP_TER;
			break;
		case 173: // Transform terrain
			type = eSpecType::TRANS_TER;
			break;
		case 135: case 136: // If terrain
			type = eSpecType::IF_TER_TYPE;
			break;
		case 137: case 142: // If has gold (and maybe take)
			type = eSpecType::IF_HAS_GOLD;
			ex2a = (old.type - 137) / 5;
			break;
		case 138: case 143: // If has food (and maybe take)
			type = eSpecType::IF_HAS_FOOD;
			ex2a = (old.type - 138) / 5;
			break;
		case 139: case 144: // If item on space (and maybe take)
			type = eSpecType::IF_ITEM_CLASS_ON_SPACE;
			ex2c = (old.type - 139) / 5;
			break;
		case 140: case 145: // If have item class (and maybe take)
			type = eSpecType::IF_HAVE_ITEM_CLASS;
			ex2a = (old.type - 140) / 5;
			break;
		case 141: case 146: // If equip item class (and maybe take)
			type = eSpecType::IF_EQUIP_ITEM_CLASS;
			ex2a = (old.type - 131) / 5;
			break;
		case 182: // Destroy all monsters of particular type
			type = eSpecType::TOWN_NUKE_MONSTS;
			break;
		case 183: // Destroy all monsters, or all friendly / all hostile
			type = eSpecType::TOWN_NUKE_MONSTS;
			ex1a = -ex1a;
			break;
		case 193: // Split party
			if(ex2a > 0) ex2a = 10;
			break;
		case 194: // Reunite party
			if(ex1a > 0) ex1a = 10;
			ex2a = 0;
			break;
			// Party statuses (three nodes collapsed into one)
		case 104:
			type = eSpecType::AFFECT_STATUS;
			ex1b = 0;
			ex2a = int(ePartyStatus::STEALTH);
			break;
		case 105:
			type = eSpecType::AFFECT_STATUS;
			ex1b = 0;
			ex2a = int(ePartyStatus::FIREWALK);
			break;
		case 106:
			type = eSpecType::AFFECT_STATUS;
			ex1b = 0;
			ex2a = int(ePartyStatus::FLIGHT);
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
			// Affect status effect (eleven individual node types were collapsed into one)
		case 87:
			type = eSpecType::AFFECT_STATUS;
			ex1c = int(eStatus::POISON);
			break;
		case 88:
			type = eSpecType::AFFECT_STATUS;
			ex1c = int(eStatus::HASTE_SLOW);
			break;
		case 89:
			type = eSpecType::AFFECT_STATUS;
			ex1c = int(eStatus::INVULNERABLE);
			break;
		case 90:
			type = eSpecType::AFFECT_STATUS;
			ex1c = int(eStatus::MAGIC_RESISTANCE);
			break;
		case 91:
			type = eSpecType::AFFECT_STATUS;
			ex1c = int(eStatus::WEBS);
			break;
		case 92:
			type = eSpecType::AFFECT_STATUS;
			ex1c = int(eStatus::DISEASE);
			break;
		case 93:
			type = eSpecType::AFFECT_STATUS;
			ex1c = int(eStatus::INVISIBLE);
			break;
		case 94:
			type = eSpecType::AFFECT_STATUS;
			ex1c = int(eStatus::BLESS_CURSE);
			break;
		case 95:
			type = eSpecType::AFFECT_STATUS;
			ex1c = int(eStatus::DUMB);
			break;
		case 96:
			type = eSpecType::AFFECT_STATUS;
			ex1c = int(eStatus::ASLEEP);
			break;
		case 97:
			type = eSpecType::AFFECT_STATUS;
			ex1c = int(eStatus::PARALYZED);
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

std::ostream& operator << (std::ostream& out, eSpecType e) {
	return out << (int) e;
}

// TODO: This should probably understand symbolic names as well?
std::istream& operator >> (std::istream& in, eSpecType& e) {
	int i;
	in >> i;
	e = (eSpecType) i;
	if(getNodeCategory(e) == eSpecCat::INVALID)
		e = eSpecType::INVALID;
	return in;
}

// Key:
// space - no button
// m - Create/Edit button to edit message pair (covers msg1 and msg2 together)
// M - Create/Edit button to edit single message
// $ - As above, but always a scenario message
// d - Create/Edit button to edit dialog message sequence (covers msg1 and msg2 together)
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
// { - Choose button to select a spell pattern
// } - As above, but allows you to select which version of the rotateable field
// e - Choose button to select a status effect
// E - Choose button to select a party status effect
// w - Choose button to select main party status effect
// 0..9 - Choose button to select a specific type of picture
// (terrain, monster, dialog, talk, item, pc, field, boom, missile, status)
static const char*const button_dict[7][11] = {
	{ // general nodes
		" mmmMmmmmmMmmm mmmmmm   Mmm  $ mmmmmm        ", // msg1
		"                                             ", // msg2
		"                                             ", // msg3
		"          p                                 3", // pic
		"          ?                                  ", // pictype
		"    #         x  T i                  M cit  ", // ex1a
		"    &        S     ss                       c", // ex1b
		"                                             ", // ex1c
		"                                  tt         ", // ex2a
		"    %                              t         ", // ex2b
		"                                             ", // ex2c
	}, { // one-shot nodes
		"mm  md  d  mmm", // msg1
		"              ", // msg2
		"        III   ", // msg3
		"     p  p    p", // pic
		"     ?  ?    ?", // pictype
		"iI   b  i    X", // ex1a
		"     s        ", // ex1b
		"              ", // ex1c
		"     b        ", // ex2a
		"s    s  s    S", // ex2b
		"              ", // ex2c
	}, { // affect pc nodes
		"mmmmmmmmmmm       mmmmmmmm", // msg1
		"                          ", // msg2
		"          M              M", // msg3
		"                         5", // pic
		"                         s", // pictype
		"      w q          AP  a  ", // ex1a
		"                          ", // ex1b
		"       e                 Q", // ex1c
		"                  K     E ", // ex2a
		" D                        ", // ex2b
		" x                        ", // ex2c
	}, { // if-then nodes
		"                  f     $  $  ", // msg1
		"                  s           ", // msg2
		"                              ", // msg3
		"                              ", // pic
		"                              ", // pictype
		" T I  w     APae     Qq $ *   ", // ex1a
		"ssss  sss ssssss s s sssss =  ", // ex1b
		"                s         ssss", // ex1c
		"     t                 K$     ", // ex2a
		"s   ss   s     +    s==+s  =  ", // ex2b
		"               =           s  ", // ex2c
	}, { // town nodes
		"mmmmmmmmmmmmmmm   dddmmmmmmmmmmmm", // msg1
		"                                 ", // msg2
		"                                 ", // msg3
		" 8                ppp            ", // pic
		"                  ???            ", // pictype
		"            c             L   {  ", // ex1a
		"              s s s      s @     ", // ex1b
		"                               }}", // ex1c
		"@  7     !  c    T  T i        FD", // ex2a
		"     DD          /               ", // ex2b
		" x x             :  :            ", // ex2c
	}, { // rectangle nodes
		"mmmmmmmmm", // msg1
		"         ", // msg2
		"         ", // msg3
		"         ", // pic
		"         ", // pictype
		"    tt   ", // sdf1
		"         ", // unused
		"         ", // ex1c
		"F    t   ", // sdf2
		"         ", // unused
		"         ", // ex2c
	}, { // outdoors nodes
		"  mm", // msg1
		"    ", // msg2
		"    ", // msg3
		"    ", // pic
		"    ", // pictype
		"    ", // ex1a
		"    ", // ex1b
		"    ", // ex1c
		"    ", // ex2a
		"    ", // ex2b
		"    ", // ex2c
	}
};

static int offsets[] = {
	int(eSpecType::NONE),
	int(eSpecType::ONCE_GIVE_ITEM),
	int(eSpecType::SELECT_TARGET),
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
	if(self == eSpecType::NONE) return "nop";
	return get_str("specials-opcodes", int(self));
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
		case eSpecCat::INVALID:
			return "error";
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
