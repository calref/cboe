#include "scen.undo.hpp"

#include <boost/filesystem.hpp>

#include "gfx/gfxsheets.hpp"
#include "fileio/resmgr/res_image.hpp"
#include "fileio/resmgr/res_sound.hpp"
#include "sounds.hpp"

#include "scenario/scenario.hpp"
#include "scenario/area.hpp"
#include "scen.actions.hpp"
#include "scen.core.hpp"
#include "scen.townout.hpp"

extern bool editing_town;
extern short cur_town;
extern cTown* town;
extern location cur_out;
extern short cen_x;
extern short cen_y;
extern cScenario scenario;
extern cArea* get_current_area();
extern cOutdoors* current_terrain;
extern void start_town_edit();
extern void start_out_edit();
extern void redraw_screen();
extern eScenMode overall_mode;
extern eDrawMode draw_mode;
extern void make_field_type(short i,short j,eFieldType field_type,field_stroke_t& stroke);
extern void take_field_type(short i,short j,eFieldType field_type,clear_field_stroke_t& stroke);

cTerrainAction::cTerrainAction(std::string name, area_ref_t area, bool reversed) : cAction(name, reversed),
	area(area) {}
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
		set_current_town(area.town_num);
		start_town_edit();
	}else{
		set_current_out(area.out_sec);
		start_out_edit();
	}

	// If the last stored view location of this town/section can't see the change site,
	// move the view center
	if(!((abs((short) (cen_x - area.where.x)) <=4) && (abs((short) (cen_y - area.where.y)) <= 4))){
		cen_x = area.where.x;
		cen_y = area.where.y;
		size_t max_dim = area.is_town ? town->max_dim : current_terrain->max_dim;
		cen_x = minmax(4, max_dim - 5, cen_x);
		cen_y = minmax(4, max_dim - 5, cen_y);
	}
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

bool aPlaceEraseSpecial::undo_me() {
	cArea* cur_area = get_current_area();
	auto& specials = cur_area->special_locs;
	specials.erase(std::remove(specials.begin(), specials.end(), for_redo));
	return true;
}

bool aPlaceEraseSpecial::redo_me() {
	cArea* cur_area = get_current_area();
	auto& specials = cur_area->special_locs;
	specials.push_back(for_redo);
	return true;
}

bool aSetSpecial::undo_me() {
	cArea* cur_area = get_current_area();
	auto& specials = cur_area->special_locs;
	for(spec_loc_t& special : specials){
		if(special == area.where) special.spec = old_num;
	}
	return true;
}

bool aSetSpecial::redo_me() {
	cArea* cur_area = get_current_area();
	auto& specials = cur_area->special_locs;
	for(spec_loc_t& special : specials){
		if(special == area.where) special.spec = new_num;
	}
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

// If the town isn't part of the scenario when this action gets discarded,
// delete the object
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
	: cTerrainAction(name, closest_to_view(items), !place)
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

// The action is cleared from the tree, so erase objects it owns
aResizeOutdoors::~aResizeOutdoors() {
	// If the resize happened, delete sections it deleted
	if(isDone()){
		for(auto sec : sections_removed) delete sec.second;
	}
	// If it was undone, delete sections it created
	else{
		for(auto sec : sections_added) delete sec.second;
	}
}

bool aResizeOutdoors::undo_me() {
	rectangle reverse_mod = { -mod.top, -mod.left, -mod.bottom, -mod.right };
	apply_outdoor_shift(reverse_mod);
	for(auto sec : sections_removed){
		scenario.outdoors[sec.first.x][sec.first.y] = sec.second;
	}
	// Update current location - point to same sector if possible
	cur_out.x += reverse_mod.left;
	cur_out.y += reverse_mod.top;
	clamp_current_section();
	start_out_edit();
	return true;
}

bool aResizeOutdoors::redo_me() {
	apply_outdoor_shift(mod);
	for(auto sec : sections_added){
		scenario.outdoors[sec.first.x][sec.first.y] = sec.second;
	}
	// Update current location - point to same sector if possible
	cur_out.x += mod.left;
	cur_out.y += mod.top;
	clamp_current_section();
	start_out_edit();
	return true;
}

// The action is cleared from the tree, so erase objects it owns
aImportTown::~aImportTown() {
	// If the import happened, delete the old town
	if(isDone()){
		delete old_town;
	}
	// If it was undone, delete the new town
	else{
		delete new_town;
	}
}

bool aImportTown::undo_me() {
	scenario.towns[which] = old_town;
	clamp_view_center(old_town);
	set_current_town(which);
	start_town_edit();
	return true;
}

bool aImportTown::redo_me() {
	scenario.towns[which] = new_town;
	clamp_view_center(new_town);
	set_current_town(which);
	start_town_edit();
	return true;
}

// The action is cleared from the tree, so erase objects it owns
aImportOutdoors::~aImportOutdoors() {
	// If the import happened, delete the old section
	if(isDone()){
		delete old_out;
	}
	// If it was undone, delete the new section
	else{
		delete new_out;
	}
}

bool aImportOutdoors::undo_me() {
	scenario.outdoors[which.x][which.y] = old_out;
	set_current_out(which);
	start_out_edit();
	return true;
}

bool aImportOutdoors::redo_me() {
	scenario.outdoors[which.x][which.y] = new_out;
	set_current_out(which);
	start_out_edit();
	return true;
}

bool aPlaceTownEntrance::undo_me() {
	town->start_locs[which_entrance].x = old_loc.x;
	town->start_locs[which_entrance].y = old_loc.y;
	return true;
}

bool aPlaceTownEntrance::redo_me() {
	town->start_locs[which_entrance].x = new_loc.x;
	town->start_locs[which_entrance].y = new_loc.y;
	return true;
}

bool aPlaceStartLocation::undo_me() {
	if(old_where.is_town){
		scenario.which_town_start = old_where.town_num;
		scenario.where_start = old_where.where;
	}else{
		scenario.out_sec_start = old_where.out_sec;
		scenario.out_start = old_where.where;
	}
	return true;
}

bool aPlaceStartLocation::redo_me() {
	if(old_where.is_town){
		scenario.which_town_start = area.town_num;
		scenario.where_start = area.where;
	}else{
		scenario.out_sec_start = area.out_sec;
		scenario.out_start = area.where;
	}
	return true;
}

bool aClearFields::undo_me() {
	field_stroke_t discard_stroke;
	for(auto& space : stroke){
		for(eFieldType field_type : space.second){
			make_field_type(space.first.x, space.first.y, field_type, discard_stroke);
		}
	}

	return true;
}

bool aClearFields::redo_me() {
	clear_field_stroke_t discard_stroke;
	for(auto& space : stroke){
		for(eFieldType field_type : space.second){
			take_field_type(space.first.x, space.first.y, field_type, discard_stroke);
		}
	}

	return true;
}

bool aPlaceFields::undo_me() {
	clear_field_stroke_t discard_stroke;
	for(auto& space : stroke){
		take_field_type(space.x, space.y, type, discard_stroke);
	}
	return true;
}

bool aPlaceFields::redo_me() {
	field_stroke_t discard_stroke;
	for(auto& space : stroke){
		make_field_type(space.x, space.y, type, discard_stroke);
	}
	return true;
}

bool aToggleOutFields::undo_me() {
	for(auto& space : stroke){
		if(is_road)
			current_terrain->roads[space.x][space.y] = !on;
		else
			current_terrain->special_spot[space.x][space.y] = !on;
	}
	return true;
}

bool aToggleOutFields::redo_me() {
	for(auto& space : stroke){
		if(is_road)
			current_terrain->roads[space.x][space.y] = on;
		else
			current_terrain->special_spot[space.x][space.y] = on;
	}
	return true;
}

static fs::path get_pic_dir() {
	extern fs::path tempDir;
	extern std::string scenario_temp_dir_name;
	fs::path pic_dir = tempDir/scenario_temp_dir_name/"graphics";
	if(!scenario.scen_file.has_extension()) // It's an unpacked scenario
		pic_dir = scenario.scen_file/"graphics";
	return pic_dir;
}

extern cCustomGraphics spec_scen_g;

bool aCreateGraphicsSheet::undo_me() {
	fs::path sheetPath = get_pic_dir()/("sheet" + std::to_string(index) + ".png");
	if(fs::exists(sheetPath)) fs::remove(sheetPath);
	if(index == spec_scen_g.numSheets - 1) {
		spec_scen_g.sheets.pop_back();
		spec_scen_g.numSheets--;
		ResMgr::graphics.free("sheet" + std::to_string(index));
	}
	return true;
}

bool aCreateGraphicsSheet::redo_me() {
	fs::path sheetPath = get_pic_dir()/("sheet" + std::to_string(index) + ".png");
	if(index == spec_scen_g.numSheets) {
		spec_scen_g.sheets.emplace_back();
		spec_scen_g.init_sheet(index);
		spec_scen_g.sheets[index]->copyToImage().saveToFile(sheetPath.string().c_str());
		spec_scen_g.numSheets++;
	}else{
		sf::Image img;
		img.create(280, 360);
		img.saveToFile(sheetPath.string().c_str());
	}
	return true;
}

bool aDeleteGraphicsSheet::undo_me() {
	fs::path sheetPath = get_pic_dir()/("sheet" + std::to_string(index) + ".png");
	// Undo shifting other sheets left
	if(move_others && fs::exists(sheetPath)){
		for(int other_index = spec_scen_g.numSheets - 1; other_index >= index; --other_index){
			int old_index = other_index + 1;
			fs::path from = get_pic_dir()/("sheet" + std::to_string(other_index) + ".png");
			fs::path to = get_pic_dir()/("sheet" + std::to_string(old_index) + ".png");
			if(!fs::exists(from)) continue; // Just in case
			fs::remove(to);
			fs::rename(from, to);
		}
	}

	image.saveToFile(sheetPath.string().c_str());
	if(index <= spec_scen_g.numSheets + 1) spec_scen_g.numSheets++;
	return true;
}

bool aDeleteGraphicsSheet::redo_me() {
	fs::path sheetPath = get_pic_dir()/("sheet" + std::to_string(index) + ".png");

	if(index < spec_scen_g.numSheets){
		if(move_others){
			spec_scen_g.sheets.erase(spec_scen_g.sheets.begin() + index);
			spec_scen_g.numSheets--;
			for(int which_pic = index; which_pic < spec_scen_g.numSheets; which_pic++) {
				fs::path from = get_pic_dir()/("sheet" + std::to_string(which_pic + 1) + ".png");
				fs::path to = get_pic_dir()/("sheet" + std::to_string(which_pic) + ".png");
				if(!fs::exists(from)) continue; // Just in case
				fs::remove(to);
				fs::rename(from, to);
				ResMgr::graphics.free("sheet" + std::to_string(which_pic));
			}
		}else{
			spec_scen_g.numSheets = index;
			spec_scen_g.sheets.resize(index);
			ResMgr::graphics.free("sheet" + std::to_string(index));
		}
	}

	if(fs::exists(sheetPath)) fs::remove(sheetPath);

	return true;
}

bool aReplaceGraphicsSheet::undo_me() {
	if(index >= spec_scen_g.numSheets) {
		std::string resName = "sheet" + std::to_string(index);
		fs::path toPath = get_pic_dir()/(resName + ".png");

		old_image.saveToFile(toPath.string().c_str());
		ResMgr::graphics.free(resName);
	}else{
		spec_scen_g.replace_sheet(index, old_image);
	}

	return true;
}

bool aReplaceGraphicsSheet::redo_me() {
	if(index >= spec_scen_g.numSheets) {
		std::string resName = "sheet" + std::to_string(index);
		fs::path toPath = get_pic_dir()/(resName + ".png");

		new_image.saveToFile(toPath.string().c_str());
		ResMgr::graphics.free(resName);
	}else{
		spec_scen_g.replace_sheet(index, new_image);
	}

	return true;
}

bool aCreateDeleteSound::undo_me() {
	fs::remove(get_snd_path(index).string());
	return true;
}

bool aCreateDeleteSound::redo_me() {
	ResMgr::sounds.free(sound_to_fname(index));
	return sound.saveToFile(get_snd_path(index).string());
}

bool aReplaceSound::undo_me() {
	ResMgr::sounds.free(sound_to_fname(index));
	return old_sound.saveToFile(get_snd_path(index).string());
}

bool aReplaceSound::redo_me() {
	ResMgr::sounds.free(sound_to_fname(index));
	return new_sound.saveToFile(get_snd_path(index).string());
}

bool aEditScenarioDetails::undo_me() {
	scen_set_details(scenario, old_details);
	return true;
}

bool aEditScenarioDetails::redo_me() {
	scen_set_details(scenario, new_details);
	return true;
}

bool aEditIntro::undo_me() {
	scen_set_intro(scenario, old_intro);
	return true;
}

bool aEditIntro::redo_me() {
	scen_set_intro(scenario, new_intro);
	return true;
}

bool aClassifyGraphics::undo_me() {
	scenario.custom_graphics = old_types;
	return true;
}

bool aClassifyGraphics::redo_me() {
	scenario.custom_graphics = new_types;
	return true;
}

bool aEditAdvancedDetails::undo_me() {
	scen_set_advanced(scenario, old_advanced);
	return true;
}

bool aEditAdvancedDetails::redo_me() {
	scen_set_advanced(scenario, new_advanced);
	return true;
}

bool aEditTownVarying::undo_me() {
	scenario.town_mods = old_varying;
	return true;
}

bool aEditTownVarying::redo_me() {
	scenario.town_mods = new_varying;
	return true;
}

bool aEditScenTimers::undo_me() {
	scenario.scenario_timers = old_timers;
	return true;
}

bool aEditScenTimers::redo_me() {
	scenario.scenario_timers = new_timers;
	return true;
}

bool aEditItemShortcut::undo_me() {
	scenario.storage_shortcuts[which] = old_shortcut;
	return true;
}

bool aEditItemShortcut::redo_me() {
	scenario.storage_shortcuts[which] = new_shortcut;
	return true;
}

bool aEditTownDetails::undo_me() {
	town_set_details(*scenario.towns[which], old_details);
	return true;
}

bool aEditTownDetails::redo_me() {
	town_set_details(*scenario.towns[which], new_details);
	return true;
}

bool aEditTownWandering::undo_me() {
	scenario.towns[which]->wandering = old_wandering;
	scenario.towns[which]->wandering_locs = old_wandering_locs;
	return true;
}

bool aEditTownWandering::redo_me() {
	scenario.towns[which]->wandering = new_wandering;
	scenario.towns[which]->wandering_locs = new_wandering_locs;
	return true;
}

bool aEditTownAdvancedDetails::undo_me() {
	town_set_advanced(which, *scenario.towns[which], scenario, old_details);
	return true;
}

bool aEditTownAdvancedDetails::redo_me() {
	town_set_advanced(which, *scenario.towns[which], scenario, new_details);
	return true;
}

bool aEditTownTimers::undo_me() {
	scenario.towns[which]->timers = old_timers;
	return true;
}

bool aEditTownTimers::redo_me() {
	scenario.towns[which]->timers = new_timers;
	return true;
}

bool aEditOutDetails::undo_me() {
	out_set_details(*scenario.outdoors[out_sec.x][out_sec.y], old_details);
	return true;
}

bool aEditOutDetails::redo_me() {
	out_set_details(*scenario.outdoors[out_sec.x][out_sec.y], new_details);
	return true;
}

bool aEditOutEncounter::undo_me() {
	cOutdoors& outdoors = *scenario.outdoors[out_sec.x][out_sec.y];
	auto& encounters = (mode == 0 ? outdoors.wandering : outdoors.special_enc);
	encounters[which] = old_enc;
	return true;
}

bool aEditOutEncounter::redo_me() {
	cOutdoors& outdoors = *scenario.outdoors[out_sec.x][out_sec.y];
	auto& encounters = (mode == 0 ? outdoors.wandering : outdoors.special_enc);
	encounters[which] = new_enc;
	return true;
}

// as a cTerrainAction, this one will already have the correct outdoor section active
bool aMoveOutEncounterLoc::undo_me() {
	current_terrain->wandering_locs[which] = old_loc;
	return true;
}

bool aMoveOutEncounterLoc::redo_me() {
	current_terrain->wandering_locs[which] = area.where;
	return true;
}

bool aEditPersonality::undo_me() {
	scenario.towns[town_num]->talking.people[which] = old_pers;
	// Show the change
	start_dialogue_editing();
	return true;
}

bool aEditPersonality::redo_me() {
	scenario.towns[town_num]->talking.people[which] = new_pers;
	// Show the change
	start_dialogue_editing();
	return true;
}

bool aCreateDeleteTalkNode::undo_me() {
	scenario.towns[town_num]->talking.talk_nodes.erase(scenario.towns[town_num]->talking.talk_nodes.begin() + which);
	// Show the change
	start_dialogue_editing();
	return true;
}

bool aCreateDeleteTalkNode::redo_me() {
	scenario.towns[town_num]->talking.talk_nodes.insert(scenario.towns[town_num]->talking.talk_nodes.begin() + which, node);
	// Show the change
	start_dialogue_editing();
	return true;
}

bool aEditTalkNode::undo_me() {
	scenario.towns[town_num]->talking.talk_nodes[which] = old_node;
	return true;
}

bool aEditTalkNode::redo_me() {
	scenario.towns[town_num]->talking.talk_nodes[which] = new_node;
	return true;
}

bool aDeleteLocString::undo_me() {
	if(is_sign){
		area->sign_locs.push_back(sign);
		start_string_editing(is_town ? STRS_TOWN_SIGN : STRS_OUT_SIGN);
	}else{
		area->area_desc.push_back(desc);
		start_string_editing(is_town ? STRS_TOWN_RECT : STRS_OUT_RECT);
	}
	return true;
}

bool aDeleteLocString::redo_me() {
	if(is_sign){
		area->sign_locs.pop_back();
		start_string_editing(is_town ? STRS_TOWN_SIGN : STRS_OUT_SIGN);
	}else{
		area->area_desc.pop_back();
		start_string_editing(is_town ? STRS_TOWN_RECT : STRS_OUT_RECT);
	}
	return true;
}

bool aClearLocString::undo_me() {
	if(is_sign){
		area->sign_locs[which] = old_sign;
		start_string_editing(is_town ? STRS_TOWN_SIGN : STRS_OUT_SIGN);
	}else{
		area->area_desc[which] = old_desc;
		start_string_editing(is_town ? STRS_TOWN_RECT : STRS_OUT_RECT);
	}
	return true;
}

bool aClearLocString::redo_me() {
	if(is_sign){
		area->sign_locs[which] = new_sign;
		start_string_editing(is_town ? STRS_TOWN_SIGN : STRS_OUT_SIGN);
	}else{
		area->area_desc[which] = new_desc;
		start_string_editing(is_town ? STRS_TOWN_RECT : STRS_OUT_RECT);
	}
	return true;
}

bool aEditClearString::undo_me() {
	if(str_mode == STRS_TOWN) set_current_town(which_town);
	else if(str_mode == STRS_OUT) set_current_out(which_out);
	fetch_str(str_mode, which) = old_value;
	start_string_editing(str_mode);
	return true;
}

bool aEditClearString::redo_me() {
	if(str_mode == STRS_TOWN) set_current_town(which_town);
	else if(str_mode == STRS_OUT) set_current_out(which_out);
	fetch_str(str_mode, which) = new_value;
	start_string_editing(str_mode);
	return true;
}

bool aDeleteString::undo_me() {
	if(str_mode == STRS_TOWN) set_current_town(which_town);
	else if(str_mode == STRS_OUT) set_current_out(which_out);
	fetch_str_list(str_mode).push_back(str);
	start_string_editing(str_mode);
	return true;
}

bool aDeleteString::redo_me() {
	if(str_mode == STRS_TOWN) set_current_town(which_town);
	else if(str_mode == STRS_OUT) set_current_out(which_out);
	fetch_str_list(str_mode).pop_back();
	start_string_editing(str_mode);
	return true;
}

bool aCreateStrings::undo_me() {
	if(str_mode == STRS_TOWN) set_current_town(which_town);
	else if(str_mode == STRS_OUT) set_current_out(which_out);
	auto& str_list = fetch_str_list(str_mode);
	for(int i = 0; i < strs.size(); ++i){
		str_list.pop_back();
	}
	start_string_editing(str_mode);
	return true;
}

bool aCreateStrings::redo_me() {
	if(str_mode == STRS_TOWN) set_current_town(which_town);
	else if(str_mode == STRS_OUT) set_current_out(which_out);
	auto& str_list = fetch_str_list(str_mode);
	for(auto str : strs){
		str_list.push_back(str);
	}
	start_string_editing(str_mode);
	return true;
}

bool aCreateDeleteSpecial::undo_me() {
	switch(mode){
		case 0:
			scenario.scen_specials.pop_back();
			break;
		case 1:
			set_current_out(which_out);
			current_terrain->specials.pop_back();
			break;
		case 2:
			set_current_town(which_town);
			town->specials.pop_back();
			break;
		default:
			break;
	}
	start_special_editing(mode);
	return true;
}

bool aCreateDeleteSpecial::redo_me() {
	switch(mode){
		case 0:
			scenario.scen_specials.push_back(spec);
			break;
		case 1:
			set_current_out(which_out);
			current_terrain->specials.push_back(spec);
			break;
		case 2:
			set_current_town(which_town);
			town->specials.push_back(spec);
			break;
		default:
			break;
	}
	start_special_editing(mode);
	return true;
}

bool aEditSpecial::undo_me() {
	switch(mode){
		case 0:
			scenario.scen_specials[which] = old_spec;
			break;
		case 1:
			set_current_out(which_out);
			current_terrain->specials[which] = old_spec;
			break;
		case 2:
			set_current_town(which_town);
			town->specials[which] = old_spec;
			break;
		default:
			break;
	}
	start_special_editing(mode);
	return true;
}

bool aEditSpecial::redo_me() {
	switch(mode){
		case 0:
			scenario.scen_specials[which] = new_spec;
			break;
		case 1:
			set_current_out(which_out);
			current_terrain->specials[which] = new_spec;
			break;
		case 2:
			set_current_town(which_town);
			town->specials[which] = new_spec;
			break;
		default:
			break;
	}
	start_special_editing(mode);
	return true;
}

bool aEditTownBounds::undo_me() {
	if(for_saved_items){
		if(old_rect.empty()) scenario.store_item_rects.erase(area.town_num);
		else scenario.store_item_rects[area.town_num] = old_rect;
	}else{
		scenario.towns[area.town_num]->in_town_rect = old_rect;
	}
	return true;
}

bool aEditTownBounds::redo_me() {
	if(for_saved_items){
		scenario.store_item_rects[area.town_num] = new_rect;
	}else{
		scenario.towns[area.town_num]->in_town_rect = new_rect;
	}
	return true;
}

bool aCreateAreaRect::undo_me() {
	area->area_desc[which] = {0, 0, 0, 0, "*"};
	return true;
}

bool aCreateAreaRect::redo_me() {
	area->area_desc[which] = rect;
	return true;
}

location closest_to_view(stroke_ter_changes_t changes) {
	std::vector<location> locs;
	for(auto& it : changes){
		locs.push_back(it.first);
	}
	return closest_point(locs, loc(cen_x, cen_y));
}

location closest_to_view(item_changes_t changes) {
	std::vector<location> locs;
	for(auto& it : changes){
		locs.push_back(it.second.loc);
	}
	return closest_point(locs, loc(cen_x, cen_y));
}