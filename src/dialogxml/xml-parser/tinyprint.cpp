//
//  tinyprint.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-01-13.
//
//

#include "tinyprint.h"
#include <stdexcept>

using namespace ticpp;

Printer::Printer(std::string fname, std::ostream& to) : doc(fname), stream(to) {
	Declaration* decl = new Declaration("1.0", "UTF-8", "no");
	doc.LinkEndChild(decl);
}

void Printer::OpenElement(std::string tagName) {
	openElements.push(new Element(tagName));
}

void Printer::CloseElement(std::string tagName) {
	if(openElements.empty())
		throw std::out_of_range("No elements left to close!");
	if(tagName != openElements.top()->Value())
		throw std::logic_error("Mismatched closing tag! (Expected " + openElements.top()->Value() + ")");
	Element* top = openElements.top();
	openElements.pop();
	PushNode(top);
}

void Printer::PushElement(std::string tagName) {
	PushNode(new Element(tagName));
}

void Printer::PushComment(std::string comment) {
	PushNode(new Comment(comment));
}

void Printer::PushStylesheet(std::string value, std::string href) {
	PushNode(new StylesheetReference(value, href));
}

void Printer::PushNode(Node* node) {
	if(openElements.empty())
		doc.LinkEndChild(node);
	else openElements.top()->LinkEndChild(node);
}

Printer::~Printer() {
	while(!openElements.empty())
		CloseElement(openElements.top()->Value());
	stream << doc;
}
