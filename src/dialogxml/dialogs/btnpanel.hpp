#ifndef DIALOG_BTNPANEL_H
#define DIALOG_BTNPANEL_H

#include <string>
#include <vector>
#include <functional>
#include <boost/optional.hpp>
#include "dialog.hpp"
#include "dialogxml/widgets/ledgroup.hpp"

/// A dialog that presents a list of labeled tiny buttons, plus an OK and Cancel button.
/// The list may span several pages.
class cButtonPanel {
	const size_t per_page = 5;
	cDialog dlg;
	bool onLeft();
	bool onRight();
	bool onCancel(cDialog& me);
	bool onOkay(cDialog& me);
	void attachHandlers();
	void fillPage();
	size_t lastPage() const;
	std::vector<std::string> strings;
	std::vector<std::function<void(cButtonPanel&)>> click_handlers;
	size_t page, cur;
	cButtonPanel(cDialog* parent);
public:
	/// Initializes a dialog from a list of strings.
	/// @param strs A list of all strings in the dialog.
	/// @param click_handlers A list of click handlers for the strings
	/// @param title The title to show in the dialog.
	/// @param parent Optionally, a parent dialog.
	explicit cButtonPanel(const std::vector<std::string>& strs, std::vector<std::function<void(cButtonPanel&)>> click_handlers, std::string title, std::string ok_str = "", cDialog* parent = nullptr);
	/// Reference the cDialog powering this choice dialog, perhaps to customize details of it.
	/// @return A pointer to the dialog.
	cDialog* operator->();
	/// Show the dialog.
	/// @return True if OK was pressed, false if Cancel pressed
	bool show();
	/// Set the dialog's title.
	/// @param title The new title.
	void setTitle(const std::string& title);
	/// Get the list of strings.
	std::vector<std::string> getStrings() const { return strings; }
};

#endif
