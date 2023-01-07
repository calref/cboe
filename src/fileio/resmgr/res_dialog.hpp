/*
 *  res_dialog.hpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20-01-07.
 *
 */

#ifndef BOE_RES_DIALOG_HPP
#define BOE_RES_DIALOG_HPP

#include <string>
#include "ticpp.h"
#include "resmgr.hpp"

struct DialogDefn {
	/// The dialog's ID is the base filename without the extension
	std::string id;
	/// The XML definition of the dialog.
	ticpp::Document defn;
};

using DialogRsrc = ResMgr::cPointer<DialogDefn>;

namespace ResMgr {
	extern cPool<DialogDefn> dialogs;
}

#endif
