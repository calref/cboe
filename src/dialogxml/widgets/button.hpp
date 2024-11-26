/*
 *  button.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef BUTTON_H
#define BUTTON_H

#include "control.hpp"

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


/// A clickable button control.
class cButton : public cControl {
public:
	/// @copydoc cDialog::init()
	static void init();
	bool parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) override;
	bool parseContent(ticpp::Node& content, int n, std::string tagName, std::string fname, std::string& text) override;
	void validatePostParse(ticpp::Element& elem, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& elems) override;
	location getPreferredSize() const override;
	void recalcRect() override;
	/// Set the type of this button.
	/// @param newType The desired button type.
	void setBtnType(eBtnType newType);
	/// Get the type of this button.
	/// @return The button type.
	eBtnType getBtnType() const;
	/// Create a new button.
	/// @param parent The parent dialog.
	explicit cButton(cDialog& parent);
	/// Create a button outside of a dialog
	/// @param parent The parent window
	explicit cButton(sf::RenderWindow& parent);
	bool isClickable() const override;
	bool isFocusable() const override;
	bool isScrollable() const override;
	virtual ~cButton();
	void draw() override;
	/// @copydoc cControl::getSupportedHandlers
	///
	/// @todo Document possible handlers
	std::set<eDlogEvt> getSupportedHandlers() const override {
		return {EVT_CLICK};
	}
	cButton& operator=(cButton& other) = delete;
	cButton(cButton& other) = delete;
	void setTextSize(short size) { textSize = size; }
protected:
	/// The type of button.
	eBtnType type;
	/// Construct a new button.
	/// @param parent The parent dialog.
	/// @param t The type of control. Should be either CTRL_LED or CTRL_BTN.
	cButton(cDialog& parent,eControlType t);
private:
	bool manageFormat(eFormat prop, bool set, boost::any* val) override;
	void defaultTextSize();
	std::string fromList;
	static rectangle btnRects[13][2];
protected:
	/// Size of the button's descriptive text
	short textSize = 0;
	/// Determines whether the button's label should be word wrapped.
	bool wrapLabel;
	/// The button's text colour; only used by LED and tiny buttons
	sf::Color textClr;
	/// The index in buttons of the texture for each button type.
	static size_t btnGW[14];
	/// The names of the textures that hold the graphics for the buttons.
	static std::string buttons[7];
};

#endif
