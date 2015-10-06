
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

// After this are some globals that are referenced from common code but not defined, and not used in the test cases
#include "graphtool.hpp"
sf::RenderWindow mainPtr;
std::string scenario_temp_dir_name = "test_scenario";
cCustomGraphics spec_scen_g;

// And these are referenced from the scenario code, though not used in test cases
#include "scenario.hpp"
location cur_out;
short cur_town;
cScenario scenario;
