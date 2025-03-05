//
//  scen.sdfpicker.hpp
//  BoE Scenario Editor
//
//  Created by Celtic Minstrel on 2025-03-02.
//

#ifndef BoE_scen_sdfpicker_h
#define BoE_scen_sdfpicker_h

#include <string>
#include "location.hpp"
#include "dialogxml/dialogs/dialog.hpp"

class cTilemap;

class cStuffDonePicker {
	cDialog dlog;
	// Note: x and y in initial_sdf and chosen_sdf are (c, r), but the flag array and name map
	// are indexed by [r][c]. Remember to flip the order when necessary.
	location initial_sdf, chosen_sdf, viewport;
	cTilemap* grid;
	cTilemap* row_labels;
	cTilemap* col_labels;
	int rows, cols;
	bool skipNextFocus = true;
	void clamp_sdf();
	void fill_names();
	void save_names();
	void select_active();
	bool handle_close(std::string item_hit);
	bool handle_scroll(std::string item_hit);
	bool handle_select(bool losing);
	bool handle_focus(std::string item_hit, bool losing);
public:
	cStuffDonePicker(location sdf);
	location run();
};

#endif
