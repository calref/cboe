
#ifndef BOE_GAME_ITEMDATA_H
#define BOE_GAME_ITEMDATA_H

#include "item.h"

cItemRec get_stored_item(short which);
cItemRec get_food();
cItemRec	get_weapon(short loot);
cItemRec	get_armor(short loot);
cItemRec get_helm(short loot);
cItemRec get_gloves(short loot);
cItemRec get_boots(short loot);
cItemRec	get_shield(short loot);
cItemRec	get_potion(short loot);
cItemRec	get_scroll(short loot);
cItemRec	get_missile(short loot);
cItemRec	get_poison(short loot);
cItemRec get_wand(short loot);
cItemRec get_ring(short loot);
cItemRec get_necklace(short loot);
//item_record_type convert_item (short_item_record_type s_item);
cItemRec pull_item_of_type(unsigned int loot_max,short min_val,short max_val,eItemType t1, eItemType t2 = eItemType::NO_ITEM, eItemType t3 = eItemType::NO_ITEM);

#endif
