
#include <string>
#include "dialogxml/widgets/pict.hpp" // for ePicType
#include "location.hpp" // for info_rect_t
class cDialog;

bool cre(short val,short min,short max,std::string text1,std::string text2,cDialog* parent) ;
void display_strings(char *text1, char *text2,
	char *title,short sound_num,short graphic_num,short graphic_type,cDialog* parent);
void put_choice_pics(short g_type);
pic_num_t choose_graphic(short cur_choice,ePicType g_type,cDialog* parent, bool static_only = false);
short choose_background(short cur_choice, cDialog* parent);
short choose_text_res(std::string res_list,short first_t,short last_t,unsigned short cur_choice,cDialog* parent,const char *title);
short choose_text(eStrType list, unsigned short cur_choice, cDialog* parent,std::string title);
short choose_text_editable(std::vector<std::string>& list, short cur_choice, cDialog* parent, std::string title);
short choose_pattern(short cur_choice, cDialog* parent, bool expandRotatable);
bool edit_text_str(short which_str,eStrMode mode,bool& is_new,bool loop = true,short min_str = 0,short max_str = -1);
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
snd_num_t choose_sound(short cur, cDialog* parent, std::string title = "Which sound?");
