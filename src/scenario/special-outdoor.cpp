//
//  special-outdoor.cpp
//  Common
//
//  Created by Celtic Minstrel on 2025-02-20.
//

#include "special.hpp"

// Note: If adding a new node type below, be sure to adjust the end point here too.
node_category_info_t CAT_OUTD{eSpecType::OUT_MAKE_WANDER, eSpecType::OUT_MOVE_PARTY};

namespace {
	node_properties_t S_WANDER = node_builder_t(eSpecType::OUT_MAKE_WANDER);
	node_properties_t S_TOWN = node_builder_t(eSpecType::OUT_FORCE_TOWN)
		.msg()
		.loc(eSpecField::EX2A, eSpecField::EX2B, eLocType::SPECIFIED_TOWN, eSpecField::EX1A)
		.ex1a(STRT_TOWN)
		.ex1b(STRT_DIR);
	node_properties_t S_ENCOUNTER = node_builder_t(eSpecType::OUT_PLACE_ENCOUNTER)
		.msg();
	node_properties_t S_TELEPORT = node_builder_t(eSpecType::OUT_MOVE_PARTY)
		.msg()
		.loc(eSpecField::EX1A, eSpecField::EX1B, eLocType::ACTIVE_OUT);
}
