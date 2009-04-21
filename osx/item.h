/*
 *  item.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include "location.h"

class cItemRec {
public:
	short variety, item_level;
	char awkward, bonus, protection, charges, type, magic_use_type;
	unsigned char graphic_num,ability, ability_strength,type_flag, is_special;
	short value;
	unsigned char weight, special_class;
	location item_loc;
	char full_name[25], name[15];
	unsigned char treas_class,item_properties;
private:
	unsigned char reserved1;
	unsigned char reserved2;
public:
	//string desc; // for future use
	unsigned char rec_treas_class() const;
	bool is_ident() const;
	bool is_property() const;
	bool is_magic() const;
	bool is_contained() const;
	bool is_cursed() const;
	bool is_concealed() const;
	void set_ident(bool b);
	void set_property(bool b);
	void set_magic(bool b);
	void set_contained(bool b);
	void set_cursed(bool b);
	void set_concealed(bool b);
	short item_weight() const;
	
	cItemRec();
	cItemRec(long preset);
};

/*
 typedef struct {
 short variety, item_level;
 char awkward, bonus, protection, charges, type;
 unsigned char graphic_num,ability, type_flag, is_special;
 short value;
 Boolean identified, magic;
 unsigned char weight, description_flag;
 char full_name[25], name[15];
 unsigned char reserved1,reserved2;
 unsigned char magic_use_type, ability_strength, treas_class, real_abil;
 } short_item_record_type;
 */