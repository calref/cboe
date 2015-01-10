
#include <cstdio>
#include <cstring>

//#include "item.h"

#include "boe.global.h"

#include "classes.h"
#include "boe.itemdata.h"
#include "mathutil.hpp"
//extern piles_of_stuff_dumping_type *data_store;

//item_record_type convert_item (short_item_record_type s_item);

short loot_min[5] = {0,0,5,50,400};
short loot_max[5] = {3,8,40,800,4000};
extern cUniverse univ;

//// whole file
//item_record_type	return_dummy_item()
//{
//	item_record_type	dummy_item = {0,0, 0,0,0,0,0,0, 0,0,0,0,0, 0, 0,0, {0,0},"", "",0,0,0,0};
//
//	return dummy_item;
//}



cItemRec get_stored_item(short which) {
	cItemRec s_item;
	
	if((which >= 400) || (which < 0)) {
		s_item = cItemRec();
		return s_item;
	}
	
	s_item = univ.scenario.scen_items[which];
	return s_item;
}

cItemRec get_food() {
	cItemRec food('food');
//		{11,0, 0,0,0,0,0,0, 62,0,0,0,0, 0, 0,0, {0,0},"Food", "Food",0,0,0,0};
	food.graphic_num += get_ran(1,0,2);
	food.item_level = get_ran(1,5,10);
	if(get_ran(1,0,9) == 5)
		food.graphic_num = 113;
	if(get_ran(1,0,9) == 5)
		food.graphic_num = 114;
	// food doesn't always appear
	if(get_ran(1,0,2) != 1)
		food.variety = eItemType::NO_ITEM;
	
	return food;
}


cItemRec pull_item_of_type(unsigned int loot_max,short min_val,short max_val,eItemType t1, eItemType t2, eItemType t3) {
	short i,j,val;
	cItemRec temp_i;
	
	// occasionally get nice item
	if(get_ran(1,0,160) == 80) {
		loot_max += 2;
		max_val += 2000;
	}
	for(i = 0; i < 80; i++) {
		j = get_ran(1,0,399);
		temp_i = get_stored_item(j);
		if(temp_i.variety == eItemType::NO_ITEM) continue;
		if((temp_i.variety == t1) || (temp_i.variety == t2) || (temp_i.variety == t3)) {
			val = (temp_i.charges > 0) ? temp_i.charges * temp_i.value : temp_i.value;
			if((val >= min_val) && (val <= max_val) && (temp_i.treas_class != 0) &&
				(temp_i.treas_class <= loot_max))
				return temp_i;
		}
	}
	temp_i = cItemRec();
	return temp_i;
}

cItemRec get_weapon(short loot) {
	cItemRec weapon;
	
	if(loot == 0)
		return cItemRec();
	weapon = pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::ONE_HANDED,eItemType::TWO_HANDED);
	
	return weapon;
	
}

cItemRec get_armor(short loot) {
	short r1;
	cItemRec armor;
	
	if(loot == 0)
		return cItemRec();
	r1 = get_ran(1,(loot - 1) * 5 + 124,142);
	
	armor = pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::ARMOR);
	
	return armor;
}

cItemRec get_helm(short loot) {
	
	return  pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::HELM);
}

cItemRec get_gloves(short loot) {
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::GLOVES);
}



cItemRec get_boots(short loot) {
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::BOOTS);
}

cItemRec get_shield(short loot) {
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::SHIELD);
}

cItemRec get_potion(short loot) {
	cItemRec p;
	
	if(get_ran(1,0,80) < 20 * (4 - loot))
		p = pull_item_of_type(loot,loot_min[loot],loot_max[loot] / 2,eItemType::POTION);
	else p = pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::POTION);
	
	return p;
}

cItemRec get_scroll(short loot) {
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::SCROLL);
}

cItemRec get_missile(short loot) {
	if(get_ran(1,0,2) < 2)
		return pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::ARROW,eItemType::THROWN_MISSILE,eItemType::BOW);
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::CROSSBOW,eItemType::BOLTS,eItemType::MISSILE_NO_AMMO);
	
}

cItemRec get_poison(short loot) {
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::WEAPON_POISON);
}

cItemRec get_wand(short loot) {
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::WAND);
}

cItemRec get_ring(short loot) {
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::RING);
}

cItemRec get_necklace(short loot) {
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::NECKLACE);
	
}
