//
//  tinyprint.hpp
//  BoE
//
//  Created by Celtic Minstrel on 15-01-13.
//
//

#ifndef BoE_tinyprint_hpp
#define BoE_tinyprint_hpp

#include "ticpp.h"
#include <string>
#include <stack>
#include <boost/lexical_cast.hpp>
#include <ios>

namespace ticpp {
	class Printer : public Visitor {
		Document doc;
		std::ostream& stream;
		std::stack<Element*> openElements;
	public:
		Printer(std::string fname, std::ostream& to);
		~Printer();
		void OpenElement(std::string tagName);
		void CloseElement(std::string tagName);
		void PushElement(std::string tagName);
		void PushComment(std::string comment);
		void PushStylesheet(std::string value, std::string href);
		void PushNode(Node* node);
		template<typename T> void PushAttribute(std::string attrName, T attrVal) {
			openElements.top()->SetAttribute(attrName, boost::lexical_cast<std::string>(attrVal));
		}
		template<typename T> void PushText(T textVal, bool cdata = false) {
			PushNode(new Text(boost::lexical_cast<std::string>(textVal), cdata));
		}
		template<typename T> void PushElement(std::string tagName, T elemVal, bool cdata = false) {
			OpenElement(tagName);
			PushText(elemVal, cdata);
			CloseElement(tagName);
		}
	};
}

template<> inline void ticpp::Printer::PushAttribute(std::string attrName, bool attrVal) {
	PushAttribute(attrName, attrVal ? "true" : "false");
}

template<> inline void ticpp::Printer::PushElement(std::string attrName, bool attrVal, bool cdata) {
	PushElement(attrName, attrVal ? "true" : "false", cdata);
}

template<> inline void ticpp::Printer::PushText(bool textVal, bool cdata) {
	PushText(textVal ? "true" : "false", cdata);
}

#endif
