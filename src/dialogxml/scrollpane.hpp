//
//  scrollpane.hpp
//  BoE
//
//  Created by Celtic Minstrel on 15-10-01.
//
//

#ifndef BoE_scrollpane_hpp
#define BoE_scrollpane_hpp

/// @file
/// Scroll-pane-related classes and types

#include "scrollbar.hpp"

/// A container element that has a fixed viewport and a vertical scrollbar.
class cScrollPane : public cContainer {
	cScrollbar scroll;
	std::map<std::string, cControl*> contents;
	rectangle globalFrame;
	bool frameStyle;
	bool framed = false;
public:
	/// Create a new scroll pane
	explicit cScrollPane(cDialog& parent);
	std::string parse(ticpp::Element& who, std::string fname);
	void attachClickHandler(click_callback_t f) throw(xHandlerNotSupported);
	void attachFocusHandler(focus_callback_t f) throw(xHandlerNotSupported);
	bool triggerClickHandler(cDialog& me, std::string id, eKeyMod mods);
	bool handleClick(location where);
	void setFormat(eFormat prop, short val) throw(xUnsupportedProp);
	short getFormat(eFormat prop) throw(xUnsupportedProp);
	void setColour(sf::Color clr) throw(xUnsupportedProp);
	sf::Color getColour() throw(xUnsupportedProp);
	bool hasChild(std::string id);
	cControl& getChild(std::string id);
	storage_t store();
	void restore(storage_t to);
	bool isClickable();
	/// Add a new control to this pane.
	/// @param ctrl A pointer to the control, which should already be constructed.
	/// @param key A key to be used to look up the control later.
	/// @note This function is intended for internal use, which is why it takes a control pointer instead of a unique key.
	void addChild(cControl* ctrl, std::string key);
	/// Recalculate the pane's bounding rect based on its contained controls.
	void recalcRect();
	/// Get the pane's current scroll position.
	/// @return The current position.
	long getPosition();
	/// Get the pane's maximum scroll position.
	/// @return The maximum value.
	long getMaximum();
	/// Set the pane's current scroll position.
	/// @param to The new position.
	void setPosition(long to);
	void draw();
	cScrollPane& operator=(cScrollPane& other) = delete;
	cScrollPane(cScrollPane& other) = delete;
};

#endif
