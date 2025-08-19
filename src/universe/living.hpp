//
//  living.hpp
//  BoE
//
//  Created by Celtic Minstrel on 15-01-24.
//
//

#ifndef BoE_living_hpp
#define BoE_living_hpp

#include <map>
#include <string>

#include "location.hpp"
#include "damage.hpp"
#include "spell.hpp"

enum class eSpellNote {
	NONE = 0,
	SCARED = 1, SLOWED = 2, WEAKENED = 3, POISONED = 4, CURSED = 5,
	RAVAGED = 6, UNDAMAGED = 7, STONED = 8, GAZES = 9, RESISTS = 10,
	DRAINS = 11, SHOOTS = 12, THROWS_SPEAR = 13, THROWS_ROCK = 14, THROWS_RAZORDISK = 15,
	HITS = 16, DISAPPEARS = 17, MISSES = 18, WEBBED = 19, CHOKES = 20,
	SUMMONED = 21, DUMBFOUNDED = 22, CHARMED = 23, RECORDED = 24, DISEASED = 25,
	AVATAR = 26, SPLITS = 27, ASLEEP = 28, AWAKE = 29, PARALYZED = 30,
	ACID = 31, SPINES = 32, SUMMONS = 33, CURED = 34, HASTED = 35,
	BLESSED = 36, CLEANS_WEBS = 37, FEEL_BETTER = 38, MIND_CLEAR = 39, ALERT = 40,
	HEALED = 41, DRAINED_HP = 42, SP_RECHARGED = 43, DRAINED_SP = 44, REVIVED = 45,
	DIES = 46, RALLIES = 47, CLEANS_ACID = 48, BREAKS_BARRIER = 49, BREAKS_FORCECAGE = 50,
	OBLITERATED = 51, TRAPPED = 52, THROWS_DART = 53, THROWS_KNIFE = 54, FIRES_RAY = 55,
	GAZES2 = 56, BREATHES_ON = 57, THROWS_WEB = 58, SPITS = 59, BREATHES = 60,
	HEAT_RAY = 61, DRAINED_XP = 62, PROTECTED = 63, KILLED = 64,
};

class iLiving {
public:
	// HACK: This is only really marked mutable so that I can use operator[] from const methods
	mutable std::map<eStatus,short> status;
	short ap = 0;
	eDirection direction = DIR_HERE;
	short marked_damage = 0; // for use during animations
	
	virtual bool is_alive() const = 0;
	virtual bool is_friendly() const = 0; // Return true if friendly to the party.
	virtual bool is_friendly(const iLiving& other) const = 0; // Return true if friendly to living entity
	virtual bool is_shielded() const = 0; // Checks for martyr's shield in any form - status, monster abil, item abil
	virtual int get_shared_dmg(int base_dmg) const = 0; // And this goes with the above.
	
	virtual void apply_status(eStatus which, int how_much);
	virtual void clear_brief_status();
	virtual void clear_bad_status();
	virtual void void_sanctuary();
	
	virtual void heal(int how_much) = 0;
	virtual void poison(int how_much) = 0;
	virtual void cure(int how_much) = 0;
	virtual void acid(int how_much) = 0;
	virtual void curse(int how_much) = 0;
	virtual void slow(int how_much) = 0;
	virtual void web(int how_much) = 0;
	virtual void disease(int how_much) = 0;
	virtual void dumbfound(int how_much) = 0;
	virtual void scare(int how_much) = 0;
	virtual void sleep(eStatus type, int how_much, int adj) = 0; // Also handles paralysis, charm, and forcecage
	virtual void avatar() = 0;
	virtual void drain_sp(int how_much, bool allow_resist) = 0;
	virtual void restore_sp(int how_much) = 0;
	
	virtual int get_health() const = 0;
	virtual int get_magic() const = 0;
	virtual int get_level() const = 0;
	virtual location get_loc() const = 0;
	virtual std::string get_name() const = 0;
	
	void spell_note(eSpellNote note) const;
	void cast_spell_note(eSpell spell) const;
	void print_attacks(const iLiving& target) const;
	void damaged_msg(int how_much, int extra) const;
	
	iLiving();
	iLiving(const iLiving& other);
	virtual ~iLiving() = default;
	
	static void(* print_result)(std::string);
};

#endif
