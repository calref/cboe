//
//  item_variety.hpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-13.
//
//

#ifndef BoE_ITEM_VARIETY_HPP
#define BoE_ITEM_VARIETY_HPP

enum class eItemType {
	NO_ITEM = 0,
	ONE_HANDED = 1,
	TWO_HANDED = 2,
	GOLD = 3,
	BOW = 4,
	ARROW = 5,
	THROWN_MISSILE = 6,
	POTION = 7, // potion/magic item
	SCROLL = 8, // scroll/magic item
	WAND = 9,
	TOOL = 10,
	FOOD = 11,
	SHIELD = 12,
	ARMOR = 13,
	HELM = 14,
	GLOVES = 15,
	SHIELD_2 = 16,
	// don't know why a second type of shield is used ; it is actually checked
	// in the armor code -- see below
	// and you can't equip another (12) shield while wearing it ... I didn't
	// find a single item with this property in the bladbase.exs ...
	BOOTS = 17,
	RING = 18,
	NECKLACE = 19,
	WEAPON_POISON = 20,
	NON_USE_OBJECT = 21,
	PANTS = 22,
	CROSSBOW = 23,
	BOLTS = 24,
	MISSILE_NO_AMMO = 25, //e.g slings
	SPECIAL = 26,
	QUEST = 27,
};

enum class eItemCat {
	MISC, MISSILE_WEAPON, MISSILE_AMMO, HANDS,
};

struct item_variety_t {
	eItemType self;
	bool is_armour, is_weapon, is_missile;
	int equip_count, num_hands;
	eItemCat exclusion;
};

const item_variety_t& operator*(eItemType type);

enum class eItemUse {HELP_ONE, HARM_ONE, HELP_ALL, HARM_ALL};

enum class eEnchant {PLUS_ONE, PLUS_TWO, PLUS_THREE, SHOOT_FLAME, FLAMING, PLUS_FIVE, BLESSED};

std::ostream& operator << (std::ostream& out, eItemUse e);
std::istream& operator >> (std::istream& in, eItemUse& e);
std::ostream& operator << (std::ostream& out, eItemType e);
std::istream& operator >> (std::istream& in, eItemType& e);

#endif
