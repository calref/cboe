//
//  dialogs.cpp
//  BoE
//
//  Created by Nat Nelson on 2023-01-06
//

#include "catch.hpp"
#include <boost/filesystem.hpp>
#include "fileio/resmgr/res_dialog.hpp"
#include "dialogxml/dialogs/dialog.hpp"

TEST_CASE("Construct each type of dialog in the engine") {
	fs::path dialogsPath = fs::current_path()/".."/"Blades of Exile"/"data"/"dialogs";
	fs::directory_iterator it{dialogsPath};
	auto end = fs::directory_iterator{};
	
	while (it != end) {
		fs::path path = it->path();
		std::string filename = path.stem().string();
		CAPTURE(filename);
		CHECK_NOTHROW({
			DialogDefn* defn = load_dialog_defn(path);
			cDialog dialog(*defn);
			delete defn;
		});
		
		++it;
	}
}