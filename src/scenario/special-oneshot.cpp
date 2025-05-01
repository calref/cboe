//
//  special-oneshot.cpp
//  Common
//
//  Created by Celtic Minstrel on 2025-02-19.
//

#include "special.hpp"
#include "special-conditions.hpp"

// Note: If adding a new node type below, be sure to adjust the end point here too.
node_category_info_t CAT_ONCE{eSpecType::ONCE_GIVE_ITEM, eSpecType::ONCE_TRAP};

namespace {
	node_properties_t S_GIVE_ITEM = node_builder_t(eSpecType::ONCE_GIVE_ITEM)
		.sdf()
		.msg()
		.ex1a(STRT_ITEM)
		.ex2b(eSpecPicker::NODE);
	node_properties_t S_GIVE_SPECITEM = node_builder_t(eSpecType::ONCE_GIVE_SPEC_ITEM)
		.sdf()
		.msg()
		.ex1a(STRT_SPEC_ITEM)
		.ex1b(eSpecPicker::TOGGLE);
	node_properties_t S_NONE = node_builder_t(eSpecType::ONCE_NULL)
		.sdf()
		.no_preview();
	node_properties_t S_SETSDF = node_builder_t(eSpecType::ONCE_SET_SDF)
		.sdf()
		.no_preview();
	node_properties_t S_MSG = node_builder_t(eSpecType::ONCE_DISPLAY_MSG)
		.sdf()
		.msg();
	node_properties_t S_DIALOG = node_builder_t(eSpecType::ONCE_DIALOG)
		.sdf()
		.msg1(eSpecPicker::MSG_SEQUENCE)
		.msg3(eSpecPicker::TOGGLE)
		.pic()
		.ex1a(STRT_BUTTON)
		.ex2a(STRT_BUTTON)
		.ex1b(eSpecPicker::NODE)
		.ex2b(eSpecPicker::NODE);
	// TODO implement preview
	node_properties_t S_ITEM_DIALOG = node_builder_t(eSpecType::ONCE_GIVE_ITEM_DIALOG)
		.sdf()
		.msg()
		.msg3(STRT_SPEC_ITEM)
		.pic()
		.ex1a(STRT_ITEM)
		.ex2b(eSpecPicker::NODE)
		.no_preview();
	node_properties_t S_OUTENC = node_builder_t(eSpecType::ONCE_OUT_ENCOUNTER)
		.sdf()
		.msg();
	node_properties_t S_TOWNENV = node_builder_t(eSpecType::ONCE_TOWN_ENCOUNTER)
		.sdf()
		.msg();
	// TODO implement preview
	node_properties_t S_TRAP = node_builder_t(eSpecType::ONCE_TRAP)
		.sdf()
		.msg()
		.pic()
		.ex1a(STRT_TRAP)
		.when(eSpecField::EX1A == 13, 1)
			.ex2b(+eSpecPicker::NODE)
		.end();
}
