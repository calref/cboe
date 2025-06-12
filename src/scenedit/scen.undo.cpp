#include "scen.undo.hpp"

#include "scenario/scenario.hpp"
#include "scenario/area.hpp"
#include "scen.actions.hpp"

extern bool editing_town;
extern short cur_town;
extern cTown* town;
extern location cur_out;
extern short cen_x;
extern short cen_y;
extern cScenario scenario;
extern cArea* get_current_area();
extern void start_town_edit();
extern void start_out_edit();
extern void redraw_screen();
extern void set_current_town(int,bool first_restore = false);
extern eScenMode overall_mode;
extern eDrawMode draw_mode;

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
	// if not in MODE_EDIT_TYPES, show it
	if(!(overall_mode == MODE_EDIT_TYPES && draw_mode == DRAW_TERRAIN)){
		start_type_editing(DRAW_TERRAIN);
		// TODO Go to scroll maximum
	}
	for(cTerrain ter : terrains){
		scenario.ter_types.pop_back();
	}
	return true;
}

bool aCreateDeleteTerrain::redo_me() {
	// if not in MODE_EDIT_TYPES, show it
	if(!(overall_mode == MODE_EDIT_TYPES && draw_mode == DRAW_TERRAIN)){
		start_type_editing(DRAW_TERRAIN);
		// TODO Go to scroll maximum
	}
	for(cTerrain ter : terrains){
		scenario.ter_types.push_back(ter);
	}
	return true;
}

bool aEditClearTerrain::undo_me() {
	// if not in MODE_EDIT_TYPES, show it
	if(!(overall_mode == MODE_EDIT_TYPES && draw_mode == DRAW_TERRAIN)){
		start_type_editing(DRAW_TERRAIN);
		// TODO scroll to show the type
	}
	scenario.ter_types[which] = before;
	return true;
}

bool aEditClearTerrain::redo_me() {
	// if not in MODE_EDIT_TYPES, show it
	if(!(overall_mode == MODE_EDIT_TYPES && draw_mode == DRAW_TERRAIN)){
		start_type_editing(DRAW_TERRAIN);
		// TODO scroll to show the type
	}
	scenario.ter_types[which] = after;
	return true;
}

bool aEditClearMonster::undo_me() {
	// if not in MODE_EDIT_TYPES, show it
	if(!(overall_mode == MODE_EDIT_TYPES && draw_mode == DRAW_MONST)){
		start_type_editing(DRAW_MONST);
		// TODO scroll to show the type
	}
	scenario.scen_monsters[which] = before;
	return true;
}

bool aEditClearMonster::redo_me() {
	// if not in MODE_EDIT_TYPES, show it
	if(!(overall_mode == MODE_EDIT_TYPES && draw_mode == DRAW_MONST)){
		start_type_editing(DRAW_MONST);
		// TODO scroll to show the type
	}
	scenario.scen_monsters[which] = after;
	return true;
}

bool aEditClearItem::undo_me() {
	// if not in MODE_EDIT_TYPES, show it
	if(!(overall_mode == MODE_EDIT_TYPES && draw_mode == DRAW_ITEM)){
		start_type_editing(DRAW_ITEM);
		// TODO scroll to show the type
	}
	scenario.scen_items[which] = before;
	return true;
}

bool aEditClearItem::redo_me() {
	// if not in MODE_EDIT_TYPES, show it
	if(!(overall_mode == MODE_EDIT_TYPES && draw_mode == DRAW_ITEM)){
		start_type_editing(DRAW_ITEM);
		// TODO scroll to show the type
	}
	scenario.scen_items[which] = after;
	return true;
}

bool aCreateDeleteMonster::undo_me() {
	// if not in MODE_EDIT_TYPES, show it
	if(!(overall_mode == MODE_EDIT_TYPES && draw_mode == DRAW_MONST)){
		start_type_editing(DRAW_MONST);
		// TODO Go to scroll maximum
	}
	for(cMonster monst : monsters){
		scenario.scen_monsters.pop_back();
	}
	return true;
}

bool aCreateDeleteMonster::redo_me() {
	// if not in MODE_EDIT_TYPES, show it
	if(!(overall_mode == MODE_EDIT_TYPES && draw_mode == DRAW_MONST)){
		start_type_editing(DRAW_MONST);
		// TODO Go to scroll maximum
	}
	for(cMonster monst : monsters){
		scenario.scen_monsters.push_back(monst);
	}
	return true;
}

bool aCreateDeleteItem::undo_me() {
	// if not in MODE_EDIT_TYPES, show it
	if(!(overall_mode == MODE_EDIT_TYPES && draw_mode == DRAW_ITEM)){
		start_type_editing(DRAW_ITEM);
		// TODO Go to scroll max
	}
	for(cItem item : items){
		scenario.scen_items.pop_back();
	}
	return true;
}

bool aCreateDeleteItem::redo_me() {
	// if not in MODE_EDIT_TYPES, show it
	if(!(overall_mode == MODE_EDIT_TYPES && draw_mode == DRAW_ITEM)){
		start_type_editing(DRAW_ITEM);
		// TODO Go to scroll max
	}
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

bool aPlaceEraseVehicle::undo_me() {
	auto& all = is_boat ? scenario.boats : scenario.horses;
	all[which] = cVehicle();
	return true;
}

bool aPlaceEraseVehicle::redo_me() {
	auto& all = is_boat ? scenario.boats : scenario.horses;
	all[which] = vehicle;
	return true;
}

bool aEditVehicle::undo_me() {
	auto& all = is_boat ? scenario.boats : scenario.horses;
	all[which] = old_vehicle;
	return true;
}

bool aEditVehicle::redo_me() {
	auto& all = is_boat ? scenario.boats : scenario.horses;
	all[which] = new_vehicle;
	return true;
}

bool aEditSignText::undo_me() {
	cArea* cur_area = get_current_area();
	auto& signs = cur_area->sign_locs;
	auto iter = std::find(signs.begin(), signs.end(), area.where);
	iter->text = old_text;
	return true;
}

bool aEditSignText::redo_me() {
	cArea* cur_area = get_current_area();
	auto& signs = cur_area->sign_locs;
	auto iter = std::find(signs.begin(), signs.end(), area.where);
	iter->text = new_text;
	return true;
}

// cTerrainAction makes sure current_terrain properly references the outdoor section where the edit happened
bool aEditTownEntrance::undo_me() {
	set_town_entrance(area.where, old_town);
	return true;
}

bool aEditTownEntrance::redo_me() {
	set_town_entrance(area.where, new_town);
	return true;
}

bool aClearProperty::undo_me() {
	for(size_t i = 0; i < town->preset_items.size(); ++i){
		town->preset_items[i].property = old_property[i];
	}
	return true;
}

bool aClearProperty::redo_me() {
	for(cTown::cItem& item : town->preset_items){
		item.property = false;
	}
	return true;
}

bool aEditPlacedItem::undo_me() {
	town->preset_items[which] = old_item;
	return true;
}

bool aEditPlacedItem::redo_me() {
	town->preset_items[which] = new_item;
	return true;
}

bool aEditPlacedCreature::undo_me() {
	town->creatures[which] = old_creature;
	return true;
}

bool aEditPlacedCreature::redo_me() {
	town->creatures[which] = new_creature;
	return true;
}

bool aCreateDeleteSpecialItem::undo_me() {
	// If not editing special items, show it
	if(overall_mode != MODE_EDIT_SPECIAL_ITEMS){
		start_special_item_editing();
		// TODO Go to scroll maximum
	}
	scenario.special_items.pop_back();
	return true;
}

bool aCreateDeleteSpecialItem::redo_me() {
	// If not editing special items, show it
	if(overall_mode != MODE_EDIT_SPECIAL_ITEMS){
		start_special_item_editing();
		// TODO Go to scroll maximum
	}
	scenario.special_items.push_back(item);
	return true;
}

bool aCreateDeleteQuest::undo_me() {
	// If not editing quests, show it
	if(overall_mode != MODE_EDIT_QUESTS){
		start_quest_editing();
		// TODO Go to scroll maximum
	}
	scenario.quests.pop_back();
	return true;
}

bool aCreateDeleteQuest::redo_me() {
	// If not editing quests, show it
	if(overall_mode != MODE_EDIT_QUESTS){
		start_quest_editing();
		// TODO Go to scroll maximum
	}
	scenario.quests.push_back(quest);
	return true;
}

bool aCreateDeleteShop::undo_me() {
	// If not editing shops, show it
	if(overall_mode != MODE_EDIT_SHOPS){
		start_shops_editing();
		// TODO Go to scroll maximum
	}
	scenario.shops.pop_back();
	return true;
}

bool aCreateDeleteShop::redo_me() {
	// If not editing shops, show it
	if(overall_mode != MODE_EDIT_SHOPS){
		start_shops_editing();
		// TODO Go to scroll maximum
	}
	scenario.shops.push_back(shop);
	return true;
}

bool aEditClearSpecialItem::undo_me() {
	// If not editing special items, show it
	if(overall_mode != MODE_EDIT_SPECIAL_ITEMS){
		start_special_item_editing();
		// TODO scroll to show the item
	}
	scenario.special_items[which] = before;
	return true;
}

bool aEditClearSpecialItem::redo_me() {
	// If not editing special items, show it
	if(overall_mode != MODE_EDIT_SPECIAL_ITEMS){
		start_special_item_editing();
		// TODO scroll to show the item
	}
	scenario.special_items[which] = after;
	return true;
}

bool aEditClearQuest::undo_me() {
	// If not editing quests, show it
	if(overall_mode != MODE_EDIT_QUESTS){
		start_quest_editing();
		// TODO scroll to show the quest
	}
	scenario.quests[which] = before;
	return true;
}

bool aEditClearQuest::redo_me() {
	// If not editing quests, show it
	if(overall_mode != MODE_EDIT_QUESTS){
		start_quest_editing();
		// TODO scroll to show the quest
	}
	scenario.quests[which] = after;
	return true;
}

bool aEditClearShop::undo_me() {
	// If not editing shops, show it
	if(overall_mode != MODE_EDIT_SHOPS){
		start_shops_editing();
		// TODO scroll to show the shop
	}
	scenario.shops[which] = before;
	return true;
}

bool aEditClearShop::redo_me() {
	// If not editing shops, show it
	if(overall_mode != MODE_EDIT_SHOPS){
		start_shops_editing();
		// TODO scroll to show the shop
	}
	scenario.shops[which] = after;
	return true;
}