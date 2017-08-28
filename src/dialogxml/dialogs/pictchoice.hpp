/*
 *  dlogutil.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef DIALOG_PICTCHOICE_H
#define DIALOG_PICTCHOICE_H

#include <vector>
#include <functional>
#include <utility>

#include "dialog.hpp"
#include "button.hpp"
#include "pictypes.hpp"

/// A dialog that presents a list of icons with LEDs and allows you to choose one.
/// The list may span several pages.
class cPictChoice {
	static const size_t per_page = 36;
	bool didAccept;
	cDialog dlg;
	void attachHandlers();
	bool onLeft();
	bool onRight();
	bool onCancel();
	bool onOkay();
	bool onSelect(bool losing);
	void fillPage();
	std::vector<std::pair<pic_num_t,ePicType>> picts;
	size_t page, cur;
	cLedGroup* leds;
	std::function<void(cPictChoice&,int)> select_handler;
public:
	/// Initializes a dialog from a list of icons.
	/// @param pics A list of all icons in the dialog.
	/// @param t The type of icons to show; all icons are assumed to be of the same type.
	/// @param parent Optionally, a parent dialog.
	cPictChoice(const std::vector<pic_num_t>& pics, ePicType t, cDialog* parent = nullptr);
	/// Initializes a dialog from a list of icons.
	/// @param pics A list of all icons in the dialog as {num,type} pairs.
	/// @param parent Optionally, a parent dialog.
	cPictChoice(const std::vector<std::pair<pic_num_t,ePicType>>& pics, cDialog* parent = nullptr);
	/// Initializes a dialog from an iterator pair.
	/// @param begin An iterator to the first icon in the dialog.
	/// @param end An iterator to one past the last icon in the dialog.
	/// @param t The type of icons to show; all icons are assumed to be of the same type.
	/// @param parent Optionally, a parent dialog.
	cPictChoice(std::vector<pic_num_t>::const_iterator begin, std::vector<pic_num_t>::const_iterator end, ePicType t, cDialog* parent = nullptr);
	/// Initializes a dialog from an index pair.
	/// @param first The number of the first icon in the dialog.
	/// @param last The number of the last icon in the dialog.
	/// @param t The type of icons to show; all icons are assumed to be of the same type.
	/// @param parent Optionally, a parent dialog.
	cPictChoice(pic_num_t first, pic_num_t last, ePicType t, cDialog* parent = nullptr);
	/// Attach a handler to be called when the selected item changes.
	/// @param f A function that takes a reference to the dialog and the index of the newly selected item.
	void attachSelectHandler(std::function<void(cPictChoice&,int)> f);
	/// Reference the cDialog powering this choice dialog, perhaps to customize details of it.
	/// @return A pointer to the dialog.
	cDialog* operator->();
	/// Show the dialog.
	/// @param cur_sel The index of the icon that should be initially selected when the dialog is shown.
	/// @return false if the user clicked Cancel, true otherwise.
	bool show(size_t cur_sel);
	/// Get the chosen icon.
	/// @return The number of the chosen icon.
	pic_num_t getPicChosen();
	/// Get the chosen icon.
	/// @return The type of the chosen icon.
	ePicType getPicChosenType();
	/// Get the index of the selected icon in the original list.
	/// @return The index
	size_t getSelected();
};

#endif
