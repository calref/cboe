

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

// After this are some globals that are referenced from common code but not defined, and not used in the test cases
#include "gfx/gfxsheets.hpp"
#include "universe/universe.hpp"

sf::RenderWindow& mainPtr()
{
	static sf::RenderWindow instance;
	return instance;
}
std::string scenario_temp_dir_name = "test_scenario";
cCustomGraphics spec_scen_g;
cUniverse univ;
std::vector<fs::path> extra_scen_dirs;
std::string help_text_rsrc;

bool check_for_interrupt(std::string) { return false; }
void save_prefs() {}

// And these are referenced from the scenario code, though not used in test cases
#include "scenario/scenario.hpp"
#include "tools/undo.hpp"
location cur_out;
short cur_town;
cScenario scenario;
bool change_made;
cUndoList undo_list;
