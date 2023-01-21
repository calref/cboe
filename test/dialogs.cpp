//
//  dialogs.cpp
//  BoE
//
//  Created by Nat Nelson on 2023-01-06
//

#include "catch.hpp"
#include <boost/filesystem.hpp>
#include "fileio/resmgr/res_dialog.hpp"
#include "fileio/resmgr/res_font.hpp"
#include "fileio/resmgr/res_image.hpp"
#include "dialogxml/dialogs/dialog.hpp"

static void load_dialog(fs::path path) {
	DialogDefn* defn = nullptr;
	try {
		defn = load_dialog_defn(path);
		cDialog dialog(*defn);
	} catch(...) {
		if(defn != nullptr) delete defn;
		throw;
	}
}

TEST_CASE("Construct each type of dialog in the engine") try {
	ResMgr::fonts.pushPath(fs::current_path()/".."/"rsrc"/"fonts");
	ResMgr::graphics.pushPath(fs::current_path()/".."/"rsrc"/"graphics");
	fs::path dialogsPath = fs::current_path()/".."/"rsrc"/"dialogs";
	
	for(fs::directory_iterator it{dialogsPath}; it != fs::directory_iterator{}; ++it) {
		fs::path path = it->path();
		if(path.extension() != ".xml") continue;
		std::string filename = path.stem().string();
		CAPTURE(filename);
		CHECK_NOTHROW(load_dialog(path));
	}
} catch(...) {
	ResMgr::fonts.popPath();
	ResMgr::graphics.popPath();
	throw;
}
