//
//  skills_traits.hpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-13.
//
//

#ifndef BoE_SKILLS_TRAITS_HPP
#define BoE_SKILLS_TRAITS_HPP

enum class eSkill {
	INVALID = -1,
	STRENGTH = 0,
	DEXTERITY = 1,
	INTELLIGENCE = 2,
	EDGED_WEAPONS = 3,
	BASHING_WEAPONS = 4,
	POLE_WEAPONS = 5,
	THROWN_MISSILES = 6,
	ARCHERY = 7,
	DEFENSE = 8,
	MAGE_SPELLS = 9,
	PRIEST_SPELLS = 10,
	MAGE_LORE = 11,
	ALCHEMY = 12,
	ITEM_LORE = 13,
	DISARM_TRAPS = 14,
	LOCKPICKING = 15,
	ASSASSINATION = 16,
	POISON = 17,
	LUCK = 18,
	MAX_HP = 19,
	MAX_SP = 20,
	// Magic values; only for check_party_stat()
	CUR_HP = 100,
	CUR_SP = 101,
	CUR_XP = 102,
	CUR_SKILL = 103,
	CUR_LEVEL = 104,
};

enum class eTrait {
	TOUGHNESS = 0,
	MAGICALLY_APT = 1,
	AMBIDEXTROUS = 2,
	NIMBLE = 3,
	CAVE_LORE = 4,
	WOODSMAN = 5,
	GOOD_CONST = 6,
	HIGHLY_ALERT = 7,
	STRENGTH = 8,
	RECUPERATION = 9,
	SLUGGISH = 10,
	MAGICALLY_INEPT = 11,
	FRAIL = 12,
	CHRONIC_DISEASE = 13,
	BAD_BACK = 14,
	PACIFIST = 15,
	ANAMA = 16,
};

std::ostream& operator << (std::ostream& out, eSkill e);
std::istream& operator >> (std::istream& in, eSkill& e);
std::ostream& operator << (std::ostream& out, eTrait e);
std::istream& operator >> (std::istream& in, eTrait& e);

#endif
