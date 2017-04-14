//
//  dlogevt.hpp
//  BoE
//
//  Created by Celtic Minstrel on 15-10-03.
//
//

#ifndef BoE_dlogevt_hpp
#define BoE_dlogevt_hpp

#include <functional>
#include <string>
#include <type_traits>
#include "dialog.keys.hpp"

/// Represents an event that can occur in a dialog.
enum eDlogEvt {
	EVT_CLICK,		///< A click event - the mouse has been pressed and released.
	EVT_FOCUS,		///< A focus event - a control has gained focus.
	EVT_DEFOCUS,	///< A defocus event - a control is losing focus.
	EVT_SCROLL,		///< A scroll event - a control has changed in response to the mouse wheel.
};

/// A metafunction to obtain the signature of a handler for a given event.
/// The first parameter is always cDialog& and the second paramater is
/// usually std::string. Return type and additional parameters, if any,
/// may vary.
/// @tparam t The type of event you need a handler for.
/// @return An std::function type representing the even handler
/// as member type.
template<eDlogEvt t> struct event_fcn;
class cDialog;

/// @typedef type
/// @memberof event_fcn
/// @brief The return value; an std::function instantiation.

/// Specialization of event_fcn for EVT_CLICK.
template<> struct event_fcn<EVT_CLICK> {
	/// The signature for a click event handler.
	/// Requires the dialog, the ID of the clicked element, and the current modifier key state.
	/// Returns nothing.
	using type = std::function<void(cDialog&, std::string, eKeyMod)>;
};

/// Specialization of event_fcn for EVT_FOCUS.
template<> struct event_fcn<EVT_FOCUS> {
	/// The signature for a focus event handler.
	/// Requires only the dialog and the ID of the clicked element.
	/// Returns nothing.
	using type = std::function<void(cDialog&, std::string)>;
};

/// Specialization of event_fcn for EVT_DEFOCUS.
template<> struct event_fcn<EVT_DEFOCUS> {
	/// The signature of a defocus event handler.
	/// Requires only the dialog and the ID of the clicked element.
	/// Returns false if defocus should be cancelled.
	using type = std::function<bool(cDialog&, std::string)>;
};

/// Specialization of event_fcn for EVT_SCROLL.
template<> struct event_fcn<EVT_SCROLL> {
	/// The signature of a scroll event handler.
	/// Requires the dialog, the ID of the clicked element, and the resulting value.
	/// Returns false if scrolling should be cancelled.
	using type = std::function<bool(cDialog&, std::string, int)>;
};

#endif
