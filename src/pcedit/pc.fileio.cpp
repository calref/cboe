
#include <cstring>
#include <cstdio>

#include "universe.hpp"
#include "pc.fileio.hpp"
#include "pc.graphics.hpp"
#include "sounds.hpp"
#include "pc.editors.hpp"
#include "mathutil.hpp"
#include "fileio.hpp"

extern cUniverse univ;
extern bool party_in_scen,scen_items_loaded;
extern void update_item_menu();
void load_base_item_defs();

extern fs::path progDir;

void leave_town() {
	univ.party.town_num = 200;
}

void remove_party_from_scen() {
	univ.exportGraphics();
	univ.exportSummons();
	univ.party.scen_name = "";
	party_in_scen = false;
	load_base_item_defs();
}

void load_base_item_defs(){
	fs::path basePath = progDir/"Blades of Exile Base"/"bladbase.boes";
	scen_items_loaded = load_scenario(basePath, univ.scenario);
}
