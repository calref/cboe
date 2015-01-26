/*
 *  button.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef BUTTON_H
#define BUTTON_H

/// @file
/// Button-related classes and types.

#include <SFML/Graphics.hpp>

#include <string>
#include <map>
#include <vector>
#include "control.hpp"
#include "graphtool.hpp" // for eFont

/// A button type.
enum eBtnType {	// w x h
	BTN_SM = 0,	/**< A small 23x23 button */// (PICT id 2000 / 2001)
	BTN_REG,	/**< A normal-sized 63x23 button */// (PICT id 2002 / 2003)
	BTN_LG,		/**< A large 102x23 button */// (PICT id 2004 / 2005)
	BTN_HELP,	/**< A small 16x13 help button - a white bubble with a ? mark */// (PICT id 2006 / 2007)
	BTN_LEFT,	/**< A normal-sized 63x23 button with a left-pointing arrow */// (PICT id 2008 / 2009)
	BTN_RIGHT,	/**< A normal-sized 63x23 button with a right-pointing arrow */// (PICT id 2010 / 2011)
	BTN_UP,		/**< A normal-sized 63x23 button with an up-pointing arrow */// (PICT id 2012 / 2013)
	BTN_DOWN,	/**< A normal-sized 63x23 button with a down-pointing arrow */// (PICT id 2014 / 2015)
	BTN_TINY,	/**< A tiny 14x10 button, same size as an LED */// (PICT id 2021)
	BTN_DONE,	/**< A normal-sized 63x23 button with "Done" on it */// (PICT id 2022 / 2023)
	BTN_TALL,	/**< A tall 63x40 button */// (PICT id 2024 / 2025)
	BTN_TRAIT,	/**< A tall 63x40 button with "Race Good/Bad Traits" on it */// (PICT id 2026 / 2027)
	BTN_PUSH,	/**< A round red 30x30 push button */// (PICT id 2028 / 2029)
	BTN_LED,	/**< A tiny 14x10 LED button */// (PICT id 2018 / 2019 / 2020)
};

/// Represents the state of an LED button.
/// Generally, led_red is used to indicate a selected button.
/// Currently, led_green is only used by the spell selection dialog,
/// where led_red indicates the spell is available amd led_green indicates it is selected.
enum eLedState {led_green = 0, led_red, led_off};


/// A clickable button control.
class cButton : public cControl {
public:
	/// @copydoc cDialog::init()
	static void init();
	void attachClickHandler(click_callback_t f) throw();
	void attachFocusHandler(focus_callback_t f) throw(xHandlerNotSupported);
	bool triggerClickHandler(cDialog& me, std::string id, eKeyMod mods);
	//virtual void setPict(short pict, short type) = 0;
	void setFormat(eFormat prop, short val) throw(xUnsupportedProp);
	short getFormat(eFormat prop) throw(xUnsupportedProp);
	void setColour(sf::Color clr) throw(xUnsupportedProp);
	sf::Color getColour() throw(xUnsupportedProp);
	/// Set the type of this button.
	/// @param newType The desired button type.
	void setBtnType(eBtnType newType);
	/// Get the type of this button.
	/// @return The button type.
	eBtnType getBtnType();
	/// Create a new button.
	/// @param parent The parent dialog.
	explicit cButton(cDialog* parent);
	bool isClickable();
	virtual ~cButton();
	void draw();
	cButton& operator=(cButton& other) = delete;
	cButton(cButton& other) = delete;
protected:
	/// The type of button.
	eBtnType type;
	/// The click handler.
	click_callback_t onClick;
	/// Construct a new button.
	/// @param parent The parent dialog.
	/// @param t The type of control. Should be either CTRL_LED or CTRL_BTN.
	cButton(cDialog* parent,eControlType t);
private:
	bool wrapLabel;
	bool labelWithKey;
	std::string fromList;
	static rectangle btnRects[13][2];
protected:
	/// The button's text colour; only used by LED and tiny buttons
	sf::Color textClr;
	/// The index in buttons of the texture for each button type.
	static size_t btnGW[14];
	/// The textures that hold the graphics for the buttons.
	static sf::Texture buttons[7];
};

/// A LED button that can be either on or off.
/// Additionally, it supports two possible colours, red and green.
/// By default, it behaves like a checkbox, turning on or off when clicked.
/// This default behaviour always assumes a red LED.
class cLed : public cButton {
public:
	/// @copydoc cDialog::init()
	static void init();
	/// A handler that can be attached as a click handler to prevent the
	/// default toggle-selected action of an LED.
	/// @return true to indicate the event should continue.
	static bool noAction(cDialog&,std::string,eKeyMod) {return true;}
	void attachClickHandler(click_callback_t f) throw();
	void attachFocusHandler(focus_callback_t f) throw();
	bool triggerClickHandler(cDialog& me, std::string id, eKeyMod mods);
	bool triggerFocusHandler(cDialog& me, std::string id, bool losingFocus);
	void setFormat(eFormat prop, short val) throw(xUnsupportedProp);
	short getFormat(eFormat prop) throw(xUnsupportedProp);
	storage_t store();
	void restore(storage_t to);
	/// Create a new LED button.
	/// @param parent The parent dialog.
	explicit cLed(cDialog* parent);
	virtual ~cLed();
	/// Set the LED's current state,.
	/// @param to The new state.
	void setState(eLedState to);
	/// Get the LED's current state.
	/// @return The current state.
	eLedState getState();
	void draw();
	cLed& operator=(cLed& other) = delete;
	cLed(cLed& other) = delete;
private:
	eLedState state;
	eFont textFont;
	short textSize;
	static rectangle ledRects[3][2];
	focus_callback_t onFocus;
};

/// A group of LED buttons that behave like radio buttons.
/// As with other standard LEDs, this always assumes red LEDs.
///
/// When an LED in the group is clicked, the following sequence of events are fired:
///
/// 1. The click handler of the clicked LED.
/// 2. The click handler of the LED group.
/// 3. The focus handler of the currently selected LED (if any), with the third parameter true.
/// 4. The focus handler of the clicked LED, with the third parameter false.
/// 5. The focus handler of the LED group, with the third parameter false.
///
/// If at any stage a handler returns false, the entire sequence is aborted, and the
/// selection is not changed. A click within the group's space but not on any choice
/// triggers no events.
/// @note When the focus handlers of the individual LEDs are called, the selection has not yet been updated,
/// so calling the LED group's getSelected() method will still return the previous selection.
/// However, when the focus handler of the LED group is called, the selection _has_ been updated.,
/// so getSelected() will return the new selection. (This is the reason for the getPreviousSelection() method.)
class cLedGroup : public cControl {
	std::vector<cLed> btns;
	click_callback_t onClick;
	focus_callback_t onFocus;
	std::map<std::string,cLed*> choices;
	std::string fromList;
	std::string curSelect, prevSelect;
	std::string clicking;
	cLedGroup& operator=(cLedGroup& other) = delete;
	cLedGroup(cLedGroup& other) = delete;
public:
	/// @copydoc cControl::attachClickHandler()
	///
	/// The click handler is called whenever an LED in the group is clicked, even if it's the currently selected LED.
	void attachClickHandler(click_callback_t f) throw();
	/// @copydoc cControl::attachFocusHandler()
	///
	/// An LED group triggers focus handlers when a choice other than the currently selected one is clicked.
	/// The third parameter is always false for an LED group's focus handler.
	/// You can determine what changed using getPrevSelection() and getSelected(), and can do whatever post-processing
	/// you want, including selecting a completely different option.
	void attachFocusHandler(focus_callback_t f) throw();
	bool triggerClickHandler(cDialog& me, std::string id, eKeyMod mods);
	bool triggerFocusHandler(cDialog& me, std::string id, bool losingFocus);
	storage_t store();
	void restore(storage_t to);
	/// Add a new LED to this group.
	/// @param ctrl A pointer to the LED, which should already be constructed.
	/// @param key A key to be used to look up the LED later.
	/// @note This function is intended for internal use, which is why it takes a control pointer instead of a unique key.
	void addChoice(cLed* ctrl, std::string key);
	/// Disable one of the choices in this group.
	/// @param id The unique key of the choice.
	void disable(std::string id);
	/// Enable one of the choices in this group.
	/// @param id The unique key of the choice.
	void enable(std::string id);
	using cControl::show;
	using cControl::hide;
	/// Hide one of the choices in this group.
	/// @param id The unique key of the choice.
	void hide(std::string id);
	/// Show one of the choices in this group.
	/// @param id The unique key of the choice.
	void show(std::string id);
	void setFormat(eFormat prop, short val) throw(xUnsupportedProp);
	short getFormat(eFormat prop) throw(xUnsupportedProp);
	void setColour(sf::Color clr) throw(xUnsupportedProp);
	sf::Color getColour() throw(xUnsupportedProp);
	/// Create a new LED group.
	/// @param parent The parent dialog.
	explicit cLedGroup(cDialog* parent);
	bool isClickable();
	bool handleClick(location where);
	virtual ~cLedGroup();
	/// Get one of the LED's in this group.
	/// @param id The unique key of the choice.
	/// @return A reference to the LED object.
	/// @throw std::invalid_argument if the choice does not exist in the group.
	cLed& operator[](std::string id);
	/// Set the currently selected LED in this group.
	/// @param id The unique key of the choice.
	/// @throw std::invalid_argument if the choice does not exist in the group.
	void setSelected(std::string id);
	/// Get the currently selected choice.
	/// @return id The unique key of the choice.
	std::string getSelected();
	/// Get the previously selected choice.
	/// @return id The unique key of the choice.
	/// @note This is intended for use by focus handlers.
	///
	/// This refers to the element that was last selected before the selection changed to the current selection.
	std::string getPrevSelection();
	/// Recalculate the LED group's bounding rect.
	/// Call this after adding choices to the group to ensure that the choice is within the bounding rect.
	/// If a choice is not within the bounding rect, it will not respond to clicks.
	void recalcRect();
	/// A convenience type for making an iterator into the choice map.
	typedef std::map<std::string,cLed*>::iterator ledIter;
	void draw();
};
#endif
