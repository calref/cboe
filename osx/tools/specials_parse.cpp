//
//  specials_parse.cpp
//  BoE
//
//  Created by Celtic Minstrel on 14-12-02.
//
//

#include <boost/spirit/include/qi.hpp>

#include "special.h"

using namespace boost::spirit::qi;

std::string temp_symbol;
int cur_node;

symbols<char, eSpecNodeType> opcode;
symbols<char, int> defn;

void init() {cur_node = 0; temp_symbol.clear();}
void prep_add_symbol(char c) {temp_symbol += c;}
void add_symbol(int i) {defn.add(temp_symbol, i); temp_symbol.clear();}
void skip_to(int i) {cur_node = i;}

auto ws = char_(" \t");
auto comment = char_('#') >> *(print | char_('\t'));
auto symbol = char_("A-Za-z$_-")[prep_add_symbol] >> *char_("A-Za-z0-9$_-")[prep_add_symbol];
auto val = int_ | defn;

auto datcode = lit("sdf") | lit("pic") | lit("msg") | lit("ex1") | lit("ex2") | lit("goto");

auto command = datcode >> ws >> *ws >> val >> (eps | *ws >> char_(',') >> *ws >> val >> (eps | *ws >> char_(',') >> *ws >> val));

auto def_line = *ws >> lit("def") >> ws >> *ws >> symbol >> *ws >> char_('=') >> *ws >> uint_[add_symbol] >> (comment | eps) >> eol;
auto cmd_line = *ws >> (command | eps) >> *ws >> (comment | eps) >> eol;
auto op_line = *ws >> char_('@') >> opcode >> (eps | *ws >> char_('=') >> *ws >> val[skip_to]) >> (comment | eps) >> eol;

auto command_block = op_line >> *(cmd_line | def_line);

auto nodes_file = eps[init] >> *def_line >> *command_block;

struct initer {
	initer() {
		opcode.add
			("null", SPEC_NULL)
			("set-sdf", SPEC_SET_SDF)
			("inc-sdf", SPEC_INC_SDF)
			("disp-msg", SPEC_DISPLAY_MSG)
			("secret-pass", SPEC_SECRET_PASSAGE)
			("disp-sm-msg", SPEC_DISPLAY_SM_MSG)
			("flip-sdf", SPEC_FLIP_SDF)
			("filter-context", SPEC_OUT_BLOCK)
			("block-move", SPEC_CANT_ENTER)
			("change-time", SPEC_CHANGE_TIME)
			("start-timer-scen", SPEC_SCEN_TIMER_START)
			("play-sound", SPEC_PLAY_SOUND)
			("change-horse", SPEC_CHANGE_HORSE_OWNER)
			("change-boat", SPEC_CHANGE_BOAT_OWNER)
			("town-visible", SPEC_SET_TOWN_VISIBILITY)
			("set-event", SPEC_MAJOR_EVENT_OCCURRED)
			("force-give", SPEC_FORCED_GIVE)
			("buy-item-class", SPEC_BUY_ITEMS_OF_TYPE)
			("call-global", SPEC_CALL_GLOBAL)
			("set-sdf-row", SPEC_SET_SDF_ROW)
			("copy-sdf", SPEC_COPY_SDF)
			("rest", SPEC_REST)
			("set-wander-fight", SPEC_WANDERING_WILL_FIGHT)
			("end-scen", SPEC_END_SCENARIO)
			("once-give-item", SPEC_ONCE_GIVE_ITEM)
			("once-give-spec-item", SPEC_ONCE_GIVE_SPEC_ITEM)
			("once", SPEC_ONCE_NULL)
			("once-set-sdf", SPEC_ONCE_SET_SDF)
			("once-disp-msg", SPEC_ONCE_DISPLAY_MSG)
			("once-dlog", SPEC_ONCE_DIALOG)
			("once-give-dlog", SPEC_ONCE_GIVE_ITEM_DIALOG)
			("once-encounter", SPEC_ONCE_OUT_ENCOUNTER)
			("once-trap", SPEC_ONCE_TRAP)
			("select-pc", SPEC_SELECT_PC)
			("damage", SPEC_DAMAGE)
			("hp", SPEC_AFFECT_HP)
			("sp", SPEC_AFFECT_SP)
			("xp", SPEC_AFFECT_XP)
			("skill-pts", SPEC_AFFECT_SKILL_PTS)
			("death", SPEC_AFFECT_DEADNESS)
			("status", SPEC_AFFECT_POISON)
			("statistic", SPEC_AFFECT_STAT)
			("spell", SPEC_AFFECT_MAGE_SPELL)
			("gold", SPEC_AFFECT_GOLD)
			("food", SPEC_AFFECT_FOOD)
			("alchemy", SPEC_AFFECT_ALCHEMY)
			("stealth", SPEC_AFFECT_STEALTH)
			("firewalk", SPEC_AFFECT_FIREWALK)
			("flight", SPEC_AFFECT_FLIGHT)
			("if-sdf", SPEC_IF_SDF)
			("if-town", SPEC_IF_TOWN_NUM)
			("if-spec-item", SPEC_IF_HAVE_SPECIAL_ITEM)
			("if-sdf-compare", SPEC_IF_SDF_COMPARE)
			("if-ter", SPEC_IF_TOWN_TER_TYPE)
			("if-gold", SPEC_IF_HAS_GOLD_AND_TAKE)
			("if-food", SPEC_IF_HAS_FOOD_AND_TAKE)
			("if-item-class-on-space", SPEC_IF_ITEM_CLASS_ON_SPACE_AND_TAKE)
			("if-item-class", SPEC_IF_HAVE_ITEM_CLASS_AND_TAKE)
			("if-item-class-equip", SPEC_IF_EQUIP_ITEM_CLASS_AND_TAKE)
			("if-day", SPEC_IF_DAY_REACHED)
			("if-field", SPEC_IF_BARRELS)
			("if-object", SPEC_IF_CRATES)
			("if-event", SPEC_IF_EVENT_OCCURRED)
			("if-cave-lore", SPEC_IF_HAS_CAVE_LORE)
			("if-woodsman", SPEC_IF_HAS_WOODSMAN)
			("if-mage-lore", SPEC_IF_ENOUGH_MAGE_LORE)
			("if-response", SPEC_IF_TEXT_RESPONSE)
			("if-sdf-eq", SPEC_IF_SDF_EQ)
			("town-attitude", SPEC_MAKE_TOWN_HOSTILE)
			("change-ter", SPEC_TOWN_CHANGE_TER)
			("swap-ter", SPEC_TOWN_SWAP_TER)
			("trans-ter", SPEC_TOWN_TRANS_TER)
			("move-party", SPEC_TOWN_MOVE_PARTY)
			("hit-space", SPEC_TOWN_HIT_SPACE)
			("explode-space", SPEC_TOWN_EXPLODE_SPACE)
			("lock-space", SPEC_TOWN_LOCK_SPACE)
			("unlock-space", SPEC_TOWN_UNLOCK_SPACE)
			("anim-explode", SPEC_TOWN_SFX_BURST)
			("make-wandering", SPEC_TOWN_CREATE_WANDERING)
			("place-monst", SPEC_TOWN_PLACE_MONST)
			("destroy-most", SPEC_TOWN_DESTROY_MONST)
			("nuke-monsts", SPEC_TOWN_NUKE_MONSTS)
			("lever-generic", SPEC_TOWN_GENERIC_LEVER)
			("portal-generic", SPEC_TOWN_GENERIC_PORTAL)
			("stair-generic", SPEC_TOWN_GENERIC_STAIR)
			("button-generic", SPEC_TOWN_GENERIC_BUTTON)
			("lever", SPEC_TOWN_LEVER)
			("portal", SPEC_TOWN_PORTAL)
			("stair", SPEC_TOWN_STAIR)
			("set-sector", SPEC_TOWN_RELOCATE)
			("place-item", SPEC_TOWN_PLACE_ITEM)
			("split-party", SPEC_TOWN_SPLIT_PARTY)
			("unite-party", SPEC_TOWN_REUNITE_PARTY)
			("start-timer-town", SPEC_TOWN_TIMER_START)
			("rect-place-field", SPEC_RECT_PLACE_BLADE)
			("rect-cleanse", SPEC_RECT_CLEANSE)
			("rect-place-sfx", SPEC_RECT_PLACE_SFX)
			("rect-place-object", SPEC_RECT_PLACE_OBJECT)
			("rect-move-items", SPEC_RECT_MOVE_ITEMS)
			("rect-destroy-items", SPEC_RECT_DESTROY_ITEMS)
			("rect-change-ter", SPEC_RECT_CHANGE_TER)
			("rect-swap-ter", SPEC_RECT_SWAP_TER)
			("rect-trans-ter", SPEC_RECT_TRANS_TER)
			("rect-lock", SPEC_RECT_LOCK)
			("rect-unlock", SPEC_RECT_UNLOCK)
			("make-out-monst", SPEC_OUT_PLACE_ENCOUNTER)
			("start-shop", SPEC_OUT_STORE)
		;
	}
};

static struct initer initer;

