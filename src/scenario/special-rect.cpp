//
//  special-rect.cpp
//  Common
//
//  Created by Celtic Minstrel on 2025-02-20.
//

#include "special.hpp"

// Note: If adding a new node type below, be sure to adjust the end point here too.
node_category_info_t CAT_RECT{eSpecType::RECT_PLACE_FIELD, eSpecType::RECT_UNLOCK};

namespace {
	node_properties_t S_FIELDS = node_builder_t(eSpecType::RECT_PLACE_FIELD)
		.msg()
		.rect()
		.sdf2(+eSpecPicker::FIELD);
	node_properties_t S_EXPLORE = node_builder_t(eSpecType::RECT_SET_EXPLORED)
		.msg()
		.rect();
	node_properties_t S_MOVE = node_builder_t(eSpecType::RECT_MOVE_ITEMS)
		.msg()
		.rect()
		.loc(eSpecField::SDF1, eSpecField::SDF2);
	node_properties_t S_DESTROY = node_builder_t(eSpecType::RECT_DESTROY_ITEMS)
		.msg()
		.rect();
	node_properties_t S_CHANGE = node_builder_t(eSpecType::RECT_CHANGE_TER)
		.msg()
		.rect()
		.sdf1(STRT_TER);
	node_properties_t S_SWAP = node_builder_t(eSpecType::RECT_SWAP_TER)
		.msg()
		.rect()
		.sdf1(STRT_TER)
		.sdf2(STRT_TER);
	node_properties_t S_TRANS = node_builder_t(eSpecType::RECT_TRANS_TER)
		.msg()
		.rect();
	node_properties_t S_LOCK = node_builder_t(eSpecType::RECT_LOCK)
		.msg()
		.rect();
	node_properties_t S_UNLOCK = node_builder_t(eSpecType::RECT_UNLOCK)
		.msg()
		.rect();
}
