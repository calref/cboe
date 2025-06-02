#include "scen.undo.hpp"

#include "scenario/scenario.hpp"
#include "scenario/area.hpp"

extern bool editing_town;
extern short cur_town;
extern short cur_town;
extern location cur_out;
extern short cen_x;
extern short cen_y;
extern cScenario scenario;
extern cArea* get_current_area();
extern void start_town_edit();
extern void start_out_edit();
extern void redraw_screen();
extern void set_current_town(int,bool first_restore = false);

cTerrainAction::cTerrainAction(std::string name, short town_num, location where) : cAction(name) {
	area.is_town = true;
	area.town_num = town_num;
	area.where = where;
}
cTerrainAction::cTerrainAction(std::string name, location out_sec, location where) : cAction(name) {
	area.is_town = false;
	area.out_sec = out_sec;
	area.where = where;
}
cTerrainAction::cTerrainAction(std::string name, location where) : cAction(name) {
	area.is_town = editing_town;
	// One of these two will be ignored
	area.town_num = cur_town;
	area.out_sec = cur_out;
	
	area.where = where;
}

void cTerrainAction::showChangeSite() {
	if(area.is_town){
		cur_town = area.town_num;
	}else{
		cur_out = area.out_sec;
	}
	editing_town = area.is_town;

	// TODO this isn't working and I don't know why:
	cen_x = area.where.x;
	cen_y = area.where.y;
	redraw_screen();
}

void cTerrainAction::undo() {
	showChangeSite();
	cAction::undo();
}
void cTerrainAction::redo() {
	showChangeSite();
	cAction::redo();
}

// Undo/Redo implementations for actions:

bool aEraseSpecial::undo_me() {
	cArea* cur_area = get_current_area();
	auto& specials = cur_area->special_locs;
	specials.push_back(for_redo);
	return true;
}
bool aEraseSpecial::redo_me() {
	cArea* cur_area = get_current_area();
	auto& specials = cur_area->special_locs;
	specials.pop_back();
	return true;
}

aCreateDeleteTown::aCreateDeleteTown(bool create, cTown* t)
	: cAction(create ? "Create Town" : "Delete Last Town", !create)
	, theTown(t)
{}

bool aCreateDeleteTown::undo_me() {
	scenario.towns.resize(scenario.towns.size() - 1);
	set_current_town(min(cur_town, scenario.towns.size() - 1));
	return true;
}

bool aCreateDeleteTown::redo_me() {
	scenario.towns.push_back(theTown);
	set_current_town(scenario.towns.size() - 1);
	return true;
}

aCreateDeleteTown::~aCreateDeleteTown() {
	if(isDone() == reversed) delete theTown;
}

bool aDrawTerrain::undo_me() {
	cArea* cur_area = get_current_area();
	for(auto change : changes){
		cur_area->terrain(change.first.x, change.first.y) = change.second.old_num;
	}
	return true;
}
bool aDrawTerrain::redo_me() {
	cArea* cur_area = get_current_area();
	for(auto change : changes){
		cur_area->terrain(change.first.x, change.first.y) = change.second.new_num;
	}
	return true;
}
