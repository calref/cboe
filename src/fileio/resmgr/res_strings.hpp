/*
 *  restypes.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 10-08-25.
 *
 */

#ifndef BOE_RES_STRINGS_HPP
#define BOE_RES_STRINGS_HPP

#include <vector>
#include <string>
#include "resmgr.hpp"

using StringList = std::vector<std::string>;
using StringRsrc = ResMgr::cPointer<StringList>;

namespace ResMgr {
	extern cPool<StringList> strings;
}

#endif
