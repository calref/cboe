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
#include "button.hpp"

/// A dialog that presents a list of strings with LEDs and allows you to choose one.
/// The list may span several pages.
class cStringChoice {
	static const size_t per_page = 40;
	cDialog dlg;
	bool onLeft();
	bool onRight();
	bool onCancel(cDialog& me);
	bool onOkay(cDialog& me);
	bool onSelect(bool losing);
	void attachHandlers();
	void fillPage();
	std::vector<std::string> strings;
	size_t page, cur;
	cLedGroup* leds;
	std::function<void(cStringChoice&,int)> select_handler;
public:
	/// Initializes a dialog from a list of strings.
	/// @param strs A list of all strings in the dialog.
	/// @param title The title to show in the dialog.
	/// @param parent Optionally, a parent dialog.
	explicit cStringChoice(std::vector<std::string>& strs, std::string title, cDialog* parent = nullptr);
	/// Initializes a dialog from an iterator pair.
	/// @param begin An iterator to the first string in the dialog.
	/// @param end An iterator to one past the last string in the dialog.
	/// @param title The title to show in the dialog.
	/// @param parent Optionally, a parent dialog.
	/// @note Currently, only vector iterators are supported.
	cStringChoice(std::vector<std::string>::iterator begin, std::vector<std::string>::iterator end, std::string title, cDialog* parent = nullptr);
	/// Attach a handler to be called when the selected item changes.
	/// @param f A function that takes a reference to the dialog and the index of the newly selected item.
	void attachSelectHandler(std::function<void(cStringChoice&,int)> f);
	/// Reference the cDialog powering this choice dialog, perhaps to customize details of it.
	/// @return A pointer to the dialog.
	cDialog* operator->();
	/// Show the dialog.
	/// @param selectedIndex The index of the string that should be initially selected when the dialog is shown.
	/// @return The index of the newly selected string; if the user cancelled, this will be equal to selectedIndex.
	/// If initialized from an iterator range, this will be relative to begin.
	size_t show(size_t selectedIndex);
};

#endif
