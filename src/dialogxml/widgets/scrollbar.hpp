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
#include "tools/event_listener.hpp"
#include "tools/drawable.hpp"

/// Specifies the style of a scrollbar.
enum eScrollStyle {
	SCROLL_WHITE, ///< The white scrollbar, matching the help and PC buttons.
	SCROLL_LED, ///< A dark scrollbar matching the LEDs and dialog buttons
};

/// A simple vertical scrollbar.
/// This has no coupling with scrollable data; that must be handled externally by
/// using the methods to get the scrollbar's position.
/// Alternatively, it can be used as a slider control.
class cScrollbar : public cControl, public iEventListener, public iDrawable {
	int pos, max, pgsz;
	std::string link;
	// Make sure this is equal to the number of constants in eScrollStyle
	static const int NUM_STYLES = 2;
	// Constants to index the rect arrays
	enum {
		VERT, VERT_PRESSED, HORZ, HORZ_PRESSED
	};
	// Note: For horizontal scrollbars, up is left and down is right.
	enum eScrollbarPart {
		PART_UP,
		PART_PGUP,
		PART_THUMB,
		PART_PGDN,
		PART_DOWN,
	} pressedPart;
	void handlePressedPart(eScrollbarPart pressedPart);
	eScrollStyle style = SCROLL_WHITE;
	bool vert = true;
	static std::string scroll_textures[NUM_STYLES];
	static const rectangle up_rect[NUM_STYLES][4], down_rect[NUM_STYLES][4], bar_rect[NUM_STYLES][4], thumb_rect[NUM_STYLES][4];
	// Mouse wheel scrolling events inside this rectangle will be handled by the scrollbar.
	// Should at least cover the scrollbar itself, but can extend outside (example: scrolling
	// in the inventory area). 
	rectangle wheel_event_rect {0, 0, 0, 0};
	void draw_vertical(), draw_horizontal();
	eScrollbarPart location_to_part(const location& location) const;
	location mouse_pressed_at;
	int drag_start_position;
	bool handle_mouse_pressed(const sf::Event&);
	bool handle_mouse_moved(const sf::Event&);
	bool handle_mouse_released(const sf::Event&);
	bool handle_mouse_wheel_scrolled(const sf::Event&);
	void handle_thumb_drag(const location&);
public:
	/// @copydoc cDialog::init()
	static void init();
	bool parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) override;
	void validatePostParse(ticpp::Element& who, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& nodes) override;
	location getPreferredSize() const override;
	/// Create a new scrollbar.
	/// @param parent The parent.
	explicit cScrollbar(iComponent& parent);
	bool handleClick(location where, cFramerateLimiter& fps_limiter) override;
	storage_t store() const override;
	void restore(storage_t to) override;
	bool isClickable() const override;
	bool isFocusable() const override;
	bool isScrollable() const override;
	/// Get the scrollbar thumb's current position.
	/// @return The current position.
	long getPosition() const;
	/// Get the scrollbar thumb's maximum value.
	/// @return The maximum value.
	long getMaximum() const;
	/// Get the scrollbar thumb's page size.
	/// @return The page size.
	///
	/// The page size is the number of steps scrolled when a click is received
	/// in the area between the arrow buttons and the scrollbar thumb.
	long getPageSize() const;
	/// Check whether the scrollbar is vertical.
	/// @return True if it is vertical, false if it is horizontal.
	bool isVertical() const;
	/// Get the linked control.
	///
	/// A linked control is one that always reflect's the scrollbar's current
	/// value as its text.
	/// @return The ID of the linked control, or an empty string if none.
	std::string getLink() const;
	/// Get the scrollbar style.
	/// @return The style
	eScrollStyle getStyle() const;
	/// Set the scrollbar thumb's current position.
	/// @param to The new position.
	void setPosition(long to, bool record = false);
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
	/// Set the scrollbar style.
	/// @param newStyle The new style.
	void setStyle(eScrollStyle newStyle);
	void set_wheel_event_rect(rectangle);
	virtual void draw() override;
	virtual bool handle_event(const sf::Event&) override;
	/// @copydoc cControl::getSupportedHandlers
	///
	/// @todo Document possible handlers
	std::set<eDlogEvt> getSupportedHandlers() const override {
		return {EVT_CLICK, EVT_SCROLL};
	}
	cScrollbar& operator=(cScrollbar& other) = delete;
	cScrollbar(cScrollbar& other) = delete;
};

#endif
