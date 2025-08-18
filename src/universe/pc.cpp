/*
 *  pc.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <utility>

#include "universe.hpp"
#include "oldstructs.hpp"
#include "mathutil.hpp"
#include "fileio/fileio.hpp"
#include "fileio/tagfile.hpp"
#include "sounds.hpp"
#include "dialogxml/dialogs/strdlog.hpp"

extern short skill_bonus[21];
// A nice convenient bitset with just the low 30 bits set, for initializing spells
const uint32_t cPlayer::basic_spells = std::numeric_limits<uint32_t>::max() >> 2;

bool cPlayer::give_help_enabled = false;

void cPlayer::import_legacy(legacy::pc_record_type old){
	main_status = (eMainStatus) old.main_status;
	name = old.name;
	for(short i = 0; i < 19; i++) {
		eSkill skill = eSkill(i);
		skills[skill] = old.skills[i];
	}
	max_health = old.max_health;
	cur_health = old.cur_health;
	max_sp = old.max_sp;
	cur_sp = old.cur_sp;
	experience = old.experience;
	skill_pts = old.skill_pts;
	level = old.level;
	// Note that advan and exp_adj are both unused in legacy BoE
	// advan is always 15x false, and exp_adj is always 100
	exp_adj = old.exp_adj;
	for(short i = 0; i < 15; i++){
		status[(eStatus) i] = old.status[i];
		eTrait trait = eTrait(i);
		traits[trait] = old.traits[i];
	}
	for(short i = 0; i < 24; i++){
		items[i].import_legacy(old.items[i]);
		equip[i] = old.equip[i];
	}
	for(short i = 0; i < 62; i++){
		priest_spells[i] = old.priest_spells[i];
		mage_spells[i] = old.mage_spells[i];
	}
	which_graphic = old.which_graphic;
	weap_poisoned.slot = old.weap_poisoned;
	race = (eRace) old.race;
	direction = eDirection(old.direction);
}

short cPlayer::get_tnl() const {
	// Omitting a race from this list gives it a value of 0, thanks to the defaulting implementation of operator[]
	static std::map<const eRace, const int> rp = {{eRace::NEPHIL,12},{eRace::SLITH,20},{eRace::VAHNATAI,18}};
	static std::map<const eTrait, const short> ap = {
		{eTrait::TOUGHNESS,10}, {eTrait::MAGICALLY_APT,20}, {eTrait::AMBIDEXTROUS,8}, {eTrait::NIMBLE,10}, {eTrait::CAVE_LORE,4},
		{eTrait::WOODSMAN,6}, {eTrait::GOOD_CONST,10}, {eTrait::HIGHLY_ALERT,7}, {eTrait::STRENGTH,12}, {eTrait::RECUPERATION,15},
		{eTrait::SLUGGISH,-10}, {eTrait::MAGICALLY_INEPT,-8}, {eTrait::FRAIL,-8}, {eTrait::CHRONIC_DISEASE,-20}, {eTrait::BAD_BACK,-8},
		{eTrait::PACIFIST,-40}, {eTrait::ANAMA,-10},
	};
	
	short tnl = 100 + rp[race], store_per = 100;
	for(int i = 0; i < 17; i++) {
		eTrait trait = eTrait(i);
		if(traits[trait])
			store_per += ap[trait];
	}
	
	tnl = percent(tnl, store_per);
	
	return max(tnl, 10);
}

void cPlayer::avatar() {
	heal(300);
	cure(8);
	status[eStatus::BLESS_CURSE] = 8;
	status[eStatus::HASTE_SLOW] = 8;
	status[eStatus::INVULNERABLE] = 3;
	status[eStatus::MAGIC_RESISTANCE] = 8;
	status[eStatus::WEBS] = 0;
	status[eStatus::DISEASE] = 0;
	if(status[eStatus::DUMB] > 0)
		status[eStatus::DUMB] = 0;
	status[eStatus::MARTYRS_SHIELD] = 8;
}

void cPlayer::drain_sp(int drain, bool allow_resist) {
	if(drain <= 0) return;
	int mu = skills[eSkill::MAGE_SPELLS];
	int cl = skills[eSkill::PRIEST_SPELLS];
	if(allow_resist) {
		if(mu > 0 && cur_sp > 4)
			drain = min(cur_sp, drain / 3);
		else if(cl > 0 && cur_sp > 10)
			drain = min(cur_sp, drain / 2);
	}
	cur_sp -= drain;
	if(cur_sp < 0) cur_sp = 0;
}

void cPlayer::scare(int) {
	// TODO: Not implemented
}

void cPlayer::void_sanctuary() {
	if(status[eStatus::INVISIBLE] > 0 && print_result)
		print_result("You become visible!");
	iLiving::void_sanctuary();
}

void cPlayer::heal(int amt) {
	if(!is_alive()) return;
	if(cur_health >= max_health) return;
	cur_health += amt;
	if(cur_health > max_health)
		cur_health = max_health;
	if(cur_health < 0)
		cur_health = 0;
}

void cPlayer::cure(int amt) {
	if(!is_alive()) return;
	if(status[eStatus::POISON] <= amt)
		status[eStatus::POISON] = 0;
	else status[eStatus::POISON] -= amt;
	one_sound(51);
}

// if how_much < 0, bless
void cPlayer::curse(int how_much) {
	if(!is_alive()) return;
	if(how_much > 0)
		how_much -= get_prot_level(eItemAbil::STATUS_PROTECTION,int(eStatus::BLESS_CURSE)) / 2;
	apply_status(eStatus::BLESS_CURSE, -how_much);
	if(print_result) {
		if(how_much < 0)
			print_result("  " + name + " blessed.");
		else if(how_much > 0)
			print_result("  " + name + " cursed.");
	}
	if(give_help_enabled) {
		if(how_much > 0)
			give_help(59,0);
		else if(how_much > 0)
			give_help(34,0);
	}
}

void cPlayer::dumbfound(int how_much) {
	if(!is_alive()) return;
	short r1 = get_ran(1,0,90);
	if(has_abil_equip(eItemAbil::WILL)) {
		if(print_result)
			print_result("  Ring of Will glows.");
		r1 -= 10;
	}
	how_much -= get_prot_level(eItemAbil::STATUS_PROTECTION,int(eStatus::DUMB)) / 4;
	if(r1 < level)
		how_much -= 2;
	if(how_much <= 0) {
		if(print_result)
			print_result("  " + name + " saved.");
		return;
	}
	apply_status(eStatus::DUMB, how_much);
	if(print_result)
		print_result("  " + name + " dumbfounded.");
	one_sound(67);
	if(give_help_enabled)
		give_help(28,0);
}

void cPlayer::disease(int how_much) {
	if(!is_alive()) return;
	short r1 = get_ran(1,1,100);
	if(r1 < level * 2)
		how_much -= 2;
	if(how_much <= 0) {
		if(print_result)
			print_result("  " + name + " saved.");
		return;
	}
	how_much -= get_prot_level(eItemAbil::STATUS_PROTECTION,int(eStatus::DISEASE)) / 2;
	if(traits[eTrait::FRAIL] && how_much > 1)
		how_much++;
	if(traits[eTrait::FRAIL] && how_much == 1 && get_ran(1,0,1) == 0)
		how_much++;
	apply_status(eStatus::DISEASE, how_much);
	if(print_result)
		print_result("  " + name + " diseased.");
	one_sound(66);
	if(give_help_enabled)
		give_help(29,0);
}

void cPlayer::sleep(eStatus what_type,int how_much,int adjust) {
	if(what_type == eStatus::CHARM) return;
	short level = 0;
	if(!is_alive()) return;
	if(how_much == 0) return;
	
	if((what_type == eStatus::ASLEEP) &&
	   (race == eRace::UNDEAD || race == eRace::SKELETAL || race == eRace::SLIME ||
		race == eRace::STONE || race == eRace::PLANT))
		return;
	if(what_type == eStatus::ASLEEP || what_type == eStatus::PARALYZED) {
		how_much -= get_prot_level(eItemAbil::WILL) / 2;
		level = get_prot_level(eItemAbil::FREE_ACTION);
		how_much -= (what_type == eStatus::ASLEEP) ? level : level * 300;
		how_much -= get_prot_level(eItemAbil::STATUS_PROTECTION,int(what_type)) / 4;
	} else if(what_type == eStatus::FORCECAGE)
		how_much -= 1 + get_prot_level(eItemAbil::STATUS_PROTECTION,int(what_type)) / 8;
	
	short r1 = get_ran(1,1,100) + adjust;
	if(what_type == eStatus::FORCECAGE)
		r1 -= stat_adj(eSkill::MAGE_LORE);
	if(r1 < 30 + level * 2)
		how_much = -1;
	if(what_type == eStatus::ASLEEP && (traits[eTrait::HIGHLY_ALERT] || status[eStatus::ASLEEP] < 0))
		how_much = -1;
	if(how_much <= 0) {
		if(print_result)
			print_result("  " + name + " resisted.");
		return;
	}
	status[what_type] = how_much;
	if(print_result) {
		if(what_type == eStatus::ASLEEP)
			print_result("  " + name + " falls asleep.");
		else if(what_type == eStatus::FORCECAGE)
			print_result("  " + name + " is trapped!");
		else print_result("  " + name + " paralyzed.");
	}
	if(what_type == eStatus::ASLEEP)
		play_sound(96);
	else play_sound(90);
	if(what_type != eStatus::FORCECAGE)
		ap = 0;
	if(give_help_enabled) {
		if(what_type == eStatus::ASLEEP)
			give_help(30,0);
		else if(what_type == eStatus::PARALYZED)
			give_help(32,0);
		else if(what_type == eStatus::FORCECAGE)
			give_help(46,0);
	}
}

// if how_much < 0, haste
void cPlayer::slow(int how_much) {
	if(!is_alive()) return;
	if(how_much > 0)
		how_much -= get_prot_level(eItemAbil::STATUS_PROTECTION,int(eStatus::HASTE_SLOW)) / 2;
	apply_status(eStatus::HASTE_SLOW, -how_much);
	if(print_result) {
		if(how_much < 0)
			print_result("  " + name + " hasted.");
		else if(how_much > 0)
			print_result("  " + name + " slowed.");
	}
	if(give_help_enabled)
		give_help(35,0);
}

void cPlayer::web(int how_much) {
	if(!is_alive()) return;
	if(how_much > 0)
		how_much -= get_prot_level(eItemAbil::STATUS_PROTECTION,int(eStatus::WEBS)) / 2;
	apply_status(eStatus::WEBS, how_much);
	if(print_result)
		print_result("  " + name + " webbed.");
	one_sound(17);
	give_help(31,0);
}

void cPlayer::acid(int how_much) {
	if(!is_alive()) return;
	if(has_abil_equip(eItemAbil::STATUS_PROTECTION,int(eStatus::ACID))) {
		if(print_result)
			print_result("  " + name + " resists acid.");
		return;
	}
	status[eStatus::ACID] += how_much;
	if(print_result)
		print_result("  " + name + " covered with acid!");
	one_sound(42);
}

void cPlayer::restore_sp(int amt) {
	if(!is_alive()) return;
	if(amt <= 0) return;
	if(cur_sp >= max_sp) return;
	cur_sp += amt;
	if(cur_sp > max_sp)
		cur_sp = max_sp;
	if(cur_sp < 0)
		cur_sp = 0;
}

void cPlayer::poison(int how_much) {
	if(!is_alive()) return;
	how_much -= get_prot_level(eItemAbil::STATUS_PROTECTION,int(eStatus::POISON)) / 2;
	how_much -= get_prot_level(eItemAbil::FULL_PROTECTION) / 3;
	
	if(traits[eTrait::FRAIL] && how_much > 1)
		how_much++;
	if(traits[eTrait::FRAIL] && how_much == 1 && get_ran(1,0,1) == 0)
		how_much++;
	
	if(how_much > 0) {
		apply_status(eStatus::POISON, how_much);
		if(print_result)
			print_result("  " + name + " poisoned.");
		one_sound(17);
		give_help(33,0);
	}
}

short cPlayer::stat_adj(eSkill which) const {
	// This is one place where we use the base skill level instead of the adjusted skill level
	// Using the adjusted skill level here would alter the original mechanics of stat-boosting items
	short tr = skill_bonus[skills[which]];
	if(which == eSkill::INTELLIGENCE) {
		if(traits[eTrait::MAGICALLY_APT])
			tr++;
	}
	if(which == eSkill::STRENGTH) {
		if(traits[eTrait::STRENGTH])
			tr++;
		if(race == eRace::VAHNATAI)
			tr -= 2;
	}
	// TODO: Use ability strength?
	if(has_abil_equip(eItemAbil::BOOST_STAT,int(which)))
		tr++;
	return tr;
}

bool cPlayer::is_alive() const {
	return main_status == eMainStatus::ALIVE;
}

bool cPlayer::is_friendly() const {
	// Currently players can't be charmed, but that might change in the future
	if(status[eStatus::CHARM] > 0)
		return false;
	return true;
}

bool cPlayer::is_friendly(const iLiving& other) const {
	if(status[eStatus::CHARM] > 0) {
		if(other.is_friendly()) return false;
		// TODO: If charmed players becomes a thing, they should match the attitude (A or B) of whoever charmed them
		if(const cCreature* monst = dynamic_cast<const cCreature*>(&other))
			return monst->attitude == eAttitude::HOSTILE_A;
		// If we get here, the other is also a charmed player.
		return true;
	}
	return other.is_friendly();
}

bool cPlayer::is_shielded() const {
	if(status[eStatus::MARTYRS_SHIELD] > 0)
		return true;
	if(get_prot_level(eItemAbil::MARTYRS_SHIELD) > 0)
		return true;
	return false;
}

int cPlayer::get_shared_dmg(int base) const {
	int martyr1 = status[eStatus::MARTYRS_SHIELD];
	int martyr2 = get_prot_level(eItemAbil::MARTYRS_SHIELD);
	if(martyr1 + martyr2 <= 0) return 0;
	if(get_ran(1,1,20) < martyr2) return base + max(1, martyr2 / 5);
	return base;
}

location cPlayer::get_loc() const {
	if(party && (combat_pos.x < 0 || combat_pos.y < 0))
		return party->get_loc();
	return combat_pos;
}

int cPlayer::get_health() const {
	return cur_health;
}

int cPlayer::get_magic() const {
	return cur_sp;
}

int cPlayer::get_level() const {
	return level;
}

void cPlayer::sort_items() {
	using it = eItemType;
	static std::map<eItemType, const short> item_priority = {
		{it::NO_ITEM, 20}, {it::ONE_HANDED, 8}, {it::TWO_HANDED, 8}, {it::GOLD, 20}, {it::BOW, 9},
		{it::ARROW, 9}, {it::THROWN_MISSILE, 3}, {it::POTION, 2}, {it::SCROLL, 1}, {it::WAND, 0},
		{it::TOOL, 7}, {it::FOOD, 20}, {it::SHIELD, 10}, {it::ARMOR, 10}, {it::HELM, 10},
		{it::GLOVES, 10}, {it::SHIELD_2, 10}, {it::BOOTS, 10}, {it::RING, 5}, {it::NECKLACE, 6},
		{it::WEAPON_POISON, 4}, {it::NON_USE_OBJECT, 11}, {it::PANTS, 12}, {it::CROSSBOW, 9}, {it::BOLTS, 9},
		{it::MISSILE_NO_AMMO, 9}, {it::QUEST, 20}, {it::SPECIAL, 20}
	};
	bool no_swaps = false;
	
	while(!no_swaps) {
		no_swaps = true;
		for(int i = 0; i < 23; i++)
			if(item_priority[items[i + 1].variety] <
			   item_priority[items[i].variety]) {
			  	no_swaps = false;
				std::swap(items[i + 1], items[i]);
				bool temp_equip = equip[i];
				equip[i] = equip[i + 1];
				equip[i + 1] = temp_equip;
				if(weap_poisoned.slot == i + 1)
					weap_poisoned.slot--;
				else if(weap_poisoned.slot == i)
					weap_poisoned.slot++;
			}
	}
}

eBuyStatus cPlayer::give_item(cItem item, int flags) {
	if(main_status != eMainStatus::ALIVE)
		return eBuyStatus::DEAD;
	
	bool do_print = flags & GIVE_DO_PRINT;
	bool allow_overload = flags & GIVE_ALLOW_OVERLOAD;
	int equip_type = flags & GIVE_EQUIP_FORCE;
	bool check_only = flags & GIVE_CHECK_ONLY;

	// If you passed both GIVE_DO_PRINT and GIVE_CHECK_ONLY for some reason,
	// the buffer would lie to you and sounds would play.
	if(check_only) do_print = false;

	if(item.variety == eItemType::NO_ITEM)
		return eBuyStatus::OK;
	if(item.variety == eItemType::GOLD) {
		if(!party) return eBuyStatus::NO_SPACE;
		if(!check_only)
			party->gold += item.item_level;
		if(do_print && print_result)
			print_result("You get some gold.");
		return eBuyStatus::OK;
	}
	if(item.variety == eItemType::FOOD) {
		if(!party) return eBuyStatus::NO_SPACE;
		if(!check_only)
			party->food += item.item_level;
		if(do_print && print_result)
			print_result("You get some food.");
		return eBuyStatus::OK;
	}
	if(item.variety == eItemType::SPECIAL) {
		if(!party) return eBuyStatus::NO_SPACE;
		if(!check_only)
			party->spec_items.insert(item.item_level);
		if(do_print && print_result)
			print_result("You get a special item.");
		return eBuyStatus::OK;
	}
	if(item.variety == eItemType::QUEST) {
		if(!party) return eBuyStatus::NO_SPACE;
		if(!check_only)
			party->active_quests[item.item_level] = cJob(party->calc_day());
		if(do_print && print_result)
			print_result("You get a quest.");
		return eBuyStatus::OK;
	}
	if(!allow_overload && item.item_weight() > free_weight()) {
	  	if(do_print && print_result) {
			play_sound(41);
			print_result("Item too heavy to carry.");
		}
		return eBuyStatus::TOO_HEAVY;
	}

	cInvenSlot real_free_space = has_space();
	cInvenSlot extra_space = cInvenSlot(*this, INVENTORY_SIZE);

	cInvenSlot* space_ptr = &real_free_space;

	if(!real_free_space){
		// Check if using the extra slot for stacking items would result in everything fitting:
		*extra_space = item;
		if(combine_things(true)){
			space_ptr = &extra_space;
		}
		*extra_space = cItem();
	}

	cInvenSlot free_space = *space_ptr;
	if(free_space) {
		if(check_only) return eBuyStatus::OK;

		item.property = false;
		item.contained = false;
		item.held = false;
		*free_space = item;
		
		if(do_print && print_result) {
			std::ostringstream announce;
			announce << "  " << name << " gets ";
			if(!item.ident)
				announce << item.name;
			else announce << item.full_name;
			announce << '.';
			print_result(announce.str());
		}
		
		if(equip_type != 0 && (*item.variety).equip_count) {
			if(!equip_item(free_space.slot, false) && equip_type != GIVE_EQUIP_SOFT) {
				eItemCat exclude = (*item.variety).exclusion;
				int rem1 = INVENTORY_SIZE, rem2 = INVENTORY_SIZE;
				for(int i = 0; i < INVENTORY_SIZE; i++) {
					if(i == free_space.slot) continue;
					if(!equip[i]) continue;
					eItemCat check_exclude = (*items[i].variety).exclusion;
					if(exclude != check_exclude) continue;
					if(exclude == eItemCat::MISC && item.variety != items[i].variety)
						continue;
					if(exclude == eItemCat::HANDS) {
						if(rem1 == INVENTORY_SIZE) {
							if(item.variety == eItemType::ONE_HANDED || item.variety == eItemType::TWO_HANDED || rem2 < INVENTORY_SIZE)
								rem1 = i;
							if(rem1 < INVENTORY_SIZE) continue;
						}
						if(rem2 == INVENTORY_SIZE) {
							if(item.variety == eItemType::SHIELD || item.variety == eItemType::SHIELD_2 || rem1 < INVENTORY_SIZE)
								rem2 = i;
						}
					} else if(rem1 < INVENTORY_SIZE)
						rem1 = i;
				}
				bool can_rem1 = rem1 < INVENTORY_SIZE && (!items[rem1].cursed || equip_type == GIVE_EQUIP_FORCE);
				bool can_rem2 = rem2 < INVENTORY_SIZE && (!items[rem2].cursed || equip_type == GIVE_EQUIP_FORCE);
				if(exclude == eItemCat::HANDS) {
					if((*item.variety).num_hands == 2 && can_rem1 && can_rem2) {
						equip[rem1] = false;
						equip[rem2] = false;
					} else if((*item.variety).num_hands == 1) {
						if(can_rem1) equip[rem1] = false;
						else if(can_rem2) equip[rem2] = false;
					}
					if((rem1 == weap_poisoned.slot && !equip[rem1]) || (rem2 == weap_poisoned.slot && !equip[rem2])) {
						status[eStatus::POISONED_WEAPON] = 0;
						weap_poisoned.clear();
					}
				} else if(can_rem1)
					equip[rem1] = false;
				equip_item(free_space.slot, false);
			}
		}
		
		combine_things();
		sort_items();
		return eBuyStatus::OK;
	}
	return eBuyStatus::NO_SPACE;
}

eBuyStatus cPlayer::can_give_item(cItem item) const {
	return const_cast<cPlayer*>(this)->give_item(item, GIVE_CHECK_ONLY);
}

bool cPlayer::equip_item(int which_item, bool do_print) {
	const cItem& item = items[which_item];
	if((*item.variety).equip_count == 0) {
		if(do_print && print_result)
			print_result("Equip: Can't equip this item.");
		return false;
	}
	unsigned short num_this_type = 0, hands_occupied = 0;
	for(int i = 0; i < INVENTORY_SIZE; i++)
		if(equip[i]) {
			if(items[i].variety == item.variety)
				num_this_type++;
			hands_occupied += (*items[i].variety).num_hands;
		}
	
	eItemCat equip_item_type = (*item.variety).exclusion;
	// Now if missile is already equipped, no more missiles
	if(equip_item_type == eItemCat::MISSILE_AMMO || equip_item_type == eItemCat::MISSILE_WEAPON) {
		for(int i = 0; i < INVENTORY_SIZE; i++)
			if(equip[i] && (*items[i].variety).exclusion == equip_item_type) {
				if(do_print && print_result) {
					print_result("Equip: You have something of this type");
					print_result("  equipped.");
				}
				return false;
			}
	}
	
	size_t hands_free = 2 - hands_occupied;
	if(hands_free < (*item.variety).num_hands) {
		if(do_print && print_result)
			print_result("Equip: Not enough free hands");
		return false;
	} else if((*item.variety).equip_count <= num_this_type) {
		if(do_print && print_result)
			print_result("Equip: Can't equip another");
		return false;
	}
	equip[which_item] = true;
	if(do_print && print_result)
		print_result("Equip: OK");
	return true;
}

bool cPlayer::unequip_item(int which_item, bool do_print) {
	if(!equip[which_item]) {
		if(do_print && print_result)
			print_result("Equip: Not equipped");
		return false;
	}
	if(items[which_item].cursed) {
		if(do_print && print_result)
			print_result("Equip: Item is cursed.");
		return false;
	}
	equip[which_item] = false;
	if(do_print && print_result)
		print_result("Equip: Unequipped");
	if(weap_poisoned.slot == which_item && status[eStatus::POISONED_WEAPON] > 0) {
		if(do_print && print_result)
			print_result("  Poison lost.");
		status[eStatus::POISONED_WEAPON] = 0;
		weap_poisoned.clear();
	}
	return true;
}

const std::pair<cInvenSlot, cInvenSlot> cPlayer::get_weapons() const {
	return const_cast<cPlayer*>(this)->get_weapons();
}

std::pair<cInvenSlot, cInvenSlot> cPlayer::get_weapons() {
	cInvenSlot weap1 = has_type_equip(eItemType::ONE_HANDED);
	if(weap1) {
		equip[weap1.slot] = false;
		cInvenSlot weap2 = has_type_equip(eItemType::ONE_HANDED);
		equip[weap1.slot] = true;
		return std::make_pair(weap1, weap2);
	} else {
		cInvenSlot weap2 = has_type_equip(eItemType::TWO_HANDED);
		return std::make_pair(weap2, weap1);
	}
}

short cPlayer::max_weight() const {
	return 100 + (15 * min(skill(eSkill::STRENGTH),20)) + (traits[eTrait::STRENGTH] * 30)
		+ (traits[eTrait::BAD_BACK] * -50) + (race == eRace::VAHNATAI) * -25;
}

short cPlayer::cur_weight() const {
	short weight = 0;
	bool airy = false,heavy = false;
	
	for(int i = 0; i < INVENTORY_SIZE; i++)
		if(items[i].variety != eItemType::NO_ITEM) {
			weight += items[i].item_weight();
			if(items[i].ability == eItemAbil::LIGHTER_OBJECT)
				airy = true;
			if(items[i].ability == eItemAbil::HEAVIER_OBJECT)
				heavy = true;
		}
	if(airy)
		weight -= 30;
	if(heavy)
		weight += 30;
	if(weight < 0)
		weight = 0;
	return weight;
}

short cPlayer::free_weight() const {
	return max_weight() - cur_weight();
}

short cPlayer::armor_encumbrance() const {
	short total = 0;
	for(short i = 0; i < INVENTORY_SIZE; i++) {
		if(equip[i]) total += items[i].awkward;
	}
	return total;
}

short cPlayer::total_encumbrance(const std::array<short, 51>& reduce_chance) const {
	short total = 0;
	
	short burden = free_weight();
	if(burden < 0) total += burden / -10;
	
	for(short i = 0; i < INVENTORY_SIZE; i++)
		if(equip[i]) {
			short item_encumbrance = items[i].awkward;
			if(items[i].ability == eItemAbil::ENCUMBERING)
				item_encumbrance += items[i].abil_strength;
			if(item_encumbrance == 1 && get_ran(1,0,130) < reduce_chance[skill(eSkill::DEFENSE)])
				item_encumbrance--;
			if(item_encumbrance > 1 && get_ran(1,0,70) < reduce_chance[skill(eSkill::DEFENSE)])
				item_encumbrance--;
			total += item_encumbrance;
		}
	return total;
}

cInvenSlot cPlayer::has_space() {
	for(int i = 0; i < INVENTORY_SIZE; i++) {
		if(items[i].variety == eItemType::NO_ITEM)
			return cInvenSlot(*this, i);
	}
	return cInvenSlot(*this);
}

const cInvenSlot cPlayer::has_space() const {
	return const_cast<cPlayer*>(this)->has_space();
}

bool cPlayer::combine_things(bool check_only) {
	bool can_combine = false;
	// Here it is correct to check items.size() because the extra slot is *for* combining things.
	for(int i = 0; i < items.size(); i++) {
		if(items[i].variety != eItemType::NO_ITEM && items[i].type_flag > 0 && items[i].ident) {
			for(int j = i + 1; j < items.size(); j++)
				if(items[j].variety != eItemType::NO_ITEM && items[j].type_flag == items[i].type_flag && items[j].ident) {
					can_combine = true;	
					if(check_only) continue;
					if(print_result) print_result("(items combined)");
					short test = items[i].charges + items[j].charges;
					if(test > 125) {
						items[i].charges = 125;
						if(print_result) print_result("(Can have at most 125 of any item.)");
					}
					else items[i].charges += items[j].charges;
				 	if(equip[j]) {
				 		equip[i] = true;
				 		equip[j] = false;
					}
					take_item(j);
				}
		}
		if(items[i].variety != eItemType::NO_ITEM && items[i].charges < 0)
			items[i].charges = 1;
	}
	return can_combine;
}

short cPlayer::get_prot_level(eItemAbil abil, short dat) const {
	int sum = 0;
	for(int i = 0; i < INVENTORY_SIZE; i++) {
		if(items[i].variety == eItemType::NO_ITEM) continue;
		if(items[i].ability != abil) continue;
		if(!equip[i]) continue;
		if(dat >= 0 && dat != items[i].abil_data.value) continue;
		sum += items[i].abil_strength;
	}
	return sum; // TODO: Should we return -1 if the sum is 0?
}

template<typename Fcn>
cInvenSlot cPlayer::find_item_matching(Fcn fcn) {
	for(short i = 0; i < INVENTORY_SIZE; i++)
		if(items[i].variety != eItemType::NO_ITEM && fcn(i, items[i]))
			return cInvenSlot(*this, i);
	return cInvenSlot(*this);
}

cInvenSlot cPlayer::has_abil_equip(eItemAbil abil,short dat) {
	return find_item_matching([this,abil,dat](int i, const cItem& item) {
		if(item.variety == eItemType::NO_ITEM) return false;
		if(item.ability != abil) return false;
		if(!equip[i]) return false;
		if(dat >= 0 && dat != item.abil_data.value) return false;
		return true;
	});
}

const cInvenSlot cPlayer::has_abil_equip(eItemAbil abil,short dat) const {
	return const_cast<cPlayer*>(this)->has_abil_equip(abil,dat);
}

cInvenSlot cPlayer::has_abil(eItemAbil abil,short dat) {
	return find_item_matching([this,abil,dat](int, const cItem& item) {
		if(item.variety == eItemType::NO_ITEM) return false;
		if(item.ability != abil) return false;
		if(item.charges == 0) return false;
		if(dat >= 0 && dat != item.abil_data.value) return false;
		return true;
	});
}

const cInvenSlot cPlayer::has_abil(eItemAbil abil,short dat) const {
	return const_cast<cPlayer*>(this)->has_abil(abil,dat);
}

cInvenSlot cPlayer::has_type_equip(eItemType type) {
	return find_item_matching([this,type](int i, const cItem& item) {
		return equip[i] && item.variety == type;
	});
}

const cInvenSlot cPlayer::has_type_equip(eItemType type) const {
	return const_cast<cPlayer*>(this)->has_type_equip(type);
}

cInvenSlot cPlayer::has_type(eItemType type) {
	return find_item_matching([type](int, const cItem& item) {
		return item.variety == type;
	});
}

const cInvenSlot cPlayer::has_type(eItemType type) const {
	return const_cast<cPlayer*>(this)->has_type(type);
}

cInvenSlot cPlayer::has_class_equip(unsigned int item_class) {
	return find_item_matching([this,item_class](int i, const cItem& item) {
		return equip[i] && item.special_class == item_class;
	});
}

const cInvenSlot cPlayer::has_class_equip(unsigned int item_class) const {
	return const_cast<cPlayer*>(this)->has_class_equip(item_class);
}

cInvenSlot cPlayer::has_class(unsigned int item_class, bool require_charges) {
	return find_item_matching([item_class, require_charges](int, const cItem& item) {
		return item.special_class == item_class && (!require_charges || item.charges > 0 || item.max_charges == 0);
	});
}

const cInvenSlot cPlayer::has_class(unsigned int item_class, bool require_charges) const {
	return const_cast<cPlayer*>(this)->has_class(item_class, require_charges);
}

cInvenSlot::operator bool() const {
	return slot < owner.items.size();
}

bool cInvenSlot::operator !() const {
	return slot >= owner.items.size();
}

cItem* cInvenSlot::operator->() {
	return &owner.items[slot];
}

const cItem* cInvenSlot::operator->() const {
	return &owner.items[slot];
}

cItem& cInvenSlot::operator*() {
	return owner.items[slot];
}

const cItem& cInvenSlot::operator*() const {
	return owner.items[slot];
}

short cPlayer::skill(eSkill skill) const {
	int bulk_bonus = 0;
	if(skill >= eSkill::EDGED_WEAPONS && skill <= eSkill::DEFENSE)
		bulk_bonus = get_prot_level(eItemAbil::BOOST_WAR);
	else if(skill >= eSkill::MAGE_SPELLS && skill <= eSkill::ITEM_LORE)
		bulk_bonus = get_prot_level(eItemAbil::BOOST_MAGIC);
	return min(20, skills[skill] + get_prot_level(eItemAbil::BOOST_STAT, int(skill))) + bulk_bonus;
}

eBuyStatus cPlayer::ok_to_buy(short cost,cItem item) const {
	if(!party) return eBuyStatus::NO_SPACE;
	if(item.variety == eItemType::SPECIAL) {
		if(party->spec_items.count(item.item_level))
			return eBuyStatus::HAVE_LOTS;
	} else if(item.variety == eItemType::QUEST) {
		if(party->active_quests[item.item_level].status != eQuestStatus::AVAILABLE)
			return eBuyStatus::HAVE_LOTS;
	} else if(item.variety != eItemType::GOLD && item.variety != eItemType::FOOD) {
		for(int i = 0; i < INVENTORY_SIZE; i++)
			if(items[i].variety != eItemType::NO_ITEM && items[i].type_flag == item.type_flag && items[i].charges > 123)
				return eBuyStatus::HAVE_LOTS;
		
		eBuyStatus pc_can_fit = can_give_item(item);
		if(pc_can_fit != eBuyStatus::OK)
			return pc_can_fit;
	}
	if(cost > party->gold)
		return eBuyStatus::NEED_GOLD;
	return eBuyStatus::OK;
}

void cPlayer::take_item(int which_item) {
	if(which_item == INVENTORY_SIZE){
		// Taking the extra slot which is only for pre-stacking items
		items[INVENTORY_SIZE] = cItem();
		return;
	}
	if(weap_poisoned.slot == which_item && status[eStatus::POISONED_WEAPON] > 0) {
		if(print_result) print_result("  Poison lost.");
		status[eStatus::POISONED_WEAPON] = 0;
		weap_poisoned.clear();
	}
	if(weap_poisoned.slot > which_item && status[eStatus::POISONED_WEAPON] > 0)
		weap_poisoned.slot--;
	
	for(int i = which_item; i < INVENTORY_SIZE-1; i++) {
		items[i] = items[i + 1];
		equip[i] = equip[i + 1];
	}
	items[INVENTORY_SIZE-1] = cItem();
	equip[INVENTORY_SIZE-1] = false;
}

void cPlayer::remove_charge(int which_item) {
	if(items[which_item].charges > 0) {
		items[which_item].charges--;
		if(items[which_item].charges == 0 && !items[which_item].rechargeable) {
			take_item(which_item);
		}
	}
}

void cPlayer::finish_create() {
	// Start items
	switch(race) {
		case eRace::HUMAN:
			items[0] = cItem(ITEM_KNIFE);
			items[1] = cItem(ITEM_BUCKLER);
			break;
		case eRace::NEPHIL:
			items[0] = cItem(ITEM_BOW);
			items[1] = cItem(ITEM_ARROW);
			break;
		case eRace::SLITH:
			items[0] = cItem(ITEM_POLEARM);
			items[1] = cItem(ITEM_HELM);
			break;
		case eRace::VAHNATAI:
			items[0] = cItem(ITEM_ROBE);
			items[1] = cItem(ITEM_RAZORDISK);
			break;
		default: break; // Silence compiler warning
			// It's impossible to reach this point, anyway.
			// The only way to get a PC of other races is with the Create PC node,
			// which doesn't call this.
	}
	equip[0] = true;
	equip[1] = true;
	// Racial stat adjustments
	if(race == eRace::NEPHIL)
		skills[eSkill::DEXTERITY] += 2;
	if(race == eRace::SLITH) {
		skills[eSkill::STRENGTH] += 2;
		skills[eSkill::INTELLIGENCE] += 1;
	}
	if(race == eRace::VAHNATAI) {
		skills[eSkill::INTELLIGENCE] += 2;
		// The Vahnatai signature spells
		if(skills[eSkill::MAGE_SPELLS] >= 4)
			mage_spells[34] = mage_spells[35] = true;
	}
	// Bonus spell points for spellcasters
	max_sp += skills[eSkill::MAGE_SPELLS] * 3 + skills[eSkill::PRIEST_SPELLS] * 3;
	cur_sp = max_sp;
	// If they trained in mage spells but then decided to be Anama, give them back the skill points.
	// But do it here so that they still get the bonus spell points from that training.
	if(traits[eTrait::ANAMA] && skills[eSkill::MAGE_SPELLS] > 0) {
		skill_pts += 6 * skills[eSkill::MAGE_SPELLS];
		skills[eSkill::MAGE_SPELLS] = 0;
	}
}

cPlayer::cPlayer(cParty& party) : cPlayer(no_party) {
	this->party = &party;
	unique_id = party.next_pc_id++;
}

cPlayer::cPlayer(no_party_t) : weap_poisoned(*this) {
	main_status = eMainStatus::ABSENT;
	// This newline caused a bug that took ~16 hours to figure out...
	// So I'm leaving it in here, think of it as the bug's head on a spike.
	// name = "\n";
	name = "";
	
	skills[eSkill::STRENGTH] = 1;
	skills[eSkill::DEXTERITY] = 1;
	skills[eSkill::INTELLIGENCE] = 1;
	cur_health = 6;
	max_health = 6;
	cur_sp = 0;
	max_sp = 0;
	experience = 0;
	skill_pts = 65;
	level = 1;
	exp_adj = 100;
	std::fill(items.begin(), items.end(), cItem());
	equip.reset();
	
	priest_spells = basic_spells;
	mage_spells = basic_spells;
	which_graphic = 0;
	
	race = eRace::HUMAN;
	direction = DIR_N;
	combat_pos = {-1,-1};
}

cPlayer::cPlayer(cParty& party,ePartyPreset key,short slot) : cPlayer(party) {
	main_status = eMainStatus::ALIVE;
	unique_id = slot + 1000;
	party.next_pc_id = max(unique_id + 1, party.next_pc_id);
	if(key == PARTY_DEBUG){
		switch(slot) {
			case 0:
				name = "Gunther";
				break;
			case 1:
				name = "Yanni";
				break;
			case 2:
				name = "Mandolin";
				break;
			case 3:
				name = "Pete";
				break;
			case 4:
				name = "Vraiment";
				break;
			case 5:
				name = "Goo";
				break;
		}
		skills[eSkill::STRENGTH] = 20;
		skills[eSkill::DEXTERITY] = 20;
		skills[eSkill::INTELLIGENCE] = 20;
		for(short i = 3; i < 19; i++) {
			eSkill skill = eSkill(i);
			skills[skill] = 8;
		}
		cur_health = 60;
		max_health = 60;
		cur_sp = 90;
		max_sp = 90;
		experience = 0;
		skill_pts = 60;
		level = 1;
		exp_adj = 50;
		std::fill(items.begin(), items.end(), cItem());
		equip.reset();
		
		priest_spells.set();
		mage_spells.set();
		which_graphic = slot * 3 + 1;	// 1, 4, 7, 10, 13, 16
		if(slot == 2) which_graphic++;
		
		for(short i = 0; i < 10; i++) {
			eTrait trait = eTrait(i);
			traits[trait] = true;
		}
		
		race = eRace::HUMAN;
		direction = DIR_N;
	}else if(key == PARTY_DEFAULT){
		static std::map<eSkill, short> pc_stats[6] = {
			{
				{eSkill::STRENGTH,8}, {eSkill::DEXTERITY,6}, {eSkill::INTELLIGENCE,2},
				{eSkill::EDGED_WEAPONS,6}, {eSkill::ITEM_LORE,1}, {eSkill::ASSASSINATION,2},
			}, {
				{eSkill::STRENGTH,8}, {eSkill::DEXTERITY,7}, {eSkill::INTELLIGENCE,2},
				{eSkill::POLE_WEAPONS,6}, {eSkill::THROWN_MISSILES,3}, {eSkill::DEFENSE,3},
				{eSkill::POISON,2},
			}, {
				{eSkill::STRENGTH,8}, {eSkill::DEXTERITY,6}, {eSkill::INTELLIGENCE,2},
				{eSkill::EDGED_WEAPONS,2}, {eSkill::BASHING_WEAPONS,2}, {eSkill::ARCHERY,4},
				{eSkill::DISARM_TRAPS,4}, {eSkill::LOCKPICKING,4}, {eSkill::POISON,2}, {eSkill::LUCK,1},
			}, {
				{eSkill::STRENGTH,3}, {eSkill::DEXTERITY,2}, {eSkill::INTELLIGENCE,6},
				{eSkill::EDGED_WEAPONS,2}, {eSkill::THROWN_MISSILES,2},
				{eSkill::MAGE_SPELLS,3}, {eSkill::MAGE_LORE,3}, {eSkill::ITEM_LORE,1},
			}, {
				{eSkill::STRENGTH,2}, {eSkill::DEXTERITY,2}, {eSkill::INTELLIGENCE,6},
				{eSkill::EDGED_WEAPONS,3}, {eSkill::THROWN_MISSILES,2},
				{eSkill::MAGE_SPELLS,2}, {eSkill::PRIEST_SPELLS,1}, {eSkill::MAGE_LORE,4},
				{eSkill::LUCK,1},
			}, {
				{eSkill::STRENGTH,2}, {eSkill::DEXTERITY,2}, {eSkill::INTELLIGENCE,6},
				{eSkill::BASHING_WEAPONS,2}, {eSkill::THROWN_MISSILES,2}, {eSkill::DEFENSE,1},
				{eSkill::PRIEST_SPELLS,3}, {eSkill::MAGE_LORE,3}, {eSkill::ALCHEMY,2},
			},
		};
		static const short pc_health[6] = {22,24,24,16,16,18};
		static const short pc_sp[6] = {0,0,0,20,20,21};
		static const short pc_graphics[6] = {3,32,29,16,23,14};
		static const short pc_race[6] = {0,2,1,0,0,0};
		static const std::set<eTrait> pc_t[6] = {
			{eTrait::AMBIDEXTROUS, eTrait::GOOD_CONST, eTrait::MAGICALLY_INEPT},
			{eTrait::TOUGHNESS, eTrait::WOODSMAN, eTrait::SLUGGISH},
			{eTrait::NIMBLE, eTrait::FRAIL},
			{eTrait::MAGICALLY_APT},
			{eTrait::CAVE_LORE, eTrait::GOOD_CONST, eTrait::HIGHLY_ALERT, eTrait::BAD_BACK},
			{eTrait::MAGICALLY_APT},
		};
		
		main_status = eMainStatus::ALIVE;
		switch(slot) {
			case 0:
				name = "Jenneke";
				break;
			case 1:
				name = "Thissa";
				break;
			case 2:
				name = "Frrrrrr";
				break;
			case 3:
				name = "Adrianna";
				break;
			case 4:
				name = "Feodoric";
				break;
			case 5:
				name = "Michael";
				break;
				
		}
		
		for(short i = 0; i < 19; i++) {
			eSkill skill = eSkill(i);
			skills[skill] = pc_stats[slot][skill];
		}
		cur_health = pc_health[slot];
		max_health = pc_health[slot];
		experience = 0;
		skill_pts = 0;
		level = 1;
		
		std::fill(items.begin(), items.end(), cItem());
		equip.reset();
		cur_sp = pc_sp[slot];
		max_sp = pc_sp[slot];
		for(short i = 0; i < 15; i++) {
			eTrait trait = eTrait(i);
			traits[trait] = pc_t[slot].count(trait);
		}
		
		race = (eRace) pc_race[slot];
		direction = DIR_N;
		
		which_graphic = pc_graphics[slot];
	}
}

cPlayer::cPlayer(cParty& party, const cPlayer& other) : cPlayer(no_party, other) {
	this->party = &party;
	unique_id = party.next_pc_id++;
}

cPlayer::cPlayer(no_party_t, const cPlayer& other)
	: iLiving(other)
	, main_status(other.main_status)
	, name(other.name)
	, skills(other.skills)
	, max_health(other.max_health)
	, cur_health(other.cur_health)
	, max_sp(other.max_sp)
	, cur_sp(other.cur_sp)
	, experience(other.experience)
	, skill_pts(other.skill_pts)
	, level(other.level)
	, items(other.items)
	, equip(other.equip)
	, priest_spells(other.priest_spells)
	, mage_spells(other.mage_spells)
	, which_graphic(other.which_graphic)
	, weap_poisoned(*this, other.weap_poisoned.slot)
	, traits(other.traits)
	, race(other.race)
	, last_cast(other.last_cast)
	, combat_pos(other.combat_pos)
	, parry(other.parry)
	, last_attacked(nullptr)
{}

cPlayer::cPlayer(cPlayer&& other) : weap_poisoned(*this, other.weap_poisoned.slot) {
	swap(*this, other);
}

void swap(cPlayer& lhs, cPlayer& rhs) {
	using std::swap;
	// Don't swap the party reference!
	swap(lhs.main_status, rhs.main_status);
	swap(lhs.status, rhs.status);
	swap(lhs.name, rhs.name);
	swap(lhs.skills, rhs.skills);
	swap(lhs.max_health, rhs.max_health);
	swap(lhs.cur_health, rhs.cur_health);
	swap(lhs.max_sp, rhs.max_sp);
	swap(lhs.cur_sp, rhs.cur_sp);
	swap(lhs.experience, rhs.experience);
	swap(lhs.skill_pts, rhs.skill_pts);
	swap(lhs.level, rhs.level);
	swap(lhs.items, rhs.items);
	swap(lhs.equip, rhs.equip);
	swap(lhs.priest_spells, rhs.priest_spells);
	swap(lhs.mage_spells, rhs.mage_spells);
	swap(lhs.which_graphic, rhs.which_graphic);
	swap(lhs.weap_poisoned.slot, rhs.weap_poisoned.slot);
	swap(lhs.traits, rhs.traits);
	swap(lhs.race, rhs.race);
	swap(lhs.unique_id, rhs.unique_id);
	swap(lhs.last_cast, rhs.last_cast);
	swap(lhs.combat_pos, rhs.combat_pos);
	swap(lhs.parry, rhs.parry);
	swap(lhs.last_attacked, rhs.last_attacked);
}

void operator += (eMainStatus& stat, eMainStatus othr){
	if(othr == eMainStatus::SPLIT)
		stat = (eMainStatus) (10 + (int)stat);
	else if(stat == eMainStatus::SPLIT)
		stat = (eMainStatus) (10 + (int)othr);
}

void operator -= (eMainStatus& stat, eMainStatus othr){
	if(othr == eMainStatus::SPLIT)
		stat = (eMainStatus) (-10 + (int)stat);
	else if(stat == eMainStatus::SPLIT)
		stat = (eMainStatus) (-10 + (int)othr);
}

void cPlayer::writeTo(cTagFile& file) const {
	auto& page = file.add();
	page["UID"] << unique_id;
	page["STATUS"] << -1 << main_status;
	page["NAME"] << name;
	page["SKILL"] << eSkill::MAX_HP << max_health;
	if(max_sp > 0) {
		page["SKILL"] << eSkill::MAX_SP << max_sp;
	}
	page["SKILL"].encodeSparse(skills);
	page["HEALTH"] << cur_health;
	page["MANA"] << cur_sp;
	page["EXPERIENCE"] << experience;
	page["SKILLPTS"] << skill_pts;
	page["LEVEL"] << level;
	page["STATUS"].encodeSparse(status);
	if(exp_adj != 100) {
		page["EXPADJ"] << exp_adj;
	}
	for(int i = 0; i < equip.size(); i++) {
		if(equip[i]) {
			page["EQUIP"] << i;
		}
	}
	for(int i = 0; i < mage_spells.size(); i++) {
		if(mage_spells[i]) {
			page["MAGE"] << i;
		}
	}
	for(int i = 0; i < priest_spells.size(); i++) {
		if(priest_spells[i]) {
			page["PRIEST"] << i;
		}
	}
	auto traits = this->traits;
	for(int i = 0; i <= int(eTrait::ANAMA); i++) {
		eTrait trait = eTrait(i);
		if(traits[trait]) {
			page["TRAIT"] << i;
		}
	}
	page["ICON"] << which_graphic;
	page["RACE"] << race;
	page["DIRECTION"] << direction;
	if(weap_poisoned) {
		page["POISON"] << weap_poisoned.slot;
	}
	for(int i = 0; i < INVENTORY_SIZE; i++) {
		if(items[i].variety != eItemType::NO_ITEM) {
			auto& item_page = file.add();
			item_page["ITEM"] << i;
			items[i].writeTo(item_page);
		}
	}
}

void cPlayer::readFrom(const cTagFile& file, bool preview) {
	for(const auto& page : file) {
		if(page.index() == 0) {
			page["STATUS"] >> eStatus::MAIN >> main_status;
			page["LEVEL"] >> level;
			page["ICON"] >> which_graphic;

			if(preview)
				return;

			status.clear();
			page["STATUS"].extractSparse(status);
			status.erase(eStatus::MAIN);
			
			cur_sp = max_sp = ap = 0;
			page["NAME"] >> name;
			page["UID"] >> unique_id;
			page["HEALTH"] >> cur_health;
			page["MANA"] >> cur_sp;
			page["EXPERIENCE"] >> experience;
			page["SKILLPTS"] >> skill_pts;
			page["DIRECTION"] >> direction;
			page["RACE"] >> race;
			skills.clear();
			page["SKILL"].extractSparse(skills);
			max_health = skills[eSkill::MAX_HP];
			max_sp = skills[eSkill::MAX_SP];
			skills.erase(eSkill::MAX_HP);
			skills.erase(eSkill::MAX_SP);
			skills.erase(eSkill::CUR_HP);
			skills.erase(eSkill::CUR_SP);
			skills.erase(eSkill::CUR_XP);
			skills.erase(eSkill::CUR_LEVEL);
			skills.erase(eSkill::CUR_SKILL);
			skills.erase(eSkill::INVALID);
			
			if(page.contains("EXPADJ")) {
				page["EXPADJ"] >> exp_adj;
			} else exp_adj = 100;
			
			equip.reset();
			for(size_t n = 0; n < page["EQUIP"].size(); n++) {
				size_t i = equip.size();
				page["EQUIP"] >> i;
				if(i < equip.size()) equip[i] = true;
			}
			
			mage_spells.reset();
			for(size_t n = 0; n < page["MAGE"].size(); n++) {
				size_t i = mage_spells.size();
				page["MAGE"] >> i;
				if(i < mage_spells.size()) mage_spells[i] = true;
			}
			
			priest_spells.reset();
			for(size_t n = 0; n < page["PRIEST"].size(); n++) {
				size_t i = priest_spells.size();
				page["PRIEST"] >> i;
				if(i < priest_spells.size()) priest_spells[i] = true;
			}
			
			traits.clear();
			for(size_t n = 0; n < page["TRAIT"].size(); n++) {
				eTrait trait;
				page["TRAIT"] >> trait;
				traits[trait] = true;
			}
			
			weap_poisoned.clear();
			if(page.contains("POISON")) {
				page["POISON"] >> weap_poisoned.slot;
			}
			
			if(party) {
				// TODO: This probably belongs somewhere other than here
				party->next_pc_id = max(unique_id + 1, party->next_pc_id);
			}
		} else if(page.getFirstKey() == "ITEM") {
			size_t i = INVENTORY_SIZE;
			page["ITEM"] >> i;
			if(i >= INVENTORY_SIZE) continue;
			items[i].readFrom(page);
		}
	}
}