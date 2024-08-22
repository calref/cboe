//
//  creature.hpp
//  BoE
//
//  Created by Celtic Minstrel on 15-01-24.
//
//

#ifndef BoE_creature_hpp
#define BoE_creature_hpp

#include <iosfwd>
#include "location.hpp"
#include "scenario/monster.hpp"
#include "living.hpp"

class cTagFile_Page;

enum class eCreatureStatus { DEAD, IDLE, ALERTED };

class cCreature : public cMonster, public cTownperson, public iLiving {
public:
	static const short charm_odds[21];
	eCreatureStatus active = eCreatureStatus::DEAD;
	eAttitude attitude;
	location cur_loc;
	short summon_time = 0;
	bool party_summoned = false;
	short target = 6;
	location targ_loc;
	short health = 0;
	short mp = 0;
	short max_mp = 0;
	short morale = 0,m_morale = 0; // these are calculated in-game based on the level
	
	cCreature();
	cCreature(int num);
	
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
	
	int get_health() const override;
	int get_magic() const override;
	int get_level() const override;
	
	bool is_alive() const override;
	bool is_friendly() const override;
	bool is_friendly(const iLiving& other) const override;
	bool is_shielded() const override;
	int get_shared_dmg(int base_dmg) const override;
	location get_loc() const override;
	
	int magic_adjust(int base);
	
	void spell_note(int which) const;
	void cast_spell_note(eSpell spell) const;
	void print_attacks(iLiving* target) const;
	void breathe_note() const;
	void damaged_msg(int how_much, int extra) const;
	void killed_msg() const;
	bool on_space(location loc) const;
	
	void import_legacy(legacy::creature_data_type old);
	void writeTo(cTagFile_Page& file) const;
	void readFrom(const cTagFile_Page& file);
};

std::ostream& operator<< (std::ostream& out, eCreatureStatus status);
std::istream& operator>> (std::istream& in, eCreatureStatus& status);

#endif
