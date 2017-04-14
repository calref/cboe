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
#include <set>
#include <boost/any.hpp>
#include "dialog.hpp"

#include "location.hpp"

/// Formatting properties
enum eFormat {
	TXT_FRAME,	///< Whether to draw a frame around the control. Should be a boolean (true or false).
	TXT_FONT,	///< The control's text font. Should be one of the constants FONT_PLAIN, FONT_BOLD, FONT_DUNGEON, FONT_MAIDEN.
	TXT_SIZE,	///< The control's text size. Should be an integer indicating point size.
	TXT_WRAP,	///< Whether the control should wrap. Should be a boolean (true or false).
	TXT_FRAMESTYLE, ///< The control's frame style. Should be an enum from @ref eFrameStyle.
};

/// Frame styles
enum eFrameStyle {
	FRM_INSET,	///< An outline style that makes it look like the interior is slightly depressed.
	FRM_OUTSET,	///< An outline style that makes it look like the interior is slightly raise.
	FRM_SOLID,	///< A solid outline.
	FRM_DOUBLE,	///< A solid double outline.
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
	CTRL_STACK,	///< A group of controls that represents one element in an array
	CTRL_SCROLL,///< A scrollbar
	CTRL_PANE,	///< A scroll pane
};

/// Thrown when you try to set a handler that the control does not support.
class xHandlerNotSupported : public std::exception {
	static const char* msg[4];
	eDlogEvt evt;
public:
	/// Construct a new exception.
	/// @param t The type of event.
	xHandlerNotSupported(eDlogEvt t);
	/// @return The error message.
	const char* what() const throw();
};

/// Thrown when you try to set or retrieve a formatting property that the control does not support.
class xUnsupportedProp : public std::exception {
	eFormat whichProp;
	mutable char* msg;
public:
	/// Construct a new exception.
	/// @param prop The unsupported format property.
	xUnsupportedProp(eFormat prop) throw();
	~xUnsupportedProp() throw();
	/// @return The error message.
	const char* what() const throw();
};

/// The superclass of all dialog controls.
/// Some controls can be created in an arbitrary window, rather than a dialog controlled by cDialog.
/// In this case, the event loop of the parent window is responsible for calling draw() when the control needs
/// to be drawn, handleClick() when a mousedown event is received within the control's bounding rect, and
/// triggerClickHandler() if a click occurs, either because handleClick() returns true or because
/// a keyboard event is received that should trigger the control.
///
/// All control subclasses must have a constructor that takes a single cDialog& parameter,
/// in order for the parsing template to work.
class cControl {
public:
	using storage_t = std::map<std::string, boost::any>;
	/// Parses the control from an XML element
	virtual std::string parse(ticpp::Element& who, std::string fname) = 0;
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
	/// Attach an event handler to this control.
	/// @tparam t The type of event to attach.
	/// @param handler The event handler function or functor. Its signature depends on the event type.
	/// @return The previous handler that has been overridden, if any.
	/// @throw xHandlerNotSupported if the event type is not supported by this control.
	/// @note Only one handler can be set at a time for any given event. To remove a handler, set it to nullptr.
	template<eDlogEvt t> typename event_fcn<t>::type attachEventHandler(typename event_fcn<t>::type handler) {
		if(getSupportedHandlers().count(t) == 0) throw xHandlerNotSupported(t);
		auto old_handler = event_handlers[t];
		if(handler) event_handlers[t] = handler;
		else event_handlers[t].clear();
		if(old_handler.empty()) return nullptr;
		return boost::any_cast<typename event_fcn<t>::type>(old_handler);
	}
	/// Attach a click handler to this control.
	/// @param f The click handler to attach.
	/// @throw xHandlerNotSupported if this control does not support click handlers. Most controls do support click handlers.
	/// @deprecated in favour of @ref attachEventHandler
	/// @note Only one click handler can be set at a time. To remove the click handler, set it to null.
	///
	/// A click handler must be able to accept three parameters: a reference to the containing dialog, the unique key of the
	/// clicked item, and a representation of any modifier keys that are currently held.
	virtual void attachClickHandler(std::function<bool(cDialog&,std::string,eKeyMod)> f) throw(xHandlerNotSupported);
	/// Attach a focus handler to this control.
	/// @param f The focus handler to attach.
	/// @throw xHandlerNotSupported if this control does not support focus handlers. Most controls do not support focus handlers.
	/// @deprecated in favour of @ref attachEventHandler
	/// @note Only one focus handler can be set at a time. To remove the focus handler, set it to null.
	///
	/// A focus handler must be able to accept three parameters: a reference to the containing dialog, the unique key of the
	/// clicked item, and a boolean indicating whether focus is being lost or gained; a value of true indicates that
	/// focus is being lost, while false indicates it's being gained. Most handlers will only need to act when the
	/// third parameter is true. If the handler returns false, the focus change is cancelled.
	virtual void attachFocusHandler(std::function<bool(cDialog&,std::string,bool)> f) throw(xHandlerNotSupported);
	/// Trigger an event on this control.
	/// @tparam t The type of event to trigger.
	/// @tparam Params Additional parameters, depending on the event type.
	/// @param dlg The parent dialog of this control.
	/// @param args Additional arguments, depending on the event type.
	/// @return The result of the event handler. If there was no handler, and the handler would've returned a bool,
	/// then true is returned. This also applies if the event type is not supported by the control.
	template<eDlogEvt t, typename... Params> typename event_fcn<t>::type::result_type triggerEvent(cDialog& dlg, Params... args) {
		using fcn_t = typename event_fcn<t>::type;
		if(event_handlers[t].empty())
			return callHandler(fcn_t(), dlg, args...);
		auto handler = boost::any_cast<fcn_t>(event_handlers[t]);
		return callHandler(handler, dlg, args...);
	}
	/// Check if a handler is assigned for a given event.
	/// @param t The type of event to check for.
	/// @return True if one is assigned, false otherwise.
	bool haveHandler(eDlogEvt t) {
		return !event_handlers[t].empty();
	}
	/// Trigger the click handler for this control.
	/// @param me A reference to the current dialog.
	/// @param id The unique key of this control.
	/// @param mods The currently-held keyboard modifiers.
	/// @return true if the event should continue, false if it should be cancelled.
	virtual bool triggerClickHandler(cDialog& me, std::string id, eKeyMod mods) final;
	/// Trigger the focus handler for this control.
	/// @param me A reference to the current dialog.
	/// @param id The unique key of this control.
	/// @param losingFocus true if this control is losing focus, false if it is gaining focus.
	/// @return true if the event should continue, false if it should be cancelled.
	virtual bool triggerFocusHandler(cDialog& me, std::string id, bool losingFocus) final;
	/// Make this control visible.
	virtual void show(); // cd_activate_item true
	/// Make this control invisible.
	virtual void hide(); // cd_activate_item false
	/// Check if this control is visible.
	/// @return true if it's visible
	bool isVisible(); // cd_get_active
	/// Check if this control is a container which contains other controls.
	/// @return true if it's a container
	/// @note Generally you shouldn't override this. If you need a container, then
	/// extend @ref cContainer instead of cControl.
	virtual bool isContainer() {return false;}
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
	/// Check if the control is focusable.
	/// @return true if it's focusable.
	/// @note This does not indicate whether the control supports focus and defocus handlers.
	virtual bool isFocusable() = 0;
	/// Check if the control is scrollable.
	/// @return true if it's scrollable.
	/// @note This does not indicate whether the control supports scroll handlers.
	virtual bool isScrollable() = 0;
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
	/// Get a view of the control's current state.
	/// @return A map of string keys to boost::any values, representing the control's state.
	virtual storage_t store();
	/// Restore the control to a previous state.
	/// @param to A state previously returned from store()
	virtual void restore(storage_t to);
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
	/// Returns a list of event handlers that this control supports.
	/// @return The list of handlers as a std::set.
	///
	/// See the documentation of this method in subclasses for explanations of what handlers
	/// each control supports and how those handlers work for that control.
	virtual const std::set<eDlogEvt> getSupportedHandlers() const = 0;
	/// Called when a click event is triggered on this control. Override this to
	/// customize the behaviour of clicks on the control. Note that, if you override it, you are
	/// responsible for calling the passed handler, if it exists. (Test for existance with operator!.)
	/// @param handler The handler that should be triggered as a result of this click.
	/// @param dlg The dialog in which the event was triggered; intended to be passed to the handler.
	/// @param id The ID of the control that triggered the event; intended to be passed to the handler.
	/// @param mods The key modifiers currently pressed; intended to be passed to the handler.
	virtual void callHandler(event_fcn<EVT_CLICK>::type handler, cDialog& dlg, std::string id, eKeyMod mods) {
		if(handler) handler(dlg, id, mods);
	}
	/// Called when a focus event is triggered on this control. Override this to
	/// customize the behaviour of focusing on the control. Note that, if you override it, you are
	/// responsible for calling the passed handler, if it exists. (Test for existance with operator!.)
	/// @param handler The handler that should be triggered as a result of this focus.
	/// @param dlg The dialog in which the event was triggered; intended to be passed to the handler.
	/// @param id The ID of the control that triggered the event; intended to be passed to the handler.
	virtual void callHandler(event_fcn<EVT_FOCUS>::type handler, cDialog& dlg, std::string id) {
		if(handler) handler(dlg, id);
	}
	/// Called when a defocus event is triggered on this control. Override this to
	/// customize the behaviour of defocusing on the control. Note that, if you override it, you are
	/// responsible for calling the passed handler, if it exists. (Test for existance with operator!.)
	/// @param handler The handler that should be triggered as a result of this defocus.
	/// @param dlg The dialog in which the event was triggered; intended to be passed to the handler.
	/// @param id The ID of the control that triggered the event; intended to be passed to the handler.
	/// @return Whether the defocus should be allowed to happen.
	virtual bool callHandler(event_fcn<EVT_DEFOCUS>::type handler, cDialog& dlg, std::string id) {
		if(handler) return handler(dlg, id);
		return true;
	}
	/// Called when a scroll event is triggered on this control. Override this to
	/// customize the behaviour of scrolling on the control. Note that, if you override it, you are
	/// responsible for calling the passed handler, if it exists. (Test for existance with operator!.)
	/// @param handler The handler that should be triggered as a result of this scroll.
	/// @param dlg The dialog in which the event was triggered; intended to be passed to the handler.
	/// @param id The ID of the control that triggered the event; intended to be passed to the handler.
	/// @param newVal The new value of the scrollbar.
	/// @return Whether the scrollbar should be allowed to scroll to this point.
	virtual bool callHandler(event_fcn<EVT_SCROLL>::type handler, cDialog& dlg, std::string id, int newVal) {
		if(handler) return handler(dlg, id, newVal);
		return true;
	}
	/// Parses an HTML colour code.
	/// Recognizes three-digit hex, six-digit hex, and HTML colour names.
	/// @param code The colour code to parse.
	static sf::Color parseColor(std::string code);
	/// Parses a key code.
	/// @param what The code to parse.
	static cKey parseKey(std::string what);
	/// Filters newlines and tabs from a string, leaving spaces intact.
	/// @param toFilter The string to filter.
	static std::string dlogStringFilter(std::string toFilter);
	/// The parent dialog of the control.
	/// May be null, if the control was created via cControl(eControlType,sf::RenderWindow&).
	cDialog* parent;
	/// This control's labelling control.
	cControl* labelCtrl = nullptr;
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
	eFrameStyle frameStyle;
	/// The control's attached key.
	cKey key;
	/// Draw a frame around the control.
	/// @param amt How much to offset the frame from the control's bounding rect.
	/// @param med_or_lt true to use a darker colour for the frame.
	/// @note The TXT_FRAMESTYLE formatting property is normally used for the second parameter.
	void drawFrame(short amt, eFrameStyle frameStyle);
	/// Redraws the parent dialog, if any.
	/// Intended to be called from handleClick(), where there is usually a minor event loop happening.
	void redraw();
private:
	friend class cDialog; // TODO: This is only so it can access parseColour... hack!
	eControlType type;
	std::map<eDlogEvt, boost::any> event_handlers;
};

/// A superclass to represent a control that contains other controls.
class cContainer : public cControl {
	void callHandler(event_fcn<EVT_CLICK>::type onClick, cDialog& me, std::string id, eKeyMod mods) override;
	std::string clicking;
public:
	/// Create a new container control attached to an arbitrary window, rather than a dialog.
	/// @param t The type of the control.
	/// @param p The parent window.
	cContainer(eControlType t, sf::RenderWindow& p) : cControl(t, p) {}
	/// Create a new container control attached to a dialog.
	/// @param t The type of the control.
	/// @param p The parent dialog.
	cContainer(eControlType t, cDialog& p) : cControl(t, p) {}
	/// Check if a control exists with a given ID.
	/// @param id The unique key of the control.
	/// @return true if it exists.
	virtual bool hasChild(std::string id) = 0;
	/// Get a reference to a child control.
	/// @param id The unique key of the control.
	/// @throw std::invalid_argument if the control does not exist.
	/// @return a reference to the requested control.
	virtual cControl& getChild(std::string id) = 0;
	/// Executes a function for every control in this container.
	/// @param callback A function taking a string as its first argument
	/// and a control reference as its second argument.
	virtual void forEach(std::function<void(std::string,cControl&)> callback) = 0;
	/// @copydoc getChild()
	cControl& operator[](std::string id) {return getChild(id);}
	bool isContainer() override {return true;}
	bool handleClick(location where) override;
};

// This is defined here instead of in dialog.hpp because it needs cControl to be complete.
/// @note You need to include control.hpp to use this.
template<eDlogEvt t> void cDialog::attachEventHandlers(typename event_fcn<t>::type handler, const std::vector<std::string>& controls) {
	cDialog& me = *this;
	for(std::string control : controls) {
		me[control].attachEventHandler<t>(handler);
	}
}

#endif
