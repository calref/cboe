/*
 *  message.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef MESSAGE_H
#define MESSAGE_H

/// @file
/// Message-related classes and types.

#include <SFML/Graphics.hpp>

#include <string>
#include "control.hpp"
#include "render_text.hpp"

/// A simple static text message.
/// This class can also create a frame for grouping controls or a clickable area.
class cTextMsg : public cControl {
public:
	std::string parse(ticpp::Element& who, std::string fname);
	void setFormat(eFormat prop, short val) throw(xUnsupportedProp);
	short getFormat(eFormat prop) throw(xUnsupportedProp);
	void setColour(sf::Color clr) throw(xUnsupportedProp);
	sf::Color getColour() throw(xUnsupportedProp);
	/// Create a new text message.
	/// @param parent The parent dialog.
	explicit cTextMsg(cDialog& parent);
	/// Create a new text message without a parent dialog.
	/// @param parent The parent window.
	explicit cTextMsg(sf::RenderWindow& parent);
	bool isClickable();
	bool isFocusable();
	bool isScrollable();
	virtual ~cTextMsg();
	void draw();
	/// @copydoc cControl::getSupportedHandlers
	///
	/// @todo Document possible handlers
	const std::set<eDlogEvt> getSupportedHandlers() const {
		return {EVT_CLICK};
	}
	cTextMsg& operator=(cTextMsg& other) = delete;
	cTextMsg(cTextMsg& other) = delete;
private:
	bool drawFramed;
	short textSize;
	eFont textFont;
	sf::Color color;
	std::string fromList;
};
#endif
