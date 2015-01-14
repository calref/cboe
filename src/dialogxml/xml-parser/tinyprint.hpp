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
		void PushComment(std::string comment);
		void PushStylesheet(std::string value, std::string href);
		void PushNode(Node* node);
		template<typename T> void PushAttribute(std::string attrName, T attrVal) {
			openElements.top()->SetAttribute(attrName, boost::lexical_cast<std::string>(attrVal));
		}
		template<typename T> void PushText(T textVal) {
			PushNode(new Text(boost::lexical_cast<std::string>(textVal)));
		}
		template<typename T> void PushElement(std::string tagName, T elemVal) {
			OpenElement(tagName);
			PushText(elemVal);
			CloseElement(tagName);
		}
	};
}

#if 0
class TINYXML2_LIB XMLPrinter : public XMLVisitor
{
public:
	/** Construct the printer. If the FILE* is specified,
	 this will print to the FILE. Else it will print
	 to memory, and the result is available in CStr().
	 If 'compact' is set to true, then output is created
	 with only required whitespace and newlines.
	 */
	XMLPrinter( FILE* file=0, bool compact = false, int depth = 0 );
	virtual ~XMLPrinter() {}
	/** If streaming, write the BOM and declaration. */
	void PushHeader( bool writeBOM, bool writeDeclaration );
	/** If streaming, start writing an element.
	 The element must be closed with CloseElement()
	 */
	void OpenElement( const char* name, bool compactMode=false );
	/// If streaming, add an attribute to an open element.
	void PushAttribute( const char* name, const char* value );
	void PushAttribute( const char* name, int value );
	void PushAttribute( const char* name, unsigned value );
	void PushAttribute( const char* name, bool value );
	void PushAttribute( const char* name, double value );
	/// If streaming, close the Element.
	virtual void CloseElement( bool compactMode=false );
	/// Add a text node.
	void PushText( const char* text, bool cdata=false );
	/// Add a text node from an integer.
	void PushText( int value );
	/// Add a text node from an unsigned.
	void PushText( unsigned value );
	/// Add a text node from a bool.
	void PushText( bool value );
	/// Add a text node from a float.
	void PushText( float value );
	/// Add a text node from a double.
	void PushText( double value );
	/// Add a comment
	void PushComment( const char* comment );
	void PushDeclaration( const char* value );
	void PushUnknown( const char* value );
	virtual bool VisitEnter( const XMLDocument& /*doc*/ );
	virtual bool VisitExit( const XMLDocument& /*doc*/ ) {
		return true;
	}
	virtual bool VisitEnter( const XMLElement& element, const XMLAttribute* attribute );
	virtual bool VisitExit( const XMLElement& element );
	virtual bool Visit( const XMLText& text );
	virtual bool Visit( const XMLComment& comment );
	virtual bool Visit( const XMLDeclaration& declaration );
	virtual bool Visit( const XMLUnknown& unknown );
	/**
	 If in print to memory mode, return a pointer to
	 the XML file in memory.
	 */
	const char* CStr() const {
		return _buffer.Mem();
	}
	/**
	 If in print to memory mode, return the size
	 of the XML file in memory. (Note the size returned
	 includes the terminating null.)
	 */
	int CStrSize() const {
		return _buffer.Size();
	}
	/**
	 If in print to memory mode, reset the buffer to the
	 beginning.
	 */
	void ClearBuffer() {
		_buffer.Clear();
		_buffer.Push(0);
	}
protected:
	virtual bool CompactMode( const XMLElement& ) { return _compactMode; }
	/** Prints out the space before an element. You may override to change
	 the space and tabs used. A PrintSpace() override should call Print().
	 */
	virtual void PrintSpace( int depth );
	void Print( const char* format, ... );
	void SealElementIfJustOpened();
	bool _elementJustOpened;
	DynArray< const char*, 10 > _stack;
private:
	void PrintString( const char*, bool restrictedEntitySet ); // prints out, after detecting entities.
	bool _firstElement;
	FILE* _fp;
	int _depth;
	int _textDepth;
	bool _processEntities;
	bool _compactMode;
	enum {
		ENTITY_RANGE = 64,
		BUF_SIZE = 200
	};
	bool _entityFlag[ENTITY_RANGE];
	bool _restrictedEntityFlag[ENTITY_RANGE];
	DynArray< char, 20 > _buffer;
};
#endif

#endif
