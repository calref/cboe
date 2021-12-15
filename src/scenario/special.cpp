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

#include "strdlog.hpp"
#include "oldstructs.hpp"
#include "utility.hpp"

#include "spell.hpp"
#include "skills_traits.hpp"
#include "damage.hpp"
#include "fields.hpp"
#include "pict.hpp"

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
	if (type==eSpecType::NONE) // do not write nop node
		return;
	// TODO: Output only the needed values somehow
	file << '@' << (*type).opcode() << " = " << n << '\n';
	file << "\tsdf " << sd1 << ", " << sd2 << '\n';
	file << "\tmsg " << m1 << ", " << m2 << ", " << m3 << '\n';
	file << "\tpic " << pic << ", " << pictype << '\n';
	file << "\tex1 " << ex1a << ", " << ex1b << ", " << ex1c << '\n';
	file << "\tex2 " << ex2a << ", " << ex2b << ", " << ex2c << '\n';
	file << "\tgoto " << jumpto << '\n';
}

static cPictNum port_graphic_num(int pic) {
	// from dialog-converting.txt
	/*
	-1			solid black
	0 + x		number of terrain graphic
	300 + x		animated terrain graphic (grabs the first frame only)
	400 + x		monster graphic num
	700 + x		dlog graphic (large dlog graphics were done by using four of these arranged in the correct way)
	800 + x		pc graphic
	900 + x		B&W graphic - the PICT resource for this does not exist
	950			null item
	1000 + x	Talking face
	1100		item info help
	1200		pc screen help
	1300		combat ap
	1400-1402	button help
	1410-1412	large scen graphics
	1500		stat symbols help
	1600 + x	scen graphics
	1700 + x	anim graphic -- drawn from fields_gworld, so a boom or barrier icon?
	1800 + x	items
	2000 + x	custom graphics up to 2399
	2400 + x	custom graphics up to 2799, BUT it's a split graphic ...
	   it looks at the size of rect, and places a 32 x 32 or 36 x 36 graphic drawn
	   from the custom gworld, depending on the size of rect. half of graphic is
	   drawn from one custom slot, and half is drawn from next one.
	+3000		suppress drawing a frame around the graphic
	 */
	if (pic<0)
		return cPictNum(pic, PIC_NONE);
	if (pic>3000) pic-=3000; // suppress drawing a frame around the graphic
	if (pic<300) {
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
		return cPictNum(pic, PIC_TER);
	}
	if (pic<400)
		return cPictNum(pic-300, PIC_TER_ANIM);
	if (pic<700)
		return cPictNum(pic-400,PIC_MONST);
	if (pic<800)
		return cPictNum(pic-700, PIC_DLOG);
	if (pic<900)
		return cPictNum(pic-800, PIC_PC);
	if (pic<1000)// ARGH: normally bwpats, force an error picture
		return cPictNum(800, PIC_TER);
	if (pic<1100) // can we really get some talk here?
		return cPictNum(pic-1000, PIC_TALK);
#if 0
	if (pic == 1100 || pic==1200 || pic==1300 || (pic>=1400 && pic<=1402)) // maybe, but the picture's size will be bad and hide the message...
		return cPictNum(pic, PIC_FULL);
#endif
	if (pic<2000) // ARGH: unsure force an error picture
		return cPictNum(800, PIC_TER);
	if (pic<2400)
		return cPictNum(pic-2000, PIC_CUSTOM_TER);
	if (pic<2800)
		return cPictNum(pic-2400, PIC_CUSTOM_DLOG); // or pict custom monster large (in fact this one seems to depend on the dialog final size)
	// ARGH: not expected, force an error picture
	return cPictNum(800, PIC_TER);
}
	
void cSpecial::import_legacy(legacy::special_node_type const &old){
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
		case 4: // Secret passage
			type = eSpecType::CANT_ENTER;
			ex1a = 0;
			ex2a = 1;
			break;
		case 5: type = eSpecType::DISPLAY_SM_MSG; break;
		case 6: type = eSpecType::FLIP_SDF; break;
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
		case 11: // Can't enter
			type = eSpecType::CANT_ENTER;
			ex1a = ex1a==0 ? 0 : 1;
			ex2a = 0;
			break;
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
		case 24: // ritual of sanctification
			type = eSpecType::IF_CONTEXT;
			ex1c = jumpto;
			jumpto = ex1b;
			ex1a = int(eSpecCtx::TARGET);
			ex1b = int(eSpell::RITUAL_SANCTIFY);
			break;
		case 25: type = eSpecType::REST; break;
		case 26: // Wandering will fight
			type = eSpecType::CANT_ENTER;
			ex1a = 1 - ex1a;
			ex2a = 0;
			break;
		case 27: type = eSpecType::END_SCENARIO; break;
		case 28: // not present in the first Mac versions
			type = eSpecType::DISPLAY_PICTURE;
			showWarning("This scenario contains a Display Picture special node created by the 'Classic Windows' version of the game. Although this version of the game also supports a Display Picture node, the format is incompatible, and automatic conversion is impossible.", "If this is not your scenario, consider contacting the scenario designer to get this fixed.");
			ex1a = 0;
			break;
		case 29: // not present in the first Mac versions
			type = eSpecType::SDF_RANDOM;
			break;

			// 30-49 were undefined

		case 50: type = eSpecType::ONCE_GIVE_ITEM; break;
		case 51: type = eSpecType::ONCE_GIVE_SPEC_ITEM; break;
		case 52: type = eSpecType::ONCE_NULL; break;
		case 53: type = eSpecType::ONCE_SET_SDF; break;
		case 54: type = eSpecType::ONCE_DISPLAY_MSG; break;
			// 55,56,57 were standard dialogs (handle latter)
			// 58,59,60 were give item dialogs (handle latter)
		case 61: type = eSpecType::ONCE_OUT_ENCOUNTER; break;
		case 62: type = eSpecType::ONCE_TOWN_ENCOUNTER; break;
		case 63: // Trap used to force a specific picture
			type = eSpecType::ONCE_TRAP;
			pic = 27;
			pictype = PIC_DLOG;
			break;
			
			// 64-79 were undefined
			
		case 80: type = eSpecType::SELECT_TARGET; break;
		case 81: type = eSpecType::DAMAGE; break;
		case 82: type = eSpecType::AFFECT_HP; break;
		case 83: type = eSpecType::AFFECT_SP; break;
		case 84: type = eSpecType::AFFECT_XP; break;
		case 85: type = eSpecType::AFFECT_SKILL_PTS; break;
		case 86: type = eSpecType::AFFECT_DEADNESS; break;
			// Affect status effect (eleven individual node types were collapsed into one)
		case 87: case 88:  case 89:
		case 90: case 91:  case 92:
		case 93: case 94:  case 95:
		case 96: case 97: {
			eStatus const status[]= {
				eStatus::POISON, eStatus::HASTE_SLOW, eStatus::INVULNERABLE,
				eStatus::MAGIC_RESISTANCE, eStatus::WEBS, eStatus::DISEASE,
				eStatus::INVISIBLE, eStatus::BLESS_CURSE, eStatus::DUMB,
				eStatus::ASLEEP, eStatus::PARALYZED
			};
			type = eSpecType::AFFECT_STATUS;
			ex1c = int(status[old.type-87]);
			break;
		}
		case 98: type = eSpecType::AFFECT_STAT; break;
		case 99: case 100: // Add mage/priest spell TODO: Merge these by adding 100 if it's a priest spell
			if(old.type == 99) type = eSpecType::AFFECT_MAGE_SPELL;
			else type = eSpecType::AFFECT_PRIEST_SPELL;
			ex1a += 30;
			ex1b = 0; // Meaning give spell, not take
			break;
		case 101: type = eSpecType::AFFECT_GOLD; break;
		case 102: type = eSpecType::AFFECT_FOOD; break;
		case 103: type = eSpecType::AFFECT_ALCHEMY; break;
			// Party statuses (three nodes collapsed into one)
		case 104: case 105: case 106: {
			ePartyStatus const status[]={ ePartyStatus::STEALTH, ePartyStatus::FIREWALK, ePartyStatus::FLIGHT};
			type = eSpecType::AFFECT_PARTY_STATUS;
			ex1b = 0;
			ex2a = int(status[old.type-104]);
			break;
		}

			// 107-129 were undefined
		case 130: type = eSpecType::IF_SDF; break;
		case 131: type = eSpecType::IF_TOWN_NUM; break;
		case 132: type = eSpecType::IF_RANDOM; break;
		case 133: type = eSpecType::IF_HAVE_SPECIAL_ITEM; break;
		case 134: type = eSpecType::IF_SDF_COMPARE; break;
		case 135: case 136: // If terrain
			type = eSpecType::IF_TER_TYPE;
			break;
		case 137: case 142: // If has gold (and maybe take)
		case 138: case 143: // If has food (and maybe take)
		case 139: case 144: // If item on space (and maybe take)
		case 140: case 145: // If have item class (and maybe take)
		case 141: case 146: { // If equip item class (and maybe take)
			eSpecType const types[]={
				eSpecType::IF_HAS_GOLD, eSpecType::IF_HAS_FOOD, eSpecType::IF_ITEM_CLASS_ON_SPACE,
				eSpecType::IF_HAVE_ITEM_CLASS, eSpecType::IF_EQUIP_ITEM_CLASS
			};
			type = types[(old.type-137)%5];
			if (old.type==139 || old.type==144) // ex1a, ex2a is used to store the item'slocation
				ex2c = (old.type<142 ? 0 : 1);
			else
				ex2a = (old.type<142 ? 0 : 1);
			break;
		}
		case 147: type = eSpecType::IF_DAY_REACHED; break;
		case 148: case 149: // if barrels or crates
			type = eSpecType::IF_FIELDS;
			m1 = old.type == 148 ? OBJECT_BARREL : OBJECT_CRATE;
			m2 = ex1b;
			ex1a = ex1b = 0;
			ex2a = ex2b = 64;
			sd1 = 1;
			sd2 = std::numeric_limits<short>::max();
			break;
		case 150: type = eSpecType::IF_EVENT_OCCURRED; break;
		case 151: case 152: // if has cave lore or woodsman
			type = eSpecType::IF_TRAIT;
			ex1a = old.type - 147;
			// Set it to require at least 1 PC:
			ex2a = 1;
			ex2b = 2;
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
					// id between 10 and 160 will be fixed later by port_special_text_response, ...
			ex1a -= 160;
			ex2a -= 160;
			break;
		case 155: type = eSpecType::IF_SDF_EQ; break;
		case 156: // not present in the first Mac versions
			type = eSpecType::IF_SPECIES;
			break;

			// 157-169 were undefined
			
		case 170: type = eSpecType::MAKE_TOWN_HOSTILE; ex2a=1; break;
		case 171: type = eSpecType::CHANGE_TER; break; // Change town terrain
		case 172: type = eSpecType::SWAP_TER; break;
		case 173: type = eSpecType::TRANS_TER; break;
		case 174: type = eSpecType::TOWN_MOVE_PARTY; break;
		case 175: type = eSpecType::TOWN_HIT_SPACE; break;
		case 176: type = eSpecType::TOWN_EXPLODE_SPACE; break;
		case 177: type = eSpecType::TOWN_LOCK_SPACE; break;
		case 178: type = eSpecType::TOWN_UNLOCK_SPACE; break;
		case 179: type = eSpecType::TOWN_SFX_BURST; break;
		case 180: type = eSpecType::TOWN_CREATE_WANDERING; break;
		case 181: type = eSpecType::TOWN_PLACE_MONST; break;
		case 182: // Destroy all monsters of particular type
			type = eSpecType::TOWN_NUKE_MONSTS;
			if (ex1a>=-2 && ex1a<=0) // FIXME: if ex1a:type==0, we do not want to remove all monsters
				ex1a=-3;
			break;
		case 183: // Destroy all monsters, or all friendly / all hostile
			type = eSpecType::TOWN_NUKE_MONSTS;
			ex1a = -ex1a;
			break;
		case 184: type = eSpecType::TOWN_GENERIC_LEVER; break;
		case 185: type = eSpecType::TOWN_GENERIC_PORTAL; break;
		case 186: type = eSpecType::TOWN_GENERIC_BUTTON; break;
		case 187: type = eSpecType::TOWN_GENERIC_STAIR; break;
			// 188 was town lever (handle latter)
			// 189 was town portal (handle latter)
			// 190 was town stair (handle latter)
		case 191: type = eSpecType::TOWN_RELOCATE; break;
		case 192: type = eSpecType::TOWN_PLACE_ITEM; break;
		case 193: // Split party
			type = eSpecType::TOWN_SPLIT_PARTY;
			if(ex2a > 0) ex2a = 10;
			break;
		case 194: // Reunite party
			type = eSpecType::TOWN_REUNITE_PARTY;
			if(ex1a > 0) ex1a = 10;
			ex2a = 0;
			break;
		case 195: type = eSpecType::TOWN_TIMER_START; break;
		case 196: // not present in the first Mac versions
			type = eSpecType::TOWN_CHANGE_LIGHTING;
			break;
		case 197: // not present in the first Mac versions
			type = eSpecType::TOWN_SET_ATTITUDE;
			break;

			// 198-199 were undefined
			
			// Place fields (twelve individual node types were collapsed into one)
		case 200: case 201: case 202:
		case 203: case 204: case 205:
		case 206: case 207: case 208:
		case 209: case 210: {
			eFieldType const fields[]={
				WALL_FIRE, WALL_FORCE, WALL_ICE,
				WALL_BLADES, CLOUD_STINK, CLOUD_SLEEP,
				FIELD_QUICKFIRE, BARRIER_FIRE, BARRIER_FORCE,
				FIELD_DISPEL, SFX_SMALL_BLOOD
			};
			type = eSpecType::RECT_PLACE_FIELD;
			sd2 = fields[old.type-200];
			break;
		}
		case 211:
			type = eSpecType::RECT_PLACE_FIELD;
			switch(old.sd2) {
				case 0: sd2 = FIELD_WEB; break;
				case 1: sd2 = OBJECT_BARREL; break;
				case 2: sd2 = OBJECT_CRATE; break;
			}
			break;
		case 212: type = eSpecType::RECT_MOVE_ITEMS; break;
		case 213: type = eSpecType::RECT_DESTROY_ITEMS; break;
		case 214: type = eSpecType::RECT_CHANGE_TER; break;
		case 215: type = eSpecType::RECT_SWAP_TER; break;
		case 216: type = eSpecType::RECT_TRANS_TER; break;
		case 217: type = eSpecType::RECT_LOCK; break;
		case 218: type = eSpecType::RECT_UNLOCK; break;
			
			// 219-224 were undefined
			
		case 225: type = eSpecType::OUT_MAKE_WANDER; break;
		case 226: type = eSpecType::CHANGE_TER; break; // Change outdoor terrain
		case 227: type = eSpecType::OUT_PLACE_ENCOUNTER; break;
		case 228: type = eSpecType::OUT_MOVE_PARTY; break;
		case 229: // Outdoor store - fix spell IDs
			type = eSpecType::ENTER_SHOP;
			if(ex1b == 1 || ex1b == 2)
				ex1a += 30;
			break;
			
			// 230-255 were undefined
			
		case 55: case 58:
		case 56: case 59: case 188: // Large dialogs with terrain graphics
		case 57: case 60: // Large dialogs with monster graphics
		case 189: case 190: {// Large dialogs with 36x36 dialog graphics
			if (pic>=1000) {
				if (old.type>=55 && old.type<=60 ) pic=(pic%1000)+2000;
				if (old.type==55 || old.type==58) pic+=400;
			}
			cPictNum picTyp=port_graphic_num(pic);
			pic=picTyp.num;
			pictype=picTyp.type;
			m3 = m2;
			m2 = -1;
			if(old.type<=57) type = eSpecType::ONCE_DIALOG; // 55-57
			else if(old.type<=60) type = eSpecType::ONCE_GIVE_ITEM_DIALOG; // 58-60
			else if(old.type == 188) type = eSpecType::TOWN_LEVER;
			else if(old.type == 189) type = eSpecType::TOWN_PORTAL;
			else type = eSpecType::TOWN_STAIR; // 190
			if(type != eSpecType::ONCE_DIALOG) break;
			// Duplicate Leave button
			if(old.ex1a == 20) ex1a = 9;
			if(old.ex2a == 20) ex2a = 9;
			break;
		}
			
		case -1:
			break;
		default:
			if(old.type >= 0 && old.type < 255)
				showError("Unrecognized node type found: " + std::to_string(old.type));
			else std::cout << "Unrecognized node type found: " << old.type
				<< "\n\tNote: This could indicate corruption in the scenario, but more likely is just a result of garbage data in unused nodes or in the memory structures they were read into. The unrecognized node type has been replaced with invalid type -1.";
	}
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
// t - Choose button to select a terrain type
// c - Choose button to select a monster type
// C - Choose button to select a monster statistic
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
// & - Choose button to select a shop
// % - Choose button to select shop cost adjustment
// { - Choose button to select a spell pattern
// } - As above, but allows you to select which version of the rotateable field
// ^ - Choose button to select a positioning mode
// e - Choose button to select a status effect
// E - Choose button to select a party status effect
// w - Choose button to select main party status effect
// j - Choose button to select a quest
// J - Choose button to select a quest status
// < - Choose button to select a cardinal direction
// ~ - Choose button to select a weapon enchantment
// _ - Choose button to select a full sheet
// 0..9 - Choose button to select a specific type of picture
// (terrain, monster, dialog, talk, item, pc, field, boom, missile, status)
static const char*const button_dict[7][11] = {
	{ // general nodes
		" mmmMmmmmmMmmm mmmmmm   Mmm  $ mmmmmm        mmm", // msg1
		"                                                ", // msg2
		"                          M                     ", // msg3
		"          p               p                 3   ", // pic
		"          ?               ?                     ", // pictype
		"    &         x  T i    _             M cit  j  ", // ex1a
		"    %        S     ss                       cJ  ", // ex1b
		"                                                ", // ex1c
		"                                  tt            ", // ex2a
		"                                   t            ", // ex2b
		"                                                ", // ex2c
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
		"mmmmmmmmmmmmmmmmmmmmmmmmmmmm", // msg1
		"                            ", // msg2
		"          M              M  ", // msg3
		"              s          5  ", // pic
		"                         s  ", // pictype
		"      w q     i    AP  a    ", // ex1a
		"              ~            s", // ex1b
		"       e                 Q  ", // ex1c
		"                 CK     E   ", // ex2a
		" D                          ", // ex2b
		" x                          ", // ex2c
	}, { // if-then nodes
		"                  f     $  $   ", // msg1
		"                  s            ", // msg2
		"                               ", // msg3
		"                               ", // pic
		"                               ", // pictype
		" T I  w     APae     Qq $ *   j", // ex1a
		"ssss  sss ssssss s s sssss =  J", // ex1b
		"                s         sssss", // ex1c
		"     t                 K$      ", // ex2a
		"s   ss   s     +    s==+s  =   ", // ex2b
		"               =           s   ", // ex2c
	}, { // town nodes
		"mmmmmmmmmmmmmm    dddmmmmmmmmmmmmmM", // msg1
		"                                   ", // msg2
		"                                   ", // msg3
		" 8                ppp              ", // pic
		"                  ???              ", // pictype
		"             c            L   {    ", // ex1a
		"              s s s      s @       ", // ex1b
		"                               }}  ", // ex1c
		"@  7     ! c     T  T i        FD  ", // ex2a
		"     DD          /               ^ ", // ex2b
		" x x     x       :  :              ", // ex2c
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
		" mmm", // msg1
		"    ", // msg2
		"    ", // msg3
		"    ", // pic
		"    ", // pictype
		" T  ", // ex1a
		" <  ", // ex1b
		"    ", // ex1c
		"    ", // ex2a
		"    ", // ex2b
		"    ", // ex2c
	}
};

static int const offsets[] = {
	int(eSpecType::NONE),
	int(eSpecType::ONCE_GIVE_ITEM),
	int(eSpecType::SELECT_TARGET),
	int(eSpecType::IF_SDF),
	int(eSpecType::MAKE_TOWN_HOSTILE),
	int(eSpecType::RECT_PLACE_FIELD),
	int(eSpecType::OUT_MAKE_WANDER),
};

static eSpecCat getNodeCategory(eSpecType node) {
	int code = (int) node;
	if(code >= 0 && code <= 47)
		return eSpecCat::GENERAL;
	if(code >= 50 && code <= 63)
		return eSpecCat::ONCE;
	if(code >= 80 && code <= 107)
		return eSpecCat::AFFECT;
	if(code >= 130 && code <= 160)
		return eSpecCat::IF_THEN;
	if(code >= 170 && code <= 204)
		return eSpecCat::TOWN;
	if(code >= 210 && code <= 218)
		return eSpecCat::RECT;
	if(code >= 225 && code <= 228)
		return eSpecCat::OUTDOOR;
	return eSpecCat::INVALID;
}

static std::map<eSpecType, node_properties_t> loadProps() {
	std::map<eSpecType, node_properties_t> allNodeProps;
	for(unsigned short i = 0; i <= int(eSpecType::MAX_SPEC_TYPE); i++) {
		eSpecType check = (eSpecType) i;
		eSpecCat category = getNodeCategory(check);
		if(category == eSpecCat::INVALID) continue;
		node_properties_t props;
		props.self = check;
		props.cat = category;
		int j = int(category), k = i - offsets[j];
		props.m1_btn = button_dict[j][0][k];
		props.m2_btn = button_dict[j][1][k];
		props.m3_btn = button_dict[j][2][k];
		props.p_btn = button_dict[j][3][k];
		props.pt_btn = button_dict[j][4][k];
		if(category != eSpecCat::RECT) {
			props.sd1_btn = ' ';
			props.x1a_btn = button_dict[j][5][k];
			props.x1b_btn = button_dict[j][6][k];
		} else props.sd1_btn = button_dict[j][5][k];
		props.x1c_btn = button_dict[j][7][k];
		if(category != eSpecCat::RECT) {
			props.sd2_btn = ' ';
			props.x2a_btn = button_dict[j][8][k];
			props.x2b_btn = button_dict[j][9][k];
		} else props.sd2_btn = button_dict[j][8][k];
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
	static node_properties_t invalid;
	static std::map<eSpecType, node_properties_t> allNodeProps = loadProps();
	auto iter = allNodeProps.find(t);
	return iter == allNodeProps.end() ? invalid : iter->second;
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
