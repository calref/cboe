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

#endif