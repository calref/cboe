//
//  special-condition.cpp
//  Common
//
//  Created by Celtic Minstrel on 2025-02-20.
//

#include "special.hpp"

// Note: If adding a new node type below, be sure to adjust the end point here too.
node_category_info_t CAT_COND{eSpecType::IF_SDF, eSpecType::IF_QUEST};

namespace {
	node_properties_t S_SDF = node_builder_t(eSpecType::IF_SDF)
		.sdf()
		.ex1b(eSpecPicker::NODE)
		.ex2b(eSpecPicker::NODE);
	node_properties_t S_TOWN = node_builder_t(eSpecType::IF_TOWN_NUM)
		.ex1a(STRT_TOWN)
		.ex1b(eSpecPicker::NODE);
	node_properties_t S_RANDOM = node_builder_t(eSpecType::IF_RANDOM)
		.ex1b(eSpecPicker::NODE);
	node_properties_t S_SPECITEM = node_builder_t(eSpecType::IF_HAVE_SPECIAL_ITEM)
		.ex1a(STRT_SPEC_ITEM)
		.ex1b(eSpecPicker::NODE);
	node_properties_t S_SDFCMP = node_builder_t(eSpecType::IF_SDF_COMPARE)
		.sdf()
		.sdf(eSpecField::EX1A, eSpecField::EX1B)
		.ex2b(eSpecPicker::NODE);
	node_properties_t S_TERRAIN = node_builder_t(eSpecType::IF_TER_TYPE)
		.loc(eSpecField::EX1A, eSpecField::EX1B)
		.ex2a(STRT_TER)
		.ex2b(eSpecPicker::NODE);
	node_properties_t S_ALIVE = node_builder_t(eSpecType::IF_ALIVE)
		.ex1a(STRT_STATUS)
		.ex1b(eSpecPicker::NODE);
	node_properties_t S_GOLD = node_builder_t(eSpecType::IF_HAS_GOLD)
		.ex1b(eSpecPicker::NODE)
		.ex2a(eSpecPicker::TOGGLE);
	node_properties_t S_FOOD = node_builder_t(eSpecType::IF_HAS_FOOD)
		.ex1b(eSpecPicker::NODE)
		.ex2a(eSpecPicker::TOGGLE);
	node_properties_t S_ITEM_THERE = node_builder_t(eSpecType::IF_ITEM_CLASS_ON_SPACE)
		.loc(eSpecField::EX1A, eSpecField::EX1B)
		.ex2a(eSpecPicker::ITEM_CLASS)
		.ex2b(eSpecPicker::NODE)
		.ex2c(eSpecPicker::TOGGLE);
	node_properties_t S_ITEM_OWNED = node_builder_t(eSpecType::IF_HAVE_ITEM_CLASS)
		.ex1a(eSpecPicker::ITEM_CLASS)
		.ex1b(eSpecPicker::NODE)
		.ex2a(eSpecPicker::TOGGLE);
	node_properties_t S_ITEM_EQUIP = node_builder_t(eSpecType::IF_EQUIP_ITEM_CLASS)
		.ex1a(eSpecPicker::ITEM_CLASS)
		.ex1b(eSpecPicker::NODE)
		.ex2a(eSpecPicker::TOGGLE);
	node_properties_t S_MAGE = node_builder_t(eSpecType::IF_MAGE_SPELL)
		.ex1a(STRT_MAGE)
		.ex1b(eSpecPicker::NODE);
	node_properties_t S_PRIEST = node_builder_t(eSpecType::IF_PRIEST_SPELL)
		.ex1a(STRT_PRIEST)
		.ex1b(eSpecPicker::NODE);
	node_properties_t S_ALCHEMY = node_builder_t(eSpecType::IF_RECIPE)
		.ex1a(STRT_ALCHEMY)
		.ex1b(eSpecPicker::NODE);
	node_properties_t S_STATUS = node_builder_t(eSpecType::IF_STATUS)
		.ex1a(eSpecPicker::STATUS)
		.ex1b(eSpecPicker::NODE)
		.ex2b(STRT_ACCUM)
		.ex2c(STRT_CMP);
	node_properties_t S_LOOK = node_builder_t(eSpecType::IF_LOOKING)
		.ex1c(eSpecPicker::NODE);
	node_properties_t S_DAY = node_builder_t(eSpecType::IF_DAY_REACHED)
		.ex1b(eSpecPicker::NODE);
	node_properties_t S_FIELDS = node_builder_t(eSpecType::IF_FIELDS)
		.rect()
		.msg1(eSpecPicker::FIELD)
		.msg2(eSpecPicker::NODE);
	node_properties_t S_PARTY_SIZE = node_builder_t(eSpecType::IF_PARTY_SIZE)
		.ex1a(eSpecPicker::TOGGLE)
		.ex1b(eSpecPicker::NODE)
		.ex2a(eSpecPicker::TOGGLE);
	node_properties_t S_EVENT = node_builder_t(eSpecType::IF_EVENT_OCCURRED)
		.ex1b(eSpecPicker::EVENT)
		.ex2b(eSpecPicker::NODE);
	node_properties_t S_RACE = node_builder_t(eSpecType::IF_SPECIES)
		.ex1a(STRT_RACE)
		.ex1b(eSpecPicker::NODE)
		.ex2b(STRT_CMP);
	node_properties_t S_TRAIT = node_builder_t(eSpecType::IF_TRAIT)
		.ex1a(STRT_TRAIT)
		.ex1b(eSpecPicker::NODE)
		.ex2b(STRT_CMP);
	node_properties_t S_STAT = node_builder_t(eSpecType::IF_STATISTIC)
		.ex1b(eSpecPicker::NODE)
		.ex2a(+STRT_SKILL)
		.ex2b(STRT_ACCUM);
	node_properties_t S_TEXT = node_builder_t(eSpecType::IF_TEXT_RESPONSE)
		.msg1(+eSpecPicker::MSG_SINGLE)
		.ex1a(+eSpecPicker::MSG_SINGLE)
		.ex1b(eSpecPicker::NODE)
		.ex2a(+eSpecPicker::MSG_SINGLE)
		.ex2b(eSpecPicker::NODE);
	node_properties_t S_SDFEQ = node_builder_t(eSpecType::IF_SDF_EQ)
		.sdf()
		.ex1b(eSpecPicker::NODE);
	node_properties_t S_CONTEXT = node_builder_t(eSpecType::IF_CONTEXT)
		.ex1a(STRT_CONTEXT)
		.ex1b(eSpecPicker::TOGGLE)
		.ex1c(eSpecPicker::NODE);
	node_properties_t S_NUM = node_builder_t(eSpecType::IF_NUM_RESPONSE)
		.msg1(+eSpecPicker::MSG_SINGLE)
		.pict(STRT_CMP_MODE)
		.ptyp(eSpecPicker::NODE)
		.ex1b(STRT_CMP)
		.ex1c(eSpecPicker::NODE)
		.ex2b(STRT_CMP)
		.ex2c(eSpecPicker::NODE);
	node_properties_t S_BOAT = node_builder_t(eSpecType::IF_IN_BOAT)
		.ex1b(STRT_BOAT)
		.ex1c(eSpecPicker::NODE);
	node_properties_t S_HORSE = node_builder_t(eSpecType::IF_ON_HORSE)
		.ex1b(STRT_HORSE)
		.ex1c(eSpecPicker::NODE);
	node_properties_t S_QUEST = node_builder_t(eSpecType::IF_QUEST)
		.ex1a(STRT_QUEST)
		.ex1b(STRT_QUEST_STATUS)
		.ex1c(eSpecPicker::NODE);
}
