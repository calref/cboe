//
//  special-town.cpp
//  Common
//
//  Created by Celtic Minstrel on 2025-02-20.
//

#include "special.hpp"
#include "special-conditions.hpp"

// Note: If adding a new node type below, be sure to adjust the end point here too.
node_category_info_t CAT_TOWN{eSpecType::MAKE_TOWN_HOSTILE, eSpecType::TOWN_PLACE_LABEL};

namespace {
	node_properties_t S_ATTITUDE = node_builder_t(eSpecType::MAKE_TOWN_HOSTILE)
		.msg()
		.ex2a(STRT_ATTITUDE);
	node_properties_t S_MISSILE = node_builder_t(eSpecType::TOWN_RUN_MISSILE)
		.msg()
		.pict(PIC_MISSILE)
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::ACTIVE_TOWN)
		.loc(eSpecField::EX2A, eSpecField::EX2B, eLocType::ACTIVE_TOWN)
		.ex1c(STRT_PATH)
		.ex2c(eSpecPicker::SOUND);
	node_properties_t S_ATTACK = node_builder_t(eSpecType::TOWN_MONST_ATTACK)
		.msg()
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::ACTIVE_TOWN);
	node_properties_t S_BOOM = node_builder_t(eSpecType::TOWN_BOOM_SPACE)
		.msg()
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::ACTIVE_TOWN)
		.ex2a(PIC_BOOM)
		.ex2c(eSpecPicker::SOUND);
	node_properties_t S_TELEPORT = node_builder_t(eSpecType::TOWN_MOVE_PARTY)
		.msg()
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::ACTIVE_TOWN)
		.ex2a(eSpecPicker::TOGGLE);
	node_properties_t S_HIT = node_builder_t(eSpecType::TOWN_HIT_SPACE)
		.msg()
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::ACTIVE_TOWN)
		.ex2b(eSpecPicker::DAMAGE_TYPE);
	node_properties_t S_EXPLODE = node_builder_t(eSpecType::TOWN_EXPLODE_SPACE)
		.msg()
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::ACTIVE_TOWN)
		.ex2b(eSpecPicker::DAMAGE_TYPE);
	node_properties_t S_LOCK = node_builder_t(eSpecType::TOWN_LOCK_SPACE)
		.msg()
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::ACTIVE_TOWN);
	node_properties_t S_UNLOCK = node_builder_t(eSpecType::TOWN_UNLOCK_SPACE)
		.msg()
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::ACTIVE_TOWN);
	node_properties_t S_BURST = node_builder_t(eSpecType::TOWN_SFX_BURST)
		.msg()
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::ACTIVE_TOWN)
		.ex2a(eSpecPicker::EXPLOSION)
		.ex2b(eSpecPicker::TOGGLE)
		.ex2c(eSpecPicker::SOUND);
	node_properties_t S_WANDER = node_builder_t(eSpecType::TOWN_CREATE_WANDERING)
		.msg();
	node_properties_t S_SPAWN = node_builder_t(eSpecType::TOWN_PLACE_MONST)
		.msg()
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::ACTIVE_TOWN)
		.ex2a(STRT_MONST)
		.ex2b(eSpecPicker::TOGGLE);
	node_properties_t S_KILL = node_builder_t(eSpecType::TOWN_DESTROY_MONST)
		.msg()
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::ACTIVE_TOWN);
	node_properties_t S_NUKE = node_builder_t(eSpecType::TOWN_NUKE_MONSTS)
		.msg()
		.ex1a(STRT_MONST);
	node_properties_t S_LEVER_G = node_builder_t(eSpecType::TOWN_GENERIC_LEVER)
		.ex1b(eSpecPicker::NODE);
	node_properties_t S_PORTAL_G = node_builder_t(eSpecType::TOWN_GENERIC_PORTAL)
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::ACTIVE_TOWN);
	node_properties_t S_BUTTON_G = node_builder_t(eSpecType::TOWN_GENERIC_BUTTON)
		.ex1b(eSpecPicker::NODE);
	node_properties_t S_STAIR_G = node_builder_t(eSpecType::TOWN_GENERIC_STAIR)
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::SPECIFIED_TOWN, eSpecField::EX2A)
		.ex2a(STRT_TOWN)
		.ex2b(STRT_STAIR)
		.ex2c(STRT_STAIR_MODE)
		.jump(eSpecPicker::NONE);
	node_properties_t S_LEVER = node_builder_t(eSpecType::TOWN_LEVER)
		.msg1(eSpecPicker::MSG_SEQUENCE)
		.pic()
		.ex1b(eSpecPicker::NODE);
	node_properties_t S_PORTAL = node_builder_t(eSpecType::TOWN_PORTAL)
		.msg1(eSpecPicker::MSG_SEQUENCE)
		.pic()
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::ACTIVE_TOWN);
	node_properties_t S_STAIR = node_builder_t(eSpecType::TOWN_STAIR)
		.msg1(eSpecPicker::MSG_SEQUENCE)
		.pic()
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::SPECIFIED_TOWN, eSpecField::EX2A)
		.ex2a(STRT_TOWN)
		.ex2b(eSpecPicker::TOGGLE)
		.ex2c(STRT_STAIR_MODE)
		.jump(eSpecPicker::NONE);
	node_properties_t S_OUTDOOR = node_builder_t(eSpecType::TOWN_RELOCATE)
		.msg()
		.field_pair(eSpecField::EX1A, eSpecField::EX1B, STRT_SECTOR)
		.loc(eSpecField::EX2A, eSpecField::EX2B, eLocType::SPECIFIED_OUT, eSpecField::EX1A);
	node_properties_t S_ITEM = node_builder_t(eSpecType::TOWN_PLACE_ITEM)
		.msg()
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::ACTIVE_TOWN)
		.ex2a(STRT_ITEM)
		.ex2b(eSpecPicker::TOGGLE);
	node_properties_t S_SPLIT = node_builder_t(eSpecType::TOWN_SPLIT_PARTY)
		.msg()
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::ACTIVE_TOWN)
		.ex2a(eSpecPicker::SOUND);
	node_properties_t S_REUNITE = node_builder_t(eSpecType::TOWN_REUNITE_PARTY)
		.msg()
		.ex1a(eSpecPicker::SOUND)
		.ex2a(eSpecPicker::TOGGLE);
	node_properties_t S_TIMER = node_builder_t(eSpecType::TOWN_TIMER_START)
		.msg()
		.ex1b(eSpecPicker::NODE);
	node_properties_t S_LIGHT = node_builder_t(eSpecType::TOWN_CHANGE_LIGHTING)
		.msg()
		.ex1a(STRT_LIGHT)
		.ex2b(eSpecPicker::TOGGLE);
	node_properties_t S_CHARM = node_builder_t(eSpecType::TOWN_SET_ATTITUDE)
		.msg()
		.ex1b(STRT_ATTITUDE);
	node_properties_t S_CAMERA = node_builder_t(eSpecType::TOWN_SET_CENTER)
		.msg();
	node_properties_t S_FOG = node_builder_t(eSpecType::TOWN_LIFT_FOG)
		.msg()
		.ex1a(eSpecPicker::TOGGLE);
	node_properties_t S_TARGET = node_builder_t(eSpecType::TOWN_START_TARGETING)
		.msg()
		.ex1a(eSpecPicker::SPELL_PATTERN)
		.ex2a(eSpecPicker::NODE)
		.ex2b(eSpecPicker::TOGGLE)
		.ex2c(eSpecPicker::TOGGLE);
	node_properties_t S_FIELDS = node_builder_t(eSpecType::TOWN_SPELL_PAT_FIELD)
		.msg()
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::ACTIVE_TOWN)
		.ex1c(+eSpecPicker::SPELL_PATTERN)
		.ex2a(+eSpecPicker::FIELD);
	node_properties_t S_PATTERN = node_builder_t(eSpecType::TOWN_SPELL_PAT_BOOM)
		.msg()
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::ACTIVE_TOWN)
		.ex1c(+eSpecPicker::SPELL_PATTERN)
		.ex2a(eSpecPicker::DAMAGE_TYPE)
		.ex2c(STRT_SPELL_PAT_MODE);
	node_properties_t S_WARP = node_builder_t(eSpecType::TOWN_RELOCATE_CREATURE)
		.msg()
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::ACTIVE_TOWN)
		.ex2b(STRT_POS_MODE);
	node_properties_t S_LABEL = node_builder_t(eSpecType::TOWN_PLACE_LABEL)
		.msg1(eSpecPicker::MSG_SINGLE)
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::ACTIVE_TOWN)
		.ex2a(STRT_LABEL_ALIGN);
}
