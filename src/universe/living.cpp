//
//  living.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-01-24.
//
//

#include "living.hpp"

#include <algorithm>
#include <tuple>
#include <fmt/format.h>
#include "mathutil.hpp"

void iLiving::apply_status(eStatus which, int how_much) {
	if(!is_alive()) return;
	
	int lo, hi;
	std::tie(lo, hi) = status_bounds(which);
	
	if(which == eStatus::ASLEEP || which == eStatus::DUMB) {
		// No "wrapping" allowed for these effects.
		if(status[which] < 0) hi = 0;
		else if(status[which] >= 0) lo = 0;
	}
	
	status[which] = minmax(lo,hi,status[which] + how_much);
}

void iLiving::clear_bad_status() {
	std::map<eStatus, short> old;
	status.swap(old);
	std::remove_copy_if(old.begin(), old.end(), std::inserter(status, status.begin()), [](std::pair<const eStatus, short> kv) {
		return (*kv.first).isNegative ? kv.second > 0 : kv.second < 0;
	});
}

void iLiving::clear_brief_status() {
	std::map<eStatus, short> old;
	status.swap(old);
	std::remove_copy_if(old.begin(), old.end(), std::inserter(status, status.begin()), [](std::pair<const eStatus, short> kv) -> bool {
		if(kv.first == eStatus::POISON) return false;
		if(kv.first == eStatus::DISEASE) return false;
		if(kv.first == eStatus::DUMB) return false;
		if(kv.first == eStatus::ACID && kv.second > 2)
			return false;
		return true;
	});
}

void iLiving::void_sanctuary() {
	if(status[eStatus::INVISIBLE] > 0)
		status[eStatus::INVISIBLE] = 0;
}

iLiving::iLiving() {}

iLiving::iLiving(const iLiving& other)
	: status(other.status)
	, ap(other.ap)
	, direction(other.direction)
	, marked_damage(other.marked_damage)
{}

void(* iLiving::print_result)(std::string) = nullptr;

void iLiving::spell_note(eSpellNote which_mess) const {
	if(!print_result) return;
	std::string msg;
	switch(which_mess) {
		case eSpellNote::SCARED:
			msg = "  {} scared.";
			break;
			
		case eSpellNote::SLOWED:
			msg = "  {} slowed.";
			break;
			
		case eSpellNote::WEAKENED:
			msg = "  {} weakened.";
			break;
			
		case eSpellNote::POISONED:
			msg = "  {} poisoned.";
			break;
			
		case eSpellNote::CURSED:
			msg = " {} cursed.";
			break;
			
		case eSpellNote::RAVAGED:
			msg = "  {} ravaged.";
			break;
			
		case eSpellNote::UNDAMAGED:
			msg = "  {} undamaged.";
			break;
			
		case eSpellNote::STONED:
			msg = "  {} is stoned.";
			break;
		case eSpellNote::GAZES:
			msg = "  Gazes at {}.";
			break;
		case eSpellNote::RESISTS:
			msg = "  {} resists.";
			break;
		case eSpellNote::DRAINS:
			msg = "  Drains {}.";
			break;
		case eSpellNote::SHOOTS:
			msg = "  Shoots at {}.";
			break;
		case eSpellNote::THROWS_SPEAR:
			msg = "  Throws spear at {}.";
			break;
		case eSpellNote::THROWS_ROCK:
			msg = "  Throws rock at {}.";
			break;
		case eSpellNote::THROWS_RAZORDISK:
			msg = "  Throws razordisk at {}.";
			break;
		case eSpellNote::HITS:
			msg = "  Hits {}.";
			break;
		case eSpellNote::DISAPPEARS:
			msg = "  {} disappears.";
			break;
		case eSpellNote::MISSES:
			msg = "  Misses {}.";
			break;
		case eSpellNote::WEBBED:
			msg = "  {} is webbed.";
			break;
		case eSpellNote::CHOKES:
			msg = "  {} chokes.";
			break;
		case eSpellNote::SUMMONED:
			msg = "  {} summoned.";
			break;
		case eSpellNote::DUMBFOUNDED:
			msg = "  {} is dumbfounded.";
			break;
		case eSpellNote::CHARMED:
			msg = "  {} is charmed.";
			break;
		case eSpellNote::RECORDED:
			msg = "  {} is recorded.";
			break;
		case eSpellNote::DISEASED:
			msg = "  {} is diseased.";
			break;
		case eSpellNote::AVATAR:
			msg = "  {} is an avatar!";
			break;
		case eSpellNote::SPLITS:
			msg = "  {} splits!";
			break;
		case eSpellNote::ASLEEP:
			msg = "  {} falls asleep.";
			break;
		case eSpellNote::AWAKE:
			msg = "  {} wakes up.";
			break;
		case eSpellNote::PARALYZED:
			msg = "  {} paralyzed.";
			break;
		case eSpellNote::ACID:
			msg = "  {} covered with acid.";
			break;
		case eSpellNote::SPINES:
			msg = "  Fires spines at {}.";
			break;
		case eSpellNote::SUMMONS:
			msg = "  {} summons aid.";
			break;
		case eSpellNote::CURED:
			msg = "  {} is cured.";
			break;
		case eSpellNote::HASTED:
			msg = "  {} is hasted.";
			break;
		case eSpellNote::BLESSED:
			msg = "  {} is blessed.";
			break;
		case eSpellNote::CLEANS_WEBS:
			msg = "  {} cleans webs.";
			break;
		case eSpellNote::FEEL_BETTER:
			msg = "  {} feels better.";
			break;
		case eSpellNote::MIND_CLEAR:
			msg = "  {} mind cleared.";
			break;
		case eSpellNote::ALERT:
			msg = "  {} feels alert.";
			break;
		case eSpellNote::HEALED:
			msg = "  {} is healed.";
			break;
		case eSpellNote::DRAINED_HP:
			msg = "  {} drained of health.";
			break;
		case eSpellNote::SP_RECHARGED:
			msg = "  {} magic recharged.";
			break;
		case eSpellNote::DRAINED_SP:
			msg = "  {} drained of magic.";
			break;
		case eSpellNote::REVIVED:
			msg = "  {} returns to life!";
			break;
		case eSpellNote::DIES:
			msg = "  {} dies.";
			break;
		case eSpellNote::RALLIES:
			msg = "  {} rallies its courage.";
			break;
		case eSpellNote::CLEANS_ACID:
			msg = "  {} cleans off acid.";
			break;
		case eSpellNote::BREAKS_BARRIER:
			msg = "  {} breaks barrier.";
			break;
		case eSpellNote::BREAKS_FORCECAGE:
			msg = "  {} breaks force cage.";
			break;
		case eSpellNote::OBLITERATED:
			msg = "  {} is obliterated!";
			break;
		case eSpellNote::TRAPPED:
			msg = "  {} is trapped!";
			break;
		case eSpellNote::THROWS_DART:
			msg = "  Throws dart at {}.";
			break;
		case eSpellNote::THROWS_KNIFE:
			msg = "  Throws knife at {}.";
			break;
		case eSpellNote::FIRES_RAY:
			msg = "  Fires ray at {}.";
			break;
		case eSpellNote::GAZES2:
			msg = "  Gazes at {}.";
			break;
		case eSpellNote::BREATHES_ON:
			msg = "  Breathes on {}.";
			break;
		case eSpellNote::THROWS_WEB:
			msg = "  Throws web at {}.";
			break;
		case eSpellNote::SPITS:
			msg = "  Spits at {}.";
			break;
		case eSpellNote::BREATHES:
			msg = "  {} breaths.";
			break;
		default:
			msg += "{}: Unknown action " + std::to_string(int(which_mess));
	}
	
	if(which_mess != eSpellNote::NONE)
		print_result(fmt::format(msg, get_name()));
}

void iLiving::print_attacks(const iLiving& target) const {
	if(!print_result) return;
	std::string target_name;
	if(&target == this) {
		target_name = "themself";
	} else target_name = target.get_name();
	print_result(fmt::format("{} attacks {}", get_name(), target_name));
}

void iLiving::cast_spell_note(eSpell spell) const {
	if(!print_result) return;
	print_result(fmt::format("{} casts:", get_name()));
	print_result(fmt::format("  {}", (*spell).name()));
}

void iLiving::damaged_msg(int how_much,int how_much_spec) const {
	if(!print_result) return;
	if(how_much_spec > 0)
		print_result(fmt::format("  {} takes {}+{}", get_name(), how_much, how_much_spec));
	else print_result(fmt::format("  {} takes {}", get_name(), how_much));
}
