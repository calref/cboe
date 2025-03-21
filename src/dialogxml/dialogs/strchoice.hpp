/*
 *  dlogutil.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef DIALOG_STRINGCHOICE_H
#define DIALOG_STRINGCHOICE_H

#include <string>
#include <vector>
#include <functional>
#include "dialog.hpp"
#include "dialogxml/widgets/ledgroup.hpp"

/// A dialog that presents a list of strings with LEDs and allows you to choose one.
/// The list may span several pages.
class cStringChoice {
	const bool editable = false;
	const size_t per_page = 40;
	cDialog dlg;
	bool onLeft();
	bool onRight();
	bool onCancel(cDialog& me);
	bool onOkay(cDialog& me);
	bool onSelect(bool losing);
	bool onFocus(std::string which, bool losing);
	bool onSearch(cDialog& me);
	void clearHighlights();
	bool highlightSearch();
	void attachHandlers();
	void fillPage();
	void savePage();
	size_t lastPage() const;
	std::vector<std::string> strings;
	size_t page, cur;
	std::string search_str;
	bool search_open = false;
	cLedGroup* leds;
	std::function<void(cStringChoice&,int)> select_handler;
	cStringChoice(cDialog* parent, bool editable = false);
public:
	/// Initializes a dialog from a list of strings.
	/// @param strs A list of all strings in the dialog.
	/// @param title The title to show in the dialog.
	/// @param parent Optionally, a parent dialog.
	explicit cStringChoice(const std::vector<std::string>& strs, std::string title, cDialog* parent = nullptr, bool editable = false);
	/// Initializes a dialog from an iterator pair.
	/// @param begin An iterator to the first string in the dialog.
	/// @param end An iterator to one past the last string in the dialog.
	/// @param title The title to show in the dialog.
	/// @param parent Optionally, a parent dialog.
	/// @tparam Iter The iterator type
	template<typename Iter>
	cStringChoice(Iter begin, Iter end, std::string title, cDialog* parent = nullptr) : cStringChoice(parent) {
		setTitle(title);
		std::copy(begin, end, std::back_inserter(strings));
		attachHandlers();
	}
	/// Attach a handler to be called when the selected item changes.
	/// @param f A function that takes a reference to the dialog and the index of the newly selected item.
	void attachSelectHandler(std::function<void(cStringChoice&,int)> f);
	/// Reference the cDialog powering this choice dialog, perhaps to customize details of it.
	/// @return A pointer to the dialog.
	cDialog* operator->();
	/// Show the dialog.
	/// @param selectedIndex The index of the string that should be initially selected when the dialog is shown.
	/// @return The index of the newly selected string; if the user cancelled, this will be equal to the initial
	/// selectedIndex you provide. (So, pass -1 or something to signify that cancelling means the result is invalid.)
	/// If initialized from an iterator range, this will be relative to begin.
	size_t show(size_t selectedIndex);
	/// Set the dialog's title.
	/// @param title The new title.
	void setTitle(const std::string& title);
	/// Get the list of strings.
	std::vector<std::string> getStrings() const { return strings; }
};

#endif
