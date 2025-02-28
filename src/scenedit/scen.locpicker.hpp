//
//  scen.locpicker.hpp
//  BoE
//
//  Created by Celtic Minstrel on 2025-02-28.
//

#ifndef BoE_scen_locpicker_h
#define BoE_scen_locpicker_h

#include <string>
#include "location.hpp"
#include "dialogxml/dialogs/dialog.hpp"

class cArea;
class cTilemap;

class cLocationPicker {
	location initial_loc, chosen_loc, viewport;
	cArea* area;
	cDialog dlog;
	cTilemap* map;
	void clamp_loc();
	void fill_terrain();
	void place_pointer();
	bool handle_close(std::string item_hit);
	bool handle_scroll(std::string item_hit);
	bool handle_select(std::string item_hit);
public:
	cLocationPicker(location loc, cArea& area, const std::string& title, cDialog* parent);
	void setArea(cArea& newArea) ;
	void setTitle(const std::string& title);
	location run();
	cDialog* operator->();
};

#endif
