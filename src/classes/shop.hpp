//
//  shop.hpp
//  BoE
//
//  Created by Celtic Minstrel on 14-12-26.
//
//

#ifndef BoE_DATA_SHOP_HPP
#define BoE_DATA_SHOP_HPP

#include <array>
#include <string>
#include "item.h"

enum class eShopItemType {
	EMPTY = 0,
	ITEM = 1,
	HEAL_WOUNDS = 700,
	CURE_POISON = 701,
	CURE_DISEASE = 702,
	CURE_PARALYSIS = 703,
	CURE_DUMBFOUNDING = 708,
	REMOVE_CURSE = 704,
	DESTONE = 705,
	RAISE_DEAD = 706,
	RESURRECT = 707,
	MAGE_SPELL = 800,
	PRIEST_SPELL = 900,
	ALCHEMY = 500,
	FOOD = 600,
	SKILL = 1000,
};

struct cShopItem {
	eShopItemType type = eShopItemType::EMPTY;
	int cost;
	size_t quantity;
	cItem item;
};

class cShop {
	std::array<cShopItem,30> items;
	int cost_adj;
	std::string name;
	eShopType type;
	size_t firstEmpty();
public:
	static const size_t INFINITE = 0;
	cShop();
	cShop(eShopType type, int adj, std::string name);
	void addItem(cItem item, size_t quantity);
	void addSpecial(eShopItemType type, int n = 0);
	template<typename Iter> void addItems(Iter begin, Iter end, size_t quantity) {
		while(begin != end) addItem(*begin++, quantity);
	}
	size_t size();
	cShopItem getItem(size_t i) const;
	eShopType getType() const;
	int getCostAdjust() const;
	std::string getName() const;
	void takeOne(size_t i);
	void clearItem(size_t i);
	void clear();
};

#endif
