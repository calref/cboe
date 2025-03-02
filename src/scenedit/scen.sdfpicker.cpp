//
//  scen.sdfpicker.cpp
//  BoE Scenario Editor
//
//  Created by Celtic Minstrel on 2025-03-02.
//

#include "scen.sdfpicker.hpp"

#include "dialogxml/widgets/field.hpp"
#include "dialogxml/widgets/ledgroup.hpp"
#include "dialogxml/widgets/tilemap.hpp"
#include "fileio/resmgr/res_dialog.hpp"
#include "scenario/scenario.hpp"
#include "sounds.hpp"

extern cScenario scenario;

cStuffDonePicker::cStuffDonePicker(location sdf)
	: dlog(*ResMgr::dialogs.get("choose-sdf"))
	, initial_sdf(sdf)
	, chosen_sdf(sdf)
{
	grid = dynamic_cast<cTilemap*>(&dlog["map"]);
	row_labels = dynamic_cast<cTilemap*>(&dlog["rows"]);
	col_labels = dynamic_cast<cTilemap*>(&dlog["cols"]);
	rows = grid->getNumRows();
	cols = grid->getNumCols();
	clamp_sdf();
	fill_names();
	handle_scroll(""); // Hide unusable scroll buttons
	using namespace std::placeholders;
	dlog.attachClickHandlers(std::bind(&cStuffDonePicker::handle_close, this, _2), {"done", "cancel"});
	dlog.attachClickHandlers(std::bind(&cStuffDonePicker::handle_scroll, this, _2), {"up", "down", "left", "right"});
	grid->attachFocusHandlers(std::bind(&cStuffDonePicker::handle_focus, this, _2, _3), "name");
	dlog["choice"].attachFocusHandler(std::bind(&cStuffDonePicker::handle_select, this, _3));
}

bool cStuffDonePicker::handle_close(std::string item_hit) {
	dlog.toast(item_hit == "done");
	return true;
}

bool cStuffDonePicker::handle_scroll(std::string item_hit) {
	if(!item_hit.empty()) save_names();
	if(item_hit == "up") {
		if(viewport.y > 0) viewport.y -= rows;
	} else if(item_hit == "down") {
		if(viewport.y < scenario.sdf_names[0].size() - rows) viewport.y += rows;
	} else if(item_hit == "left") {
		if(viewport.x > 0) viewport.x -= cols;
	} else if(item_hit == "right") {
		if(viewport.x < scenario.sdf_names.size() - cols) viewport.x += cols;
	}
	if(viewport.x == 0) dlog["left"].hide();
	else dlog["left"].show();
	if(viewport.y == 0) dlog["up"].hide();
	else dlog["up"].show();
	if(viewport.x >= scenario.sdf_names.size() - cols) dlog["right"].hide();
	else dlog["right"].show();
	if(viewport.y >= scenario.sdf_names[0].size() - rows) dlog["down"].hide();
	else dlog["down"].show();
	fill_names();
	if(!item_hit.empty()) select_active();
	return true;
}

bool cStuffDonePicker::handle_select(bool losing) {
	if(losing) return true;
	std::string item_hit = dynamic_cast<cLedGroup&>(dlog["choice"]).getSelected();
	location clickedFlag = grid->getCellIdx(dlog[item_hit]);
	chosen_sdf = {viewport.x + clickedFlag.x, viewport.y + clickedFlag.y};
	auto& field = dynamic_cast<cTextField&>(grid->getChild("name", clickedFlag.x, clickedFlag.y));
	skipNextFocus = true;
	dlog.setFocus(&field);
	return true;
}

bool cStuffDonePicker::handle_focus(std::string item_hit, bool losing) {
	if(losing) return true;
	if(skipNextFocus) {
		skipNextFocus = false;
		return true;
	}
	auto& field = dlog[item_hit];
	if(!field.getText().empty()) return true;
	location clickedFlag = grid->getCellIdx(field);
	chosen_sdf = {viewport.x + clickedFlag.x, viewport.y + clickedFlag.y};
	play_sound(34);
	auto& group = dynamic_cast<cLedGroup&>(dlog["choice"]);
	// TODO: It would be nice to do it like this, but getChild currently doesn't recurse to children of children.
	// auto& led = grid->getChild("select", clickedFlag.x, clickedFlag.y);
	// group.setSelected(led.getName());
	auto name = field.getName();
	auto hyphen = name.find_last_of("-");
	group.setSelected("select" + name.substr(hyphen));
	return true;
}

location cStuffDonePicker::run() {
	skipNextFocus = true;
	dlog.run([this](cDialog&) { select_active(); });
	save_names();
	if(dlog.accepted()) return chosen_sdf;
	return initial_sdf;
}

void cStuffDonePicker::clamp_sdf() {
	chosen_sdf.x = minmax(0, scenario.sdf_names.size() - 1, chosen_sdf.x);
	chosen_sdf.y = minmax(0, scenario.sdf_names[0].size() - 1, chosen_sdf.y);
	viewport.x = cols * floor(chosen_sdf.x / float(cols));
	viewport.y = rows * floor(chosen_sdf.y / float(rows));
}

void cStuffDonePicker::fill_names() {
	for(int x = 0; x < cols; x++) {
		for(int y = 0; y < rows; y++) {
			auto& field = grid->getChild("name", x, y);
			location sdf = viewport;
			sdf.x += x;
			sdf.y += y;
			field.setText(scenario.sdf_names[sdf.x][sdf.y]);
			if(x == 0) {
				// Add row labels
				row_labels->getChild("row", 0, y).setTextToNum(sdf.y);
			}
			if(y == 0) {
				// Add column labels
				col_labels->getChild("col", x, 0).setTextToNum(sdf.x);
			}
		}
	}
}

void cStuffDonePicker::save_names() {
	for(int x = 0; x < cols; x++) {
		for(int y = 0; y < rows; y++) {
			auto& field = grid->getChild("name", x, y);
			location sdf = viewport;
			sdf.x += x;
			sdf.y += y;
			scenario.sdf_names[sdf.x][sdf.y] = field.getText();
		}
	}
}

void cStuffDonePicker::select_active() {
	auto& group = dynamic_cast<cLedGroup&>(dlog["choice"]);
	if(chosen_sdf.x >= viewport.x && chosen_sdf.x < viewport.x + cols && chosen_sdf.y >= viewport.y && chosen_sdf.y < viewport.y + rows) {
		location relative_sdf = chosen_sdf;
		relative_sdf.x -= viewport.x;
		relative_sdf.y -= viewport.y;
		auto& field = dynamic_cast<cTextField&>(grid->getChild("name", relative_sdf.x, relative_sdf.y));
		// TODO: It would be nice to do it like this, but getChild currently doesn't recurse to children of children.
		// auto& led = grid->getChild("select", relative_sdf.x, relative_sdf.y);
		// group.setSelected(led.getName());
		auto name = field.getName();
		auto hyphen = name.find_last_of("-");
		group.setSelected("select" + name.substr(hyphen));
		dlog.setFocus(&field);
	} else {
		group.setSelected("");
	}
}
