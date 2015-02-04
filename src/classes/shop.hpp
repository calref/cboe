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
#include "pictypes.hpp" // for pic_num_t

enum class eShopType {NORMAL, ALLOW_DEAD, RANDOM};

enum class eShopPrompt {SHOPPING, HEALING, MAGE, PRIEST, SPELLS, ALCHEMY, TRAINING};

enum class eShopItemType {
	EMPTY,
	// These ones must have these numbers in order for old scenarios to be ported correctly
	ITEM = 1,
	MAGE_SPELL = 2,
	PRIEST_SPELL = 3,
	ALCHEMY = 4,
	SKILL,
	TREASURE,
	CLASS,
	OPTIONAL,
	CALL_SPECIAL,
	HEAL_WOUNDS,
	CURE_POISON,
	CURE_DISEASE,
	CURE_ACID,
	CURE_PARALYSIS,
	REMOVE_CURSE,
	DESTONE,
	RAISE_DEAD,
	RESURRECT,
	CURE_DUMBFOUNDING,
};

struct cShopItem {
	eShopItemType type = eShopItemType::EMPTY;
	size_t quantity;
	cItem item;
	int getCost(int adj);
};

class cShop {
	std::array<cShopItem,30> items;
	int cost_adj;
	std::string name;
	eShopType type;
	eShopPrompt prompt;
	pic_num_t face;
	size_t firstEmpty();
public:
	static const size_t INFINITE = 0;
	cShop();
	cShop(eShopType type, eShopPrompt prompt, pic_num_t pic, int adj, std::string name);
	explicit cShop(long preset);
	void addItem(cItem item, size_t quantity, int chance = 100);
	void addSpecial(std::string name, std::string descr, pic_num_t pic, int node, int cost, int quantity);
	void addSpecial(eShopItemType type, int n = 0);
	template<typename Iter> void addItems(Iter begin, Iter end, size_t quantity) {
		while(begin != end) addItem(*begin++, quantity);
	}
	void replaceItem(size_t i, cShopItem newItem);
	size_t size();
	cShopItem getItem(size_t i) const;
	eShopType getType() const;
	int getCostAdjust() const;
	std::string getName() const;
	pic_num_t getFace() const;
	eShopPrompt getPrompt() const;
	void setCostAdjust(int adj);
	void setName(std::string name);
	void takeOne(size_t i);
	void clearItem(size_t i);
	void clear();
};

#endif
