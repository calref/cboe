#ifndef DRAWABLE_MANAGER_HPP
#define DRAWABLE_MANAGER_HPP

#include <string>
#include <memory>
#include <unordered_map>
#include <map>
#include "drawable.hpp"

class cDrawableManager {
	
public:
	
	void draw_all ();
	
	template <typename T> void add_drawable (int layer_id, std::string const name, std::shared_ptr <T> drawable) {
		this->layers[layer_id][name] = std::dynamic_pointer_cast <iDrawable> (drawable);
	}
	
private:
	
	using Layer = std::unordered_map <std::string, std::shared_ptr <iDrawable>>;
	
	std::map <int, Layer> layers;
};

#endif
