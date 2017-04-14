//
//  shop.cpp
//  BoE
//
//  Created by Celtic Minstrel on 14-12-27.
//
//

#include "shop.hpp"
#include <numeric>
#include <iostream>
#include "mathutil.hpp"
#include "utility.hpp"

std::map<eSkill,short> skill_cost = {
	{eSkill::STRENGTH,3}, {eSkill::DEXTERITY,3}, {eSkill::INTELLIGENCE,3},
	{eSkill::EDGED_WEAPONS,2}, {eSkill::BASHING_WEAPONS,2}, {eSkill::POLE_WEAPONS,2},
	{eSkill::THROWN_MISSILES,1}, {eSkill::ARCHERY,2}, {eSkill::DEFENSE,2},
	{eSkill::MAGE_SPELLS,6}, {eSkill::PRIEST_SPELLS,5}, {eSkill::MAGE_LORE,1},
	{eSkill::ALCHEMY,2}, {eSkill::ITEM_LORE,4}, {eSkill::DISARM_TRAPS,2},
	{eSkill::LOCKPICKING,1}, {eSkill::ASSASSINATION,4}, {eSkill::POISON,2},
	{eSkill::LUCK,5},
};
std::map<eSkill,short> skill_max = {
	{eSkill::STRENGTH,20}, {eSkill::DEXTERITY,20}, {eSkill::INTELLIGENCE,20},
	{eSkill::EDGED_WEAPONS,20}, {eSkill::BASHING_WEAPONS,20}, {eSkill::POLE_WEAPONS,20},
	{eSkill::THROWN_MISSILES,20}, {eSkill::ARCHERY,20}, {eSkill::DEFENSE,20},
	{eSkill::MAGE_SPELLS,7}, {eSkill::PRIEST_SPELLS,7}, {eSkill::MAGE_LORE,20},
	{eSkill::ALCHEMY,20}, {eSkill::ITEM_LORE,10}, {eSkill::DISARM_TRAPS,20},
	{eSkill::LOCKPICKING,20}, {eSkill::ASSASSINATION,20}, {eSkill::POISON,20},
	{eSkill::LUCK,20},
};
std::map<eSkill,short> skill_g_cost = {
	{eSkill::STRENGTH,50}, {eSkill::DEXTERITY,50}, {eSkill::INTELLIGENCE,50},
	{eSkill::EDGED_WEAPONS,40}, {eSkill::BASHING_WEAPONS,40}, {eSkill::POLE_WEAPONS,40},
	{eSkill::THROWN_MISSILES,30}, {eSkill::ARCHERY,50}, {eSkill::DEFENSE,40},
	{eSkill::MAGE_SPELLS,250}, {eSkill::PRIEST_SPELLS,250}, {eSkill::MAGE_LORE,25},
	{eSkill::ALCHEMY,100}, {eSkill::ITEM_LORE,200}, {eSkill::DISARM_TRAPS,30},
	{eSkill::LOCKPICKING,20}, {eSkill::ASSASSINATION,100}, {eSkill::POISON,80},
	{eSkill::LUCK,0},
};
// The index here is the skill's level, not the skill itself; thus 20 is the max index since no skill can go above 20.
short skill_bonus[21] = {
	-3,-3,-2,-1,0,0,1,1,1,2,
	2,2,3,3,3,3,4,4,4,5,5};

static long cost_mult[7] = {5,7,10,13,16,20,25};

cShop::cShop() {}

cShop::cShop(eShopType type, eShopPrompt prompt, pic_num_t pic, int adj, std::string name):
	cost_adj(adj),
	name(name),
	type(type),
	prompt(prompt),
	face(pic)
{}

cShop::cShop(std::string name) : cShop(eShopType::NORMAL, eShopPrompt::SHOPPING, 0, 0, name) {}

cShop::cShop(long preset) {
	const short loot_index[10] = {1,1,1,1,2,2,2,3,3,4};
	
	if(preset == 'junk') {
		type = eShopType::RANDOM;
		prompt = eShopPrompt::SHOPPING;
		face = 0;
		name = "Magic Shop";
		for(int i = 0; i < 10; i++)
			addSpecial(eShopItemType::TREASURE, loot_index[i]);
	} else if(preset == 'heal') {
		type = eShopType::ALLOW_DEAD;
		prompt = eShopPrompt::HEALING;
		face = 41;
		name = "Healing";
		addSpecial(eShopItemType::HEAL_WOUNDS);
		addSpecial(eShopItemType::CURE_POISON);
		addSpecial(eShopItemType::CURE_DISEASE);
		addSpecial(eShopItemType::CURE_PARALYSIS);
		addSpecial(eShopItemType::CURE_DUMBFOUNDING);
		addSpecial(eShopItemType::REMOVE_CURSE);
		addSpecial(eShopItemType::DESTONE);
		addSpecial(eShopItemType::RAISE_DEAD);
		addSpecial(eShopItemType::RESURRECT);
	}
}

size_t cShop::firstEmpty() {
	for(size_t i = 0; i < items.size(); i++) {
		if(items[i].type == eShopItemType::EMPTY)
			return i;
	}
	return items.size();
}

size_t cShop::size() {
	return std::count_if(items.begin(), items.end(), [](cShopItem& item) {
		return item.type != eShopItemType::EMPTY;
	});
}

void cShop::addItem(size_t n, cItem item, size_t quantity, int chance) {
	size_t i = firstEmpty();
	if(i >= items.size()) return;
	if(item.variety == eItemType::NO_ITEM) return;
	items[i].type = chance == 100 ? eShopItemType::ITEM : eShopItemType::OPT_ITEM;
	items[i].item = item;
	items[i].item.ident = true;
	items[i].quantity = quantity;
	items[i].index = n;
	if(chance < 100)
		items[i].quantity = min(999,quantity) + chance * 1000;
}

void cShop::refreshItems(std::vector<cItem>& fromList) {
	// The purpose of this is to load in the actual item data from the scenario list
	for(size_t i = 0; i < items.size(); i++) {
		if(items[i].type == eShopItemType::ITEM || items[i].type == eShopItemType::OPT_ITEM)
			items[i].item = fromList[items[i].index];
	}
}

static cItem store_mage_spells(short which_s) {
	cItem spell('spel');
	static const short cost[62] = {
		// TODO: Costs for the level 1-3 spells
		5,5,5,5,5,5,5,5,5,5,
		5,5,5,5,5,5,5,5,5,5,
		5,5,5,5,5,5,5,5,5,5,
		150,200,150,1000,1200,400,300,200,
		200,250,500,1500,300,  250,125,150,
		400,450, 800,600,700,600,7500, 500,
		5000,3000,3500,4000,4000,4500,7000,5000
	};
	
	if(which_s != minmax(0,61,which_s))
		which_s = 0;
	spell.item_level = which_s;
	spell.value = cost[which_s];
	spell.full_name = get_str("magic-names",which_s + 1);
	return spell;
}

static cItem store_priest_spells(short which_s) {
	cItem spell('spel');
	static const short cost[62] = {
		// TODO: Costs for the level 1-3 spells
		5,5,5,5,5,5,5,5,5,5,
		5,5,5,5,5,5,5,5,5,5,
		5,5,5,5,5,5,5,5,5,5,
		100,150,75,400,200, 100,80,250,
		400,400,1200,600,300, 600,350,250,
		500,500,600,800, 1000,900,400,600,
		2500,2000,4500,4500,3000,3000,2000,2000
	};
	
	if(which_s != minmax(0,61,which_s))
		which_s = 0;
	spell.item_level = which_s;
	spell.value = cost[which_s];
	spell.full_name = get_str("magic-names",which_s + 101);
	return spell;
}

static cItem store_alchemy(short which_s) {
	cItem spell('spel');
	static const short val[20] = {
		50,75,30,130,100,
		150, 200,200,300,250,
		300, 500,600,750,700,
		1000,10000,5000,7000,12000
	};
	
	if(which_s != minmax(0,19,which_s))
		which_s = 0;
	spell.item_level = which_s;
	spell.value = val[which_s];
	spell.full_name = get_str("magic-names",which_s + 200);
	return spell;
}

void cShop::addSpecial(eShopItemType type, int n) {
	if(type == eShopItemType::EMPTY || type == eShopItemType::CALL_SPECIAL) return;
	if(type == eShopItemType::OPT_ITEM || type == eShopItemType::ITEM) return;
	replaceSpecial(firstEmpty(), type, n);
}

void cShop::replaceSpecial(size_t i, eShopItemType type, int n) {
	// TODO: Make this a std::map instead
	static const short heal_costs[10] = {50,30,80,90,100,250,500,1000,3000,100};
	static const char*const heal_types[] = {
		"Heal Damage","Cure Poison","Cure Disease","Cure Acid","Cure Paralysis",
		"Uncurse Items","Cure Stoned Character","Raise Dead","Resurrection","Cure Dumbfounding"
	};
	if(type == eShopItemType::EMPTY) return;
	if(type == eShopItemType::ITEM) return;
	if(type == eShopItemType::OPT_ITEM) return;
	if(type == eShopItemType::CALL_SPECIAL) return;
	if(i >= items.size()) return;
	items[i].type = type;
	if(type >= eShopItemType::HEAL_WOUNDS)
		items[i].index = int(type) - int(eShopItemType::HEAL_WOUNDS);
	else items[i].index = n;
	if(type == eShopItemType::MAGE_SPELL)
		items[i].item = store_mage_spells(n);
	else if(type == eShopItemType::PRIEST_SPELL)
		items[i].item = store_priest_spells(n);
	else if(type == eShopItemType::ALCHEMY)
		items[i].item = store_alchemy(n);
	else if(type == eShopItemType::SKILL) {
		items[i].item.graphic_num = 108;
		items[i].item.item_level = n;
		items[i].item.full_name = get_str("skills", n * 2 + 1);
	} else if(type == eShopItemType::TREASURE) {
		items[i].item.graphic_num = 17;
		items[i].item.item_level = n;
		items[i].item.full_name = "Treasure (type " + std::to_string(n) + ")";
	} else if(type == eShopItemType::CLASS) {
		items[i].item.graphic_num = 105;
		items[i].item.special_class = n;
		items[i].item.full_name = "Item of special class " + std::to_string(n);
	} else {
		items[i].item.graphic_num = 109;
		items[i].item.full_name = heal_types[items[i].index];
	}
	if(type == eShopItemType::SKILL)
		items[i].item.value = skill_g_cost[eSkill(n)] * 1.5;
	else if(type >= eShopItemType::HEAL_WOUNDS)
		items[i].item.value = heal_costs[items[i].index];
	items[i].quantity = 0;
}

void cShop::addSpecial(std::string name, std::string descr, pic_num_t pic, int node, int cost, int quantity) {
	size_t i = firstEmpty();
	if(i >= items.size()) return;
	items[i].type = eShopItemType::CALL_SPECIAL;
	items[i].quantity = quantity;
	items[i].item.full_name = name;
	items[i].item.desc = descr;
	items[i].item.graphic_num = pic;
	items[i].item.item_level = node;
	items[i].item.value = cost;
}

cShopItem cShop::getItem(size_t i) const {
	return items[i];
}

eShopType cShop::getType() const {
	return type;
}

int cShop::getCostAdjust() const {
	return cost_adj;
}

std::string cShop::getName() const {
	return name;
}

eShopPrompt cShop::getPrompt() const {
	return prompt;
}

pic_num_t cShop::getFace() const {
	return face;
}

void cShop::setName(std::string newName) {
	name = newName;
}

void cShop::setCostAdjust(int adj) {
	cost_adj = adj;
}

void cShop::setFace(pic_num_t pic) {
	face = pic;
}

void cShop::setType(eShopType t) {
	type = t;
}

void cShop::setPrompt(eShopPrompt p) {
	prompt = p;
}

void cShop::takeOne(size_t i) {
	if(items[i].quantity == 1) {
		items[i].type = eShopItemType::EMPTY;
	} else if(items[i].quantity > 0)
		items[i].quantity--;
}

void cShop::replaceItem(size_t i, cShopItem newItem) {
	if(i >= 30) return;
	items[i] = newItem;
}

void cShop::clearItem(size_t i) {
	std::copy(items.begin() + i + 1, items.end(), items.begin() + i);
	items.back().type = eShopItemType::EMPTY;
}

void cShop::clear() {
	std::fill(items.begin(), items.end(), cShopItem());
}

int cShopItem::getCost(int adj) {
	int cost = item.value;
	if(item.charges > 0)
		cost *= item.charges;
	cost *= cost_mult[adj];
	cost /= 10;
	return cost;
}
