//
//  boe.minimap.hpp
//  Blades of Exile
//
//  Created by alonso on 10/10/2021.
//

#ifndef boe_minimap_hpp
#define boe_minimap_hpp

namespace minimap {
	void init(); // create the map world
	bool is_visible();
	void set_visible(bool visible); // set the map windows visible/hidden

	void draw(bool need_refresh); // redraw the map content, if need_refresh recompute it

	bool need_redraw();
	void add_pending_redraw();

	bool pollEvent();
}
#endif /* boe_minimap_hpp */
