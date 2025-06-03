#ifndef BoE_scen_undo_h
#define BoE_scen_undo_h

#include "location.hpp"
#include "tools/undo.hpp"
#include "scenario/town.hpp"

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

// Action that modified something in town or outdoor terrain, so we should show the modified area when undoing or redoing
class cTerrainAction : public cAction {
public:
	cTerrainAction(std::string name, short town_num, location where, bool reversed = false);
	cTerrainAction(std::string name, location out_sec, location where, bool reversed = false);
	cTerrainAction(std::string name, location where, bool reversed = false);
	void undo();
	void redo();
	bool undo_me() = 0;
	bool redo_me() = 0;
private:
	/// Show where the change happened
	void showChangeSite();
	area_ref_t area;
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


#endif