
#ifndef BOE_GAME_INFODLG_H
#define BOE_GAME_INFODLG_H

#include "item.hpp"
#include "monster.hpp"
#include "pc.hpp"
#include "creature.hpp"

class cDialog;
void display_spells(eSkill mode,short force_spell,cDialog* parent);
void display_skills(eSkill force_skill,cDialog* parent);
void display_pc_item(short pc_num,short item,class cItem si,cDialog* parent);
void display_monst(short array_pos,cCreature *which_m,short mode);
void display_alchemy();
void display_traits_graphics();
void give_pc_info(short pc_num);
void adventure_notes();
void put_talk();
void talk_notes();
void journal();
void add_to_journal(short event);
void give_help(short help1,short help2,class cDialog& parent_num);
void give_help(short help1,short help2);
void put_spec_item_info (short which_i);
void put_quest_info(short which_i);

// These are defined in pc.editors.cpp since they are also used by the character editor
void pick_race_abil(cPlayer *pc,short mode,cDialog* parent = nullptr);
void display_pc(short pc_num,short mode,cDialog* parent_num);
void display_alchemy(bool allowEdit,cDialog* parent);

// Callback for recording encounter strings
class cStringRecorder {
private:
	eEncNoteType type;
	unsigned short label1, label2, label1b, label2b;
	std::string location;
public:
	cStringRecorder(eEncNoteType type) : type(type) {}
	cStringRecorder& string1(unsigned short which) {
		label1 = which;
		return *this;
	}
	cStringRecorder& string2(unsigned short which) {
		label2 = which;
		return *this;
	}
	cStringRecorder& from(unsigned short where1, unsigned short where2) {
		label1b = where1;
		label2b = where2;
		return *this;
	}
	cStringRecorder& at(std::string where) {
		location = where;
		return *this;
	}
	void operator()(cDialog&);
};

#endif
