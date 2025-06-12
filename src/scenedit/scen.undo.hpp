#ifndef BoE_scen_undo_h
#define BoE_scen_undo_h

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
	short town_num;
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

// Action that modified something in town or outdoor terrain, so we should show the modified area when undoing or redoing
class cTerrainAction : public cAction {
public:
	cTerrainAction(std::string name, short town_num, location where, bool reversed = false);
	cTerrainAction(std::string name, location out_sec, location where, bool reversed = false);
	// Construct cTerrainAction in the current town/outdoor section
	cTerrainAction(std::string name, location where, bool reversed = false);
	void undo();
	void redo();
	bool undo_me() = 0;
	bool redo_me() = 0;
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

/// Action that erased a special encounter from a spot
class aEraseSpecial : public cTerrainAction {
public:
	aEraseSpecial(spec_loc_t special) : cTerrainAction("Erase Special Encounter", special), for_redo(special) {}
	bool undo_me() override;
	bool redo_me() override;
private:
	spec_loc_t for_redo;
	bool editing_town;
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

#endif