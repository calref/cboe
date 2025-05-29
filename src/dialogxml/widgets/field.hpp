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
#include "gfx/render_text.hpp"
#include "tools/undo.hpp"
#include "tools/drawable.hpp"
#include "tools/event_listener.hpp"

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
class cTextField : public cControl, public iEventListener, public iDrawable {
public:
	bool parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) override;
	bool parseContent(ticpp::Node& content, int n, std::string tagName, std::string fname, std::string& text) override;
	/// For text fields, this is triggered when it loses or gains the input focus.
	/// @copydoc cControl::getSupportedHandlers
	///
	/// @todo Document possible handlers
	std::set<eDlogEvt> getSupportedHandlers() const override {
		return {EVT_FOCUS, EVT_DEFOCUS};
	}
	bool handleClick(location where, cFramerateLimiter& fps_limiter) override;
	bool handle_event(const sf::Event&) override;
	bool handle_mouse_pressed(const sf::Event&);
	bool handle_key_pressed(const sf::Event&, cKey& pendingKey);
	void setText(std::string to) override;
	storage_t store() const override;
	void restore(storage_t to) override;
	/// Get the current input type of the field.
	/// @return The input type.
	eFldType getInputType();
	/// Set the input type of the field.
	/// @param newType The new input type.
	void setInputType(eFldType newType);
	/// Create a new editable text field.
	/// @param parent The parent.
	explicit cTextField(iComponent& parent);
	bool isClickable() const override;
	bool isFocusable() const override;
	bool isScrollable() const override;
	virtual ~cTextField();
	void draw() override;
	/// Check if this text field currently has input focus.
	/// @return true if it it is currently focused.
	bool hasFocus() const;
	/// Handle keyboard input.
	/// @param key The keypress to handle.
	void handleInput(cKey key, bool record = false);
	cTextField& operator=(cTextField& other) = delete;
	cTextField(cTextField& other) = delete;
	/// This field is only used by cDialog during the loading process. Changing it will have no effect.
	long tabOrder = 0;
	void replay_selection(ticpp::Element& next_action);
private:
	void callHandler(event_fcn<EVT_FOCUS>::type onFocus, cDialog& me, std::string id) override;
	bool callHandler(event_fcn<EVT_DEFOCUS>::type onFocus, cDialog& me, std::string id) override;
	bool manageFormat(eFormat prop, bool set, boost::any* val) override;
	void set_ip(location clickLoc, int cTextField::* insertionPoint);
	void record_selection();
	cUndoList history;
	action_ptr current_action;
	eFldType field_type;
	bool haveFocus;
	int insertionPoint;
	int selectionPoint;
	sf::Color color;
	bool ip_visible;
	sf::Clock ip_timer, hist_timer, click_timer;
	bool changeMade = true;
	rectangle text_rect;
	std::vector<snippet_t> snippets;
	int ip_row, ip_col;
	/// Setting maxChars AFTER the player has a chance to type in the field would be a bad idea.
	/// So would calling setText() with a string longer than maxChars.
	/// Really, it should probably only be set in xml, as the attribute "max-chars".
	int maxChars = -1;
	friend class aTextInsert;
	friend class aTextDelete;
};

class aTextInsert : public cAction {
	cTextField& in;
	int at;
	std::string text;
	bool undo_me() override, redo_me() override;
public:
	aTextInsert(cTextField& in, int at, std::string text = "");
	void append(char c);
	~aTextInsert() {}
};

class aTextDelete : public cAction {
	cTextField& in;
	int start, end, ip;
	std::string text;
	bool undo_me() override, redo_me() override;
public:
	aTextDelete(cTextField& in, int start, int end);
	aTextDelete(cTextField& in, int start, std::string content, bool from_start);
	void append_front(char c);
	void append_back(char c);
	~aTextDelete() {}
};

#endif
