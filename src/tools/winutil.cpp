#include "winutil.hpp"

#include "keymods.hpp"

// The default scale should be the largest that the user's screen can fit all three
// BoE application windows (because they should probably default to match each other).
double fallback_scale() {
	static double scale = 0;
	if(scale == 0){
		sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

		short max_width = max(boe_width, max(pc_width, scen_width));
		short max_height = max(boe_height, max(pc_height, scen_height)) + getMenubarHeight();

		std::vector<double> scale_options = {1.0, 1.5, 2.0, 3.0, 4.0};
		for(auto it = scale_options.rbegin(); it != scale_options.rend(); ++it){
			short max_scaled_width = max_width * (*it);
			short max_scaled_height = max_height * (*it);

			if(max_scaled_width <= desktop.width && max_scaled_height <= desktop.height){
				scale = (*it);
				break;
			}
		}
	}
	// Hopefully no one would ever have such a small monitor to not fit the default size.
	// But just in case:
	if(scale == 0){
		scale = 1.0;
	}

	return scale;
}

// We use many nested event loops in this codebase. Each one of them
// calls pollEvent() and they each need to remember to call handleModifier()
// or else modifier keys will claim to be held forever.
// The best solution for this is to wrap pollEvent() so that it calls
// handleModifier for us every time.
bool pollEvent(sf::Window& win, sf::Event& event){
    if (win.pollEvent(event)){
        if(kb.handleModifier(event)) return false;
        return true;
    }

    return false;
}

bool pollEvent(sf::Window* win, sf::Event& event){
    return pollEvent(*win, event);
}