#include "drawable_manager.hpp"

void cDrawableManager::draw_all() {
	// layer order from lowest to highest is ensured by std::map
	for(auto & layer : this->layers) {
		for (auto & drawable : layer.second) {
			drawable.second->draw();
		}
	}
}
