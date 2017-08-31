
#include <string>
#include "pict.hpp" // for ePicType
#include "location.hpp" // for info_rect_t
class cDialog;

enum eStrType {
	STRT_MONST, STRT_ITEM, STRT_TER, STRT_BUTTON,
	STRT_SPEC_ITEM, STRT_MAGE, STRT_PRIEST, STRT_ALCHEMY,
	STRT_TOWN, STRT_SECTOR, STRT_SKILL, STRT_TRAIT, STRT_RACE,
	STRT_PICT, STRT_CMP, STRT_ACCUM, STRT_TRAP,
	STRT_ATTITUDE, STRT_STAIR, STRT_LIGHT, STRT_CONTEXT,
	STRT_SHOP, STRT_COST_ADJ, STRT_STAIR_MODE, STRT_TALK_NODE,
	STRT_STATUS, STRT_SPELL_PAT, STRT_SUMMON, STRT_TALK,
	STRT_ENCHANT, STRT_DIR, STRT_QUEST, STRT_QUEST_STATUS,
	STRT_HEALING, STRT_TREASURE, STRT_MONST_STAT, STRT_POS_MODE,
};

bool cre(short val,short min,short max,std::string text1,std::string text2,cDialog* parent) ;
void display_strings(char *text1, char *text2,
	char *title,short sound_num,short graphic_num,short graphic_type,cDialog* parent);
void put_choice_pics(short g_type);
pic_num_t choose_graphic(short cur_choice,ePicType g_type,cDialog* parent);
short choose_background(short cur_choice, cDialog* parent);
short choose_text_res(std::string res_list,short first_t,short last_t,unsigned short cur_choice,cDialog* parent,const char *title);
short choose_text(eStrType list, unsigned short cur_choice, cDialog* parent,std::string title);
bool edit_text_str(short which_str,eStrMode mode);
bool edit_spec_enc(short which_node,short mode,cDialog* parent);
short get_fresh_spec(short which_mode);
void edit_spec_text(eStrMode mode,short *str1,short *str2,cDialog* parent);
void edit_dialog_text(eStrMode mode,short *str1,cDialog* parent);
short edit_special_num(short mode,short what_start);
void edit_scen_intro();
bool edit_area_rect_str(info_rect_t& r);
size_t num_strs(eStrMode str_mode);

pic_num_t choose_damage_type(short cur, cDialog* parent, bool allow_spec);
short choose_field_type(short cur, cDialog* parent, bool includeSpec);
pic_num_t choose_status_effect(short cur, bool party, cDialog* parent);
snd_num_t choose_sound(short cur, cDialog* parent, std::string title = "Which sound?");
