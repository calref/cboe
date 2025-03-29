/*
 *  ledgroup.hpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 2020-12-21.
 *
 */

#ifndef BOE_DIALOG_LEDGROUP_HPP
#define BOE_DIALOG_LEDGROUP_HPP

#include <string>
#include <map>
#include "led.hpp"
#include "container.hpp"

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
class cLedGroup : public cContainer {
	friend class cTilemap; // So it can call parseAttribute
	std::map<std::string,cControl*> choices;
	std::string fromList;
	std::string curSelect, prevSelect;
	std::string clicking;
	void callHandler(event_fcn<EVT_CLICK>::type onClick, cDialog& me, std::string id, eKeyMod mods) override;
	cLedGroup& operator=(cLedGroup& other) = delete;
	cLedGroup(cLedGroup& other) = delete;
public:
	/// @deprecated in favour of @ref attachEventHandler
	void attachFocusHandler(std::function<bool(cDialog&,std::string,bool)> f) override;
	bool parseContent(ticpp::Node& content, int n, std::string tagName, std::string fname, std::string& text) override;
	void validatePostParse(ticpp::Element& elem, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& elems) override;
	/// @copydoc cControl::attachClickHandler()
	///
	/// The click handler is called whenever an LED in the group is clicked, even if it's the currently selected LED.
	///
	/// An LED group triggers focus handlers when a choice other than the currently selected one is clicked.
	/// The third parameter is always false for an LED group's focus handler.
	/// You can determine what changed using getPrevSelection() and getSelected(), and can do whatever post-processing
	/// you want, including selecting a completely different option.
	std::set<eDlogEvt> getSupportedHandlers() const override {
		return {EVT_CLICK, EVT_FOCUS};
	}
	storage_t store() const override;
	void restore(storage_t to) override;
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
	/// Create a new LED group.
	/// @param parent The parent.
	explicit cLedGroup(iComponent& parent);
	bool isClickable() const override;
	bool isFocusable() const override;
	bool isScrollable() const override;
	bool handleClick(location where, cFramerateLimiter& fps_limiter) override;
	virtual ~cLedGroup();
	/// Get one of the LEDs in this group.
	/// @param id The unique key of the choice.
	/// @return A reference to the LED object.
	/// @throw std::invalid_argument if the choice does not exist in the group.
	cLed& getChild(std::string id) override;
	bool hasChild(std::string id) const override;
	/// Set the currently selected LED in this group.
	/// @param id The unique key of the choice.
	/// @throw std::invalid_argument if the choice does not exist in the group.
	void setSelected(std::string id);
	/// Get the currently selected choice.
	/// @return id The unique key of the choice.
	std::string getSelected() const;
	/// Get the previously selected choice.
	/// @return id The unique key of the choice.
	/// @note This is intended for use by focus handlers.
	///
	/// This refers to the element that was last selected before the selection changed to the current selection.
	std::string getPrevSelection() const;
	/// Recalculate the LED group's bounding rect.
	/// Call this after adding choices to the group to ensure that the choice is within the bounding rect.
	/// If a choice is not within the bounding rect, it will not respond to clicks.
	void recalcRect() override;
	void forEach(std::function<void(std::string,cControl&)> callback) override;
	void draw() override;
};
#endif
