
#ifndef BOE_GAME_ITEMDATA_H
#define BOE_GAME_ITEMDATA_H

#include "item.hpp"

cItem get_stored_item(short which);
cItem get_food();
cItem get_weapon(short loot);
cItem get_armor(short loot);
cItem get_helm(short loot);
cItem get_gloves(short loot);
cItem get_boots(short loot);
cItem get_shield(short loot);
cItem get_potion(short loot);
cItem get_scroll(short loot);
cItem get_missile(short loot);
cItem get_poison(short loot);
cItem get_wand(short loot);
cItem get_ring(short loot);
cItem get_necklace(short loot);
cItem pull_item_of_type(unsigned int loot_max,short min_val,short max_val,eItemType t1, eItemType t2 = eItemType::NO_ITEM, eItemType t3 = eItemType::NO_ITEM);

#endif
