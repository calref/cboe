//
//  stack.hpp
//  BoE
//
//  Created by Celtic Minstrel on 14-12-22.
//
//

#ifndef BoE_DIALOG_STACK_HPP
#define BoE_DIALOG_STACK_HPP

#include <vector>
#include <map>
#include <string>
#include "container.hpp"

/// A stack groups several controls that represent an array of data.
/// Generally, each control would represent some aspect of a single element of the data.
/// The stack handles updating those controls to displaying different elements of the data,
/// and stores the hidden portion of the array within itself.
/// @note The stack itself provides no mechanism for switching pages. You will need
/// other controls, not within the stack, to trigger the switch.
///
/// A stack supports a click handler, which is triggered prior to passing it on to the
/// clicked control.
///
/// When the stack's page is changed, the focus handlers for any edit text fields in
/// the stack are triggered with the third parameter set to true to indicate they are
/// losing focus. If any of them return false, the page change is cancelled.
/// In addition, if one of the fields in the stack previously held the focus, its
/// focus handler is called with the third parameter set to false, to indicate that
/// it is gaining focus.
class cStack : public cContainer {
	friend class cDialog; // So it can insert controls
	size_t nPages, curPage = 0;
	std::string clicking;
	std::vector<std::map<std::string,storage_t>> storage;
	std::map<std::string,cControl*> controls;
	bool drawFramed = false;
	bool manageFormat(eFormat prop, bool set, boost::any* val) override;
	std::vector<std::vector<std::string>> templates;
	std::map<std::string, size_t> templateNames;
	size_t defaultTemplate = std::numeric_limits<size_t>::max();
	void applyTemplate(size_t n);
public:
	bool parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) override;
	bool parseContent(ticpp::Node& content, int n, std::string tagName, std::string fname, std::string& text) override;
	void validatePostParse(ticpp::Element& who, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& nodes) override;
	bool isClickable() override;
	bool isFocusable() override;
	bool isScrollable() override;
	void draw() override;
	bool hasChild(std::string id) override;
	cControl& getChild(std::string id) override;
	/// Switch the stack to a particular page.
	/// You need to do this before retrieving data from that page.
	/// @param The new page number
	/// @return false if the page could not be changed, usually due to a focus handler
	bool setPage(size_t n);
	/// Get the current page the stack is displaying.
	/// @return The current page number
	size_t getPage();
	/// Set the number of pages in the stack.
	/// @param n The new number of pages
	/// @note If you reduce the number of pages, some data will be destroyed.
	/// @note If the current page is deleted by this function, the last page will be shown, if there are any left.
	void setPageCount(size_t n);
	/// Add a new page to the end of the stack.
	/// This is equivalent to calling setPageCount(getPageCount() + 1).
	void addPage();
	/// Apply a named template to the current page.
	/// @param name The name of the template to apply
	void applyTemplate(const std::string& name);
	// Get the number of pages in the stack.
	/// @return The number of pages
	size_t getPageCount();
	/// Recalculate the stack's bounding rect based on its contained controls.
	void recalcRect() override;
	/// Adds any fields in this stack to the tab order building arrays.
	/// Meant for internal use.
	void fillTabOrder(std::vector<int>& specificTabs, std::vector<int>& reverseTabs);
	/// Create a new stack
	/// @param parent The parent dialog.
	cStack(cDialog& parent);
	/// @copydoc cControl::getSupportedHandlers
	///
	/// @todo Document possible handlers
	const std::set<eDlogEvt> getSupportedHandlers() const override {
		return {EVT_CLICK, EVT_FOCUS, EVT_DEFOCUS};
	}
	void forEach(std::function<void(std::string,cControl&)> callback) override;
	cStack& operator=(cStack& other) = delete;
	cStack(cStack& other) = delete;
};

#endif
