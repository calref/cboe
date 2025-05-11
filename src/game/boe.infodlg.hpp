
#ifndef BOE_GAME_INFODLG_H
#define BOE_GAME_INFODLG_H

#include "scenario/item.hpp"
#include "scenario/monster.hpp"
#include "universe/pc.hpp"
#include "universe/creature.hpp"

class cDialog;
void display_spells(eSkill mode,short force_spell,cDialog* parent, bool record=false);
void display_skills(eSkill force_skill,cDialog* parent, bool record=false);
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
void put_spec_item_info (short which_i);
void put_quest_info(short which_i);

// These are defined in pc.editors.cpp since they are also used by the character editor
void pick_race_abil(cPlayer *pc,short mode,cDialog* parent = nullptr);
void display_pc(short pc_num,short mode,cDialog* parent_num);
void display_alchemy(bool allowEdit,cDialog* parent);

// Callback for recording encounter strings
class cStringRecorder {
private:
	eSpecCtxType spec_type;
	eEncNoteType note_type;
	short label1, label2;
	std::string location;
public:
	cStringRecorder(eSpecCtxType spec_type, eEncNoteType note_type) : spec_type(spec_type), note_type(note_type) {}
	cStringRecorder& string1(short which) {
		label1 = which;
		return *this;
	}
	cStringRecorder& string2(short which) {
		label2 = which;
		return *this;
	}
	cStringRecorder& at(std::string where) {
		location = where;
		return *this;
	}
	void operator()(cDialog&);
};

#endif
