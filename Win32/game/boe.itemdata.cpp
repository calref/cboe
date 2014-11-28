
#include <windows.h>

#include "global.h"
#include "boe.itemdata.h"
#include <cstdio>
#include <cstring>
#include "tools/mathutil.h"

#include "globvar.h"

short loot_min[5] = {0,0,5,50,400};
short loot_max[5] = {3,8,40,800,4000};
//// whole file

item_record_type	return_dummy_item()
{
	item_record_type	dummy_item = {0,0, 0,0,0,0,0,0, 0,0,0,0,0,0, 0, 0,0, location(),"", "",0,0,0,0};

	return dummy_item;
}

item_record_type get_stored_item(short which)
{
	item_record_type s_item;

	if ((which >= 400) || (which < 0)) {
		s_item = return_dummy_item();
		return s_item;
		}

	s_item = scen_item_list->scen_items[which];
	return s_item;
}

item_record_type get_food()
{
	item_record_type food =
		{ITEM_TYPE_FOOD,0, 0,0,0,0,0,0, 62,0,0,0,0,0, 0, 0,0, location(),"Food", "Food",0,0,0,0};
	food.graphic_num += get_ran(1,0,2);
	food.item_level = get_ran(1,5,10);
	if (get_ran(1,0,9) == 5)
		food.graphic_num = 113;
	if (get_ran(1,0,9) == 5)
		food.graphic_num = 114;
	// food doesn't always appear
	if (get_ran(1,0,2) != 1)
		food.variety = ITEM_TYPE_NO_ITEM;

	return food;
}


item_record_type pull_item_of_type(short loot_max,short min_val,short max_val,short t1, short t2, short t3)
{
	short i,j,val;
	item_record_type temp_i;

	// occasionally get nice item
	if (get_ran(1,0,160) == 80) {
		loot_max += 2;
		max_val += 2000;
		}
	for (i = 0; i < 80; i++) {
		j = get_ran(1,0,399);
		temp_i = get_stored_item(j);
		if ((temp_i.variety == t1) || (temp_i.variety == t2) || (temp_i.variety == t3)) {
			val = (temp_i.charges > 0) ? temp_i.charges * temp_i.value : temp_i.value;
			if ((val >= min_val) && (val <= max_val) && (temp_i.treas_class != 0) &&
				(temp_i.treas_class <= loot_max))
				return temp_i;
			}
		}
	temp_i = return_dummy_item();
	return temp_i;
}

item_record_type	get_weapon(short loot)
{
	item_record_type weapon;

	if (loot == 0)
		return return_dummy_item();
	weapon = pull_item_of_type(loot,loot_min[loot],loot_max[loot],1,2,-1);

	return weapon;

}

item_record_type	get_armor(short loot)
{
	item_record_type armor;

	if (loot == 0)
		return return_dummy_item();

	armor = pull_item_of_type(loot,loot_min[loot],loot_max[loot],13,-1,-1);

	return armor;
}

item_record_type get_helm(short loot)
{
	return  pull_item_of_type(loot,loot_min[loot],loot_max[loot],14,-1,-1);
}

item_record_type get_gloves(short loot)
{
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],15,-1,-1);
}



item_record_type get_boots(short loot)
{
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],17,-1,-1);
}

item_record_type	get_shield(short loot)
{
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],12,-1,-1);
}

item_record_type	get_potion(short loot)
{
	item_record_type p;

	if (get_ran(1,0,80) < 20 * (4 - loot))
		p = pull_item_of_type(loot,loot_min[loot],loot_max[loot] / 2,7,-1,-1);
		else p = pull_item_of_type(loot,loot_min[loot],loot_max[loot],7,-1,-1);

	return p;
}

item_record_type	get_scroll(short loot)
{
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],8,-1,-1);
}

item_record_type	get_missile(short loot)
{
	if (get_ran(1,0,2) < 2)
		return pull_item_of_type(loot,loot_min[loot],loot_max[loot],5,6,4);
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],23,24,25);
}

item_record_type	get_poison(short loot)
{
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],20,-1,-1);
}

item_record_type get_wand(short loot)
{
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],9,-1,-1);
}

item_record_type get_ring(short loot)
{
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],18,-1,-1);
}

item_record_type get_necklace(short loot)
{
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],19,-1,-1);
}
