//
//  stack.cpp
//  BoE
//
//  Created by Celtic Minstrel on 14-12-22.
//
//

#include "stack.hpp"
#include "field.hpp"
#include "button.hpp"
#include "dialogxml/dialogs/dialog.hpp"
#include "message.hpp"
#include "pict.hpp"
#include "scrollbar.hpp"
#include <climits>

bool cStack::hasChild(std::string id) {
	return controls.find(id) != controls.end();
}

cControl& cStack::getChild(std::string id) {
	if(!hasChild(id)) throw std::invalid_argument(id + " was not found in the stack");
	return *controls[id];
}

bool cStack::manageFormat(eFormat prop, bool set, boost::any* val) {
	switch(prop) {
		case TXT_FRAME:
			if(val) {
				if(set) frameStyle = boost::any_cast<eFrameStyle>(*val);
				else *val = frameStyle;
			}
			break;
		// TODO: Colour is not supported
		default: return false;
	}
	return true;
}

bool cStack::isClickable() {
	return true;
}

bool cStack::isFocusable() {
	return false;
}

bool cStack::isScrollable() {
	return false;
}

void cStack::draw() {
	if(!isVisible()) return;
	for(auto& p : controls) {
		p.second->draw();
	}
	if(drawFramed) drawFrame(2, frameStyle);
}

bool cStack::setPage(size_t n) {
	if(n >= nPages) return false;
	if(n == curPage) return true;
	cTextField* focus = parent->getFocus();
	bool failed = false;
	for(auto p : controls) {
		const std::string& id = p.first;
		cControl& ctrl = *p.second;
		// Only trigger focus handlers if the current page still exists.
		if(curPage < nPages) {
			storage[curPage][id] = ctrl.store();
			if(!ctrl.triggerFocusHandler(*parent, id, true))
				failed = true;
			if(!failed) {
				ctrl.restore(storage[n][id]);
				if(focus == &ctrl && !ctrl.triggerFocusHandler(*parent, id, false)) {
					failed = true;
					ctrl.restore(storage[curPage][id]);
				}
			}
		}
	}
	if(!failed) curPage = n;
	return !failed;
}

size_t cStack::getPage() {
	return curPage;
}

void cStack::setPageCount(size_t n) {
	if(curPage >= n && n > 0)
		setPage(nPages - 1);
	auto added = n - nPages;
	if(n == 0) curPage = 0;
	nPages = n;
	storage.resize(nPages);
	if(added > 0 && defaultTemplate < storage.size()) {
		auto saveCur = curPage;
		for(int i = n - added; i < nPages; i++) {
			setPage(i);
			applyTemplate(defaultTemplate);
		}
		setPage(saveCur);
	}
}

void cStack::addPage() {
	setPageCount(getPageCount() + 1);
}

size_t cStack::getPageCount() {
	return nPages;
}

void cStack::recalcRect() {
	auto iter = controls.begin();
	frame = {INT_MAX, INT_MAX, 0, 0};
	while(iter != controls.end()){
		cControl& ctrl = *iter->second;
		rectangle otherFrame = ctrl.getBounds();
		if(otherFrame.right > frame.right)
			frame.right = otherFrame.right;
		if(otherFrame.bottom > frame.bottom)
			frame.bottom = otherFrame.bottom;
		if(otherFrame.left < frame.left)
			frame.left = otherFrame.left;
		if(otherFrame.top < frame.top)
			frame.top = otherFrame.top;
		iter++;
	}
	frame.inset(-6,-6);
}

void cStack::fillTabOrder(std::vector<int>& specificTabs, std::vector<int>& reverseTabs) {
	for(auto p : controls) {
		cControl& ctrl = *p.second;
		if(ctrl.getType() == CTRL_FIELD) {
			cTextField& field = dynamic_cast<cTextField&>(ctrl);
			if(field.tabOrder > 0)
				specificTabs.push_back(field.tabOrder);
			else if(field.tabOrder < 0)
				reverseTabs.push_back(field.tabOrder);
		}
	}
}

cStack::cStack(cDialog& parent) : cContainer(CTRL_STACK, parent), curPage(0), nPages(0) {}

void cStack::forEach(std::function<void(std::string,cControl&)> callback) {
	for(auto ctrl : controls)
		callback(ctrl.first, *ctrl.second);
}


bool cStack::parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) {
	if(attr.Name() == "pages") {
		try {
			size_t n;
			attr.GetValue(&n);
			setPageCount(n);
		} catch(ticpp::Exception&) {
			throw xBadVal(tagName, attr.Name(), attr.Value(), attr.Row(), attr.Column(), fname);
		}
		return true;
	}
	return cContainer::parseAttribute(attr, tagName, fname);
}

bool cStack::parseContent(ticpp::Node& content, int n, std::string tagName, std::string fname, std::string& text) {
	using namespace ticpp;
	if(content.Type() == TiXmlNode::ELEMENT) {
		auto& elem = dynamic_cast<Element&>(content);
		std::string id;
		if(elem.Value() == "page") {
			templates.emplace_back();
			Iterator<Element> iter;
			for(iter = iter.begin(&elem); iter != iter.end(); iter++) {
				if(iter->Type() == TiXmlNode::ELEMENT) {
					if(!parseChildControl(*iter, controls, id)) return false;
					templates.back().push_back(id);
				}
			}
			Iterator<Attribute> attr;
			for(attr = attr.begin(&elem); attr != attr.end(); attr++) {
				std::string name = attr->Name();
				if(name == "default") {
					std::string val = attr->Value();
					if(val == "true") {
						if(defaultTemplate != std::numeric_limits<size_t>::max())
							throw xBadAttr("page", "multiple default", attr->Row(), attr->Column(), fname);
						defaultTemplate = templates.size() - 1;
					} else if(val != "false") throw xBadVal("page", name, val, attr->Row(), attr->Column(), fname);
				} else if(name == "span") {
					int span;
					try {
						attr->GetValue(&span);
					} catch(Exception&) {
						throw xBadVal("page", name, attr->Value(), attr->Row(), attr->Column(), fname);
					}
					while(0 <-- span) {
						templates.push_back(templates.back());
					}
				} else if(name == "template") {
					templateNames[attr->Value()] = templates.size() - 1;
				} else throw xBadAttr("page", name, attr->Row(), attr->Column(), fname);
			}
			return true;
		} else return parseChildControl(elem, controls, id);
	}
	return cContainer::parseContent(content, n, tagName, fname, text);
}

void cStack::validatePostParse(ticpp::Element&, std::string, const std::set<std::string>&, const std::multiset<std::string>&) {
	// Don't defer to the superclass, since this is an abstract widget with no position
	//cControl::validatePostParse(who, fname, attrs, nodes);
	// Actual number of pages is the larger of pages= and count of <page>
	setPageCount(std::max(nPages, templates.size()));
	for(size_t i = 0; i < nPages; i++) {
		setPage(i);
		applyTemplate(i);
	}
	setPage(0);
}

void cStack::applyTemplate(size_t n) {
	if(n >= templates.size()) return;
	for(size_t i = 0; i < templates.size(); i++) {
		for(size_t j = 0; j < templates[i].size(); j++) {
			auto ctrl = controls[templates[i][j]];
			if(n == i) ctrl->show(); else ctrl->hide();
		}
	}
}

void cStack::applyTemplate(const std::string& name) {
	auto iter = templateNames.find(name);
	if(iter != templateNames.end()) applyTemplate(iter->second);
}


