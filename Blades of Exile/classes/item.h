#ifndef _ITEM_H
	#define _ITEM_H
	
#include "location.h"
	
/*  item_properties: */
/*
	0000 0001	identified
	0000 0010	someone's property
	0000 0100	magic
	0000 1000	contained in sth
	0001 0000	cursed
	0010 0000	concealed
	0100 0000	<not used>
	1000 0000	<not used>
*/

#define ITEM_PROP_IDENTIFIED 1
#define ITEM_PROP_PROPERTY 2
#define ITEM_PROP_MAGIC 4
#define ITEM_PROP_CONTAINED 8
#define ITEM_PROP_CURSED 16
#define ITEM_PROP_CONCEALED 32
	
struct item_record_type {
	short variety, item_level;
	char awkward, bonus, protection, charges, type, magic_use_type;
	unsigned char graphic_num,ability, ability_strength, type_flag, is_special,a;
	short value;
	unsigned char weight, special_class;
	location item_loc;
	char full_name[25], name[15];
	unsigned char treas_class, item_properties, reserved1, reserved2;
	
	/* functions */
	bool isIdent() const;		/* is identified ? */
	bool isMagic() const;		/* is magic ? */
	bool isContained() const;	/* is contained in sth ? */
	bool isCursed() const;		/* is cursed ? */
	bool isProperty() const;	/* is someones property? */
	bool isConcealed() const;   /* is the item ability concealed ? */
};
	
#endif
