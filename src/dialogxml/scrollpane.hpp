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
	bool framed = false;
public:
	/// Create a new scroll pane
	explicit cScrollPane(cDialog& parent);
	std::string parse(ticpp::Element& who, std::string fname) override;
	bool handleClick(location where) override;
	void setFormat(eFormat prop, short val) throw(xUnsupportedProp) override;
	short getFormat(eFormat prop) throw(xUnsupportedProp) override;
	void setColour(sf::Color clr) throw(xUnsupportedProp) override;
	sf::Color getColour() throw(xUnsupportedProp) override;
	bool hasChild(std::string id) override;
	cControl& getChild(std::string id) override;
	storage_t store() override;
	void restore(storage_t to) override;
	bool isClickable() override;
	bool isFocusable() override;
	bool isScrollable() override;
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
	/// Get the scrollbar style.
	/// @return The style
	eScrollStyle getStyle();
	/// Set the scrollbar style.
	/// @param newStyle The new style.
	void setStyle(eScrollStyle newStyle);
	void draw() override;
	/// @copydoc cControl::getSupportedHandlers
	///
	/// @todo Document possible handlers
	const std::set<eDlogEvt> getSupportedHandlers() const override {
		return {EVT_CLICK, EVT_SCROLL, EVT_FOCUS, EVT_DEFOCUS};
	}
	void forEach(std::function<void(std::string,cControl&)> callback) override;
	cScrollPane& operator=(cScrollPane& other) = delete;
	cScrollPane(cScrollPane& other) = delete;
};

#endif
