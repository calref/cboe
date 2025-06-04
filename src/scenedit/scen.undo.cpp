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

cTerrainAction::cTerrainAction(std::string name, short town_num, location where, bool reversed) : cAction(name, reversed) {
	area.is_town = true;
	area.town_num = town_num;
	area.where = where;
}
cTerrainAction::cTerrainAction(std::string name, location out_sec, location where, bool reversed) : cAction(name, reversed) {
	area.is_town = false;
	area.out_sec = out_sec;
	area.where = where;
}
cTerrainAction::cTerrainAction(std::string name, location where, bool reversed) : cAction(name, reversed) {
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

bool aCreateDeleteTerrain::undo_me() {
	// TODO if not in MODE_EDIT_TYPES, show it
	for(cTerrain ter : terrains){
		scenario.ter_types.pop_back();
	}
	return true;
}

bool aCreateDeleteTerrain::redo_me() {
	// TODO if not in MODE_EDIT_TYPES, show it
	for(cTerrain ter : terrains){
		scenario.ter_types.push_back(ter);
	}
	return true;
}

bool aCreateDeleteMonster::undo_me() {
	// TODO if not in MODE_EDIT_TYPES, show it
	for(cMonster monst : monsters){
		scenario.scen_monsters.pop_back();
	}
	return true;
}

bool aCreateDeleteMonster::redo_me() {
	// TODO if not in MODE_EDIT_TYPES, show it
	for(cMonster monst : monsters){
		scenario.scen_monsters.push_back(monst);
	}
	return true;
}

bool aCreateDeleteItem::undo_me() {
	// TODO if not in MODE_EDIT_TYPES, show it
	for(cItem item : items){
		scenario.scen_items.pop_back();
	}
	return true;
}

bool aCreateDeleteItem::redo_me() {
	// TODO if not in MODE_EDIT_TYPES, show it
	for(cItem item : items){
		scenario.scen_items.push_back(item);
	}
	return true;
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

aPlaceEraseItem::aPlaceEraseItem(std::string name, bool place, size_t index, cTown::cItem item)
	: cTerrainAction(name, item.loc, !place)
	, items({{index, item}})
{}

aPlaceEraseItem::aPlaceEraseItem(std::string name, bool place, item_changes_t items)
	: cTerrainAction(name, items.begin()->second.loc, !place)
	, items(items)
{}

bool aPlaceEraseItem::undo_me() {
	auto& town_items = scenario.towns[cur_town]->preset_items;
	for(auto change : items){
		town_items[change.first].code = -1;
	}
	return true;
}

bool aPlaceEraseItem::redo_me() {
	auto& town_items = scenario.towns[cur_town]->preset_items;
	for(auto change : items){
		town_items[change.first] = change.second;
	}
	return true;
}

aPlaceEraseCreature::aPlaceEraseCreature(std::string name, bool place, size_t index, cTownperson creature)
	: cTerrainAction(name, creature.start_loc, !place)
	, creatures({{index, creature}})
{}

aPlaceEraseCreature::aPlaceEraseCreature(std::string name, bool place, creature_changes_t creatures)
	: cTerrainAction(name, creatures.begin()->second.start_loc, !place)
	, creatures(creatures)
{}

bool aPlaceEraseCreature::undo_me() {
	auto& town_creatures = scenario.towns[cur_town]->creatures;
	for(auto change : creatures){
		town_creatures[change.first].number = 0;
	}
	return true;
}

bool aPlaceEraseCreature::redo_me() {
	auto& town_creatures = scenario.towns[cur_town]->creatures;
	for(auto change : creatures){
		town_creatures[change.first] = change.second;
	}
	return true;
}
