/*
 *  dlogutil.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef DIALOG_2STRING_H
#define DIALOG_2STRING_H

#include <string>
#include <functional>
#include "pictypes.hpp"
#include "sounds.hpp" // for snd_num_t
#include "dialog.hpp"

/// The signature of a record handler for cStrDlog.
typedef std::function<void(cDialog&)> record_callback_t;

/// A simple dialog with one or two long strings, an optional title, and an optional record button.
class cStrDlog {
	static std::string getFileName(short n_strs, ePicType type, bool hasTitle);
	cDialog dlg;
	short sound = -1;
	record_callback_t rec_f;
	bool hasRecord;
	const ePicType type;
	bool onRecord(std::string id);
	bool onDismiss();
public:
	/// Construct a string dialog.
	/// @param str1 The first string.
	/// @param str2 The second string. If left as an empty string, there will be only one string in the dialog.
	/// @param title The title. If left as an empty string, there will be no title.
	/// @param pic The icon to show at the top left.
	/// @param t The type of icon to show.
	/// @param parent Optionally, a parent dialog.
	cStrDlog(std::string str1,std::string str2,std::string title,pic_num_t pic,ePicType t,cDialog* parent = nullptr);
	/// Set a sound to be played when the dialog is shown.
	/// @param num The sound number.
	/// @return This object, for method-call chaining.
	cStrDlog& setSound(snd_num_t num);
	/// Set a record handler.
	/// @param rec The handler.
	/// @return This object, for method-call chaining.
	/// @note Only one record handler can be set at a time. To remove it, set it to null.
	/// @note The presence of the Record button is determined entirely by the presence of a record handler.
	///
	/// A record handler should take one parameter, which is a reference to the dialog.
	/// (That's the cDialog, not the cStrDlog.) It should return void.
	cStrDlog& setRecordHandler(record_callback_t rec);
	/// Reference the cDialog powering this choice dialog, perhaps to customize details of it.
	/// @return A pointer to the dialog.
	cDialog* operator->();
	/// Show the dialog.
	void show();
};

/// Shows a simple error dialog for an unrecoverable condition.
/// @param str1 The first string in the error dialog.
/// @param str2 The second string in the error dialog.
/// @param parent Optionally, a parent dialog.
void showFatalError(std::string str1, std::string str2, cDialog* parent = nullptr);
/// Shows a simple error dialog for an unrecoverable condition.
/// @param str1 The string in the error dialog.
/// @param parent Optionally, a parent dialog.
void showFatalError(std::string str1, cDialog* parent = nullptr);

/// Shows a simple error dialog for a recoverable condition.
/// @param str1 The first string in the error dialog.
/// @param str2 The second string in the error dialog.
/// @param parent Optionally, a parent dialog.
void showError(std::string str1, std::string str2, cDialog* parent = nullptr);
/// Shows a simple error dialog for a recoverable condition.
/// @param str1 The string in the error dialog.
/// @param parent Optionally, a parent dialog.
void showError(std::string str1, cDialog* parent = nullptr);

/// Shows a simple warning message dialog.
/// @param str1 The first string in the warning dialog.
/// @param str2 The second string in the warning dialog.
/// @param parent Optionally, a parent dialog.
void showWarning(std::string str1, std::string str2, cDialog* parent = nullptr);
/// Shows a simple warning message dialog.
/// @param str1 The string in the warning dialog.
/// @param parent Optionally, a parent dialog.
void showWarning(std::string str1, cDialog* parent = nullptr);

#endif
