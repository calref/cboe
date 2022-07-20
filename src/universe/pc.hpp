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
#include <limits>

#include "scenario/item.hpp"
#include "dialogxml/widgets/pictypes.hpp"
#include "living.hpp"
#include "skills_traits.hpp"
#include "race.hpp"
#include "spell.hpp"

namespace legacy { struct pc_record_type; };

static struct no_party_t {} no_party;

enum class eBuyStatus {OK, NO_SPACE, NEED_GOLD, TOO_HEAVY, HAVE_LOTS};

enum ePartyPreset {PARTY_BLANK, PARTY_DEFAULT, PARTY_DEBUG};

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
class cTagFile;

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
	cParty* party = nullptr;
	template<typename Fcn>
	cInvenSlot find_item_matching(Fcn fcn);
	// Allow the party to overwrite the PC's party reference
	// It only does this (and should ever do this) in remove_pc and replace_pc!
	// Sadly there is no good way to friend just those two functions.
	friend class cParty;
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
	short exp_adj;
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
	
	bool is_alive() const override;
	bool is_friendly(const iLiving& other) const override;
	bool is_friendly() const override;
	bool is_shielded() const override;
	int get_shared_dmg(int base_dmg) const override;
	
	int get_health() const override;
	int get_magic() const override;
	int get_level() const override;
	location get_loc() const override;
	
	void finish_create();
	void void_sanctuary() override;
	void heal(int how_much) override;
	void poison(int how_much) override;
	void cure(int how_much) override;
	void acid(int how_much) override;
	void curse(int how_much) override;
	void slow(int how_much) override;
	void web(int how_much) override;
	void disease(int how_much) override;
	void dumbfound(int how_much) override;
	void scare(int how_much) override;
	void sleep(eStatus type, int how_much, int adj) override;
	void avatar() override;
	void drain_sp(int how_much, bool allow_resist) override;
	void restore_sp(int how_much) override;
	
	void combine_things();
	void sort_items();
	bool give_item(cItem item, int flags);
	bool equip_item(int which_item, bool do_print);
	bool unequip_item(int which_item, bool do_print);
	std::pair<cInvenSlot, cInvenSlot> get_weapons();
	const std::pair<cInvenSlot, cInvenSlot> get_weapons() const;
	void take_item(int which_item);
	void remove_charge(int which_item);
	const cInvenSlot has_space() const;
	cInvenSlot has_space();
	short max_weight() const;
	short cur_weight() const;
	short free_weight() const;
	// Counts the encumbrance from armor only, excluding cursed effects; used for display
	short armor_encumbrance() const;
	// Counts the actual total encumbrance used for combat calculations
	short total_encumbrance(const std::array<short, 51>& reduce_chance) const;
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

	cPictNum get_picture_num() const;
	
	void import_legacy(legacy::pc_record_type old);
	cPlayer(no_party_t);
	cPlayer(cParty& party);
	cPlayer(cParty& party,ePartyPreset key,short slot);
	cPlayer(no_party_t, const cPlayer& other);
	cPlayer(cParty& party, const cPlayer& other);
	short get_tnl() const;
	void writeTo(cTagFile& file) const;
	void readFrom(const cTagFile& file);
	virtual ~cPlayer() = default;
	// Copy-and-swap
	friend void swap(cPlayer& lhs, cPlayer& rhs);
	cPlayer(const cPlayer& other) = delete;
	cPlayer(cPlayer&& other);
	// For now, not assignable because of an issue of how to handle the unique_id
	cPlayer& operator=(cPlayer other) = delete;
};

#endif
