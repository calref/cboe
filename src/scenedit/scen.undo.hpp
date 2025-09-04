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

#include "scen.global.hpp"


extern cScenario scenario;
extern cTown* town;
extern cArea* get_current_area();
extern bool editing_town;
extern short cur_town;
extern location cur_out;

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

location closest_to_view(stroke_ter_changes_t changes);
location closest_to_view(item_changes_t changes);

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
		cTerrainAction(name, closest_to_view(stroke_changes)),
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

class aEditTownDetails : public cAction {
	size_t which;
	town_details_t old_details;
	town_details_t new_details;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditTownDetails(size_t which, town_details_t old_details, town_details_t new_details) :
		cAction("Edit Town Details"), which(which), old_details(old_details), new_details(new_details) {}
};

class aEditTownWandering : public cAction {
	size_t which;
	std::array<cTown::cWandering,4> old_wandering;
	std::array<location, 4> old_wandering_locs;
	std::array<cTown::cWandering,4> new_wandering;
	std::array<location, 4> new_wandering_locs;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditTownWandering(size_t which, std::array<cTown::cWandering,4> old_wandering, std::array<location, 4> old_wandering_locs,
							 std::array<cTown::cWandering,4> new_wandering, std::array<location, 4> new_wandering_locs) :
		cAction("Edit Town Wandering Monsters"), which(which), old_wandering(old_wandering), old_wandering_locs(old_wandering_locs),
													new_wandering(new_wandering), new_wandering_locs(new_wandering_locs) {}
};

class aEditTownAdvancedDetails : public cAction {
	size_t which;
	town_advanced_t old_details;
	town_advanced_t new_details;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditTownAdvancedDetails(size_t which, town_advanced_t old_details, town_advanced_t new_details) :
		cAction("Edit Town Advanced Details"), which(which), old_details(old_details), new_details(new_details) {}
};

class aEditTownTimers : public cAction {
	size_t which;
	std::array<cTimer,8> old_timers;
	std::array<cTimer,8> new_timers;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditTownTimers(size_t which, std::array<cTimer,8> old_timers, std::array<cTimer,8> new_timers) :
		cAction("Edit Town Event Timers"), which(which), old_timers(old_timers), new_timers(new_timers) {}
};

class aEditOutDetails : public cAction {
	location out_sec;
	out_details_t old_details;
	out_details_t new_details;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditOutDetails(location out_sec, out_details_t old_details, out_details_t new_details) :
		cAction("Edit Outdoor Details"), out_sec(out_sec), old_details(old_details), new_details(new_details) {}
};

class aEditOutEncounter : public cAction {
	location out_sec;
	short mode;
	size_t which;
	cOutdoors::cWandering old_enc;
	cOutdoors::cWandering new_enc;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditOutEncounter(bool wandering, location out_sec, size_t which, cOutdoors::cWandering old_enc, cOutdoors::cWandering new_enc) :
		cAction(wandering ? "Edit Outdoor Wandering Encounter" : "Edit Outdoor Special Encounter"),
		out_sec(out_sec), mode(wandering ? 0 : 1), which(which), old_enc(old_enc), new_enc(new_enc) {}
};

class aMoveOutEncounterLoc : public cTerrainAction {
	size_t which;
	location old_loc;
	bool undo_me() override;
	bool redo_me() override;
public:
	aMoveOutEncounterLoc(size_t which, location old_loc, location new_loc) :
		cTerrainAction("Move Outdoor Encounter Location", new_loc), which(which), old_loc(old_loc) {}
};

class aEditPersonality : public cAction {
	size_t town_num;
	size_t which;
	cPersonality old_pers;
	cPersonality new_pers;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditPersonality(size_t town_num, size_t which, cPersonality old_pers, cPersonality new_pers) :
		cAction("Edit Personality"), town_num(town_num), which(which), old_pers(old_pers), new_pers(new_pers) {}
};

class aCreateDeleteTalkNode : public cAction {
	size_t town_num;
	size_t which;
	cSpeech::cNode node;
	bool undo_me() override;
	bool redo_me() override;
public:
	aCreateDeleteTalkNode(bool create, size_t town_num, size_t which, cSpeech::cNode node) :
		cAction(create ? "Create Talk Node" : "Delete Talk Node", !create), town_num(town_num), which(which), node(node) {}
};

class aEditTalkNode : public cAction {
	size_t town_num;
	size_t which;
	cSpeech::cNode old_node;
	cSpeech::cNode new_node;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditTalkNode(size_t town_num, size_t which, cSpeech::cNode old_node, cSpeech::cNode new_node) :
		cAction("Edit Talk Node"), town_num(town_num), which(which), old_node(old_node), new_node(new_node) {}
};

/// Action representing the deletion of a string tied to a location or rectangle in an area
class aDeleteLocString : public cAction {
	cArea* area;
	bool is_town;
	bool is_sign; // either a sign or an area description
	sign_loc_t sign;
	info_rect_t desc;
	bool undo_me() override;
	bool redo_me() override;
public:
	aDeleteLocString(cArea* area, sign_loc_t sign) :
		cAction("Delete Sign Text"), area(area), is_town(editing_town), is_sign(true), sign(sign) {}
	aDeleteLocString(cArea* area, info_rect_t desc) :
		cAction("Delete Area Description"), area(area), is_town(editing_town), is_sign(false), desc(desc) {}
};

/// Action representing clearing of a string tied to a location or rectangle in an area.
/// Separate from aEditClearString because it also clears the locations.
class aClearLocString : public cAction {
	cArea* area;
	size_t which;
	bool is_town;
	bool is_sign; // either a sign or an area description
	sign_loc_t old_sign;
	info_rect_t old_desc;
	sign_loc_t new_sign;
	info_rect_t new_desc;
	bool undo_me() override;
	bool redo_me() override;
public:
	aClearLocString(cArea* area, size_t which, sign_loc_t old_sign, sign_loc_t new_sign) :
		cAction("Clear Sign Text"), area(area), which(which), is_town(editing_town), is_sign(true), old_sign(old_sign), new_sign(new_sign) {
	}
	aClearLocString(cArea* area, size_t which, info_rect_t old_desc, info_rect_t new_desc) :
		cAction("Clear Area Description"), area(area), which(which), is_town(editing_town), is_sign(false), old_desc(old_desc), new_desc(new_desc) {
	}
};

/// Action representing editing a string of any type in the scenario, or clearing a string that has no location tied to it
class aEditClearString : public cAction {
	eStrMode str_mode;
	size_t which;
	std::string old_value;
	std::string new_value;
	bool undo_me() override;
	bool redo_me() override;
	// undo/redo for town text and outdoor text depends on global state of which town/outdoor section are active
	size_t which_town;
	location which_out;
public:
	aEditClearString(std::string name, eStrMode str_mode, size_t which, std::string old_value, std::string new_value) :
		cAction(name), str_mode(str_mode), which(which), old_value(old_value), new_value(new_value), which_town(cur_town), which_out(cur_out) {}
};

class aDeleteString : public cAction {
	eStrMode str_mode;
	std::string str;
	// undo/redo for town text and outdoor text depends on global state of which town/outdoor section are active
	size_t which_town;
	location which_out;
	bool undo_me() override;
	bool redo_me() override;
public:
	aDeleteString(std::string name, eStrMode mode, std::string str) :
		cAction(name), str_mode(mode), which_town(cur_town), which_out(cur_out), str(str) {}
};

class aCreateStrings : public cAction {
	eStrMode str_mode;
	std::vector<std::string> strs;
	// undo/redo for town text and outdoor text depends on global state of which town/outdoor section are active
	size_t which_town;
	location which_out;
	bool undo_me() override;
	bool redo_me() override;
public:
	aCreateStrings(eStrMode mode, std::vector<std::string> strs) :
		cAction(edit_string_action_name("Create", mode) + (strs.size() > 1 ? "s" : "")), str_mode(mode), strs(strs), which_town(cur_town), which_out(cur_out) {}
};

class aCreateDeleteSpecial : public cAction {
	short mode;
	cSpecial spec;
	// undo/redo for town and outdoor nodes depends on global state of which town/outdoor section are active
	size_t which_town;
	location which_out;
	bool undo_me() override;
	bool redo_me() override;
public:
	aCreateDeleteSpecial(bool create, short mode, cSpecial spec) :
		cAction(create ? "Create Special Node" : "Delete Special Node", !create), mode(mode), spec(spec), which_town(cur_town), which_out(cur_out) {}
};

class aEditSpecial : public cAction {
	short mode;
	size_t which;
	// undo/redo for town and outdoor nodes depends on global state of which town/outdoor section are active
	size_t which_town;
	location which_out;
	cSpecial old_spec;
	cSpecial new_spec;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditSpecial(short mode, size_t which, cSpecial old_spec, cSpecial new_spec) :
		cAction("Edit Special Node"), mode(mode), which(which), old_spec(old_spec), new_spec(new_spec), which_town(cur_town), which_out(cur_out) {}
	aEditSpecial(short mode, size_t which, cSpecial old_spec) :
		cAction("Clear Special Node"), mode(mode), which(which), old_spec(old_spec), new_spec(cSpecial()), which_town(cur_town), which_out(cur_out) {}
};

class aEditTownBounds : public cTerrainAction {
	rectangle old_rect;
	rectangle new_rect;
	bool for_saved_items;
	bool undo_me() override;
	bool redo_me() override;
public:
	aEditTownBounds(bool saved_items, rectangle old_rect, rectangle new_rect) :
		cTerrainAction((saved_items ?
							(old_rect.empty() ?
								"Create Saved Item Area"
								: "Edit Saved Item Area")
							:"Edit Town Boundaries"), new_rect.topLeft()),
		for_saved_items(saved_items), old_rect(old_rect), new_rect(new_rect) {}
};

class aCreateAreaRect : public cTerrainAction {
	size_t which;
	info_rect_t rect;
	cArea* area;
	bool undo_me() override;
	bool redo_me() override;
public:
	aCreateAreaRect(size_t which, info_rect_t rect) :
		cTerrainAction("Create Area Description", rect.topLeft()),
		which(which),
		rect(rect),
		area(get_current_area()) {}
};

#endif