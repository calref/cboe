/*
 *  special.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include "special.hpp"
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <boost/lexical_cast.hpp>

#include "dialogxml/dialogs/strdlog.hpp"
#include "oldstructs.hpp"
#include "utility.hpp"

#include "spell.hpp"
#include "skills_traits.hpp"
#include "damage.hpp"
#include "fields.hpp"

bool cTimer::is_valid() const {
	if(time < 0) return false;
	if(node < 0) return false;
	return true;
}

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
	ex1c = -1;
	ex2a = -1;
	ex2b = -1;
	ex2c = -1;
	jumpto = -1;
}

void cSpecial::writeTo(std::ostream& file, int n) const {
	// TODO: Output only the needed values somehow
	file << '@' << (*type).opcode() << " = " << n << '\n';
	file << "\tsdf " << sd1 << ", " << sd2 << '\n';
	file << "\tmsg " << m1 << ", " << m2 << ", " << m3 << '\n';
	file << "\tpic " << pic << ", " << pictype << '\n';
	file << "\tex1 " << ex1a << ", " << ex1b << ", " << ex1c << '\n';
	file << "\tex2 " << ex2a << ", " << ex2b << ", " << ex2c << '\n';
	file << "\tgoto " << jumpto << '\n';
}

void cSpecial::import_legacy(legacy::special_node_type& old){
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
		case 0: type = eSpecType::NONE; break;
		case 1: type = eSpecType::SET_SDF; break;
		case 2: type = eSpecType::INC_SDF; break;
		case 3: type = eSpecType::DISPLAY_MSG; break;
			// node 4 was "secret passage", handled later
		case 5: type = eSpecType::DISPLAY_SM_MSG; break;
		case 6: type = eSpecType::FLIP_SDF; break;
			// 7, 8, 9, 10 were out/town/combat/look block
			// 11 was "can't enter", handled with secret passage
		case 12: type = eSpecType::CHANGE_TIME; break;
		case 13: type = eSpecType::SCEN_TIMER_START; break;
		case 14: type = eSpecType::PLAY_SOUND; break;
		case 15: type = eSpecType::CHANGE_HORSE_OWNER; break;
		case 16: type = eSpecType::CHANGE_BOAT_OWNER; break;
		case 17: type = eSpecType::SET_TOWN_VISIBILITY; break;
		case 18: type = eSpecType::MAJOR_EVENT_OCCURRED; break;
		case 19: type = eSpecType::FORCED_GIVE; break;
		case 20: type = eSpecType::BUY_ITEMS_OF_TYPE; break;
		case 21: type = eSpecType::CALL_GLOBAL; break;
		case 22: type = eSpecType::SET_SDF_ROW; break;
		case 23: type = eSpecType::COPY_SDF; break;
			// 24 was ritual of sanctification
		case 25: type = eSpecType::REST; break;
			// 26 was originally wandering will fight, handled with can't enter
		case 27: type = eSpecType::END_SCENARIO; break;
			// 28-49 were undefined
		case 50: type = eSpecType::ONCE_GIVE_ITEM; break;
		case 51: type = eSpecType::ONCE_GIVE_SPEC_ITEM; break;
		case 52: type = eSpecType::ONCE_NULL; break;
		case 53: type = eSpecType::ONCE_SET_SDF; break;
		case 54: type = eSpecType::ONCE_DISPLAY_MSG; break;
			// 55,56,57 were standard dialogs
			// 58,59,60 were give item dialogs
		case 61: type = eSpecType::ONCE_OUT_ENCOUNTER; break;
		case 62: type = eSpecType::ONCE_TOWN_ENCOUNTER; break;
			// 63 was trap
			// 64-79 were undefined
		case 80: type = eSpecType::SELECT_TARGET; break;
		case 81: type = eSpecType::DAMAGE; break;
		case 82: type = eSpecType::AFFECT_HP; break;
		case 83: type = eSpecType::AFFECT_SP; ex1c = 0; break;
		case 84: type = eSpecType::AFFECT_XP; break;
		case 85: type = eSpecType::AFFECT_SKILL_PTS; break;
		case 86: type = eSpecType::AFFECT_DEADNESS; break;
			// 87-97 were various "affect status" nodes
		case 98: type = eSpecType::AFFECT_STAT; break;
			// 99 and 100 were "affect mage/priest spell"
		case 101: type = eSpecType::AFFECT_GOLD; break;
		case 102: type = eSpecType::AFFECT_FOOD; break;
		case 103: type = eSpecType::AFFECT_ALCHEMY; break;
			// 104-106 were various affect stealth/firewalk/flight
			// 107-129 were undefined
		case 130: type = eSpecType::IF_SDF; break;
		case 131: type = eSpecType::IF_TOWN_NUM; break;
		case 132: type = eSpecType::IF_RANDOM; break;
		case 133: type = eSpecType::IF_HAVE_SPECIAL_ITEM; break;
		case 134: type = eSpecType::IF_SDF_COMPARE; break;
			// 135, 136 were if town/out terrain
			// 137-146 were various if-have and if-have+take
		case 147: type = eSpecType::IF_DAY_REACHED; break;
			// 148 and 149 were if barrels/crates
		case 150: type = eSpecType::IF_EVENT_OCCURRED; break;
			// 151 and 152 were if cave lore/woodsman
			// 153 was if mage lore
			// 154 was if text response
		case 155: type = eSpecType::IF_SDF_EQ; break;
			// 156-169 were undefined
		case 170:
			type = eSpecType::MAKE_TOWN_HOSTILE;
			ex1a = 0;
			ex1b = -1;
			break;
			// 171-173 were change/swap/transform terrain
		case 174: type = eSpecType::TOWN_MOVE_PARTY; break;
		case 175: type = eSpecType::TOWN_HIT_SPACE; break;
		case 176: type = eSpecType::TOWN_EXPLODE_SPACE; break;
		case 177: type = eSpecType::TOWN_LOCK_SPACE; break;
		case 178: type = eSpecType::TOWN_UNLOCK_SPACE; break;
		case 179: type = eSpecType::TOWN_SFX_BURST; break;
		case 180: type = eSpecType::TOWN_CREATE_WANDERING; break;
		case 181: type = eSpecType::TOWN_PLACE_MONST; break;
			// 182 and 183 were destroy monster
		case 184: type = eSpecType::TOWN_GENERIC_LEVER; break;
		case 185: type = eSpecType::TOWN_GENERIC_PORTAL; break;
		case 186: type = eSpecType::TOWN_GENERIC_BUTTON; break;
		case 187: type = eSpecType::TOWN_GENERIC_STAIR; break;
			// 188 was town lever
			// 189 was town portal
			// 190 was town stair
		case 191: type = eSpecType::TOWN_RELOCATE; break;
		case 192: type = eSpecType::TOWN_PLACE_ITEM; break;
			// 193, 194 were split/reunite party
		case 195: type = eSpecType::TOWN_TIMER_START; break;
			// 196-199 were undefined
			// 200-211 were various rect place fields
		case 212: type = eSpecType::RECT_MOVE_ITEMS; pictype = 1; break;
		case 213: type = eSpecType::RECT_DESTROY_ITEMS; break;
		case 214: type = eSpecType::RECT_CHANGE_TER; break;
		case 215: type = eSpecType::RECT_SWAP_TER; break;
		case 216: type = eSpecType::RECT_TRANS_TER; break;
		case 217: type = eSpecType::RECT_LOCK; break;
		case 218: type = eSpecType::RECT_UNLOCK; break;
			// 219-224 were undefined
		case 225: type = eSpecType::OUT_MAKE_WANDER; break;
			// 226 was change terrain
		case 227: type = eSpecType::OUT_PLACE_ENCOUNTER; break;
		case 228: type = eSpecType::OUT_MOVE_PARTY; break;
			// 229 was outdoor shop
			// 230-255 were undefined
		case 55: case 58: case 189: case 190: // Large dialogs with 36x36 dialog graphics
			if(pic >= 700 && pic < 1000)
				pic -= 700;
			pictype = PIC_DLOG;
			m3 = m2;
			m2 = -1;
			if(old.type == 55) type = eSpecType::ONCE_DIALOG;
			else if(old.type == 58) type = eSpecType::ONCE_GIVE_ITEM_DIALOG;
			else if(old.type == 190) type = eSpecType::TOWN_STAIR;
			else type = eSpecType::TOWN_PORTAL;
			if(type != eSpecType::ONCE_DIALOG) break;
			// Duplicate Leave button
			if(old.ex1a == 20)
				ex1a = 9;
			if(old.ex2a == 20)
				ex2a = 9;
			break;
		case 57: case 60: // Large dialogs with monster graphics
			if(pic >= 400 && pic < 1000)
				pic -= 400;
			if(pic == 122) pic = 119;
			pictype = PIC_MONST;
			m3 = m2;
			m2 = -1;
			if(old.type == 57) type = eSpecType::ONCE_DIALOG;
			else if(old.type == 60) type = eSpecType::ONCE_GIVE_ITEM_DIALOG;
			if(type != eSpecType::ONCE_DIALOG) break;
			// Duplicate Leave button
			if(old.ex1a == 20)
				ex1a = 9;
			if(old.ex2a == 20)
				ex2a = 9;
			break;
		case 56: case 59: case 188: // Large dialogs with terrain graphics
			pictype = PIC_TER;
			m3 = m2;
			m2 = -1;
			switch(pic) {
				case 247: pic = 210; break;
				case 248: pic = 211; break;
				case 249: pic = 212; break;
				case 250: pic = 213; break;
				case 202: pic = 0; break;
				case 203: pic = 2; break;
				case 204: pic = 32; break;
				case 207: pic = 0; break;
				case 208: pic = 123; break;
				case 209: pic = 210; break;
				case 210: pic = 163; break;
				case 211: pic = 2; break;
				case 212: pic = 32; break;
				case 218: case 219: case 220: case 221:
				case 222: case 223: case 224: case 225:
				case 215: pic = 216; break;
				case 233: pic = 137; break;
				case 213: pic = 214; break;
				case 214: pic = 215; break;
				case 246: pic = 209; break;
				case 251: pic = 207; break;
				case 252: pic = 208; break;
			}
			if(old.type == 56) type = eSpecType::ONCE_DIALOG;
			else if(old.type == 59) type = eSpecType::ONCE_GIVE_ITEM_DIALOG;
			else type = eSpecType::TOWN_LEVER;
			if(type != eSpecType::ONCE_DIALOG) break;
			// Duplicate Leave button
			if(old.ex1a == 20)
				ex1a = 9;
			if(old.ex2a == 20)
				ex2a = 9;
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
			ex1a = int(eSpecCtx::TARGET);
			ex1b = int(eSpell::RITUAL_SANCTIFY);
			break;
		case 99: case 100: // Add mage/priest spell TODO: Merge these by adding 100 if it's a priest spell
			if(old.type == 99) type = eSpecType::AFFECT_MAGE_SPELL;
			else type = eSpecType::AFFECT_PRIEST_SPELL;
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
			// Set it to require at least 1 PC:
			ex2a = 1;
			ex2b = 2;
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
			} else ex2a = int(eSkill::MAGE_LORE);
			ex2b = 0;
			break;
		case 154: // text response
			type = eSpecType::IF_TEXT_RESPONSE;
			ex1a -= 160;
			ex2a -= 160;
			break;
		case 229: // Outdoor store - fix spell IDs
			type = eSpecType::ENTER_SHOP;
			if(ex1b == 1 || ex1b == 2)
				ex1a += 30;
			break;
		case 4: // Secret passage
			type = eSpecType::CANT_ENTER;
			ex1a = 0;
			ex2a = (old.type == 4);
			break;
		case 11: // Can't enter
			type = eSpecType::CANT_ENTER;
			ex1a = ex1a==0 ? 0 : 1;
			ex2a = 0;
			break;
		case 26: // Wandering will fight
			type = eSpecType::CANT_ENTER;
			ex1a = 1 - ex1a;
			ex2a = 0;
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
			ex2a = (old.type - 141) / 5;
			break;
		case 182: // Destroy all monsters of particular type
			type = eSpecType::TOWN_NUKE_MONSTS;
			// 0: means all monsters, -1: friend monsters, -2: hostile monster
			//    so, we must not use these values.
			if(ex1a >= -2 && ex1a <= 0)
				ex1a = -3;
			break;
		case 183: // Destroy all monsters, or all friendly / all hostile
			type = eSpecType::TOWN_NUKE_MONSTS;
			ex1a = -ex1a;
			break;
		case 193: // Split party
			type = eSpecType::TOWN_SPLIT_PARTY;
			if(ex2a > 0) ex2a = 10;
			break;
		case 194: // Reunite party
			type = eSpecType::TOWN_REUNITE_PARTY;
			if(ex1a > 0) ex1a = 10;
			ex2a = 0;
			break;
			// Party statuses (three nodes collapsed into one)
		case 104:
			type = eSpecType::AFFECT_PARTY_STATUS;
			ex1b = 0;
			ex2a = int(ePartyStatus::STEALTH);
			break;
		case 105:
			type = eSpecType::AFFECT_PARTY_STATUS;
			ex1b = 0;
			ex2a = int(ePartyStatus::FIREWALK);
			break;
		case 106:
			type = eSpecType::AFFECT_PARTY_STATUS;
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
			showWarning("This scenario contains a Display Picture special node created by the 'Classic Windows' version of the game. Although this version of the game also supports a Display Picture node, the format is incompatible, and automatic conversion is impossible.", "If this is not your scenario, consider contacting the scenario designer to get this fixed.");
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
		case -1:
			break;
		default:
			if(old.type >= 0 && old.type < 255)
				showError("Unrecognized node type found: " + std::to_string(old.type));
			else std::cout << "Unrecognized node type found: " << old.type
				<< "\n\tNote: This could indicate corruption in the scenario, but more likely is just a result of garbage data in unused nodes or in the memory structures they were read into. The unrecognized node type has been replaced with invalid type -1.";
	}
}

static eSpecCat getNodeCategory(eSpecType node) {
	for(int i = 0; i <= int(eSpecCat::OUTDOOR); i++) {
		eSpecCat cat = eSpecCat(i);
		if((*cat).contains(node)) return cat;
	}
	return eSpecCat::INVALID;
}

static std::map<eSpecType, node_properties_t>& nodeProps() {
	static std::map<eSpecType, node_properties_t> props;
	return props;
}

const node_properties_t& operator* (eSpecType t) {
	static node_properties_t invalid;
	std::map<eSpecType, node_properties_t>& allNodeProps = nodeProps();
	auto iter = allNodeProps.find(t);
	return iter == allNodeProps.end() ? invalid : iter->second;
}

// Note: While it might seem like it would be simpler to define these in a map,
// having each one as a separate extern variable serves an important purpose:
// it prevents the special node definitions from being optimized away by the linker.
const node_category_info_t& operator* (eSpecCat t) {
	extern node_category_info_t CAT_GENERAL, CAT_ONCE, CAT_AFFECT, CAT_COND, CAT_TOWN, CAT_RECT, CAT_OUTD;
	static node_category_info_t CAT_INVALID;
	switch(t) {
		case eSpecCat::GENERAL: return CAT_GENERAL;
		case eSpecCat::ONCE: return CAT_ONCE;
		case eSpecCat::AFFECT: return CAT_AFFECT;
		case eSpecCat::IF_THEN: return CAT_COND;
		case eSpecCat::TOWN: return CAT_TOWN;
		case eSpecCat::RECT: return CAT_RECT;
		case eSpecCat::OUTDOOR: return CAT_OUTD;
		case eSpecCat::INVALID: return CAT_INVALID;
	}
	return CAT_INVALID;
}

bool node_category_info_t::contains(eSpecType spec) const {
	int code = (int) spec;
	return code >= int(first) && code <= int(last);
}

node_properties_t::node_properties_t(eSpecType type)
	: self(type)
	, cat(getNodeCategory(type))
	, f_jmp(eSpecPicker::NODE)
{}

std::string node_properties_t::opcode() const {
	if(self == eSpecType::NONE) return "nop";
	return get_str("specials-opcodes", int(self));
}

static std::string get_node_string(std::string base, eSpecType type, int which) {
	eSpecCat cat = getNodeCategory(type);
	int offset = int((*cat).first), i = int(type);
	int strnum = (i - offset) * 17 + which + 1;
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

std::string node_properties_t::descr() const {
	return get_node_string("specials-text", self, 15);
}

node_function_t::node_function_t() {}

node_function_t::node_function_t(eSpecPicker button)
	: button(button)
{
	if(button == eSpecPicker::PICTURE) {
		pic_type = PIC_NONE;
	}
	if(button == eSpecPicker::NODE || button == eSpecPicker::MSG_PAIR || button == eSpecPicker::MSG_SINGLE || button == eSpecPicker::MSG_SEQUENCE) {
		force_global = false;
	}
}

node_function_t::node_function_t(eStrType str)
	: button(eSpecPicker::STRING)
	, str_type(str)
{
	// Some string types require an adjustment to the index.
	switch(str) {
		case STRT_PICT: adjust = -1; break;
		case STRT_MONST: adjust = -1; break;
		case STRT_CMP: adjust = 2; break;
		case STRT_ACCUM: adjust = 1; break;
		case STRT_STATUS: adjust = 1; break;
		default: break;
	}
}

node_function_t::node_function_t(ePicType pic)
	: button(eSpecPicker::PICTURE)
	, pic_type(pic)
{}

node_function_t operator+(eSpecPicker picker) {
	node_function_t n(picker);
	if(picker == eSpecPicker::NODE || picker == eSpecPicker::MSG_PAIR || picker == eSpecPicker::MSG_SINGLE || picker == eSpecPicker::MSG_SEQUENCE) {
		n.force_global = true;
	}
	if(picker == eSpecPicker::FIELD) {
		n.augmented = true;
	}
	return n;
}

node_function_t operator+(eStrType str) {
	node_function_t n(str);
	if(str == STRT_SPELL_PAT || str == STRT_SKILL) {
		n.augmented = true;
	}
	return n;
}

std::string node_function_t::label() const {
	return get_node_string("specials-text", self, lbl_idx);
}

node_function_t node_properties_t::sdf1(const cSpecial&) const {
	return f_sd1;
}

node_function_t node_properties_t::sdf2(const cSpecial&) const {
	return f_sd2;
}

node_function_t node_properties_t::msg1(const cSpecial&) const {
	return f_m1;
}

node_function_t node_properties_t::msg2(const cSpecial&) const {
	return f_m2;
}

node_function_t node_properties_t::msg3(const cSpecial&) const {
	return f_m3;
}

node_function_t node_properties_t::pic(const cSpecial&) const {
	return f_pic;
}

node_function_t node_properties_t::pictype(const cSpecial&) const {
	return f_pt;
}

node_function_t node_properties_t::ex1a(const cSpecial&) const {
	return f_x1a;
}

node_function_t node_properties_t::ex1b(const cSpecial&) const {
	return f_x1b;
}

node_function_t node_properties_t::ex1c(const cSpecial&) const {
	return f_x1c;
}

node_function_t node_properties_t::ex2a(const cSpecial&) const {
	return f_x2a;
}

node_function_t node_properties_t::ex2b(const cSpecial&) const {
	return f_x2b;
}

node_function_t node_properties_t::ex2c(const cSpecial&) const {
	return f_x2c;
}

node_function_t node_properties_t::jump(const cSpecial&) const {
	return f_jmp;
}

struct field_map {
	std::map<eSpecField, node_function_t(node_properties_t::*)> map = {
		{eSpecField::SDF1, &node_properties_t::f_sd1},
		{eSpecField::SDF2, &node_properties_t::f_sd2},
		{eSpecField::MSG1, &node_properties_t::f_m1},
		{eSpecField::MSG2, &node_properties_t::f_m2},
		{eSpecField::MSG3, &node_properties_t::f_m3},
		{eSpecField::PICT, &node_properties_t::f_pic},
		{eSpecField::PTYP, &node_properties_t::f_pt},
		{eSpecField::EX1A, &node_properties_t::f_x1a},
		{eSpecField::EX1B, &node_properties_t::f_x1b},
		{eSpecField::EX1C, &node_properties_t::f_x1c},
		{eSpecField::EX2A, &node_properties_t::f_x2a},
		{eSpecField::EX2B, &node_properties_t::f_x2b},
		{eSpecField::EX2C, &node_properties_t::f_x2c},
		{eSpecField::JUMP, &node_properties_t::f_jmp},
	};
};

static field_map& fields() {
	static field_map map;
	return map;
}

node_builder_t& node_builder_t::sdf() {
	// The intent is to specify that sdf1,sdf2 is a stuff done flag.
	// But specifying that two fields are a stuff done flag isn't implemented yet.
	return sdf(eSpecField::SDF1, eSpecField::SDF2);
}

node_builder_t& node_builder_t::msg() {
	return msg1(eSpecPicker::MSG_PAIR).msg2(eSpecPicker::NONE);
};

node_builder_t& node_builder_t::rect() {
	// The intent is to specify that ex1a,ex1b, and ex2a,ex2b are locations.
	// But specifying that two fields are a location isn't implemented yet.
	return loc(eSpecField::EX1A, eSpecField::EX1B).loc(eSpecField::EX2A, eSpecField::EX2B);
};

node_builder_t& node_builder_t::pic() {
	return pict(eSpecPicker::PICTURE).ptyp(STRT_PICT);
}

node_builder_t& node_builder_t::sdf1(node_function_t picker) {
	return field(eSpecField::SDF1, picker);
}

node_builder_t& node_builder_t::sdf2(node_function_t picker) {
	return field(eSpecField::SDF2, picker);
}

node_builder_t& node_builder_t::jump(node_function_t picker) {
	return field(eSpecField::JUMP, picker);
}

node_builder_t& node_builder_t::msg1(node_function_t picker) {
	return field(eSpecField::MSG1, picker);
}

node_builder_t& node_builder_t::msg2(node_function_t picker) {
	return field(eSpecField::MSG2, picker);
}

node_builder_t& node_builder_t::msg3(node_function_t picker) {
	return field(eSpecField::MSG3, picker);
}

node_builder_t& node_builder_t::pict(node_function_t picker) {
	return field(eSpecField::PICT, picker);
}

node_builder_t& node_builder_t::ptyp(node_function_t picker) {
	return field(eSpecField::PTYP, picker);
}

node_builder_t& node_builder_t::ex1a(node_function_t picker) {
	return field(eSpecField::EX1A, picker);
}

node_builder_t& node_builder_t::ex1b(node_function_t picker) {
	return field(eSpecField::EX1B, picker);
}

node_builder_t& node_builder_t::ex1c(node_function_t picker) {
	return field(eSpecField::EX1C, picker);
}

node_builder_t& node_builder_t::ex2a(node_function_t picker) {
	return field(eSpecField::EX2A, picker);
}

node_builder_t& node_builder_t::ex2b(node_function_t picker) {
	return field(eSpecField::EX2B, picker);
}

node_builder_t& node_builder_t::ex2c(node_function_t picker) {
	return field(eSpecField::EX2C, picker);
}

node_builder_t& node_builder_t::field(eSpecField field, node_function_t picker) {
	picker.self = node.self;
	node.*fields().map[field] = picker;
	return *this;
}

node_builder_t& node_builder_t::sdf(eSpecField a, eSpecField b) {
	// A stuff done flag picker isn't implemented yet.
	return *this;
}

node_builder_t& node_builder_t::loc(eSpecField a, eSpecField b) {
	// A location picker isn't implemented yet
	return *this;
}

node_builder_t::operator node_properties_t() {
	node.set_label_indices();
	std::map<eSpecType, node_properties_t>& allNodeProps = nodeProps();
	allNodeProps.emplace(node.self, node);
	return node;
}

void node_properties_t::set_label_indices() {
	f_sd1.lbl_idx = 1;
	f_sd2.lbl_idx = 2;
	f_m1.lbl_idx = 3;
	f_m2.lbl_idx = 4;
	f_m3.lbl_idx = 5;
	f_pic.lbl_idx = 6;
	f_pt.lbl_idx = 7;
	f_x1a.lbl_idx = 8;
	f_x1b.lbl_idx = 9;
	f_x1c.lbl_idx = 10;
	f_x2a.lbl_idx = 11;
	f_x2b.lbl_idx = 12;
	f_x2c.lbl_idx = 13;
	f_jmp.lbl_idx = 14;
	f_sd1.self = f_sd2.self = f_jmp.self = f_m1.self = f_m2.self = f_m3.self = f_pic.self = f_pt.self = f_x1a.self = f_x1b.self = f_x1c.self = f_x2a.self = f_x2b.self = f_x2c.self = self;
}
