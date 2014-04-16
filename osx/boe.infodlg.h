
#include "simpletypes.h"

class cDialog;
void display_spells(short mode,short force_spell,cDialog* parent);
void display_skills(short force_skill,cDialog* parent);
void display_pc_item(short pc_num,short item,cItemRec si,cDialog* parent);
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
//void display_strings_event_filter (short item_hit);
//void display_strings(char *text1, char *text2,short str_label_1,short str_label_2,short str_label_1b,	short str_label_2b, char *title,short sound_num,short graphic_num,short graphic_type,short parent_num);
//void give_error(const char *text1, const char *text2,short parent_num);
//void display_strings_with_nums(short a1,short a2, short b1, short b2,	char *title,short sound_num,short graphic_num,short graphic_type,short parent_num);

// These are defined in pc.editors.cpp since they are also used by the character editor
void pick_race_abil(cPlayer *pc,short mode,cDialog* parent);
void display_pc(short pc_num,short mode,cDialog* parent_num);
void display_alchemy(bool allowEdit);

// Callback for recording encounter strings
class cStringRecorder {
private:
	eEncNoteType str1type, str2type;
	unsigned short label1, label2, label1b, label2b;
public:
	cStringRecorder() {}
	cStringRecorder& string1(eEncNoteType type, unsigned short which, unsigned short where) {
		str1type = type;
		label1 = which;
		label1b = where;
		return *this;
	}
	cStringRecorder& string2(eEncNoteType type, unsigned short which, unsigned short where) {
		str2type = type;
		label2 = which;
		label2b = where;
		return *this;
	}
	void operator()(cDialog&);
};
