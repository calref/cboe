/*
 *  dlogutil.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef DIALOG_CHOICEDLOG_H
#define DIALOG_CHOICEDLOG_H

#include <string>
#include <vector>
#include "dialog.hpp"

/// A simple dialog that lets you select one of several buttons.
/// This class loads a definition from a file, so there can be any amount of other stuff in the dialog,
/// and the buttons could be arranged in any fashion you want.
class cChoiceDlog {
	cDialog dlg;
protected:
	/// The click handler for the dialog's buttons.
	/// @param me A reference to the current dialog.
	/// @param id The unique key of the clicked control.
	/// @return true, indicating the event should continue.
	bool onClick(cDialog& me, std::string id);
	/// Create a choice dialog, but don't initialize it.
	/// @param p Optionally, a parent dialog.
	explicit cChoiceDlog(cDialog* p = nullptr);
public:
	/// Create a choice dialog with just one button.
	/// @param file The file to load the dialog definition from.
	/// @param p Optionally, a parent dialog.
	/// @note The dialog definition file must include a button whose name attribute is "okay".
	explicit cChoiceDlog(std::string file, cDialog* p = nullptr);
	/// Create a choice dialog with several buttons.
	/// @param file The file to load the dialog definition from.
	/// @param buttons A list of the buttons to handle. All of them must be defined in the file.
	/// @param p Optionally, a parent dialog.
	cChoiceDlog(std::string file, std::vector<std::string> buttons, cDialog* p = nullptr);
	/// Reference the cDialog powering this choice dialog, perhaps to customize details of it.
	/// @return A pointer to the dialog.
	cDialog* operator->();
	/// Show the dialog.
	/// @return The unique key of the clicked button.
	std::string show();
};

#endif
