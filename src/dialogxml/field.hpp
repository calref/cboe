/*
 *  field.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef FIELD_H
#define FIELD_H

/// @file
/// Field-related classes and types.

#include <string>
#include "control.hpp"
#include "graphtool.hpp"

/// The field's expected input type.
enum eFldType {
	FLD_INT,	///< A field that accepts only integers.
	FLD_UINT,	///< A field that accepts only positive integers.
	FLD_REAL,	///< A field that accepts any real (floating-point) number.
	FLD_TEXT,	///< A field that accepts any text. This is the default.
};

/// An editable text field.
/// The text field supports multiline input and text selection.
/// It automatically scrolls to keep the insertion point in view.
/// (If there's a current selection, the mobile end of the selection is kept in view.)
/// Mouse support is currently nonexistent, except for focusing when clicked.
/// There is no Unicode support.
class cTextField : public cControl {
public:
	void attachClickHandler(click_callback_t f) throw(xHandlerNotSupported);
	/// @copydoc cControl::attachFocusHandler()
	/// For text fields, this is triggered when it loses or gains the input focus.
	void attachFocusHandler(focus_callback_t f) throw();
	bool triggerFocusHandler(cDialog& me, std::string id, bool losingFocus);
	bool handleClick(location where);
	void setFormat(eFormat prop, short val) throw(xUnsupportedProp);
	short getFormat(eFormat prop) throw(xUnsupportedProp);
	void setColour(sf::Color clr) throw(xUnsupportedProp);
	void setText(std::string to);
	storage_t store();
	void restore(storage_t to);
	/// Get the current input type of the field.
	/// @return The input type.
	eFldType getInputType();
	/// Set the input type of the field.
	/// @param newType The new input type.
	void setInputType(eFldType newType);
	sf::Color getColour() throw(xUnsupportedProp);
	/// Create a new editable text field.
	/// @param parent The parent dialog.
	explicit cTextField(cDialog* parent);
	bool isClickable();
	virtual ~cTextField();
	void draw();
	/// Check if this text field currently has input focus.
	/// @return true if it it is currently focused.
	bool hasFocus();
	/// Handle keyboard input.
	/// @param key The keypress to handle.
	void handleInput(cKey key);
	cTextField& operator=(cTextField& other) = delete;
	cTextField(cTextField& other) = delete;
	/// This field is only used by cDialog during the loading process. Changing it will have no effect.
	long tabOrder = 0;
private:
	void set_ip(location clickLoc, int cTextField::* insertionPoint);
	eFldType field_type;
	focus_callback_t onFocus;
	bool haveFocus;
	int insertionPoint;
	int selectionPoint;
	sf::Color color;
	bool ip_visible;
	sf::Clock ip_timer;
	std::vector<snippet_t> snippets;
};
#endif
