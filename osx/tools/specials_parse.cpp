//
//  specials_parse.cpp
//  BoE
//
//  Created by Celtic Minstrel on 14-12-02.
//
//

#include <boost/spirit/include/qi.hpp>

using namespace boost::spirit::qi;

std::string temp_symbol;
symbols<char, int> defn;
void prep_add_symbol(char c) {temp_symbol += c;}
void add_symbol(int i) {defn.add(temp_symbol, i); temp_symbol.clear();}

auto ws = char_(" \t");
auto comment = char_('#') >> *(print | char_('\t'));
auto symbol = char_("A-Za-z$_-")[prep_add_symbol] >> *char_("A-Za-z0-9$_-")[prep_add_symbol];
auto val = int_ | defn;

auto datcode = lit("sdf") | lit("pic") | lit("msg") | lit("ex1") | lit("ex2") | lit("goto");
auto opcode = lit("null") | lit("set-sdf") | lit("inc-sdf") | lit("disp-msg") | lit("secret-pass") | lit("disp-sm-msg") | lit("flip-sdf") |
	lit("filter-context") | lit("block-move") | lit("change-time") | lit("start-timer") | lit("play-sound") | lit("change_horse") |
	lit("change_boat") | lit("town-visible") | lit("set-event") | lit("force-give") | lit("buy-item-class") | lit("call-global") |
	lit("set-sdf-row") | lit("copy-sdf") | lit("rest") | lit("set-wander-fight") | lit("end-scen") | lit("once-give-item") |
	lit("once-give-spec-item") | lit("once") | lit("once-set-sdf") | lit("once-disp-msg") | lit("once-dlog") | lit("once-dlog-ter") |
	lit("once-dlog-monst") | lit("once-give-dlog") | lit("once-give-ter") | lit("once-give-monst") | lit("once-encounter") |
	lit("once-trap") | lit("select-pc") | lit("damage") | lit("hp") | lit("sp") | lit("xp") | lit("skill-pts") | lit("death") |
	lit("status") | lit("statistic") | lit("spell") | lit("gold") | lit("food") | lit("alchemy") | lit("stealth") | lit("firewalk") |
	lit("flight") | lit("if-sdf") | lit("if-town") | lit("if-spec-item") | lit("if-sdf-compare") | lit("if-ter") | lit("if-gold") |
	lit("if-food") | lit("if-item-class-on-space") | lit("if-item-class") | lit("if-item-class-equip") | lit("if-day") | lit("if-field") |
	lit("if-object") | lit("if-event") | lit("if-cave-lore") | lit("if-woodsman") | lit("if-mage-lore") | lit("if-response") | lit("if-sdf-eq") |
	lit("town-attitude") | lit("change-ter") | lit("swap-ter") | lit("trans-ter") | lit("move-party") | lit("hit-space") |
	lit("explode-space") | lit("lock-space") | lit("unlock-space") | lit("sfx-burst") | lit("make-wandering") | lit("place-monst") |
	lit("destroy-monst") | lit("nuke-monsts") | lit("lever-generic") | lit("portal-generic") | lit("stair-generic") | lit("button-generic") |
	lit("lever") | lit("portal") | lit("stair") | lit("set-sector") | lit("place-item") | lit("split-party") | lit("unite-party") |
	lit("start-timer") | lit("rect-place-field") | lit("rect-cleanse") | lit("rect-place-sfx") | lit("rect-place-object") |
	lit("rect-move-items") | lit("rect-destroy-items") | lit("rect-change-ter") | lit("rect-swap-ter") | lit("rect-trans-ter") |
	lit("rect-lock") | lit("rect-unlock") | lit("make-out-monst") | lit("start-shop");

auto command = datcode >> ws >> *ws >> val >> (eps | *ws >> char_(',') >> *ws >> val >> (eps | *ws >> char_(',') >> *ws >> val));

auto def_line = *ws >> lit("def") >> ws >> *ws >> symbol >> *ws >> char_('=') >> *ws >> uint_[add_symbol] >> (comment | eps) >> eol;
auto cmd_line = *ws >> (command | eps) >> *ws >> (comment | eps) >> eol;
auto op_line = *ws >> char_('@') >> opcode >> (eps | *ws >> char_('=') >> *ws >> val) >> (comment | eps) >> eol;

auto command_block = op_line >> *(cmd_line | def_line);

auto nodes_file = *def_line >> *command_block;
