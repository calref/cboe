//
//  scrollbar.h
//  BoE
//
//  Created by Celtic Minstrel on 14-03-26.
//
//

#ifndef BoE_scrollbar_h
#define BoE_scrollbar_h

/// @file
/// Scrollbar-related classes and types.

#include "control.hpp"
#include "graphtool.hpp"

/// A simple vertical scrollbar.
/// This has no coupling with scrollable data; that must be handled externally by
/// using the methods to get the scrollbar's position.
/// Alternatively, it can be used as a slider control.
class cScrollbar : public cControl {
	int pos, max, pgsz;
	std::string link;
	// Note: For horizontal scrollbars, up is left and down is right.
	enum {
		PART_UP,
		PART_PGUP,
		PART_THUMB,
		PART_PGDN,
		PART_DOWN,
	} pressedPart;
	bool vert = true;
	click_callback_t onClick;
	static sf::Texture scroll_gw;
	static const rectangle up_rect[4], down_rect[4], bar_rect[4], thumb_rect[4];
	void draw_vertical(), draw_horizontal();
public:
	/// @copydoc cDialog::init()
	static void init();
	std::string parse(ticpp::Element& who, std::string fname);
	/// Create a new scrollbar without a parent dialog.
	/// @param parent The parent window.
	explicit cScrollbar(sf::RenderWindow& parent);
	/// Create a new scrollbar.
	/// @param parent The parent dialog.
	explicit cScrollbar(cDialog& parent);
	void attachClickHandler(click_callback_t f) throw(xHandlerNotSupported);
	void attachFocusHandler(focus_callback_t f) throw(xHandlerNotSupported);
	bool triggerClickHandler(cDialog& me, std::string id, eKeyMod mods);
	bool handleClick(location where);
	void setFormat(eFormat prop, short val) throw(xUnsupportedProp);
	short getFormat(eFormat prop) throw(xUnsupportedProp);
	void setColour(sf::Color clr) throw(xUnsupportedProp);
	sf::Color getColour() throw(xUnsupportedProp);
	storage_t store();
	void restore(storage_t to);
	bool isClickable();
	/// Get the scrollbar thumb's current position.
	/// @return The current position.
	long getPosition();
	/// Get the scrollbar thumb's maximum value.
	/// @return The maximum value.
	long getMaximum();
	/// Get the scrollbar thumb's page size.
	/// @return The page size.
	///
	/// The page size is the number of steps scrolled when a click is received
	/// in the area between the arrow buttons and the scrollbar thumb.
	long getPageSize();
	/// Check whether the scrollbar is vertical.
	/// @return True if it is vertical, false if it is horizontal.
	bool isVertical();
	/// Get the linked control.
	///
	/// A linked control is one that always reflect's the scrollbar's current
	/// value as its text.
	/// @return The ID of the linked control, or an empty string if none.
	std::string getLink();
	/// Set the scrollbar thumb's current position.
	/// @param to The new position.
	void setPosition(long to);
	/// Set the scrollbar thumb's maximum value.
	/// @param to The maximum value.
	void setMaximum(long to);
	/// Set the scrollbar thumb's page size.
	/// @param to The page size.
	///
	/// The page size is the number of steps scrolled when a click is received
	/// in the area between the arrow buttons and the scrollbar thumb.
	void setPageSize(long to);
	/// Set whether the scrollbar is vertical.
	/// @param is True to make it vertical, false to make it horizontal.
	void setVertical(bool is);
	/// Set the linked control.
	///
	/// A linked control is one that always reflect's the scrollbar's current
	/// value as its text.
	/// @param l The ID of the linked control, or an empty string if none.
	void setLink(std::string l);
	void draw();
	cScrollbar& operator=(cScrollbar& other) = delete;
	cScrollbar(cScrollbar& other) = delete;
};

#endif
