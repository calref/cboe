/*
 *  pc.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#ifndef BOE_DATA_PC_H
#define BOE_DATA_PC_H

#include <string>
#include <map>
#include <iosfwd>
#include <array>
#include <bitset>
#include <cstdint>

#include "item.hpp"
#include "pictypes.hpp"
#include "living.hpp"
#include "skills_traits.hpp"
#include "race.hpp"
#include "spell.hpp"

namespace legacy { struct pc_record_type; };

enum class eBuyStatus {OK, NO_SPACE, NEED_GOLD, TOO_HEAVY, HAVE_LOTS};

enum {
	GIVE_DO_PRINT = 1,
	GIVE_ALLOW_OVERLOAD = 2,
	// These three are mutually exclusive:
	GIVE_EQUIP_SOFT = 4,
	GIVE_EQUIP_TRY = 8,
	GIVE_EQUIP_FORCE = 12,
};

class cParty;
class cPlayer;

struct cInvenSlot {
	unsigned int slot;
	explicit cInvenSlot(cPlayer& owner) : slot(std::numeric_limits<unsigned int>::max()), owner(owner) {}
	cInvenSlot(cPlayer& owner, int slot) : slot(slot), owner(owner) {}
	void clear() {
		slot = std::numeric_limits<unsigned int>::max();
	}
	explicit operator bool() const;
	bool operator !() const;
	cItem* operator->();
	const cItem* operator->() const;
	cItem& operator*();
	const cItem& operator*() const;
	friend bool operator==(const cInvenSlot& a, const cInvenSlot& b) {
		return &a.owner == &b.owner && a.slot == b.slot;
	}
private:
	cPlayer& owner;
};

inline bool operator!=(const cInvenSlot& a, const cInvenSlot& b) {
	return !(a == b);
}

class cPlayer : public iLiving {
	cParty* party;
	template<typename Fcn>
	cInvenSlot find_item_matching(Fcn fcn);
	static const int INVENTORY_SIZE = 24;
public:
	// A nice convenient bitset with just the low 30 bits set, for initializing spells
	static const uint32_t basic_spells;
	static void(* give_help)(short,short);
	eMainStatus main_status;
	std::string name;
	// HACK: This is only really marked mutable so that I can use operator[] from const methods
	mutable std::map<eSkill, short> skills;
	unsigned short max_health;
	short cur_health;
	unsigned short max_sp;
	short cur_sp;
	unsigned short experience;
	short skill_pts;
	short level;
	std::array<cItem,INVENTORY_SIZE> items;
	std::bitset<INVENTORY_SIZE> equip;
	std::bitset<62> priest_spells;
	std::bitset<62> mage_spells;
	pic_num_t which_graphic;
	cInvenSlot weap_poisoned;
	// HACK: This is only really marked mutable so that I can use operator[] from const methods
	mutable std::map<eTrait,bool> traits;
	eRace race;
	long unique_id;
	// transient stuff
	std::map<eSkill,eSpell> last_cast;
	location combat_pos;
	short parry = 0;
	iLiving* last_attacked = nullptr; // Note: Currently this is assigned but never read
	
	bool is_alive() const;
	bool is_friendly(const iLiving& other) const;
	bool is_friendly() const;
	bool is_shielded() const;
	int get_shared_dmg(int base_dmg) const;
	
	int get_health() const;
	int get_magic() const;
	int get_level() const;
	location get_loc() const;
	
	void finish_create();
	void void_sanctuary();
	void heal(int how_much);
	void poison(int how_much);
	void cure(int how_much);
	void acid(int how_much);
	void curse(int how_much);
	void slow(int how_much);
	void web(int how_much);
	void disease(int how_much);
	void dumbfound(int how_much);
	void scare(int how_much);
	void sleep(eStatus type, int how_much, int adj);
	void avatar();
	void drain_sp(int how_much);
	void restore_sp(int how_much);
	
	void combine_things();
	void sort_items();
	bool give_item(cItem item, int flags);
	bool equip_item(int which_item, bool do_print);
	bool unequip_item(int which_item, bool do_print);
	std::pair<cInvenSlot, cInvenSlot> get_weapons();
	void take_item(int which_item);
	void remove_charge(int which_item);
	const cInvenSlot has_space() const;
	cInvenSlot has_space();
	short max_weight() const;
	short cur_weight() const;
	short free_weight() const;
	short get_prot_level(eItemAbil abil, short dat = -1) const;
	
	const cInvenSlot has_abil_equip(eItemAbil abil, short dat = -1) const;
	cInvenSlot has_abil_equip(eItemAbil abil, short dat = -1);
	const cInvenSlot has_abil(eItemAbil abil, short dat = -1) const;
	cInvenSlot has_abil(eItemAbil abil, short dat = -1);
	const cInvenSlot has_type_equip(eItemType type) const;
	cInvenSlot has_type_equip(eItemType type);
	const cInvenSlot has_type(eItemType type) const;
	cInvenSlot has_type(eItemType type);
	const cInvenSlot has_class_equip(unsigned int item_class) const;
	cInvenSlot has_class_equip(unsigned int item_class);
	const cInvenSlot has_class(unsigned int item_class) const;
	cInvenSlot has_class(unsigned int item_class);
	
	short skill(eSkill skill) const;
	short stat_adj(eSkill skill) const;
	eBuyStatus ok_to_buy(short cost,cItem item) const;
	
	void join_party(cParty& p) {party = &p;}
	cPlayer* leave_party() {party = nullptr; return this;}
	
	void import_legacy(legacy::pc_record_type old);
	cPlayer(cParty& party);
	cPlayer(cParty& party,long key,short slot);
	short get_tnl();
	void writeTo(std::ostream& file) const;
	void readFrom(std::istream& file);
	virtual ~cPlayer() = default;
	// Copy-and-swap
	void swap(cPlayer& other);
	cPlayer(const cPlayer& other);
	cPlayer(cPlayer&& other);
	// For now, not assignable because of an issue of how to handle the unique_id
	cPlayer& operator=(cPlayer other) = delete;
};

#endif
