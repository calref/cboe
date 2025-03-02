/*
 *  led.hpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 2020-12-21.
 *
 */

#ifndef BOE_DIALOG_LED_HPP
#define BOE_DIALOG_LED_HPP

#include "button.hpp"
#include "gfx/render_text.hpp"

/// Represents the state of an LED button.
/// Generally, led_red is used to indicate a selected button.
/// Currently, led_green is only used by the spell selection dialog,
/// where led_red indicates the spell is available amd led_green indicates it is selected.
enum eLedState {led_green = 0, led_red, led_off};

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
	bool parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) override;
	bool parseContent(ticpp::Node& content, int n, std::string tagName, std::string fname, std::string& text) override;
	location getPreferredSize() const override;
	storage_t store() const override;
	void restore(storage_t to) override;
	/// Create a new LED button.
	/// @param parent The parent.
	explicit cLed(iComponent& parent);
	virtual ~cLed();
	/// Set the LED's current state,.
	/// @param to The new state.
	void setState(eLedState to);
	/// Get the LED's current state.
	/// @return The current state.
	eLedState getState() const;
	void draw() override;
	/// @copydoc cControl::getSupportedHandlers
	///
	/// @todo Document possible handlers
	std::set<eDlogEvt> getSupportedHandlers() const override {
		return {EVT_CLICK, EVT_FOCUS, EVT_DEFOCUS};
	}
	cLed& operator=(cLed& other) = delete;
	cLed(cLed& other) = delete;
	void defaultClickHandler(cDialog&, std::string, eKeyMod);
private:
	void callHandler(event_fcn<EVT_CLICK>::type onClick, cDialog& me, std::string id, eKeyMod mods) override;
	bool manageFormat(eFormat prop, bool set, boost::any* val) override;
	eLedState state;
	eFont textFont;
	bool textOnRight = true;
	static rectangle ledRects[3][2];
};

#endif
