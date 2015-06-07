
#include <cstdio>
#include <cstring>

#include "boe.global.hpp"

#include "universe.hpp"
#include "boe.itemdata.hpp"
#include "mathutil.hpp"

bool allow_junk_treasure = false;
short loot_min[5] = {0,0,5,50,400};
short loot_max[5] = {3,8,40,800,4000};
extern cUniverse univ;

cItem get_stored_item(short which) {
	cItem s_item;
	
	if((which >= univ.scenario.scen_items.size()) || (which < 0)) {
		s_item = cItem();
		return s_item;
	}
	
	s_item = univ.scenario.scen_items[which];
	return s_item;
}

cItem get_food() {
	cItem food('food');
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


cItem pull_item_of_type(unsigned int loot_max,short min_val,short max_val,eItemType t1, eItemType t2, eItemType t3) {
	short i,j,val;
	cItem temp_i;
	
	// occasionally get nice item
	if(get_ran(1,0,160) == 80) {
		loot_max += 2;
		max_val += 2000;
	}
	for(i = 0; i < 80; i++) {
		j = get_ran(1,0,univ.scenario.scen_items.size() - 1);
		temp_i = get_stored_item(j);
		if(temp_i.variety == eItemType::NO_ITEM) continue;
		if((temp_i.variety == t1) || (temp_i.variety == t2) || (temp_i.variety == t3)) {
			val = (temp_i.charges > 0) ? temp_i.charges * temp_i.value : temp_i.value;
			if((val >= min_val) && (val <= max_val) && (temp_i.treas_class != 0 || allow_junk_treasure) &&
				(temp_i.treas_class <= loot_max))
				return temp_i;
		}
	}
	temp_i = cItem();
	return temp_i;
}

cItem get_weapon(short loot) {
	cItem weapon;
	
	if(loot == 0)
		return cItem();
	weapon = pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::ONE_HANDED,eItemType::TWO_HANDED);
	
	return weapon;
	
}

cItem get_armor(short loot) {
	short r1;
	cItem armor;
	
	if(loot == 0)
		return cItem();
	r1 = get_ran(1,(loot - 1) * 5 + 124,142);
	
	armor = pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::ARMOR);
	
	return armor;
}

cItem get_helm(short loot) {
	
	return  pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::HELM);
}

cItem get_gloves(short loot) {
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::GLOVES);
}



cItem get_boots(short loot) {
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::BOOTS);
}

cItem get_shield(short loot) {
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::SHIELD);
}

cItem get_potion(short loot) {
	cItem p;
	
	if(get_ran(1,0,80) < 20 * (4 - loot))
		p = pull_item_of_type(loot,loot_min[loot],loot_max[loot] / 2,eItemType::POTION);
	else p = pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::POTION);
	
	return p;
}

cItem get_scroll(short loot) {
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::SCROLL);
}

cItem get_missile(short loot) {
	if(get_ran(1,0,2) < 2)
		return pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::ARROW,eItemType::THROWN_MISSILE,eItemType::BOW);
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::CROSSBOW,eItemType::BOLTS,eItemType::MISSILE_NO_AMMO);
	
}

cItem get_poison(short loot) {
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::WEAPON_POISON);
}

cItem get_wand(short loot) {
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::WAND);
}

cItem get_ring(short loot) {
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::RING);
}

cItem get_necklace(short loot) {
	return pull_item_of_type(loot,loot_min[loot],loot_max[loot],eItemType::NECKLACE);
	
}
