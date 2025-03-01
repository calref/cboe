//
//  tilemap.hpp
//  BoE
//
//  Created by Celtic Minstrel on 2025-02-01.
//

#ifndef BoE_DIALOG_TILEMAP_HPP
#define BoE_DIALOG_TILEMAP_HPP

#include "container.hpp"

/// A tilemap defines a two-dimensional array of data using a repeated template.
class cTilemap : public cContainer {
	std::map<std::string,cControl*> controls;
	size_t rows, cols, spacing = 0, cellWidth, cellHeight;
	bool manageFormat(eFormat prop, bool set, boost::any* val) override;
	location getCell(const std::string& id) const;
	static std::string buildId(const std::string& base, size_t x, size_t y);
	std::string current_cell;
	mutable int id_tries = 0;
	bool isHandlingClick = false;
public:
	std::string generateId(const std::string& baseId) const override;
	bool parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) override;
	bool parseContent(ticpp::Node& content, int n, std::string tagName, std::string fname, std::string& text) override;
	void validatePostParse(ticpp::Element& who, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& nodes) override;
	bool isClickable() const override;
	bool isFocusable() const override;
	bool isScrollable() const override;
	void draw() override;
	bool hasChild(std::string id) const override;
	cControl& getChild(std::string id) override;
	bool hasChild(std::string id, size_t x, size_t y) const;
	cControl& getChild(std::string id, size_t x, size_t y);
	bool hasChild(size_t x, size_t y) const;
	cControl& getChild(size_t x, size_t y);
	location getCellPos(size_t x, size_t y) const;
	location getCellPos(cControl& child) const;
	/// Attach the same click handler to every instance of a child control.
	/// @param handler The handler to attach.
	/// @param prefix The unique ID of the control template.
	/// @throw xHandlerNotSupported if any of the controls do not support click handlers.
	/// @throw std::invalid_argument if any of the controls do not exist.
	/// @see cControl::attachClickHandler()
	/// @deprecated in favour of @ref attachEventHandlers
	void attachClickHandlers(std::function<bool(cDialog&,std::string,eKeyMod)> handler, std::string prefix);
	bool handleClick(location where, cFramerateLimiter& fps_limiter) override;
	/// Recalculate the tilemap's bounding rect based on its contained controls.
	void recalcRect() override;
	/// Adds any fields in this tilemap to the tab order building arrays.
	/// Meant for internal use.
	void fillTabOrder(std::vector<int>& specificTabs, std::vector<int>& reverseTabs);
	/// Create a new tilemap
	/// @param parent The parent.
	cTilemap(iComponent& parent);
	/// @copydoc cControl::getSupportedHandlers
	///
	/// @todo Document possible handlers
	std::set<eDlogEvt> getSupportedHandlers() const override {
		return {EVT_CLICK, EVT_FOCUS, EVT_DEFOCUS};
	}
	void forEach(std::function<void(std::string,cControl&)> callback) override;
};

#endif
