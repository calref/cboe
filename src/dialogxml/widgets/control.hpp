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
#include <map>
#include <random>
#include <boost/any.hpp>
#include "dialogxml/dialogs/dlogevt.hpp"
#include "tools/framerate_limiter.hpp"

#include "location.hpp"

class cContainer;

// TinyXML forward declarations
namespace ticpp {
	class Attribute;
	class Element;
	class Node;
}

/// Formatting properties
enum eFormat {
	TXT_FRAME,	///< The control's frame style. Should be an enum from @ref eFrameStyle.
	TXT_FONT,	///< The control's text font. Should be one of the constants FONT_PLAIN, FONT_BOLD, FONT_DUNGEON, FONT_MAIDEN.
	TXT_SIZE,	///< The control's text size. Should be an integer indicating point size.
	TXT_WRAP,	///< Whether the control should wrap. Should be a boolean (true or false).
	TXT_COLOUR, ///< The control's text colour. Should be an sf::Color. Use the separate set/getColour functions to set this.
};

/// Frame styles
enum eFrameStyle {
	FRM_NONE,	///< No frame.
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
	CTRL_MAP,   ///< A 2-dimensional grid of identical controls
	CTRL_LINE,  ///< Just a line connecting two points
};

enum ePosition {
	POS_ABS, ///< Absolute positioning (possibly relative to a container)
	POS_REL_PLUS, ///< Positioned relative to another widget, measuring down from its bottom edge or right from its right edge
	POS_REL_NEG, ///< Positioned relative to another widget, measuring up from its top edge or left from its left edge
	POS_CONT_PLUS, ///< Positioned relative to another widget, measuring down from its top edge or right from its left edge
	POS_CONT_NEG, ///< Positioned relative to another widget, measuering up from its bottom edge or left from its right edge
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
/// All control subclasses must have a constructor that takes a single iComponent* parameter,
/// in order for the parsing template to work.
class cControl : public iComponent {
public:
	using storage_t = std::map<std::string, boost::any>;
	static const char KEY_PLACEHOLDER = '\a';
protected:
	/// Parses the control from an XML element
	/// Most controls probably don't need to override this. Override parseAttribute() and parseContent() instead.
	/// @param who A reference to the XML element being parsed.
	/// @param fname The file being parsed, for error messages.
	/// @throw xBadNode if an unsupported sub-element is detected, or if text is found in an element that does not support it
	virtual void parse(ticpp::Element& who, std::string fname);
	/// Parses an attribute on the XML element representing this control.
	/// All controls should defer to cControl::parseAttribute if they don't recognize the attribute.
	/// @param attr A reference to the XML attribute being parsed.
	/// @param tagName The name of the XML element, for error messages.
	/// @param fname The file being parsed, for error messages.
	/// @return true if the attribute is valid and was successfully parsed, false if it is unrecognized.
	/// @throw xBadVal if the attribute is allowed but its content is invalid.
	virtual bool parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname);
	/// Parses content of the XML element representing this control, either text or a nested element.
	/// Comments will not be passed to this function.
	/// @param content A reference to the XML node being parsed; it will be either an element node or a text node.
	/// @param tagName The name of the XML parent element, for error messages.
	/// @param counter The number of previously-parsed nodes of the same tag name (or previously-parsed text nodes), excluding the current one.
	/// @param[out] text If non-empty, this becomes the widget's text content.
	/// @return true if the node is recognized and allowed in this location, false otherwise.
	/// @throw xBadAttr if the node is an element and one of its attributes is not allowed.
	/// @throw xMissingAttr if the node is an element and is missing a required attribute.
	/// @throw xMissingElem if the node is an element and is missing a mandatory sub-element.
	/// @throw xBadVal if the value of the node or (if the node is an element) an attribute is invalid.
	/// The third argument can be used to accumulate the control's text.
	virtual bool parseContent(ticpp::Node& content, int counter, std::string tagName, std::string fname, std::string& text);
	/// Performs final validation of a parsed widget.
	/// This can be used to throw errors for missing required attributes or elements,
	/// or set defaults for optional values that were not found while parsing.
	/// @param who A reference to the XML element being validated.
	/// @param fname The file being parsed, for error messages
	/// @param attrs A set containing all the attributes parsed from the widget's root element.
	/// @param nodes A multiset containing all the sub-elements parsed from the widget's root element. Text nodes are also included in this set under the key "".
	/// @throw xMissingAttr if a required attribute is missing
	/// @throw xMissingElem if a required attribute is either missing or present in insufficient quantity
	virtual void validatePostParse(ticpp::Element& who, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& nodes);
	std::string name;
public:
	/// Attach a keyboard shortcut to a control. Pressing the keyboard shortcut is equivalent to clicking the control.
	/// @param key The desired keyboard shortcut.
	void attachKey(cKey key);
	/// Detach any currently assigned keyboard shortcut from the control.
	void detachKey();
	/// Check if the control has an assigned keyboard shortcut.
	/// @return true if a keyboard shortcut is assigned.
	bool hasKey() const;
	/// Retrieve the control's current keyboard shortcut.
	/// @return the currently-assigned keyboard shortcut.
	/// @note You should first check that a shortcut is assigned using hasKey().
	cKey getAttachedKey() const;
	/// Retrieve the control's current keyboard shortcut as a human-readable string.
	/// @return the currently-assigned keyboard shortcut, or an empty string if none is assigned.
	std::string getAttachedKeyDescription() const;
	inline void setDefault(bool value) { isDefaultControl = value; }
	inline bool isDefault() { return isDefaultControl; }
	inline void setEscape(bool value) { isEscapeControl = value; }
	inline bool isEscape() { return isEscapeControl; }
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
		else event_handlers[t] = nullptr;
		if(old_handler.empty()) return nullptr;
		return boost::any_cast<typename event_fcn<t>::type>(old_handler);
	}
	inline std::string getName() { return name; }
	inline void setName(std::string value) { name = value; }
	/// Attach a click handler to this control.
	/// @param f The click handler to attach.
	/// @throw xHandlerNotSupported if this control does not support click handlers. Most controls do support click handlers.
	/// @deprecated in favour of @ref attachEventHandler
	/// @note Only one click handler can be set at a time. To remove the click handler, set it to null.
	///
	/// A click handler must be able to accept three parameters: a reference to the containing dialog, the unique key of the
	/// clicked item, and a representation of any modifier keys that are currently held.
	virtual void attachClickHandler(std::function<bool(cDialog&,std::string,eKeyMod)> f);
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
	virtual void attachFocusHandler(std::function<bool(cDialog&,std::string,bool)> f);
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
	bool haveHandler(eDlogEvt t) const;
	/// Trigger the click handler for this control.
	/// @param me A reference to the current dialog.
	/// @param id The unique key of this control.
	/// @param mods The currently-held keyboard modifiers.
	/// @return true if the event should continue, false if it should be cancelled.
	bool triggerClickHandler(cDialog& me, std::string id, eKeyMod mods);
	/// Trigger the focus handler for this control.
	/// @param me A reference to the current dialog.
	/// @param id The unique key of this control.
	/// @param losingFocus true if this control is losing focus, false if it is gaining focus.
	/// @return true if the event should continue, false if it should be cancelled.
	bool triggerFocusHandler(cDialog& me, std::string id, bool losingFocus);
	/// Make this control visible.
	virtual void show(); // cd_activate_item true
	/// Make this control invisible.
	virtual void hide(); // cd_activate_item false
	/// Check if this control is visible.
	/// @return true if it's visible
	bool isVisible() const; // cd_get_active
	/// Check if this control is a container which contains other controls.
	/// @return true if it's a container
	/// @note Generally you shouldn't override this. If you need a container, then
	/// extend @ref cContainer instead of cControl.
	virtual bool isContainer() const {return false;}
	/// Set if this control is active. A control is normally active when the mouse button is held down within its bounding rect.
	/// @param active true if it should be active, false if not
	void setActive(bool active); // "active" here means "selected", so if it's a button, draw it pressed
	/// Get the type of this control
	/// @return The type of control
	eControlType getType() const;
	/// Set the control's text.
	/// @param l The new text.
	virtual void setText(std::string l);
	/// Replace the first occurrence of the given string in this control's text
	void replaceText(std::string find, std::string replace);
	/// Append a string to this control's text
	void appendText(std::string l);
	/// Fetch the control's text.
	/// @return The control's current text.
	virtual std::string getText() const;
	/// Get the bounding rect of this control.
	/// @return The control's bounding rect.
	rectangle getBounds() const;
	/// Set the bounding rect of this control.
	/// @param newBounds The new bounding rect.
	void setBounds(rectangle newBounds);
	/// Calculate a preferred size for this control.
	/// This can specify an exact preferred size or just a preferred width or height.
	/// The preferred size is only used if the size is not specified in the XML.
	/// @return The preferred size, or (0,0) if there is no preferred size.
	virtual location getPreferredSize() const {return {0,0};}
	/// Set the position of this control.
	/// @param to The new position.
	void relocate(location to);
	/// Set the position of this control relative to another control.
	/// @param to The new relative position.
	/// @param anchor The position will be calculated relative to this control's position.
	/// If nullptr, the control will be moved relative to its current position.
	/// @param horz How to place the control on the horizontal axis.
	/// @param vert How to place the control on the vertical axis.
	void relocateRelative(location to, cControl* anchor, ePosition horz, ePosition vert);
	/// Get the control's text as an integer.
	/// @return The control's text, coerced to an integer.
	long long getTextAsNum() const;
	/// Set the control's text to an integer value.
	/// @param what The desired value.
	/// @param label This label will be prefixed to the value.
	/// @note If a label is used, then @a getTextAsNum will not work to retrieve the value.
	void setTextToNum(long long what, const std::string& label = "");
	/// Set one of the control's formatting parameters.
	/// @param prop The parameter to set.
	/// @param val The desired value of the parameter.
	/// @throw xUnsupportedProp if this control doesn't support the given parameter.
	void setFormat(eFormat prop, short val);
	/// Get one of the control's formatting parameters.
	/// @param prop The parameter to retrieve.
	/// @return The value of the parameter.
	/// @throw xUnsupportedProp if this control doesn't support the given parameter.
	short getFormat(eFormat prop) const;
	/// Test if the control supports a given formatting parameter
	/// @param prop The parameter to check.
	/// @return true if supported, false if not.
	bool canFormat(eFormat prop) const;
	/// Set the control's colour (usually text colour).
	/// @param clr The desired colour.
	/// @throw xUnsupportedProp if this control does not support colour.
	void setColour(sf::Color clr);
	/// Get the control's colour.
	/// @return The current colour.
	/// @throw xUnsupportedProp if this control does not support colour.
	sf::Color getColour() const;
	/// Test if the control supports colour
	/// @return true if supported, false if not.
	bool canColour() const;
	/// Check if the control is clickable.
	/// @return true if it's clickable.
	/// @note This does not indicate whether the control supports click handlers.
	/// In fact, some controls return true only if a click handler is assigned.
	/// Others, like editable text fields, are clickable but do not support click handlers.
	virtual bool isClickable() const = 0;
	/// Check if the control is focusable.
	/// @return true if it's focusable.
	/// @note This does not indicate whether the control supports focus and defocus handlers.
	virtual bool isFocusable() const = 0;
	/// Check if the control is scrollable.
	/// @return true if it's scrollable.
	/// @note This does not indicate whether the control supports scroll handlers.
	virtual bool isScrollable() const = 0;
	/// Handles the action of clicking this control.
	/// @param where The exact location at which the mouse was pressed, relative to the dialog.
	/// @return true if the click was successful; false if it was cancelled.
	///
	/// This function should implement an event loop and terminate when the mouse button is released.
	/// It can be overridden to customize the reaction of the control to mouse events.
	/// The default implementation works for a simple clickable object such as a button that
	/// should be hilited in some way while pressed and is cancelled by releasing the mouse
	/// button outside the control's bounds.
	virtual bool handleClick(location where, cFramerateLimiter& fps_limiter);
	/// Animate and play the sound of the control being "clicked", and call its click event
	void handleKeyTriggered(cDialog& parent);
	/// Specifies that another control acts as a label for this one.
	/// The practical effect of this is that hiding or showing this control automatically hides or shows the label as well.
	/// @param label A pointer to the control that acts as a label.
	void setLabelCtrl(cControl* label);
	/// Get the control that serves as a label for this one, if any.
	cControl* getLabelCtrl();
	/// Get a view of the control's current state.
	/// @return A map of string keys to boost::any values, representing the control's state.
	virtual storage_t store() const;
	/// Restore the control to a previous state.
	/// @param to A state previously returned from store()
	virtual void restore(storage_t to);
	/// Create a new control.
	/// @param t The type of the control.
	/// @param p The parent dialog, container control, or window.
	cControl(eControlType t, iComponent& parent);
	virtual ~cControl();
	/// Draw the control into its parent window.
	virtual void draw() = 0;
	cControl& operator=(cControl& other) = delete;
	cControl(cControl& other) = delete;
	/// Sets the positioning method of this control.
	/// Note: this only has an effect when called from another control's parse function.
	void setPositioning(const std::string& anchor_id, ePosition h, ePosition v) {
		anchor = anchor_id;
		horz = h;
		vert = v;
	}
	/// Get the positioning method of this control.
	/// Note: After the parse stage, this information is lost and replaced with (ABS,ABS).
	std::pair<ePosition, ePosition> getPositioning(std::string* anchor_id = nullptr) {
		if(anchor_id) *anchor_id = anchor;
		return {horz, vert};
	}
	/// If the control automatically determines its rect based on certain criteria, override this.
	/// It will automatically be called during parsing.
	virtual void recalcRect() {}
protected:
	/// Create a new control attached to a dialog.
	/// @param t The type of the control.
	/// @param p The parent container.
	cControl(eControlType t, iComponent* p);
	/// Returns a list of event handlers that this control supports.
	/// @return The list of handlers as a std::set.
	///
	/// See the documentation of this method in subclasses for explanations of what handlers
	/// each control supports and how those handlers work for that control.
	virtual std::set<eDlogEvt> getSupportedHandlers() const = 0;
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
	/// Called to manage a format setting on this control.
	/// No action is taken if the parameter is unsupported.
	/// @param prop The parameter to manage.
	/// @param set Whether to set the value.
	/// @param val If @a set is true, set to this value, otherwise store the value here. Ignored if null.
	/// The real type of this value depends on @a prop.
	/// @return true if the parameter is supported, false otherwise.
	virtual bool manageFormat(eFormat prop, bool set, boost::any* val);
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
	cDialog* getDialog() const;
	/// The direct parent of this control.
	iComponent& getParent() const;
	/// This control's labelling control.
	cControl* labelCtrl = nullptr;
	/// The parent window of the control.
	/// This is for use in implementing draw().
	sf::RenderWindow& getWindow() override;
	/// Get the default text colour applied to new dialogs when loading from a file.
	/// @return The text colour.
	sf::Color getDefTextClr() const override;
	/// Whether the control is visible
	bool visible, depressed = false; ///< Whether the control is depressed; only applicable for clickable controls
	/// The control's bounding rect.
	rectangle frame;
	/// The control's frame style.
	eFrameStyle frameStyle;
	/// The control's attached key.
	cKey key;
	/// Whether the control is the default control of its dialog.
	bool isDefaultControl = false;
	/// Whether the control is the cancel control of its dialog.
	bool isEscapeControl = false;

	/// Draw a frame around the control.
	/// @param amt How much to offset the frame from the control's bounding rect.
	/// @param med_or_lt true to use a darker colour for the frame.
	/// @note The TXT_FRAMESTYLE formatting property is normally used for the second parameter.
	void drawFrame(short amt, eFrameStyle frameStyle);
	/// Redraws the parent dialog, if any.
	/// Intended to be called from handleClick(), where there is usually a minor event loop happening.
	void redraw();
	/// Plays the proper sound for this control being clicked on
	void playClickSound();
	static std::string generateRandomString();
	location translated_location(const sf::Vector2i) const;
private:
	friend class cDialog; // This is so it can access parseColour and anchor
	friend class cContainer; // This is so it can access anchor
	/// The control's current text.
	std::string lbl;
	eControlType type;
	std::map<eDlogEvt, boost::any> event_handlers;
	iComponent* parent;
	// Transient values only used during parsing
	ePosition horz = POS_ABS, vert = POS_ABS;
	int width = 0, height = 0;
	std::string anchor;
	bool is_link = false;
	static std::mt19937 ui_rand;
};

#endif
