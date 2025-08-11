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
#include "dialogxml/widgets/pictypes.hpp"

namespace legacy { struct special_node_type; };
class cScenario;

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

	bool operator==(const cSpecial& other) const {
		CHECK_EQ(other, type);
		CHECK_EQ(other, sd1);
		CHECK_EQ(other, sd2);
		CHECK_EQ(other, pic);
		CHECK_EQ(other, pictype);
		CHECK_EQ(other, m1);
		CHECK_EQ(other, m2);
		CHECK_EQ(other, m3);
		CHECK_EQ(other, ex1a);
		CHECK_EQ(other, ex1b);
		CHECK_EQ(other, ex1c);
		CHECK_EQ(other, ex2a);
		CHECK_EQ(other, ex2b);
		CHECK_EQ(other, ex2c);
		CHECK_EQ(other, jumpto);
		return true;
	}
	bool operator!=(const cSpecial& other) const { return !(*this == other); }
	std::string editor_hint(const cScenario& scenario) const;
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

struct node_category_info_t {
	eSpecType first = eSpecType::NONE, last = eSpecType::NONE;
	node_category_info_t() = default;
	node_category_info_t(eSpecType a, eSpecType b) : first(a), last(b) {}
	bool contains(eSpecType spec) const;
};

enum eStrType {
	STRT_MONST, STRT_ITEM, STRT_TER, STRT_BUTTON,
	STRT_SPEC_ITEM, STRT_MAGE, STRT_PRIEST, STRT_ALCHEMY,
	STRT_TOWN, STRT_SECTOR, STRT_SKILL, STRT_SKILL_CHECK,
	STRT_TRAIT, STRT_RACE, STRT_ANY_SPELL,
	STRT_PICT, STRT_CMP, STRT_ACCUM, STRT_TRAP,
	STRT_ATTITUDE, STRT_STAIR, STRT_LIGHT, STRT_CONTEXT,
	STRT_SHOP, STRT_COST_ADJ, STRT_STAIR_MODE, STRT_TALK_NODE,
	STRT_STATUS, STRT_SUMMON, STRT_TALK, STRT_TALK_NODE_PERSON,
	STRT_ENCHANT, STRT_DIR, STRT_QUEST, STRT_QUEST_STATUS,
	STRT_HEALING, STRT_TREASURE, STRT_MONST_STAT, STRT_POS_MODE,
	STRT_DEBUG_PRINT, STRT_TARG_TYPE, STRT_TARG_MODE,
	STRT_ID_MODE, STRT_CURSE_MODE, STRT_EQUIP_MODE,
	STRT_CMP_MODE, STRT_PATH, STRT_SPELL_PAT_MODE,
	STRT_LABEL_ALIGN, STRT_HORSE, STRT_BOAT,
};

enum class eSpecPicker {
	NONE,
	MSG_SINGLE, MSG_PAIR, MSG_SEQUENCE, MSG_SEQUENCE_VAR,
	PICTURE, NODE, STRING, SOUND,
	FIELD, DAMAGE_TYPE, EXPLOSION,
	STATUS, STATUS_PARTY,
	SDF, LOCATION, RECTANGLE, TOGGLE,
	EVENT, ITEM_CLASS, QUEST, JOB_BOARD,
	POINTER, SPELL_PATTERN,
};

enum class eLocType {
	ACTIVE_AUTO, ACTIVE_TOWN, ACTIVE_OUT,
	SPECIFIED_TOWN, SPECIFIED_OUT
};

enum class eSpecField { NONE, SDF1, SDF2, MSG1, MSG2, MSG3, PICT, PTYP, EX1A, EX1B, EX1C, EX2A, EX2B, EX2C, JUMP };
short operator->*(const cSpecial& spec, eSpecField fld);

struct node_function_t {
	eSpecPicker button = eSpecPicker::NONE;
	union {
		eStrType str_type; // for eSpecPicker::STRING only
		ePicType pic_type; // for eSpecPicker::PICTURE only; PIC_NONE = use pictype field from node
		eLocType loc_type; // for eSpecPicker::LOCATION only
		bool force_global; // for eSpecPicker::NODE and eSpecPicker::MSG_*
		// other pickers don't put anything in here
	};
	bool augmented = false; // only for eSpecPicker::FIELD and eSpecPicker::SPELL_PATTERN
	int adjust = 0; // only for eSpecPicker::STRING
	eSpecField continuation = eSpecField::NONE;
	std::string label() const;
	std::string help() const; // maybe we don't need this though? I guess it would be for a hypothetical help button next to each field to give addition info on how that one field works.
	node_function_t();
	node_function_t(eSpecPicker button);
	node_function_t(eStrType str);
	node_function_t(ePicType pic);
	node_function_t(eLocType loc);
private:
	eSpecType self = eSpecType::NONE;
	std::string lbl;
	int lbl_idx = 0, sub_idx = 0;
	bool needs_split = false;
	friend struct node_builder_t;
	friend struct node_properties_t;
	friend struct node_condition_builder_t;
};

node_function_t operator+(eSpecPicker);

using node_condition_t = std::function<bool(const cSpecial&)>;

struct node_properties_t {
	eSpecType self;
	eSpecCat cat;
	std::string opcode() const;
	std::string name() const, descr() const;
	node_function_t sdf1(const cSpecial&) const, sdf2(const cSpecial&) const, jump(const cSpecial&) const;
	node_function_t msg1(const cSpecial&) const, msg2(const cSpecial&) const, msg3(const cSpecial&) const;
	node_function_t pic(const cSpecial&) const, pictype(const cSpecial&) const;
	node_function_t ex1a(const cSpecial&) const, ex1b(const cSpecial&) const, ex1c(const cSpecial&) const;
	node_function_t ex2a(const cSpecial&) const, ex2b(const cSpecial&) const, ex2c(const cSpecial&) const;
	node_properties_t() : node_properties_t(eSpecType::INVALID) {}
	bool can_preview;
private:
	node_properties_t(eSpecType type);
	node_function_t get(const cSpecial& spec, eSpecField fld) const;
	void set(eSpecField fld, node_function_t fcn);
	std::map<eSpecField, node_function_t> fields;
	std::vector<std::pair<node_condition_t, node_properties_t>> conditions;
	friend struct node_builder_t;
	friend struct node_condition_builder_t;
	friend struct field_map;
};

const node_properties_t& operator* (eSpecType t);
const node_category_info_t& operator* (eSpecCat t);

struct node_condition_builder_t;

// Builds the information needed to display the correct buttons when editing a special node.
struct node_builder_t {
	node_builder_t(eSpecType type) : node(type) {}
	// Specifies that a particular field should use a specified picker.
	node_builder_t& field(eSpecField field, node_function_t picker);
	// Specifies that a particular field should use a specified picker which fills two fields as a pair.
	node_builder_t& field_pair(eSpecField main, eSpecField extra, node_function_t picker);
	// Quick overloads for each possible field.
	node_builder_t& sdf1(node_function_t picker);
	node_builder_t& sdf2(node_function_t picker);
	node_builder_t& jump(node_function_t picker);
	node_builder_t& msg1(node_function_t picker);
	node_builder_t& msg2(node_function_t picker);
	node_builder_t& msg3(node_function_t picker);
	node_builder_t& pict(node_function_t picker);
	node_builder_t& ptyp(node_function_t picker);
	node_builder_t& ex1a(node_function_t picker);
	node_builder_t& ex1b(node_function_t picker);
	node_builder_t& ex1c(node_function_t picker);
	node_builder_t& ex2a(node_function_t picker);
	node_builder_t& ex2b(node_function_t picker);
	node_builder_t& ex2c(node_function_t picker);
	// Specifies that sdf1 and sdf2 combine to define an SDF
	node_builder_t& sdf();
	// Specifies that msg1 and msg2 specify a pair of local strings
	node_builder_t& msg();
	// Specifies that pict and pictype have their typical meaning of defining a picture
	node_builder_t& pic();
	// Specifies that ex1a,ex1b and ex2a,ex2b define opposing corners of a rectangle.
	// DO NOT use for other cases where there are two points, for example defining a path!
	// Also DO NOT use for cases where an outdoor sector is identified by its coordinates.
	node_builder_t& rect(eLocType type);
	// Specifies that the indicated two fields combine to define an SDF.
	node_builder_t& sdf(eSpecField a, eSpecField b);
	// Specifies that the indicated two fields combine to define a location.
	node_builder_t& loc(eSpecField a, eSpecField b, eLocType type);
	// As above, but also notes that the area the location is in will be specified by the indicated field.
	node_builder_t& loc(eSpecField a, eSpecField b, eLocType type, eSpecField where);
	// Specifies that the node does not display a dialog, or its dialog cannot be previewed
	node_builder_t& no_preview();
	node_condition_builder_t when(node_condition_t cond, int lbl_sub);
	operator node_properties_t();
private:
	node_properties_t node;
	friend struct node_condition_builder_t;
};

struct node_condition_builder_t {
	node_builder_t& end();
	node_condition_builder_t& field(eSpecField field, node_function_t picker = eSpecPicker::NONE);
	node_condition_builder_t& field_pair(eSpecField main, eSpecField extra, node_function_t picker = eSpecPicker::NONE);
	node_condition_builder_t& sdf1(node_function_t picker = eSpecPicker::NONE);
	node_condition_builder_t& sdf2(node_function_t picker = eSpecPicker::NONE);
	node_condition_builder_t& jump(node_function_t picker = eSpecPicker::NONE);
	node_condition_builder_t& msg1(node_function_t picker = eSpecPicker::NONE);
	node_condition_builder_t& msg2(node_function_t picker = eSpecPicker::NONE);
	node_condition_builder_t& msg3(node_function_t picker = eSpecPicker::NONE);
	node_condition_builder_t& pict(node_function_t picker = eSpecPicker::NONE);
	node_condition_builder_t& ptyp(node_function_t picker = eSpecPicker::NONE);
	node_condition_builder_t& ex1a(node_function_t picker = eSpecPicker::NONE);
	node_condition_builder_t& ex1b(node_function_t picker = eSpecPicker::NONE);
	node_condition_builder_t& ex1c(node_function_t picker = eSpecPicker::NONE);
	node_condition_builder_t& ex2a(node_function_t picker = eSpecPicker::NONE);
	node_condition_builder_t& ex2b(node_function_t picker = eSpecPicker::NONE);
	node_condition_builder_t& ex2c(node_function_t picker = eSpecPicker::NONE);
	node_condition_builder_t& sdf();
	node_condition_builder_t& msg();
	node_condition_builder_t& pic();
	node_condition_builder_t& rect(eLocType type);
	node_condition_builder_t& sdf(eSpecField a, eSpecField b);
	node_condition_builder_t& loc(eSpecField a, eSpecField b, eLocType type);
	node_condition_builder_t& loc(eSpecField a, eSpecField b, eLocType type, eSpecField where);
private:
	node_condition_builder_t(node_builder_t& parent, node_condition_t cond, int sub);
	node_condition_t cond;
	node_builder_t self;
	node_builder_t& parent;
	int sub;
	friend class node_builder_t;
};

// An overview of how the builder works.
// Most of the time, the node_function_t will be constructed implicitly using
// either one of its implicit constructors or one of the overloaded unary + operators.

// An eStrType implies a string picker, ie eSpecPicker::STRING.
// However, passing eSpecPicker::STRING directly doesn't do anything useful.

// An ePicType implies a graphic picker, ie eSpecPicker::PICTURE.
// Passing eSpecPicker::PICTURE directly means that another field (usually pictype) will determine the type.

// The prefix + has a useful meaning only in a few special cases:
// +eSpecPicker::NODE means always edit a scenario node, even if the caller is a town or outdoor node.
// +eSpecPicker::MSG_* is similar – always edit a scenario string, never a town or outdoor string.
// +eSpecPicker::FIELD adds the pseudo-fields Move Mountains and Dispel to the list.
// +eSpecPicker::SPELL_PATTERN expands the "rotateable wall" into all its possible orientations
// More cases may be added.

#endif
