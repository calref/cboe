//
//  keymods.hpp
//  Common
//
//  Created by Celtic Minstrel on 2023-01-12.
//

#ifndef BOE_keymods_hpp
#define BOE_keymods_hpp

#include <SFML/Window/Event.hpp>

class keymods_t {
	bool alt = false, ctrl = false, shift = false, meta = false;
	bool up = false, down = false, left = false, right = false;
public:
	bool isAltPressed() const;
	bool isCtrlPressed() const;
	bool isShiftPressed() const;
	// Windows key, Command key, Meta key, etc
	bool isMetaPressed() const;
	// System is a special pseudo-key - it's Command on Mac and Control on other systems
	bool isSystemPressed() const;
	bool isUpPressed() const;
	bool isDownPressed() const;
	bool isLeftPressed() const;
	bool isRightPressed() const;
	bool handleModifier(const sf::Event& evt);
	void flushModifiers();
};

extern keymods_t kb;

#endif /* keymods_hpp */
