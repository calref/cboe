#include "winutil.hpp"

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