#ifndef BoE_scen_undo_h
#define BoE_scen_undo_h

#include <SFML/Audio/SoundBuffer.hpp>

#include "location.hpp"
#include "tools/undo.hpp"
#include "scenario/town.hpp"
#include "scenario/scenario.hpp"
#include "scenario/item.hpp"
#include "scenario/quest.hpp"
#include "scenario/monster.hpp"
#include "scenario/vehicle.hpp"

extern cScenario scenario;
extern cTown* town;

struct area_ref_t {
	bool is_town;
	// Can't just make the next two a union for compiler reasons.
	size_t town_num;
	location out_sec;
	location where;
};

struct ter_change_t {
	ter_num_t old_num;
	ter_num_t new_num;
};

typedef std::map<location,ter_change_t,loc_compare> stroke_ter_changes_t;
typedef std::map<size_t, cTown::cItem> item_changes_t;
typedef std::map<size_t, cTownperson> creature_changes_t;
typedef std::vector<cTerrain> terrain_type_changes_t;
typedef std::vector<cMonster> monst_type_changes_t;
typedef std::vector<class cItem> item_type_changes_t;
typedef std::map<location,cOutdoors*,loc_compare> outdoor_sections_t;
typedef std::map<location,std::vector<eFieldType>,loc_compare> clear_field_stroke_t;
typedef std::set<location,loc_compare> field_stroke_t;

// Action that modified something in town or outdoor terrain, so we should show the modified area when undoing or redoing
class cTerrainAction : public cAction {
public:
	cTerrainAction(std::string name, area_ref_t area, bool reversed = false);
	cTerrainAction(std::string name, short town_num, location where, bool reversed = false);
	cTerrainAction(std::string name, location out_sec, location where, bool reversed = false);
	// Construct cTerrainAction in the current town/outdoor section
	cTerrainAction(std::string name, location where, bool reversed = false);
	void undo() override;
	void redo() override;
private:
	/// Show where the change happened
	void showChangeSite();
protected:
	area_ref_t area;
};

/// Action that clears the 'not yours' flag from every item in town
class aClearProperty : public cTerrainAction {
	std::vector<bool> old_property;
	bool undo_me() override;
	bool redo_me() override;
public:
	aClearProperty(std::vector<bool> old_property) : cTerrainAction("Set All Items Not Property", town->preset_items[0].loc),
		old_property(old_property) {}
};

/// Action that places or erases a special encounter from a spot
class aPlaceEraseSpecial : public cTerrainAction {
public:
	aPlaceEraseSpecial(std::string name, bool place, spec_loc_t special) :
		cTerrainAction(name, special, !place),
		for_redo(special) {}
	bool undo_me() override;
	bool redo_me() override;
private:
	spec_loc_t for_redo;
	bool editing_town;
};

/// Action that sets the number of an existing special encounter
class aSetSpecial : public cTerrainAction {
	short old_num;
	short new_num;
	bool undo_me() override;
	bool redo_me() override;
public:
	aSetSpecial(location where, short old_num, short new_num) :
		cTerrainAction("Edit Special Encounter", where), old_num(old_num), new_num(new_num) {}
};

/// Action which modifies terrain tiles (i.e. paintbrush, pencil, eraser)
class aDrawTerrain : public cTerrainAction {
public:
	aDrawTerrain(std::string name, stroke_ter_changes_t stroke_changes) :
		cTerrainAction(name, stroke_changes.begin()->first), // Use arbitrary changed tile as site of change
		changes(stroke_changes) {}
	bool undo_me() override;
	bool redo_me() override;
private:
	const stroke_ter_changes_t changes;
};

/// Action which places or erases item(s) in a town
class aPlaceEraseItem : public cTerrainAction {
	bool placed;
	item_changes_t items;
	bool undo_me() override;
	bool redo_me() override;
public:
	aPlaceEraseItem(std::string name, bool place, item_changes_t items);
	aPlaceEraseItem(std::string name, bool place, size_t index, cTown::cItem item);
};

/// Action which edits an item in a town
class aEditPlacedItem : public cTerrainAction {
	size_t which;
	cTown::cItem old_item;
	cTown::cItem new_item;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditPlacedItem(size_t which, cTown::cItem old_item, cTown::cItem new_item) :
		cTerrainAction("Edit Placed Item", new_item.loc),
		which(which), old_item(old_item), new_item(new_item) {}
};

/// Action which edits a creature in a town
class aEditPlacedCreature : public cTerrainAction {
	size_t which;
	cTownperson old_creature;
	cTownperson new_creature;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditPlacedCreature(size_t which, cTownperson old_creature, cTownperson new_creature) :
		cTerrainAction("Edit Placed Creature", new_creature.start_loc),
		which(which), old_creature(old_creature), new_creature(new_creature) {}
};

/// Action which places or erases creature(s) in a town
class aPlaceEraseCreature : public cTerrainAction {
	bool placed;
	creature_changes_t creatures;
	bool undo_me() override;
	bool redo_me() override;
public:
	aPlaceEraseCreature(std::string name, bool place, creature_changes_t creatures);
	aPlaceEraseCreature(std::string name, bool place, size_t index, cTownperson creature);
};

/// Action which places or erases vehicles
class aPlaceEraseVehicle : public cTerrainAction {
	bool placed;
	bool is_boat;
	size_t which;
	cVehicle vehicle;
	bool undo_me() override;
	bool redo_me() override;
public:
	aPlaceEraseVehicle(bool place, bool is_boat, size_t which, cVehicle vehicle) :
		cTerrainAction(std::string { place ? "Place " : "Erase " } + (is_boat ? "Boat" : "Horse"), vehicle.loc, !place),
		is_boat(is_boat), which(which), vehicle(vehicle) {}
};

/// Action which edits vehicle
class aEditVehicle : public cTerrainAction {
	bool is_boat;
	size_t which;
	cVehicle old_vehicle;
	cVehicle new_vehicle;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditVehicle(bool is_boat, size_t which, cVehicle old_vehicle, cVehicle new_vehicle) :
		cTerrainAction(std::string { "Edit " } + (is_boat ? "Boat" : "Horse"), new_vehicle.loc),
		is_boat(is_boat), which(which), old_vehicle(old_vehicle), new_vehicle(new_vehicle) {}
};

/// Action which edits sign text
class aEditSignText : public cTerrainAction {
	std::string old_text;
	std::string new_text;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditSignText(location loc, std::string old_text, std::string new_text) :
		cTerrainAction("Edit Sign Text", loc), old_text(old_text), new_text(new_text) {}
};

/// Action which edits town entrance
class aEditTownEntrance : public cTerrainAction {
	long old_town;
	long new_town;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditTownEntrance(location loc, long old_town, long new_town) :
		cTerrainAction("Edit Town Entrance", loc), old_town(old_town), new_town(new_town) {}
};

/// Action which adds a new town to the end of the list, or deletes the last one
class aCreateDeleteTown : public cAction {
	bool created;
	class cTown* theTown;
	bool undo_me() override;
	bool redo_me() override;
public:
	aCreateDeleteTown(bool create, class cTown* t);
	~aCreateDeleteTown();
};

/// Action which adds new terrain type(s) to the end of the list, or deletes from the end of the list
class aCreateDeleteTerrain : public cAction {
	terrain_type_changes_t terrains;
	bool undo_me() override;
	bool redo_me() override;
public:
	aCreateDeleteTerrain(bool create, cTerrain terrain) :
		cAction(create ? "Create Terrain Type" : "Delete Terrain Type", !create),
		terrains({terrain}) {}
	aCreateDeleteTerrain(terrain_type_changes_t terrains) :
		cAction("Create Terrain Types", false),
		terrains(terrains) {}
};

/// Action which adds new monster type(s) to the end of the list, or deletes from the end of the list
class aCreateDeleteMonster : public cAction {
	monst_type_changes_t monsters;
	bool undo_me() override;
	bool redo_me() override;
public:
	aCreateDeleteMonster(bool create, cMonster monst) :
		cAction(create ? "Create Monster Type" : "Delete Monster Type", !create),
		monsters({monst}) {}
	aCreateDeleteMonster(monst_type_changes_t monsts) :
		cAction("Create Monster Types", false),
		monsters(monsts) {}
};

/// Action which adds new item type(s) to the end of the list, or deletes from the end of the list
class aCreateDeleteItem : public cAction {
	item_type_changes_t items;
	bool undo_me() override;
	bool redo_me() override;
public:
	aCreateDeleteItem(bool create, class cItem item) :
		cAction(create ? "Create Item Type" : "Delete Item Type", !create),
		items({item}) {}
	aCreateDeleteItem(item_type_changes_t items) :
		cAction("Create Item Types", false),
		items(items) {}
};

/// Action which adds new special item to the end of the list, or deletes from the end of the list
class aCreateDeleteSpecialItem : public cAction {
	cSpecItem item;
	bool undo_me() override;
	bool redo_me() override;
public:
	aCreateDeleteSpecialItem(bool create, cSpecItem item) :
		cAction(create ? "Create Special Item" : "Delete Special Item", !create),
		item(item) {}
};

/// Action which adds new quest to the end of the list, or deletes from the end of the list
class aCreateDeleteQuest : public cAction {
	cQuest quest;
	bool undo_me() override;
	bool redo_me() override;
public:
	aCreateDeleteQuest(bool create, cQuest quest) :
		cAction(create ? "Create Quest" : "Delete Quest", !create),
		quest(quest) {}
};

/// Action which adds new shop to the end of the list, or deletes from the end of the list
class aCreateDeleteShop : public cAction {
	cShop shop;
	bool undo_me() override;
	bool redo_me() override;
public:
	aCreateDeleteShop(bool create, cShop shop) :
		cAction(create ? "Create Shop" : "Delete Shop", !create),
		shop(shop) {}
};

/// Action which edits or clears a terrain type
class aEditClearTerrain : public cAction {
	ter_num_t which;
	cTerrain before;
	cTerrain after;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditClearTerrain(std::string name, ter_num_t which, cTerrain before, cTerrain after) :
		cAction(name), which(which), before(before), after(after) {}
};

/// Action which edits or clears a monster type
class aEditClearMonster : public cAction {
	mon_num_t which;
	cMonster before;
	cMonster after;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditClearMonster(std::string name, mon_num_t which, cMonster before, cMonster after) :
		cAction(name), which(which), before(before), after(after) {}
};

/// Action which edits or clears an item type
class aEditClearItem : public cAction {
	item_num_t which;
	class cItem before;
	class cItem after;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditClearItem(std::string name, item_num_t which, class cItem before, class cItem after) :
		cAction(name), which(which), before(before), after(after) {}
};

/// Action which edits or clears a special item
class aEditClearSpecialItem : public cAction {
	size_t which;
	cSpecItem before;
	cSpecItem after;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditClearSpecialItem(std::string name, size_t which, cSpecItem before, cSpecItem after) :
		cAction(name), which(which), before(before), after(after) {}
};

/// Action which edits or clears a quest
class aEditClearQuest : public cAction {
	size_t which;
	cQuest before;
	cQuest after;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditClearQuest(std::string name, size_t which, cQuest before, cQuest after) :
		cAction(name), which(which), before(before), after(after) {}
};

/// Action which edits or clears a shop
class aEditClearShop : public cAction {
	size_t which;
	cShop before;
	cShop after;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditClearShop(std::string name, size_t which, cShop before, cShop after) :
		cAction(name), which(which), before(before), after(after) {}
};

/// Action which resizes the outdoors
class aResizeOutdoors : public cAction {
	rectangle mod;
	// Sections removed when the resize happened, coordinates relative to original size
	std::map<location,cOutdoors*,loc_compare> sections_removed;
	// Sections added when the resize happened, coordinates relative to new size
	std::map<location,cOutdoors*,loc_compare> sections_added;
	bool undo_me() override;
	bool redo_me() override;
public:
	aResizeOutdoors(rectangle mod, outdoor_sections_t removed, outdoor_sections_t added) :
		cAction("Resize/Shift Outdoors"), mod(mod), sections_removed(removed), sections_added(added) {}
	~aResizeOutdoors();
};

/// Action which imports a town from another scenario
class aImportTown : public cAction {
	size_t which;
	cTown* old_town;
	cTown* new_town;
	bool undo_me() override;
	bool redo_me() override;
public:
	aImportTown(size_t which, cTown* old_town, cTown* new_town) :
		cAction("Import Town"), which(which), old_town(old_town), new_town(new_town) {}
	~aImportTown();
};

/// Action which imports an outdoor section from another scenario
class aImportOutdoors : public cAction {
	location which;
	cOutdoors* old_out;
	cOutdoors* new_out;
	bool undo_me() override;
	bool redo_me() override;
public:
	aImportOutdoors(location which, cOutdoors* old_out, cOutdoors* new_out) :
		cAction("Import Outdoor Sector"), which(which), old_out(old_out), new_out(new_out) {}
	~aImportOutdoors();
};

/// Action which places one of the four entrance locations inside a town
class aPlaceTownEntrance : public cTerrainAction {
	int which_entrance;
	location old_loc;
	location new_loc;
	bool undo_me() override;
	bool redo_me() override;
public:
	aPlaceTownEntrance(std::string name, int which, location old_loc, location new_loc) :
		cTerrainAction(name, new_loc), which_entrance(which), old_loc(old_loc), new_loc(new_loc) {}
};

/// Action which places the scenario's town or outdoor start location
class aPlaceStartLocation : public cTerrainAction {
	area_ref_t old_where;
	bool undo_me() override;
	bool redo_me() override;
public:
	aPlaceStartLocation(area_ref_t old_where, area_ref_t new_where) :
		cTerrainAction(std::string { "Place Scenario Start Loc " } + (old_where.is_town ? "(Town)" : "(Outdoors)"), new_where), old_where(old_where) {}
};

/// Action that clears all fields from tiles within a stroke
class aClearFields : public cTerrainAction {
	clear_field_stroke_t stroke;
	bool undo_me() override;
	bool redo_me() override;
public:
	aClearFields(clear_field_stroke_t stroke) : cTerrainAction("Clear Fields", stroke.begin()->first),
		stroke(stroke) {}
};

extern std::string get_editor_field_name(eFieldType e);

/// Action that draws a field on tiles within a stroke
class aPlaceFields : public cTerrainAction {
	eFieldType type;
	field_stroke_t stroke;
	bool undo_me() override;
	bool redo_me() override;
public:
	aPlaceFields(eFieldType type, field_stroke_t stroke) : cTerrainAction(std::string{"Place "} + get_editor_field_name(type), *(stroke.begin())),
		type(type), stroke(stroke) {}
};

/// Action that toggles a road or special spot outdoors within a stroke
class aToggleOutFields : public cTerrainAction {
	bool is_road;
	bool on;
	field_stroke_t stroke;
	bool undo_me() override;
	bool redo_me() override;
public:
	aToggleOutFields(bool is_road, bool on, field_stroke_t stroke): cTerrainAction(std::string{"Toggle "} + (is_road ? "Road" : "Special Spot"), *(stroke.begin())),
		is_road(is_road), on(on), stroke(stroke) {}
};

/// Action that creates a custom graphics sheet
class aCreateGraphicsSheet : public cAction {
	size_t index;
	bool undo_me() override;
	bool redo_me() override;
public:
	aCreateGraphicsSheet(size_t index) :
		cAction("Create Custom Graphics Sheet"), index(index) {}
};

/// Action that deletes a custom graphics sheet
class aDeleteGraphicsSheet : public cAction {
	size_t index;
	bool move_others;
	sf::Image image;
	bool undo_me() override;
	bool redo_me() override;
public:
	aDeleteGraphicsSheet(size_t index, bool move_others, sf::Image image) :
		cAction("Delete Custom Graphics Sheet"), index(index), image(image), move_others(move_others) {}
};

/// Action that changes a custom graphics sheet
class aReplaceGraphicsSheet : public cAction {
	size_t index;
	sf::Image old_image;
	sf::Image new_image;
	bool undo_me() override;
	bool redo_me() override;
public:
	aReplaceGraphicsSheet(std::string name, size_t index, sf::Image old_image, sf::Image new_image) :
		cAction(name), index(index), old_image(old_image), new_image(new_image) {}
};

class aCreateDeleteSound : public cAction {
	size_t index;
	sf::SoundBuffer sound;
	bool undo_me() override;
	bool redo_me() override;
public:
	aCreateDeleteSound(bool create, size_t index, sf::SoundBuffer sound) :
		cAction(create ? "Import Custom Sound" : "Delete Custom Sound", !create),
		index(index), sound(sound) {}
};

class aReplaceSound : public cAction {
	size_t index;
	sf::SoundBuffer old_sound;
	sf::SoundBuffer new_sound;
	bool undo_me() override;
	bool redo_me() override;
public:
	aReplaceSound(size_t index, sf::SoundBuffer old_sound, sf::SoundBuffer new_sound) :
		cAction("Replace Custom Sound"),
		index(index), old_sound(old_sound), new_sound(new_sound) {}
};

class aEditScenarioDetails : public cAction {
	scen_details_t old_details;
	scen_details_t new_details;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditScenarioDetails(scen_details_t old_details, scen_details_t new_details) :
		cAction("Edit Scenario Details"), old_details(old_details), new_details(new_details) {}
};

class aEditIntro : public cAction {
	scen_intro_t old_intro;
	scen_intro_t new_intro;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditIntro(scen_intro_t old_intro, scen_intro_t new_intro) :
		cAction("Edit Scenario Intro/Picture"), old_intro(old_intro), new_intro(new_intro) {}
};

class aClassifyGraphics : public cAction {
	std::vector<ePicType> old_types;
	std::vector<ePicType> new_types;
	bool undo_me() override;
	bool redo_me() override;
public:
	aClassifyGraphics(std::vector<ePicType> old_types, std::vector<ePicType> new_types) :
		cAction("Classify Custom Graphics"), old_types(old_types), new_types(new_types) {}
};

class aEditAdvancedDetails : public cAction {
	scen_advanced_t old_advanced;
	scen_advanced_t new_advanced;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditAdvancedDetails(scen_advanced_t old_advanced, scen_advanced_t new_advanced) :
		cAction("Edit Advanced Details"), old_advanced(old_advanced), new_advanced(new_advanced) {}
};

class aEditTownVarying : public cAction {
	std::array<spec_loc_t,10> old_varying;
	std::array<spec_loc_t,10> new_varying;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditTownVarying(std::array<spec_loc_t,10> old_varying, std::array<spec_loc_t,10> new_varying) :
		cAction("Edit Town Varying Entrances"), old_varying(old_varying), new_varying(new_varying) {}
};

class aEditScenTimers : public cAction {
	std::array<cTimer,20> old_timers;
	std::array<cTimer,20> new_timers;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditScenTimers(std::array<cTimer,20> old_timers, std::array<cTimer,20> new_timers) :
		cAction("Edit Scenario Event Timers"), old_timers(old_timers), new_timers(new_timers) {}
};

class aEditItemShortcut : public cAction {
	size_t which;
	cScenario::cItemStorage old_shortcut;
	cScenario::cItemStorage new_shortcut;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditItemShortcut(size_t which, cScenario::cItemStorage old_shortcut, cScenario::cItemStorage new_shortcut) :
		cAction("Edit Item Placement Shortcut"), which(which), old_shortcut(old_shortcut), new_shortcut(new_shortcut) {}
};

#endif