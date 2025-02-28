//
//  scen.locpicker.cpp
//  BoE
//
//  Created by Celtic Minstrel on 2025-02-28.
//

#include "scen.locpicker.hpp"

#include "dialogxml/widgets/pict.hpp"
#include "dialogxml/widgets/tilemap.hpp"
#include "scenario/area.hpp"
#include "scenario/scenario.hpp"
#include "fileio/resmgr/res_dialog.hpp"

extern cScenario scenario;

cLocationPicker::cLocationPicker(location loc, cArea& area, const std::string& title, cDialog* parent)
	: initial_loc(loc)
	, chosen_loc(loc)
	, area(&area)
	, dlog(*ResMgr::dialogs.get("choose-location"), parent)
{
	clamp_loc();
	map = dynamic_cast<cTilemap*>(&dlog["map"]);
	fill_terrain();
	handle_scroll(""); // Hide unusable scroll buttons
	dlog["switch"].hide();
	setTitle(title);
	using namespace std::placeholders;
	dlog.attachClickHandlers(std::bind(&cLocationPicker::handle_close, this, _2), {"done", "cancel"});
	dlog.attachClickHandlers(std::bind(&cLocationPicker::handle_scroll, this, _2), {"up", "down", "left", "right"});
	map->attachClickHandlers(std::bind(&cLocationPicker::handle_select, this, _2), "");
}

void cLocationPicker::clamp_loc() {
	chosen_loc.x = minmax(0, area->max_dim - 1, chosen_loc.x);
	chosen_loc.y = minmax(0, area->max_dim - 1, chosen_loc.y);
	viewport.x = 18 * floor(chosen_loc.x / 18.0);
	viewport.y = 18 * floor(chosen_loc.y / 18.0);
}

void cLocationPicker::setArea(cArea& newArea) {
	area = &newArea;
	clamp_loc();
	fill_terrain();
}

void cLocationPicker::setTitle(const std::string& title) {
	dlog["prompt"].setText(title);
}

void cLocationPicker::fill_terrain() {
	for(int x = 0; x < 18; x++) {
		for(int y = 0; y < 18; y++) {
			auto& pict = dynamic_cast<cPict&>(map->getChild(x, y));
			location ter_loc = viewport;
			ter_loc.x += x;
			ter_loc.y += y;
			if(ter_loc.x >= area->max_dim || ter_loc.y >= area->max_dim) pict.setPict(74);
			else {
				ter_num_t ter = area->terrain[ter_loc.x][ter_loc.y];
				pict.setPict(scenario.ter_types[ter].map_pic);
			}
		}
	}
	place_pointer();
}

void cLocationPicker::place_pointer() {
	if(chosen_loc.x >= viewport.x && chosen_loc.x < viewport.x + 18 && chosen_loc.y >= viewport.y && chosen_loc.y < viewport.y + 18) {
		location relative_loc = chosen_loc;
		relative_loc.x -= viewport.x;
		relative_loc.y -= viewport.y;
		location ctrlPos = map->getCellPos(relative_loc.x, relative_loc.y);
		ctrlPos.x += 3;
		ctrlPos.y += 3;
		dlog["zpointer"].relocate(ctrlPos);
		dlog["zpointer"].show();
	} else {
		dlog["zpointer"].hide();
	}
}

bool cLocationPicker::handle_close(std::string item_hit) {
	dlog.toast(item_hit == "done");
	return true;
}

bool cLocationPicker::handle_scroll(std::string item_hit) {
	if(item_hit == "up") {
		if(viewport.y > 0) viewport.y -= 9;
	} else if(item_hit == "down") {
		if(viewport.y < area->max_dim - 18) viewport.y += 9;
	} else if(item_hit == "left") {
		if(viewport.x > 0) viewport.x -= 9;
	} else if(item_hit == "right") {
		if(viewport.x < area->max_dim - 18) viewport.x += 9;
	}
	if(viewport.x == 0) dlog["left"].hide();
	else dlog["left"].show();
	if(viewport.y == 0) dlog["up"].hide();
	else dlog["up"].show();
	if(viewport.x >= area->max_dim - 18) dlog["right"].hide();
	else dlog["right"].show();
	if(viewport.y >= area->max_dim - 18) dlog["down"].hide();
	else dlog["down"].show();
	fill_terrain();
	return true;
}

bool cLocationPicker::handle_select(std::string item_hit) {
	location clickedLoc = map->getCellPos(dlog[item_hit]);
	location check{viewport.x + clickedLoc.x / 24, viewport.y + clickedLoc.y / 24};
	if(check.x < area->max_dim && check.y < area->max_dim) {
		chosen_loc = check;
		place_pointer();
	}
	return true;
}

location cLocationPicker::run() {
	dlog.run();
	if(dlog.accepted()) return chosen_loc;
	return initial_loc;
}

cDialog* cLocationPicker::operator->() {
	return &dlog;
}
