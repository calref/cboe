//
//  keymods.cpp
//  Common
//
//  Created by Celtic Minstrel on 2023-01-12.
//

#include "keymods.hpp"
#include <SFML/Window/Keyboard.hpp>

keymods_t kb;

bool keymods_t::isAltPressed() const {
	return alt;
}

bool keymods_t::isCtrlPressed() const {
	return ctrl;
}

bool keymods_t::isShiftPressed() const {
	return shift;
}

bool keymods_t::isMetaPressed() const {
	return meta;
}

bool keymods_t::isSystemPressed() const {
#ifdef __APPLE__
	return meta;
#else
	return ctrl;
#endif
}

bool keymods_t::isUpPressed() const {
	return up;
}

bool keymods_t::isDownPressed() const {
	return down;
}

bool keymods_t::isLeftPressed() const {
	return left;
}

bool keymods_t::isRightPressed() const {
	return right;
}

bool keymods_t::handleModifier(const sf::Event& evt) {
	if(evt.type != sf::Event::KeyPressed && evt.type != sf::Event::KeyReleased) {
		return false;
	}
	using Key = sf::Keyboard::Key;
	bool newState = evt.type == sf::Event::KeyPressed;
	switch(evt.key.code) {
		case Key::LShift:
		case Key::RShift:
			shift = newState;
			break;
		case Key::LAlt:
		case Key::RAlt:
			alt = newState;
			break;
		case Key::LControl:
		case Key::RControl:
			ctrl = newState;
			break;
		case Key::LSystem:
		case Key::RSystem:
			meta = newState;
			break;
		
		case Key::Left:
			left = newState;
			return false;
		case Key::Right:
			right = newState;
			return false;
		case Key::Up:
			up = newState;
			return false;
		case Key::Down:
			down = newState;
			return false;
			
		default: return false;
	}
	return true;
}
