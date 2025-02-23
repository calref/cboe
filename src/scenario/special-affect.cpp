//
//  special-affect.cpp
//  Common
//
//  Created by Celtic Minstrel on 2025-02-20.
//

#include "special.hpp"

// Note: If adding a new node type below, be sure to adjust the end point here too.
node_category_info_t CAT_AFFECT{eSpecType::SELECT_TARGET, eSpecType::UNSTORE_PC};

namespace {
	node_properties_t S_SELECT = node_builder_t(eSpecType::SELECT_TARGET)
		.msg()
		.ex1b(eSpecPicker::NODE);
	node_properties_t S_DAMAGE = node_builder_t(eSpecType::DAMAGE)
		.msg()
		.ex2b(eSpecPicker::DAMAGE_TYPE)
		.ex2c(eSpecPicker::SOUND);
	node_properties_t S_HEALTH = node_builder_t(eSpecType::AFFECT_HP)
		.msg();
	node_properties_t S_MANA = node_builder_t(eSpecType::AFFECT_SP)
		.msg();
	node_properties_t S_EXP = node_builder_t(eSpecType::AFFECT_XP)
		.msg();
	node_properties_t S_SKILLPT = node_builder_t(eSpecType::AFFECT_SKILL_PTS)
		.msg();
	node_properties_t S_KILL = node_builder_t(eSpecType::AFFECT_DEADNESS)
		.msg()
		.ex1a(STRT_STATUS);
	node_properties_t S_STATUS = node_builder_t(eSpecType::AFFECT_STATUS)
		.msg()
		.ex1c(eSpecPicker::STATUS);
	node_properties_t S_TRAIT = node_builder_t(eSpecType::AFFECT_TRAITS)
		.msg()
		.ex1a(STRT_TRAIT);
	node_properties_t S_ACTIONS = node_builder_t(eSpecType::AFFECT_AP)
		.msg();
	node_properties_t S_NAME = node_builder_t(eSpecType::AFFECT_NAME)
		.msg()
		.msg3(eSpecPicker::MSG_SINGLE);
	node_properties_t S_LEVEL = node_builder_t(eSpecType::AFFECT_LEVEL)
		.msg();
	node_properties_t S_MORALE = node_builder_t(eSpecType::AFFECT_MORALE)
		.msg();
	node_properties_t S_CRYSTAL = node_builder_t(eSpecType::AFFECT_SOUL_CRYSTAL)
		.msg();
	node_properties_t S_EQUIP = node_builder_t(eSpecType::GIVE_ITEM)
		.msg()
		.pict(eSpecPicker::NODE)
		.ex1a(STRT_ITEM)
		.ex1b(STRT_ENCHANT);
	node_properties_t S_TARGET = node_builder_t(eSpecType::AFFECT_MONST_TARG)
		.msg();
	node_properties_t S_ATTACK = node_builder_t(eSpecType::AFFECT_MONST_ATT)
		.msg();
	node_properties_t S_STAT_M = node_builder_t(eSpecType::AFFECT_MONST_STAT)
		.msg()
		.ex2a(STRT_MONST_STAT);
	node_properties_t S_STAT_P = node_builder_t(eSpecType::AFFECT_STAT)
		.msg()
		.ex2a(STRT_SKILL);
	node_properties_t S_MAGE = node_builder_t(eSpecType::AFFECT_MAGE_SPELL)
		.msg()
		.ex1a(STRT_MAGE);
	node_properties_t S_PRIEST = node_builder_t(eSpecType::AFFECT_PRIEST_SPELL)
		.msg()
		.ex1a(STRT_PRIEST);
	node_properties_t S_GOLD = node_builder_t(eSpecType::AFFECT_GOLD)
		.msg();
	node_properties_t S_FOOD = node_builder_t(eSpecType::AFFECT_FOOD)
		.msg();
	node_properties_t S_ALCHEMY = node_builder_t(eSpecType::AFFECT_ALCHEMY)
		.msg()
		.ex1a(STRT_ALCHEMY);
	node_properties_t S_STATUS_PARTY = node_builder_t(eSpecType::AFFECT_PARTY_STATUS)
		.msg()
		.ex1c(eSpecPicker::STATUS_PARTY);
	node_properties_t S_NEWPC = node_builder_t(eSpecType::CREATE_NEW_PC)
		.sdf()
		.msg()
		.msg3(eSpecPicker::MSG_SINGLE)
		.pict(PIC_PC)
		.ptyp(eSpecPicker::NODE)
		.ex1c(STRT_RACE);
	node_properties_t S_STOREPC = node_builder_t(eSpecType::STORE_PC)
		.sdf()
		.msg();
	node_properties_t S_UNSTOREPC = node_builder_t(eSpecType::UNSTORE_PC)
		.sdf()
		.msg()
		.ex1b(eSpecPicker::NODE);
}
