/*
 *  res_dialog.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20-01-07.
 *
 */

#include "res_dialog.hpp"
#include <iostream>
#include <fstream>

// Redeclare this instead of including "fileio.h"
extern std::ostream& std_fmterr(std::ostream& out);

class DialogLoader : public ResMgr::cLoader<DialogDefn> {
	/// Load a dialog definition from an XML file.
	DialogDefn* operator() (const fs::path& fpath) const override {
		TiXmlBase::SetCondenseWhiteSpace(false);
		ticpp::Document xml(fpath.string().c_str());
		try {
			xml.LoadFile();
		} catch(ticpp::Exception& e) {
			std::cerr << "Error reading XML file: " << e.what();
			throw ResMgr::xError(ResMgr::ERR_LOAD, "Failed to load dialog: " + fpath.string());
		}
		return new DialogDefn{fpath.stem().string(), std::move(xml)};
	}

	ResourceList expand(const std::string& name) const override {
		return {name + ".xml"};
	}

	std::string typeName() const override {
		return "dialog definition";
	}
};

// TODO: What's a good max dialogs count?
static DialogLoader loader;
ResMgr::cPool<DialogDefn> ResMgr::dialogs(loader, 80);
