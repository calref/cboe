/*
 *  dialog.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef DIALOG_H
#define DIALOG_H

/// @file
/// Dialog-related classes and types.

#include <SFML/Graphics.hpp>

#include <string>
#include <map>
#include <vector>
#include <exception>
#include <functional>

#include "ticpp.h"
#include "dialog.keys.hpp"
#include "dlogevt.hpp"
#include "location.hpp"
#include <boost/any.hpp>

class cControl;
class cTextField;

/// Specifies the relative position of a control's labelling text.
enum eLabelPos {
	LABEL_LEFT, LABEL_ABOVE, LABEL_RIGHT, LABEL_BELOW,
};

/// Defines a fancy dialog box with various controls.
class cDialog {
	typedef std::map<std::string,cControl*>::iterator ctrlIter;
	std::map<std::string,cControl*> controls;
	short bg;
	sf::Color defTextClr;
	sf::RenderWindow win;
	std::string currentFocus;
	cDialog* parent;
	std::string generateRandomString();
	void loadFromFile(std::string path);
	template<typename Iter> void handleTabOrder(std::string& itemHit, Iter begin, Iter end);
	std::vector<std::pair<std::string,cTextField*>> tabOrder;
	static cDialog* topWindow; // Tracks the frontmost dialog.
public:
	/// Performs essential startup initialization. Generally should not be called directly.
	static void init();
	/// The light background pattern used by the scenario editor dialogs.
	static const short BG_LIGHT, BG_DARK; ///< The dark background pattern used by the game dialogs.
	/// The default background pattern for newly created dialogs.
	static short defaultBackground;
	/// Create a new dialog with no items.
	/// @param p Optionally, a parent dialog.
	explicit cDialog(cDialog* p = nullptr);
	/// Creates a new dialog, loading its definition from a file.
	/// @param path The name of the file to load. It must be in the game's dialogs directory.
	/// @param p Optionally, a parent dialog.
	explicit cDialog(std::string path, cDialog* p = nullptr); // cd_create_dialog
	~cDialog(); // cd_kill_dialog
	/// Add a new control to the dialog.
	/// @param what A pointer to the control, which should already be constructed.
	/// @param ctrl_frame The control's bounding rect, which includes its position in the dialog.
	/// @param key A key to be used to look up the control later.
	/// @return false if the key is used, true if the control was added.
	/// @note This function is intended for internal use, which is why it takes a control pointer instead of a unique key.
	bool add(cControl* what, rectangle ctrl_frame, std::string key);
	/// Remove a control from the dialog.
	/// @param key The control's unique key.
	/// @return true if the key existed and the control was removed, false if the key did not exist
	bool remove(std::string key);
	/// Add a new static text control, positioned to function as a label for an existing control.
	/// @param key The unique key of the control to be labelled.
	/// @param label The text of the label.
	/// @param where Specifies the position of the label relative to the control.
	/// @param offset An offset in pixels between the control and the label.
	/// @param bold If true, the label will be bolded.
	/// @return true if the label was added, false if not (usually because it already had a label)
	/// @note Even if it returns false, the label has been attached (ie, the existing label was updated).
	bool addLabelFor(std::string key, std::string label, eLabelPos where, short offset, bool bold);
	/// Show the dialog and start its event loop. All dialogs are modal.
	/// @param onopen A function to be called after the dialog is displayed but before the event loop starts.
	void run(std::function<void(cDialog&)> onopen = nullptr); // cd_run_dialog
	/// Get the result of the dialog.
	/// @tparam type The result type.
	/// @throw boost::bad_any_cast if the provided result type is different from the type set by getResult().
	/// @return The dialog's result.
	template<typename type> type getResult(){
		return boost::any_cast<type>(result);
	}
	/// Set the result of the dialog.
	/// @tparam type The result type.
	/// @param val The result value.
	template<typename type> void setResult(const type& val){
		result = val;
	}
	/// Iterate through all the controls in the dialog.
	/// @param callback A function taking a string as its first argument
	/// and a control reference as its second argument.
	template<typename Fcn> void forEach(Fcn callback) {
		for(auto ctrl : controls) {
			callback(ctrl.first, *ctrl.second);
		}
	}
	/// Set the background pattern of the dialog.
	/// @param n The numeric index of the background pattern, which should be in the range [0,20].
	/// You can use the constants BG_LIGHT or BG_DARK to reference the most commonly used backgrounds.
	void setBg(short n);
	/// Get the background pattern of the dialog.
	/// @return The numeric index of the background pattern.
	short getBg();
	/// Set the default text colour applied to new dialogs when loading from a file.
	/// @param clr The text colour.
	void setDefTextClr(sf::Color clr);
	/// Get the default text colour applied to new dialogs when loading from a file.
	/// @return The text colour.
	sf::Color getDefTextClr();
	/// Set the focused text field.
	/// @param newFocus A pointer to the text field to receive focus.
	/// @param force If true, the change will be forced.
	/// The focus handlers for both the previously focused text field and the newly focused text field will not be triggered.
	/// @return true if the focus changed; if it returns false, it could mean either that the control did not exist in the dialog
	/// or that one of the focus handlers prevented the focus change.
	/// @note This function is intended for internal use, which is why it takes a control pointer instead of a unique key.
	/// @note If a null pointer is passed, the focus will be cleared. This means that tabbing between fields will no longer work,
	/// though clicking a field to focus it should still work.
	bool setFocus(cTextField* newFocus, bool force = false);
	/// Get the currently focused text field.
	/// @return A pointer to the currently focused field.
	cTextField* getFocus();
	/// Close the dialog.
	/// @param triggerFocus true to allow the focus handler of the currently focused text field to prevent the dialog from closing
	/// @return true unless the currently focused field prevented the dialog from closing
	/// @note The dialog is not actually closed immediately;
	/// instead, a flag is set that triggers the event loop to end and the dialog to close.
	///
	/// Generally, you would pass true in a handler for an OK button and false in a handler for a Cancel button.
	bool toast(bool triggerFocus);
	/// Reopen the dialog after closing it.
	/// This is meant to be called from within an event handler to reverse a previous call to toast();
	/// if you're already out of the dialog's event loop, you should instead call run() to reopen it.
	void untoast();
	/// Determine how the dialog exited.
	/// @return the argument passed to toast() when the dialog was closed
	bool accepted();
	/// Check if a control exists with a given ID.
	/// @param id The unique key of the control.
	/// @return true if it exists.
	bool hasControl(std::string id);
	/// Get a reference to a control.
	/// @param id The unique key of the control.
	/// @throw std::invalid_argument if the control does not exist.
	/// @return a reference to the requested control.
	cControl& getControl(std::string id);
	/// @copydoc getControl()
	cControl& operator[](std::string id);
	/// Recalculate the dialog's bounding rect.
	/// Call this after adding controls to the dialog to ensure that the control is within the bounding rect.
	void recalcRect();
	/// Attach the same handler for a given event to several controls.
	/// @tparam t The type of event to attach handlers for.
	/// @param controls A list of control IDs to attach the handlers to.
	/// @throw xHandlerNotSupported if any of the controls do not support this event type.
	/// In this event, any subsequent controls in the list will not have had the handlers attached.
	template<eDlogEvt t> void attachEventHandlers(typename event_fcn<t>::type handler, const std::vector<std::string>& controls);
	// TODO: It seems like a bad thing for these two to not use the typedefs...
	/// Attach the same click handler to several controls.
	/// @param handler The handler to attach.
	/// @param controls A list of the unique keys of the controls to which you want the handler attached.
	/// @throw xHandlerNotSupported if any of the controls do not support click handlers.
	/// @throw std::invalid_argument if any of the controls do not exist.
	/// @see cControl::attachClickHandler()
	/// @deprecated in favour of @ref attachEventHandlers
	void attachClickHandlers(std::function<bool(cDialog&,std::string,eKeyMod)> handler, std::vector<std::string> controls);
	/// Attach the same focus handler to several controls.
	/// @param handler The handler to attach.
	/// @param controls A list of the unique keys of the controls to which you want the handler attached.
	/// @throw xHandlerNotSupported if any of the controls do not support focus handlers.
	/// @throw std::invalid_argument if any of the controls do not exist.
	/// @see cControl::attachFocusHandler()
	/// @deprecated in favour of @ref attachEventHandlers
	void attachFocusHandlers(std::function<bool(cDialog&,std::string,bool)> handler, std::vector<std::string> controls);
	/// Get the bounding rect of the dialog.
	/// @return The dialog's bounding rect.
	rectangle getBounds() {return winRect;}
	/// Send keyboard input to the frontmost dialog.
	/// Currently, only text edit fields will respond to this.
	/// @return true if there was a dialog opened to send to.
	static bool sendInput(cKey key);
	/// Sets whether to animate graphics in dialogs.
	static bool doAnimations;
	/// Adds a new control described by the passed XML element.
	/// @tparam Ctrl The type of control to add.
	/// @param who The XML element describing the control.
	/// @note It is up to the caller to ensure that that the element
	/// passed describes the type of control being requested.
	template<class Ctrl> std::pair<std::string,Ctrl*> parse(ticpp::Element& who) {
		std::pair<std::string,Ctrl*> p;
		p.second = new Ctrl(*this);
		p.first = p.second->parse(who, fname);
		if(p.first == ""){
			do{
				p.first = generateRandomString();
			}while(controls.find(p.first) != controls.end());
		}
		return p;
	}
	cDialog& operator=(cDialog& other) = delete;
	cDialog(cDialog& other) = delete;
private:
	void draw();
	void process_keystroke(cKey keyHit);
	void process_click(location where, eKeyMod mods);
	bool dialogNotToast, didAccept;
	rectangle winRect;
	boost::any result;
	std::string fname;
	sf::Clock animTimer, paintTimer;
	friend class cControl;
};

/// Thrown when an invalid element is found while parsing an XML dialog definition.
class xBadNode : public std::exception {
	std::string type, dlg;
	int row, col;
	mutable const char* msg;
public:
	/// Construct a new exception.
	/// @param t The tag name of the invalid element.
	/// @param r The line number of the element in the source.
	/// @param c The column number of the element in the source.
	/// @param dlg The name of the file in which the element occurred.
	xBadNode(std::string t, int r, int c, std::string dlg) throw();
	~xBadNode() throw();
	/// @return The error message.
	const char* what() const throw();
};

/// Thrown when an invalid attribute is found while parsing an XML dialog definition.
class xBadAttr : public std::exception {
	std::string type, name, dlg;
	int row, col;
	mutable const char* msg;
public:
	/// Construct a new exception.
	/// @param t The tag name of the element with the invalid attribute.
	/// @param n The name of the invalid attribute.
	/// @param r The line number of the element in the source.
	/// @param c The column number of the element in the source.
	/// @param dlg The name of the file in which the element occurred.
	xBadAttr(std::string t,std::string n, int r, int c, std::string dlg) throw();
	~xBadAttr() throw();
	/// @return The error message.
	const char* what() const throw();
};

/// Thrown when an element is missing a required attribute while parsing an XML dialog definition.
class xMissingAttr : public std::exception {
	std::string type, name, dlg;
	int row, col;
	mutable const char* msg;
public:
	/// Construct a new exception.
	/// @param t The tag name of the element with the missing attribute.
	/// @param n The name of the missing attribute.
	/// @param r The line number of the element in the source.
	/// @param c The column number of the element in the source.
	/// @param dlg The name of the file in which the element occurred.
	xMissingAttr(std::string t,std::string n, int r, int c, std::string dlg) throw();
	~xMissingAttr() throw();
	/// @return The error message.
	const char* what() const throw();
};

/// Thrown when a required element is missing while parsing an XML dialog definition.
class xMissingElem : public std::exception {
	std::string parent, name, dlg;
	int row, col;
	mutable const char* msg;
public:
	/// Construct a new exception.
	/// @param p The tag name of the parent element.
	/// @param t The tag name of the missing element.
	/// @param r The line number of the element in the source.
	/// @param c The column number of the element in the source.
	/// @param dlg The name of the file in which the element occurred.
	xMissingElem(std::string p, std::string t, int r, int c, std::string dlg) throw();
	~xMissingElem() throw();
	/// @return The error message.
	const char* what() const throw();
};

/// Thrown when an invalid value is found anywhere within an element's or attribute's content.
class xBadVal : public std::exception {
	std::string type, name, val, dlg;
	int row, col;
	mutable const char* msg;
public:
	/// A magic value to indicate errors in an element's content, rather than an attribute's content.
	static const char*const CONTENT;
	/// Construct a new exception.
	/// @param t The tag name of the element with the invalid value.
	/// @param n The name of the attribute with the invalid value.
	/// You should pass xBadVal::CONTENT if the bad value is within an element's content.
	/// @param v The invalid value.
	/// @param r The line number of the element in the source.
	/// @param c The column number of the element in the source.
	/// @param dlg The name of the file in which the element occurred.
	xBadVal(std::string t,std::string n,std::string v, int r, int c, std::string dlg) throw();
	~xBadVal() throw();
	/// @return The error message.
	const char* what() const throw();
};

#endif
