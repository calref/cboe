/*
 *  special.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#ifndef BOE_DATA_SPECIAL_H
#define BOE_DATA_SPECIAL_H

#include <iosfwd>
#include <string>
#include <functional>
#include "location.hpp"

namespace legacy { struct special_node_type; };

static const short SDF_COMPLETE = 250;

// TODO: Add win/lose option to END_SCENARIO
// TODO: Allow OUT_MOVE_PARTY to change the current sector
enum class eSpecType {
	INVALID = -1, // A magic value used while processing nodes
	NONE = 0, SET_SDF = 1, INC_SDF = 2, DISPLAY_MSG = 3, ENTER_SHOP = 4,
	DISPLAY_SM_MSG = 5, FLIP_SDF = 6, SDF_RANDOM = 7, SDF_ADD = 8, SDF_DIFF = 9,
	STORY_DIALOG = 10, CANT_ENTER = 11, CHANGE_TIME = 12, SCEN_TIMER_START = 13, PLAY_SOUND = 14,
	CHANGE_HORSE_OWNER = 15, CHANGE_BOAT_OWNER = 16, SET_TOWN_VISIBILITY = 17, MAJOR_EVENT_OCCURRED = 18, FORCED_GIVE = 19,
	BUY_ITEMS_OF_TYPE = 20, CALL_GLOBAL = 21, SET_SDF_ROW = 22, COPY_SDF = 23, DISPLAY_PICTURE = 24,
	REST = 25, TITLED_MSG = 26, END_SCENARIO = 27, SET_POINTER = 28, SET_CAMP_FLAG = 29,
	PRINT_NUMS = 30, SDF_TIMES = 31, SDF_DIVIDE = 32, SDF_POWER = 33, CHANGE_TER = 34,
	SWAP_TER = 35, TRANS_TER = 36, CLEAR_BUF = 37, APPEND_STRING = 38, APPEND_NUM = 39,
	APPEND_MONST = 40, APPEND_ITEM = 41, APPEND_TER = 42, PAUSE = 43, START_TALK = 44,
	UPDATE_QUEST = 45, SWAP_STR_BUF = 46, STR_BUF_TO_SIGN = 47,
	
	ONCE_GIVE_ITEM = 50, ONCE_GIVE_SPEC_ITEM = 51, ONCE_NULL = 52, ONCE_SET_SDF = 53, ONCE_DISPLAY_MSG = 54,
	ONCE_DIALOG = 55, UNUSED13 = 56, UNUSED14 = 57, ONCE_GIVE_ITEM_DIALOG = 58, UNUSED15 = 59,
	UNUSED16 = 60, ONCE_OUT_ENCOUNTER = 61, ONCE_TOWN_ENCOUNTER = 62, ONCE_TRAP = 63,
	
	SELECT_TARGET = 80, DAMAGE = 81, AFFECT_HP = 82, AFFECT_SP = 83, AFFECT_XP = 84,
	AFFECT_SKILL_PTS = 85, AFFECT_DEADNESS = 86, AFFECT_STATUS = 87, AFFECT_TRAITS = 88, AFFECT_AP = 89,
	AFFECT_NAME = 90, AFFECT_LEVEL = 91, AFFECT_MORALE = 92, AFFECT_SOUL_CRYSTAL = 93, GIVE_ITEM = 94,
	AFFECT_MONST_TARG = 95, AFFECT_MONST_ATT = 96, AFFECT_MONST_STAT = 97, AFFECT_STAT = 98, AFFECT_MAGE_SPELL = 99,
	AFFECT_PRIEST_SPELL = 100, AFFECT_GOLD = 101, AFFECT_FOOD = 102, AFFECT_ALCHEMY = 103, AFFECT_PARTY_STATUS = 104,
	CREATE_NEW_PC = 105, STORE_PC = 106, UNSTORE_PC = 107,
	
	IF_SDF = 130, IF_TOWN_NUM = 131, IF_RANDOM = 132, IF_HAVE_SPECIAL_ITEM = 133, IF_SDF_COMPARE = 134,
	IF_TER_TYPE = 135, IF_ALIVE = 136, IF_HAS_GOLD = 137, IF_HAS_FOOD = 138, IF_ITEM_CLASS_ON_SPACE = 139,
	IF_HAVE_ITEM_CLASS = 140, IF_EQUIP_ITEM_CLASS = 141, IF_MAGE_SPELL = 142, IF_PRIEST_SPELL = 143, IF_RECIPE = 144,
	IF_STATUS = 145, IF_LOOKING = 146, IF_DAY_REACHED = 147, IF_FIELDS = 148, IF_PARTY_SIZE = 149,
	IF_EVENT_OCCURRED = 150, IF_SPECIES = 151, IF_TRAIT = 152, IF_STATISTIC = 153, IF_TEXT_RESPONSE = 154,
	IF_SDF_EQ = 155, IF_CONTEXT = 156, IF_NUM_RESPONSE = 157, IF_IN_BOAT = 158, IF_ON_HORSE = 159,
	IF_QUEST = 160,
	
	MAKE_TOWN_HOSTILE = 170, TOWN_RUN_MISSILE = 171, TOWN_MONST_ATTACK = 172, TOWN_BOOM_SPACE = 173, TOWN_MOVE_PARTY = 174,
	TOWN_HIT_SPACE = 175, TOWN_EXPLODE_SPACE = 176, TOWN_LOCK_SPACE = 177, TOWN_UNLOCK_SPACE = 178, TOWN_SFX_BURST = 179,
	TOWN_CREATE_WANDERING = 180, TOWN_PLACE_MONST = 181, TOWN_DESTROY_MONST = 182, TOWN_NUKE_MONSTS = 183, TOWN_GENERIC_LEVER = 184,
	TOWN_GENERIC_PORTAL = 185, TOWN_GENERIC_BUTTON = 186, TOWN_GENERIC_STAIR = 187, TOWN_LEVER = 188, TOWN_PORTAL = 189,
	TOWN_STAIR = 190, TOWN_RELOCATE = 191, TOWN_PLACE_ITEM = 192, TOWN_SPLIT_PARTY = 193, TOWN_REUNITE_PARTY = 194,
	TOWN_TIMER_START = 195, TOWN_CHANGE_LIGHTING = 196, TOWN_SET_ATTITUDE = 197, TOWN_SET_CENTER = 198, TOWN_LIFT_FOG = 199,
	TOWN_START_TARGETING = 200, TOWN_SPELL_PAT_FIELD = 201, TOWN_SPELL_PAT_BOOM = 202, TOWN_RELOCATE_CREATURE = 203, TOWN_PLACE_LABEL = 204,
	
	RECT_PLACE_FIELD = 210, RECT_SET_EXPLORED = 211, RECT_MOVE_ITEMS = 212, RECT_DESTROY_ITEMS = 213, RECT_CHANGE_TER = 214,
	RECT_SWAP_TER = 215, RECT_TRANS_TER = 216, RECT_LOCK = 217, RECT_UNLOCK = 218,
	
	OUT_MAKE_WANDER = 225, OUT_FORCE_TOWN = 226, OUT_PLACE_ENCOUNTER = 227, OUT_MOVE_PARTY = 228,
};

class cSpecial {
public:
	eSpecType type;
	short sd1;
	short sd2;
	short pic;
	short pictype;
	short m1;
	short m2;
	short m3;
	short ex1a;
	short ex1b;
	short ex1c;
	short ex2a;
	short ex2b;
	short ex2c;
	short jumpto;
	
	cSpecial();
	void import_legacy(legacy::special_node_type& old);
	void writeTo(std::ostream& file, int n) const;
};

enum class eSpecCtxType {
	SCEN, OUTDOOR, TOWN,
};

class cTimer {
public:
	long time = 0;
	eSpecCtxType node_type = eSpecCtxType::SCEN;
	short node = -1;
	bool is_valid() const;
};

// HAIL means called when initiating conversation.
// TALK means called during conversation.
enum class eSpecCtx {
	OUT_MOVE, TOWN_MOVE, COMBAT_MOVE, OUT_LOOK, TOWN_LOOK,
	ENTER_TOWN, LEAVE_TOWN, TALK, USE_SPEC_ITEM, TOWN_TIMER,
	SCEN_TIMER, PARTY_TIMER, KILL_MONST, OUTDOOR_ENC, FLEE_ENCOUNTER,
	WIN_ENCOUNTER, TARGET, USE_SPACE, SEE_MONST, MONST_SPEC_ABIL,
	TOWN_HOSTILE, ATTACKING_MELEE, ATTACKING_RANGE, ATTACKED_MELEE, ATTACKED_RANGE,
	HAIL, SHOPPING, DROP_ITEM, STARTUP,
};

struct pending_special_type {
	spec_num_t spec;
	eSpecCtx mode;
	eSpecCtxType type;
	location where;
	long long trigger_time;
};

enum class eSpecCat {
	INVALID = -1,
	GENERAL, ONCE, AFFECT, IF_THEN, TOWN, RECT, OUTDOOR
};

struct node_properties_t {
	eSpecType self;
	eSpecCat cat;
	std::string opcode() const;
	std::string name() const, descr() const;
	std::string sdf1_lbl() const, sdf2_lbl() const, sdf1_hlp() const, sdf2_hlp() const;
	std::string msg1_lbl() const, msg2_lbl() const, msg3_lbl() const, msg1_hlp() const, msg2_hlp() const, msg3_hlp() const;
	std::string pic_lbl() const, pt_lbl() const, pic_hlp() const, pt_hlp() const;
	std::string ex1a_lbl() const, ex1b_lbl() const, ex1c_lbl() const, ex1a_hlp() const, ex1b_hlp() const, ex1c_hlp() const;
	std::string ex2a_lbl() const, ex2b_lbl() const, ex2c_lbl() const, ex2a_hlp() const, ex2b_hlp() const, ex2c_hlp() const;
	std::string jmp_lbl() const, jmp_hlp() const;
	char sd1_btn, sd2_btn, m1_btn, m2_btn, m3_btn, p_btn, pt_btn;
	char x1a_btn, x1b_btn, x1c_btn, x2a_btn, x2b_btn, x2c_btn;
	node_properties_t() : self(eSpecType::INVALID), cat(eSpecCat::INVALID) {}
	node_properties_t(std::initializer_list<std::function<void(node_properties_t)>>);
};

const node_properties_t& operator* (eSpecType t);

#endif
