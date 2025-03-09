//
//  line.hpp
//  BoE
//
//  Created by Celtic Minstrel on 2025-03-08.
//

#ifndef BOE_DIALOG_LINE_HPP
#define BOE_DIALOG_LINE_HPP

#include "control.hpp"

/// A simple line drawn between two points.
class cConnector : public cControl {
public:
	bool parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) override;
	void validatePostParse(ticpp::Element& who, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& nodes) override;
	bool manageFormat(eFormat prop, bool set, boost::any* val) override;
	/// Create a connector.
	/// @param parent The parent dialog.
	explicit cConnector(iComponent& parent);
	bool isClickable() const override;
	bool isFocusable() const override;
	bool isScrollable() const override;
	virtual ~cConnector();
	void draw() override;
	/// @copydoc cControl::getSupportedHandlers
	///
	/// @todo Document possible handlers
	std::set<eDlogEvt> getSupportedHandlers() const override {
		return {EVT_CLICK};
	}
	cConnector& operator=(cConnector& other) = delete;
	cConnector(cConnector& other) = delete;
private:
	sf::Color color;
	rectangle to_rect;
	size_t thickness = 2;
	bool flip = false;
};

#endif
