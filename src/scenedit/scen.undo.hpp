#ifndef BoE_scen_undo_h
#define BoE_scen_undo_h

#include "location.hpp"
#include "tools/undo.hpp"

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

// Action that modified town or outdoor terrain, so we should show the modified area when undoing or redoing
class cTerrainAction : public cAction {
public:
	cTerrainAction(std::string name, short town_num, location where);
	cTerrainAction(std::string name, location out_sec, location where);
	cTerrainAction(std::string name, location where);
	void undo();
	void redo();
	bool undo_me() = 0;
	bool redo_me() = 0;
private:
	/// Show where the change happened
	void showChangeSite();
	area_ref_t area;
};

/// Represents the action of adding a new town to the end of the list
class aNewTown : public cAction {
	class cTown* theTown;
	bool undo_me() override;
	bool redo_me() override;
public:
	aNewTown(class cTown* t);
	~aNewTown();
};

class aEraseSpecial : public cTerrainAction {
public:
	aEraseSpecial(spec_loc_t special) : cTerrainAction("Erase Special Encounter", special), for_redo(special) {}
	bool undo_me() override;
	bool redo_me() override;
private:
	spec_loc_t for_redo;
	bool editing_town;
};

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

#endif