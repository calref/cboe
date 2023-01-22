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
#include <boost/optional.hpp>
#include "control.hpp"
#include "gfx/render_text.hpp"

/// A simple static text message.
/// This class can also create a frame for grouping controls or a clickable area.
class cTextMsg : public cControl {
public:
	bool parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) override;
	bool parseContent(ticpp::Node& content, int n, std::string tagName, std::string fname, std::string& text) override;
	void validatePostParse(ticpp::Element& who, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& nodes) override;
	bool manageFormat(eFormat prop, bool set, boost::any* val) override;
	/// Create a new text message.
	/// @param parent The parent dialog.
	explicit cTextMsg(cDialog& parent);
	/// Create a new text message without a parent dialog.
	/// @param parent The parent window.
	explicit cTextMsg(sf::RenderWindow& parent);
	bool isClickable() const override;
	bool isFocusable() const override;
	bool isScrollable() const override;
	void setFixed(bool w, bool h);
	virtual ~cTextMsg();
	void draw() override;
	void recalcRect() override;
	/// @copydoc cControl::getSupportedHandlers
	///
	/// @todo Document possible handlers
	std::set<eDlogEvt> getSupportedHandlers() const override {
		return {EVT_CLICK};
	}
	cTextMsg& operator=(cTextMsg& other) = delete;
	cTextMsg(cTextMsg& other) = delete;
private:
	short textSize;
	eFont textFont;
	sf::Color color;
	std::vector<boost::optional<std::string>> keyRefs;
	std::string fromList;
	bool underlined = false, fixedWidth = false, fixedHeight = false;
};
#endif
