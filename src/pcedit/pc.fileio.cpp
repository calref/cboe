
#include <cstring>
#include <cstdio>

#include "pc.global.hpp"
#include "universe.hpp"
#include "pc.fileio.hpp"
#include "pc.graphics.hpp"
#include "graphtool.hpp"
#include "soundtool.hpp"
#include "pc.editors.hpp"
#include "mathutil.hpp"
#include "dlogutil.hpp"
#include "restypes.hpp"
#include "fileio.hpp"

#define	DONE_BUTTON_ITEM	1

extern bool play_sounds;
extern short current_active_pc;
extern long stored_key;
extern sf::RenderWindow mainPtr;
extern cUniverse univ;

extern bool file_in_mem,party_in_scen,scen_items_loaded;

bool ae_loading = false;

typedef struct {
	char expl[96][96];
}	out_info_type;

char *party_encryptor;

std::string last_load_file = "Blades of Exile Save";

extern void update_item_menu();

extern short store_flags[3];
fs::path store_file_reply;

short give_intro_hint,display_mode;
short jl;
fs::path file_to_load;

void load_base_item_defs();
bool load_scen_item_defs(char scen_name[256]);

extern fs::path progDir;

void leave_town() {
	store_flags[0] = 5790;
}

void remove_party_from_scen() {
	univ.exportGraphics();
	univ.exportSummons();
	univ.party.scen_name = "";
	store_flags[1] = 200;
	party_in_scen = false;
	load_base_item_defs();
}

void load_base_item_defs(){
	fs::path basePath = progDir/"Blades of Exile Base"/"bladbase.exs";
	scen_items_loaded = load_scenario(basePath, univ.scenario);
}
