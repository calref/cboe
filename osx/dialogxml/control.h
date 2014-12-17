/*
 *  control.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef CONTROL_H
#define CONTROL_H

/// @file
/// Control-related classes and types.

#include <SFML/Graphics.hpp>

#include <string>
#include <exception>
#include <functional>
#include "dialog.h"

#include "location.h"

//struct cPict {
//	short pict;
//	short type;
//};

/// Formatting properties
enum eFormat {
	TXT_FRAME,	///< Whether to draw a frame around the control. Should be a boolean (true or false).
	TXT_FONT,	///< The control's text font. Should be one of the constants FONT_PLAIN, FONT_BOLD, FONT_DUNGEON, FONT_MAIDEN.
	TXT_SIZE,	///< The control's text size. Should be an integer indicating point size.
	TXT_WRAP,	///< Whether the control should wrap. Should be a boolean (true or false).
	TXT_FRAMESTYLE, ///< The control's frame style. Should be a boolean (true or false). @see cControl::drawFrame()
};

/// Specifies the type of a control.
enum eControlType {
	CTRL_UNKNOWN,
	CTRL_BTN,	///< An ordinary push button
	CTRL_LED,	///< An LED (checkbox/radiobutton)
	CTRL_PICT,	///< A picture
	CTRL_FIELD,	///< An edit text field
	CTRL_TEXT,	///< A static text object
	CTRL_GROUP,	///< A LED radiobutton-like group
	CTRL_STACK,	///< A group of controls that display pages (not implemented yet)
	CTRL_SCROLL,///< A scrollbar
};

/// The signature of a click handler.
typedef std::function<bool(cDialog&,std::string,eKeyMod)> click_callback_t;
/// The signature of a focus handler.
typedef std::function<bool(cDialog&,std::string,bool)> focus_callback_t;

/// Thrown when you try to set a handler that the control does not support.
class xHandlerNotSupported : std::exception {
	static const char* focusMsg;
	static const char* clickMsg;
	bool isFocus;
public:
	/// Construct a new exception.
	/// @param isFocus true to indicate a focus event, false for a click event.
	xHandlerNotSupported(bool isFocus);
	/// @return The error message.
	const char* what();
};

/// Thrown when you try to set or retrieve a formatting property that the control does not support.
class xUnsupportedProp : std::exception {
	eFormat whichProp;
	char* msg;
public:
	/// Construct a new exception.
	/// @param prop The unsupported format property.
	xUnsupportedProp(eFormat prop) throw();
	~xUnsupportedProp() throw();
	/// @return The error message.
	const char* what() throw();
};

/// The superclass of all dialog controls.
/// Some controls can be created in an arbitrary window, rather than a dialog controlled by cDialog.
/// In this case, the event loop of the parent window is responsible for calling draw() when the control needs
/// to be drawn, handleClick() when a mousedown event is received within the control's bounding rect, and
/// triggerClickHandler() if a click occurs, either because handleClick() returns true or because
/// a keyboard event is received that should trigger the control.
class cControl {
public:
	/// Attach a keyboard shortcut to a control. Pressing the keyboard shortcut is equivalent to clicking the control.
	/// @param key The desired keyboard shortcut.
	void attachKey(cKey key);
	/// Detach any currently assigned keyboard shortcut from the control.
	void detachKey();
	/// Set the control's text to a representation of its assigned keyboard shortcut.
	void setTextToKey();
	/// Check if the control has an assigned keyboard shortcut.
	/// @return true if a keyboard shortcut is assigned.
	bool hasKey();
	/// Retrieve the control's current keyboard shortcut.
	/// @return the currently-assigned keyboard shortcut.
	/// @note You should first check that a shortcut is assigned using hasKey().
	cKey getAttachedKey();
	/// Attach a click handler to this control.
	/// @param f The click handler to attach.
	/// @throw xHandlerNotSupported if this control does not support click handlers. Most controls do support click handlers.
	/// @note Only one click handler can be set at a time. To remove the click handler, set it to null.
	///
	/// A click handler must be able to accept three parameters: a reference to the containing dialog, the unique key of the
	/// clicked item, and a representation of any modifier keys that are currently held.
	virtual void attachClickHandler(click_callback_t f) throw(xHandlerNotSupported) = 0;
	/// Attach a focus handler to this control.
	/// @param f The focus handler to attach.
	/// @throw xHandlerNotSupported if this control does not support focus handlers. Most controls do not support focus handlers.
	/// @note Only one focus handler can be set at a time. To remove the focus handler, set it to null.
	///
	/// A focus handler must be able to accept three parameters: a reference to the containing dialog, the unique key of the
	/// clicked item, and a boolean indicating whether focus is being lost or gained; a value of true indicates that
	/// focus is being lost, while false indicates it's being gained. Most handlers will only need to act when the
	/// third parameter is true. If the handler returns false, the focus change is cancelled.
	virtual void attachFocusHandler(focus_callback_t f) throw(xHandlerNotSupported) = 0;
	/// Trigger the click handler for this control.
	/// @param me A reference to the current dialog.
	/// @param id The unique key of this control.
	/// @param mods The currently-held keyboard modifiers.
	/// @return true if the event should continue, false if it should be cancelled.
	virtual bool triggerClickHandler(cDialog& me, std::string id, eKeyMod mods);
	/// Trigger the focus handler for this control.
	/// @param me A reference to the current dialog.
	/// @param id The unique key of this control.
	/// @param losingFocus true if this control is losing focus, false if it is gaining focus.
	/// @return true if the event should continue, false if it should be cancelled.
	virtual bool triggerFocusHandler(cDialog& me, std::string id, bool losingFocus);
	//virtual void setPict(short pict, short type) = 0;
	/// Make this control visible.
	virtual void show(); // cd_activate_item true
	/// Make this control invisible.
	virtual void hide(); // cd_activate_item false
	/// Check if this control is visible.
	/// @return true if it's visible
	bool isVisible(); // cd_get_active
	/// Set if this control is active. A control is normally active when the mouse button is held down within its bounding rect.
	/// @param active true if it should be active, false if not
	void setActive(bool active); // "active" here means "selected", so if it's a button, draw it pressed
	/// Get the type of this control
	/// @return The type of control
	eControlType getType();
	/// Set the control's text.
	/// @param l The new text.
	virtual void setText(std::string l);
	/// Fetch the control's text.
	/// @return The control's current text.
	virtual std::string getText();
	/// Get the bounding rect of this control.
	/// @return The control's bounding rect.
	rectangle getBounds();
	/// Set the bounding rect of this control.
	/// @param newBounds The new bounding rect.
	void setBounds(rectangle newBounds);
	/// Set the position of this control.
	/// @param to The new position.
	void relocate(location to);
	/// Get the control's text as an integer.
	/// @return The control's text, coerced to an integer.
	long long getTextAsNum();
	/// Set the control's text to an integer value.
	/// @param what The desired value.
	void setTextToNum(long long what);
	/// Set one of the control's formatting parameters.
	/// @param prop The parameter to set.
	/// @param val The desired value of the parameter.
	/// @throw xUnsupportedProp if this control doesn't support the given parameter.
	virtual void setFormat(eFormat prop, short val) throw(xUnsupportedProp) = 0;
	/// Get one of the control's formatting parameters.
	/// @param prop The parameter to retrieve.
	/// @return The value of the parameter.
	/// @throw xUnsupportedProp if this control doesn't support the given parameter.
	virtual short getFormat(eFormat prop) throw(xUnsupportedProp) = 0;
	/// Set the control's colour (usually text colour).
	/// @param clr The desired colour.
	/// @throw xUnsupportedProp if this control does not support colour.
	virtual void setColour(sf::Color clr) throw(xUnsupportedProp) = 0;
	/// Get the control's colour.
	/// @return The current colour.
	/// @throw xUnsupportedProp if this control does not support colour.
	virtual sf::Color getColour() throw(xUnsupportedProp) = 0;
	/// Check if the control is clickable.
	/// @return true if it's clickable.
	/// @note This does not indicate whether the control supports click handlers.
	/// In fact, some controls return true only if a click handler is assigned.
	/// Others, like editable text fields, are clickable but do not support click handlers.
	virtual bool isClickable() = 0;
	/// Handles the action of clicking this control.
	/// @param where The exact location at which the mouse was pressed, relative to the dialog.
	/// @return true if the click was successful; false if it was cancelled.
	///
	/// This function should implement an event loop and terminate when the mouse button is released.
	/// It can be overridden to customize the reaction of the control to mouse events.
	/// The default implementation works for a simple clickable object such as a button that
	/// should be hilited in some way while pressed and is cancelled by releasing the mouse
	/// button outside the control's bounds.
	virtual bool handleClick(location where);
	/// Specifies that another control acts as a label for this one.
	/// The practical effect of this is that hiding or showing this control automatically hides or shows the label as well.
	/// @param label A pointer to the control that acts as a label.
	void setLabelCtrl(cControl* label);
	/// Create a new control attached to an arbitrary window, rather than a dialog.
	/// @param t The type of the control.
	/// @param p The parent window.
	cControl(eControlType t, sf::RenderWindow& p);
	/// Create a new control attached to a dialog.
	/// @param t The type of the control.
	/// @param p The parent dialog.
	cControl(eControlType t, cDialog& p);
	virtual ~cControl();
	/// Draw the control into its parent window.
	virtual void draw() = 0;
	cControl& operator=(cControl& other) = delete;
	cControl(cControl& other) = delete;
protected:
	/// The parent dialog of the control.
	/// May be null, if the control was created via cControl(eControlType,sf::RenderWindow&).
	cDialog* parent;
	/// This control's labelling control.
	cControl* labelCtrl = NULL;
	/// The parent window of the control.
	/// This is for use in implementing draw().
	sf::RenderWindow* inWindow;
	/// The control's current text.
	std::string lbl;
	/// Whether the control is visible
	bool visible, depressed = false; ///< Whether the control is depressed; only applicable for clickable controls
	/// The control's bounding rect.
	rectangle frame;
	/// The control's frame style.
	int frameStyle;
	/// The control's attached key.
	cKey key;
	/// Draw a frame around the control.
	/// @param amt How much to offset the frame from the control's bounding rect.
	/// @param med_or_lt true to use a darker colour for the frame.
	/// @note The TXT_FRAMESTYLE formatting property is normally used for the second parameter.
	void drawFrame(short amt, bool med_or_lt);
	/// Redraws the parent dialog, if any.
	/// Intended to be called from handleClick(), where there is usually a minor event loop happening.
	void redraw();
private:
	eControlType type;
};

#endif
