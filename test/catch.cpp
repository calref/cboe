
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

// After this are some globals that are referenced from common code but not defined, and not used in the test cases
#include "graphtool.hpp"
sf::Texture fields_gworld, anim_gworld, boom_gworld, dlogpics_gworld, items_gworld, missiles_gworld, monst_gworld[NUM_MONST_SHEETS];
sf::Texture pc_gworld, roads_gworld, small_ter_gworld, status_gworld, talkfaces_gworld, terrain_gworld[NUM_TER_SHEETS];
sf::Texture tiny_obj_gworld, vehicle_gworld;
sf::RenderWindow mainPtr;
std::string scenario_temp_dir_name = "test_scenario";
cCustomGraphics spec_scen_g;

// And these are referenced from the scenario code, though not used in test cases
#include "scenario.hpp"
location cur_out;
short cur_town;
cScenario scenario;
