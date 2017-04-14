/*
 *  pict.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include "pict.hpp"
#include <vector>
#include <map>
#include <stdexcept>

#include "gfxsheets.hpp"
#include "render_shapes.hpp"
#include "render_image.hpp"
#include "render_text.hpp"
#include "tiling.hpp"
#include "location.hpp"
#include "dialog.hpp"
#include "res_image.hpp"

extern sf::Texture bg_gworld;
extern cCustomGraphics spec_scen_g;
const pic_num_t cPict::BLANK = -1;

void cPict::init(){
	drawPict()[PIC_TER] = &cPict::drawPresetTer;
	drawPict()[PIC_TER_ANIM] = &cPict::drawPresetTerAnim;
	drawPict()[PIC_MONST] = &cPict::drawPresetMonstSm;
	drawPict()[PIC_DLOG] = &cPict::drawPresetDlog;
	drawPict()[PIC_TALK] = &cPict::drawPresetTalk;
	drawPict()[PIC_SCEN] = &cPict::drawPresetScen;
	drawPict()[PIC_ITEM] = &cPict::drawPresetItem;
	drawPict()[PIC_TINY_ITEM] = &cPict::drawPresetTinyItem;
	drawPict()[PIC_PC] = &cPict::drawPresetPc;
	drawPict()[PIC_FIELD] = &cPict::drawPresetField;
	drawPict()[PIC_BOOM] = &cPict::drawPresetBoom;
	drawPict()[PIC_FULL] = &cPict::drawFullSheet;
	drawPict()[PIC_MISSILE] = &cPict::drawPresetMissile;
	drawPict()[PIC_DLOG_LG] = &cPict::drawPresetDlogLg;
	drawPict()[PIC_SCEN_LG] = &cPict::drawPresetScenLg;
	drawPict()[PIC_TER_MAP] = &cPict::drawPresetTerMap;
	drawPict()[PIC_STATUS] = &cPict::drawStatusIcon;
	drawPict()[PIC_MONST_WIDE] = &cPict::drawPresetMonstWide;
	drawPict()[PIC_MONST_TALL] = &cPict::drawPresetMonstTall;
	drawPict()[PIC_MONST_LG] = &cPict::drawPresetMonstLg;
	drawPict()[PIC_CUSTOM_TER] = &cPict::drawCustomTer;
	drawPict()[PIC_CUSTOM_TER_ANIM] = &cPict::drawCustomTerAnim;
	drawPict()[PIC_CUSTOM_MONST] = &cPict::drawCustomMonstSm;
	drawPict()[PIC_CUSTOM_DLOG] = &cPict::drawCustomDlog;
	drawPict()[PIC_CUSTOM_TALK] = &cPict::drawCustomTalk;
	drawPict()[PIC_CUSTOM_SCEN] = &cPict::drawCustomTalk;
	drawPict()[PIC_CUSTOM_ITEM] = &cPict::drawCustomItem;
	drawPict()[PIC_CUSTOM_TINY_ITEM] = &cPict::drawCustomTinyItem;
	drawPict()[PIC_CUSTOM_FULL] = &cPict::drawFullSheet;
	drawPict()[PIC_CUSTOM_BOOM] = &cPict::drawCustomBoom;
	drawPict()[PIC_CUSTOM_MISSILE] = &cPict::drawCustomMissile;
	drawPict()[PIC_CUSTOM_DLOG_LG] = &cPict::drawCustomDlogLg;
	drawPict()[PIC_CUSTOM_TER_MAP] = &cPict::drawCustomTerMap;
	drawPict()[PIC_CUSTOM_MONST_WIDE] = &cPict::drawCustomMonstWide;
	drawPict()[PIC_CUSTOM_MONST_TALL] = &cPict::drawCustomMonstTall;
	drawPict()[PIC_CUSTOM_MONST_LG] = &cPict::drawCustomMonstLg;
	drawPict()[PIC_PARTY_MONST] = &cPict::drawPartyMonstSm;
	drawPict()[PIC_PARTY_SCEN] = &cPict::drawPartyScen;
	drawPict()[PIC_PARTY_ITEM] = &cPict::drawPartyItem;
	drawPict()[PIC_PARTY_PC] = &cPict::drawPartyPc;
	drawPict()[PIC_PARTY_MONST_WIDE] = &cPict::drawPartyMonstWide;
	drawPict()[PIC_PARTY_MONST_TALL] = &cPict::drawPartyMonstTall;
	drawPict()[PIC_PARTY_MONST_LG] = &cPict::drawPartyMonstLg;
}

std::map<ePicType,void(cPict::*)(short,rectangle)>& cPict::drawPict(){
	static std::map<ePicType,void(cPict::*)(short,rectangle)> f;
	return f;
}

void cPict::setFormat(eFormat prop, short val) throw(xUnsupportedProp){
	if(prop == TXT_FRAME) drawFramed = val;
	else if(prop == TXT_FRAMESTYLE) frameStyle = eFrameStyle(val);
	else if(prop == TXT_WRAP) drawScaled = !val;
	else throw xUnsupportedProp(prop);
}

short cPict::getFormat(eFormat prop) throw(xUnsupportedProp){
	if(prop == TXT_FRAME) return drawFramed;
	else if(prop == TXT_FRAMESTYLE) return frameStyle;
	else if(prop == TXT_WRAP) return !drawScaled;
	else throw xUnsupportedProp(prop);
}

void cPict::setColour(sf::Color) throw(xUnsupportedProp) {
	// TODO: Colour is not supported
}

sf::Color cPict::getColour() throw(xUnsupportedProp) {
	// TODO: Colour is not supported
	return sf::Color();
}

void cPict::setPict(pic_num_t num, ePicType type){
	picNum = num;
	picType = type;
	if(picType == PIC_MONST && picNum < 1000) {
		if(m_pic_index[num].x == 2) picType += PIC_WIDE;
		if(m_pic_index[num].y == 2) picType += PIC_TALL;
	}
	if(picType != PIC_FULL && picNum >= 1000) {
		if(picNum >= 10000) {
			picNum -= 10000;
			picType += PIC_PARTY;
		} else {
			picType += PIC_CUSTOM;
			if(picType != PIC_CUSTOM_TER_MAP)
				picNum %= 1000;
		}
	}
	recalcRect();
}

void cPict::setPict(pic_num_t num) {
	if(picType - PIC_LARGE == PIC_MONST)
		setPict(num, PIC_MONST);
	else setPict(num, picType - PIC_CUSTOM);
}

pic_num_t cPict::getPicNum(){
	return picNum;
}

ePicType cPict::getPicType(){
	return picType;
}

cPict::cPict(cDialog& parent) :
	cControl(CTRL_PICT,parent),
	drawFramed(true) {}

cPict::cPict(sf::RenderWindow& parent) :
	cControl(CTRL_PICT, parent),
	drawFramed(true) {}

bool cPict::isClickable(){
	return haveHandler(EVT_CLICK);
}

bool cPict::isFocusable(){
	return false;
}

bool cPict::isScrollable(){
	return false;
}

ePicType operator+ (ePicType lhs, ePicTypeMod rhs){
	switch(rhs){
		case PIC_PRESET: // Adding this actually clears the custom and party modifiers
			switch(lhs){
				case PIC_CUSTOM_TER:
					return PIC_TER;
				case PIC_CUSTOM_TER_ANIM:
					return PIC_TER_ANIM;
				case PIC_CUSTOM_MONST:
				case PIC_PARTY_MONST:
					return PIC_MONST;
				case PIC_CUSTOM_DLOG:
					return PIC_DLOG;
				case PIC_CUSTOM_TALK:
					return PIC_TALK;
				case PIC_CUSTOM_SCEN:
				case PIC_PARTY_SCEN:
					return PIC_SCEN;
				case PIC_CUSTOM_ITEM:
				case PIC_PARTY_ITEM:
					return PIC_ITEM;
				case PIC_CUSTOM_TINY_ITEM:
					return PIC_TINY_ITEM;
				case PIC_CUSTOM_FULL:
					return PIC_FULL;
				case PIC_CUSTOM_MISSILE:
					return PIC_MISSILE;
				case PIC_CUSTOM_DLOG_LG:
					return PIC_DLOG_LG;
				case PIC_CUSTOM_MONST_WIDE:
				case PIC_PARTY_MONST_WIDE:
					return PIC_MONST_WIDE;
				case PIC_CUSTOM_MONST_TALL:
				case PIC_PARTY_MONST_TALL:
					return PIC_MONST_TALL;
				case PIC_CUSTOM_MONST_LG:
				case PIC_PARTY_MONST_LG:
					return PIC_MONST_LG;
				case PIC_PARTY_PC:
					return PIC_PC;
				case PIC_CUSTOM_TER_MAP:
					return PIC_TER_MAP;
				case PIC_CUSTOM_BOOM:
					return PIC_BOOM;
				default:
					return lhs;
			}
		case PIC_WIDE:
			switch(lhs){
				case PIC_MONST:
					return PIC_MONST_WIDE;
				case PIC_MONST_TALL:
					return PIC_MONST_LG;
				case PIC_CUSTOM_MONST:
					return PIC_CUSTOM_MONST_WIDE;
				case PIC_CUSTOM_MONST_TALL:
					return PIC_CUSTOM_MONST_LG;
				case PIC_PARTY_MONST:
					return PIC_PARTY_MONST_WIDE;
				case PIC_PARTY_MONST_TALL:
					return PIC_PARTY_MONST_LG;
				default:
					return lhs;
			}
		case PIC_TALL:
			switch(lhs){
				case PIC_MONST:
					return PIC_MONST_TALL;
				case PIC_MONST_WIDE:
					return PIC_MONST_LG;
				case PIC_CUSTOM_MONST:
					return PIC_CUSTOM_MONST_TALL;
				case PIC_CUSTOM_MONST_WIDE:
					return PIC_CUSTOM_MONST_LG;
				case PIC_PARTY_MONST:
					return PIC_PARTY_MONST_TALL;
				case PIC_PARTY_MONST_WIDE:
					return PIC_PARTY_MONST_LG;
				default:
					return lhs;
			}
		case PIC_LARGE:
			if(lhs == PIC_DLOG)
				return PIC_DLOG_LG;
			else if(lhs == PIC_CUSTOM_DLOG)
				return PIC_CUSTOM_DLOG_LG;
			else if(lhs == PIC_SCEN)
				return PIC_SCEN_LG;
			return (lhs + PIC_WIDE) + PIC_TALL;
		case PIC_CUSTOM:
			switch(lhs){
				case PIC_TER:
					return PIC_CUSTOM_TER;
				case PIC_TER_ANIM:
					return PIC_CUSTOM_TER_ANIM;
				case PIC_MONST:
					return PIC_CUSTOM_MONST;
				case PIC_DLOG:
					return PIC_CUSTOM_DLOG;
				case PIC_TALK:
					return PIC_CUSTOM_TALK;
				case PIC_SCEN:
					return PIC_CUSTOM_SCEN;
				case PIC_ITEM:
					return PIC_CUSTOM_ITEM;
				case PIC_TINY_ITEM:
					return PIC_CUSTOM_TINY_ITEM;
				case PIC_FULL:
					return PIC_CUSTOM_FULL;
				case PIC_BOOM:
					return PIC_CUSTOM_BOOM;
				case PIC_MISSILE:
					return PIC_CUSTOM_MISSILE;
				case PIC_DLOG_LG:
					return PIC_CUSTOM_DLOG_LG;
				case PIC_TER_MAP:
					return PIC_CUSTOM_TER_MAP;
				case PIC_MONST_WIDE:
					return PIC_CUSTOM_MONST_WIDE;
				case PIC_MONST_TALL:
					return PIC_CUSTOM_MONST_TALL;
				case PIC_MONST_LG:
					return PIC_CUSTOM_MONST_LG;
				default:
					return lhs;
			}
		case PIC_PARTY:
			switch(lhs){
				case PIC_MONST:
					return PIC_PARTY_MONST;
				case PIC_SCEN:
					return PIC_PARTY_SCEN;
				case PIC_ITEM:
					return PIC_PARTY_ITEM;
				case PIC_PC:
					return PIC_PARTY_PC;
				case PIC_MONST_WIDE:
					return PIC_PARTY_MONST_WIDE;
				case PIC_MONST_TALL:
					return PIC_PARTY_MONST_TALL;
				case PIC_MONST_LG:
					return PIC_PARTY_MONST_LG;
				default:
					return lhs;
			}
	}
	return lhs;
}

ePicType operator- (ePicType lhs, ePicTypeMod rhs){
	switch(rhs){
		case PIC_PRESET:
			return lhs;
			break;
		case PIC_WIDE:
			switch(lhs){
				case PIC_MONST_WIDE:
					return PIC_MONST;
				case PIC_MONST_LG:
					return PIC_MONST_TALL;
				case PIC_CUSTOM_MONST_WIDE:
					return PIC_CUSTOM_MONST;
				case PIC_CUSTOM_MONST_LG:
					return PIC_CUSTOM_MONST_TALL;
				case PIC_PARTY_MONST_WIDE:
					return PIC_PARTY_MONST;
				case PIC_PARTY_MONST_LG:
					return PIC_PARTY_MONST_TALL;
				default:
					return lhs;
			}
		case PIC_TALL:
			switch(lhs){
				case PIC_MONST_TALL:
					return PIC_MONST;
				case PIC_MONST_LG:
					return PIC_MONST_WIDE;
				case PIC_CUSTOM_MONST_TALL:
					return PIC_CUSTOM_MONST;
				case PIC_CUSTOM_MONST_LG:
					return PIC_CUSTOM_MONST_WIDE;
				case PIC_PARTY_MONST_TALL:
					return PIC_PARTY_MONST;
				case PIC_PARTY_MONST_LG:
					return PIC_PARTY_MONST_WIDE;
				default:
					return lhs;
			}
		case PIC_LARGE:
			if(lhs == PIC_DLOG_LG)
				return PIC_DLOG;
			else if(lhs == PIC_CUSTOM_DLOG_LG)
				return PIC_CUSTOM_DLOG_LG;
			else if(lhs == PIC_SCEN_LG)
				return PIC_SCEN;
			return (lhs - PIC_WIDE) - PIC_TALL;
		case PIC_CUSTOM:
			switch(lhs){
				case PIC_CUSTOM_TER:
					return PIC_TER;
				case PIC_CUSTOM_TER_ANIM:
					return PIC_TER_ANIM;
				case PIC_CUSTOM_MONST:
					return PIC_MONST;
				case PIC_CUSTOM_DLOG:
					return PIC_DLOG;
				case PIC_CUSTOM_TALK:
					return PIC_TALK;
				case PIC_CUSTOM_SCEN:
					return PIC_SCEN;
				case PIC_CUSTOM_ITEM:
					return PIC_ITEM;
				case PIC_CUSTOM_TINY_ITEM:
					return PIC_TINY_ITEM;
				case PIC_CUSTOM_FULL:
					return PIC_FULL;
				case PIC_CUSTOM_BOOM:
					return PIC_BOOM;
				case PIC_CUSTOM_MISSILE:
					return PIC_MISSILE;
				case PIC_CUSTOM_DLOG_LG:
					return PIC_DLOG_LG;
				case PIC_CUSTOM_TER_MAP:
					return PIC_TER_MAP;
				case PIC_CUSTOM_MONST_WIDE:
					return PIC_MONST_WIDE;
				case PIC_CUSTOM_MONST_TALL:
					return PIC_MONST_TALL;
				case PIC_CUSTOM_MONST_LG:
					return PIC_MONST_LG;
				default:
					return lhs;
			}
		case PIC_PARTY:
			switch(lhs){
				case PIC_PARTY_MONST:
					return PIC_MONST;
				case PIC_PARTY_SCEN:
					return PIC_SCEN;
				case PIC_PARTY_ITEM:
					return PIC_ITEM;
				case PIC_PARTY_PC:
					return PIC_PC;
				case PIC_PARTY_MONST_WIDE:
					return PIC_MONST_WIDE;
				case PIC_PARTY_MONST_TALL:
					return PIC_MONST_TALL;
				case PIC_PARTY_MONST_LG:
					return PIC_MONST_LG;
				default:
					return lhs;
			}
	}
	return lhs;
}

ePicType operator+ (ePicTypeMod lhs, ePicType rhs){
	return rhs + lhs;
}

ePicType operator- (ePicTypeMod lhs, ePicType rhs){
	return rhs + lhs;
}

ePicType& operator+= (ePicType& lhs, ePicTypeMod rhs){
	return lhs = lhs + rhs;
}

ePicType& operator-= (ePicType& lhs, ePicTypeMod rhs){
	return lhs = lhs - rhs;
}

bool operator& (ePicType lhs, ePicTypeMod rhs) {
	return lhs == (lhs + PIC_PRESET) + rhs;
}

short cPict::animFrame = 0;

void cPict::advanceAnim() {
	animFrame++;
	if(animFrame >= 256) animFrame = 0;
}

std::string cPict::parse(ticpp::Element& who, std::string fname) {
	using namespace ticpp;
	Iterator<Attribute> attr;
	std::string name, id;
	ePicType type;
	bool wide = false, tall = false, custom = false, tiny = false;
	bool foundTop = false, foundLeft = false, foundType = false, foundNum = false; // required attributes
	rectangle frame;
	int width = 0, height = 0;
	for(attr = attr.begin(&who); attr != attr.end(); attr++){
		attr->GetName(&name);
		if(name == "name")
			attr->GetValue(&id);
		else if(name == "type"){
			std::string val;
			foundType = true;
			attr->GetValue(&val);
			if(val == "blank"){
				setPict(cPict::BLANK, PIC_TER);
				foundNum = true;
				continue;
			}else if(val == "ter")
				type = PIC_TER;
			else if(val == "teranim")
				type = PIC_TER_ANIM;
			else if(val == "monst")
				type = PIC_MONST;
			else if(val == "dlog")
				type = PIC_DLOG;
			else if(val == "talk")
				type = PIC_TALK;
			else if(val == "scen")
				type = PIC_SCEN;
			else if(val == "item")
				type = PIC_ITEM;
			else if(val == "pc")
				type = PIC_PC;
			else if(val == "field")
				type = PIC_FIELD;
			else if(val == "boom")
				type = PIC_BOOM;
			else if(val == "missile")
				type = PIC_MISSILE;
			else if(val == "full")
				type = PIC_FULL;
			else if(val == "map")
				type = PIC_TER_MAP;
			else if(val == "status")
				type = PIC_STATUS;
			else throw xBadVal("pict",name,val,attr->Row(),attr->Column(),fname);
			if(foundNum) {
				pic_num_t wasPic = getPicNum();
				setPict(wasPic, type);
			}
		}else if(name == "custom"){
			std::string val;
			attr->GetValue(&val);
			if(val == "true") custom = true;
		}else if(name == "scaled"){
			std::string val;
			attr->GetValue(&val);
			if(val == "true") setFormat(TXT_WRAP, false);
		}else if(name == "size"){
			std::string val;
			attr->GetValue(&val);
			if(val == "wide") wide = true;
			else if(val == "tall") tall = true;
			else if(val == "large") wide = tall = true;
			else if(val == "small") tiny = true;
			else throw xBadVal("pict",name,val,attr->Row(),attr->Column(),fname);
		}else if(name == "def-key"){
			std::string val;
			attr->GetValue(&val);
			try{
				attachKey(parseKey(val));
			}catch(int){
				throw xBadVal("pict",name,val,attr->Row(),attr->Column(),fname);
			}
		}else if(name == "num"){
			pic_num_t newPic;
			attr->GetValue(&newPic), foundNum = true;
			if(foundType) setPict(newPic, type);
			else setPict(newPic);
		}else if(name == "top"){
			attr->GetValue(&frame.top), foundTop = true;
		}else if(name == "left"){
			attr->GetValue(&frame.left), foundLeft = true;
		}else if(name == "width"){
			attr->GetValue(&width);
		}else if(name == "height"){
			attr->GetValue(&height);
		}else if(name == "framed"){
			std::string val;
			attr->GetValue(&val);
			if(val == "true") setFormat(TXT_FRAME, true);
			else setFormat(TXT_FRAME, false);
		}else if(name == "outline") {
			std::string val;
			attr->GetValue(&val);
			if(val == "solid") setFormat(TXT_FRAMESTYLE, FRM_SOLID);
			else if(val == "inset") setFormat(TXT_FRAMESTYLE, FRM_INSET);
			else if(val == "outset") setFormat(TXT_FRAMESTYLE, FRM_OUTSET);
			else if(val == "double") setFormat(TXT_FRAMESTYLE, FRM_DOUBLE);
		}else throw xBadAttr("pict",name,attr->Row(),attr->Column(),fname);
	}
	if(!foundType) throw xMissingAttr("pict","type",who.Row(),who.Column(),fname);
	if(!foundNum) throw xMissingAttr("pict","num",who.Row(),who.Column(),fname);
	if(!foundTop) throw xMissingAttr("pict","top",who.Row(),who.Column(),fname);
	if(!foundLeft) throw xMissingAttr("pict","left",who.Row(),who.Column(),fname);
	if(wide || tall) {
		pic_num_t wasPic = getPicNum();
		if(wide && !tall && getPicType() == PIC_MONST) setPict(wasPic, PIC_MONST_WIDE);
		else if(!wide && tall && getPicType() == PIC_MONST) setPict(wasPic, PIC_MONST_TALL);
		else if(wide && tall){
			if(getPicType() == PIC_MONST) setPict(wasPic, PIC_MONST_LG);
			else if(getPicType() == PIC_SCEN) setPict(wasPic, PIC_SCEN_LG);
			else if(getPicType() == PIC_DLOG) setPict(wasPic, PIC_DLOG_LG);
		}
	} else if(tiny && type == PIC_ITEM) {
		pic_num_t wasPic = getPicNum();
		setPict(wasPic, PIC_TINY_ITEM);
	}
	frame.right = frame.left;
	frame.bottom = frame.top;
	if(width > 0 || height > 0 || getPicType() == PIC_FULL){
		frame.right = frame.left + width;
		frame.bottom = frame.top + height;
	}
	setBounds(frame);
	pic_num_t wasPic = getPicNum();
	if(custom) {
		setPict(wasPic, getPicType() + PIC_CUSTOM);
	} else setPict(wasPic, getPicType());
	// The above line also sets the graphic's bounding rect, if necessary
	return id;
}

void cPict::recalcRect() {
	rectangle bounds = getBounds();
	switch(picType) {
		case NUM_PIC_TYPES: break;
		case PIC_TER: case PIC_CUSTOM_TER:
		case PIC_TER_ANIM: case PIC_CUSTOM_TER_ANIM:
		case PIC_MONST: case PIC_CUSTOM_MONST: case PIC_PARTY_MONST:
		case PIC_MONST_WIDE: case PIC_CUSTOM_MONST_WIDE: case PIC_PARTY_MONST_WIDE:
		case PIC_MONST_TALL: case PIC_CUSTOM_MONST_TALL: case PIC_PARTY_MONST_TALL:
		case PIC_MONST_LG: case PIC_CUSTOM_MONST_LG: case PIC_PARTY_MONST_LG:
		case PIC_ITEM: case PIC_CUSTOM_ITEM: case PIC_PARTY_ITEM:
		case PIC_PC: case PIC_PARTY_PC:
		case PIC_FIELD:
		case PIC_BOOM: case PIC_CUSTOM_BOOM:
			bounds.width() = 28;
			bounds.height() = 36;
			break;
		case PIC_DLOG: case PIC_CUSTOM_DLOG:
			bounds.width() = 36;
			bounds.height() = 36;
			break;
		case PIC_TALK: case PIC_CUSTOM_TALK:
		case PIC_SCEN: case PIC_CUSTOM_SCEN: case PIC_PARTY_SCEN:
			bounds.width() = 32;
			bounds.height() = 32;
			break;
		case PIC_TINY_ITEM: case PIC_CUSTOM_TINY_ITEM:
		case PIC_MISSILE: case PIC_CUSTOM_MISSILE:
			bounds.width() = 18;
			bounds.height() = 18;
			break;
		case PIC_DLOG_LG: case PIC_CUSTOM_DLOG_LG:
			bounds.width() = 72;
			bounds.height() = 72;
			break;
		case PIC_SCEN_LG:
			bounds.width() = 64;
			bounds.height() = 64;
			break;
		case PIC_TER_MAP: case PIC_CUSTOM_TER_MAP:
			// These are 12x12, but in the dialog we scale them up.
			bounds.width() = 24;
			bounds.height() = 24;
			break;
		case PIC_STATUS:
			bounds.width() = 12;
			bounds.height() = 12;
			break;
		case PIC_FULL:
		case PIC_CUSTOM_FULL:
			if(drawScaled) break;
			auto sheet = getSheet(SHEET_FULL, picNum);
			sf::Vector2u sz = sheet->getSize();
			bounds.width() = sz.x;
			bounds.height() = sz.y;
			break;
	}
	setBounds(bounds);
}

std::shared_ptr<sf::Texture> cPict::getSheet(eSheetType type, size_t n) {
	std::ostringstream sout;
	switch(type) {
		case NUM_SHEET_TYPES:
			break;
		case SHEET_TER:
			sout << "ter" << n + 1;
			break;
		case SHEET_TER_ANIM:
			sout << "teranim";
			break;
		case SHEET_MONST:
			sout << "monst" << n + 1;
			break;
		case SHEET_DLOG:
			sout << "dlogpics";
			break;
		case SHEET_TALK:
			sout << "talkportraits";
			break;
		case SHEET_SCEN:
			sout << "scenpics";
			break;
		case SHEET_SCEN_LG:
			sout << "bigscenpics";
			break;
		case SHEET_ITEM:
			sout << "objects";
			break;
		case SHEET_TINY_ITEM:
			sout << "tinyobj";
			break;
		case SHEET_PC:
			sout << "pcs";
			break;
		case SHEET_FIELD:
			sout << "fields";
			break;
		case SHEET_BOOM:
			sout << "booms";
			break;
		case SHEET_MISSILE:
			sout << "missiles";
			break;
		case SHEET_PARTY:
			// TODO: Implement this
			break;
		case SHEET_HEADER:
			// TODO: Implement this
			break;
		case SHEET_TER_MAP:
			sout << "termap";
			break;
		case SHEET_STATUS:
			sout << "staticons";
			break;
		case SHEET_CUSTOM:
			// TODO: Implement this
			break;
		case SHEET_FULL:
			switch(n) {
				case 1100:
					sout << "invenhelp";
					break;
				case 1200:
					sout << "stathelp";
					break;
				case 1300:
					sout << "actionhelp";
					break;
				case 1400:
					sout << "outhelp";
					break;
				case 1401:
					sout << "fighthelp";
					break;
				case 1402:
					sout << "townhelp";
					break;
				default:
					// TODO: The scenario should be allowed to define a sheet1100.png without it being ignored in favour of invenhelp.png
					sout << "sheet" << n;
			}
	}
	return ResMgr::get<ImageRsrc>(sout.str());
}

void cPict::draw(){
	sf::Color store_color;
	rectangle rect = frame;
	inWindow->setActive();
	
	if(!visible) return;
	
	if(picNum == BLANK) // Just fill with black
		fill_rect(*inWindow, rect, sf::Color::Black);
	else (this->*drawPict()[picType])(picNum,rect);
	
	if(drawFramed) drawFrame(2,frameStyle);
}

void cPict::drawPresetTer(short num, rectangle to_rect){
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_TER, num / 50);
	if(!from_gw) return;
	num = num % 50;
	rectangle from_rect = calc_rect(num % 10, num / 10);
	if(to_rect.right - to_rect.left > 28)
		to_rect.inset(4,0);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawPresetTerAnim(short num, rectangle to_rect){
	rectangle from_rect = calc_rect(4 * (num / 5) + animFrame % 4, num % 5);
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_TER_ANIM);
	if(to_rect.right - to_rect.left > 28) {
		to_rect.inset(4,0);
		to_rect.right = to_rect.left + 28;
	}
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

static rectangle calcDefMonstRect(short i, short animFrame){
	rectangle r = calc_rect(2 * (i / 10), i % 10);
	switch(animFrame % 4){ // Sequence is right-facing, attack, left-facing, attack
		case 1:
			r.offset(112,0);
			break;
		case 2:
			r.offset(28,0);
			break;
		case 3:
			r.offset(140,0);
			break;
	}
	return r;
}

void cPict::drawPresetMonstSm(short num, rectangle to_rect){
	short m_start_pic = m_pic_index[num].i;
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_MONST, m_start_pic / 20);
	if(!from_gw) return;
	m_start_pic = m_start_pic % 20;
	rectangle from_rect = calcDefMonstRect(m_start_pic, animFrame);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawPresetMonstWide(short num, rectangle to_rect){
	rectangle small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28; to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	
	short m_start_pic = m_pic_index[num].i;
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_MONST, m_start_pic / 20);
	if(!from_gw) return;
	rectangle from_rect = calcDefMonstRect(m_start_pic % 20, animFrame);
	small_monst_rect.offset(to_rect.left,to_rect.top + 7);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	m_start_pic = m_pic_index[num].i + 1;
	from_gw = getSheet(SHEET_MONST, m_start_pic / 20);
	if(!from_gw) return;
	from_rect = calcDefMonstRect(m_start_pic % 20, animFrame);
	small_monst_rect.offset(14,0);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
}

void cPict::drawPresetMonstTall(short num, rectangle to_rect){
	rectangle small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	
	short m_start_pic = m_pic_index[num].i;
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_MONST, m_start_pic / 20);
	if(!from_gw) return;
	rectangle from_rect = calcDefMonstRect(m_start_pic % 20, animFrame);
	small_monst_rect.offset(to_rect.left + 7,to_rect.top);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	m_start_pic = m_pic_index[num].i + 1;
	from_gw = getSheet(SHEET_MONST, m_start_pic / 20);
	if(!from_gw) return;
	from_rect = calcDefMonstRect(m_start_pic % 20, animFrame);
	small_monst_rect.offset(0,18);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
}

void cPict::drawPresetMonstLg(short num, rectangle to_rect){
	rectangle small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	
	short m_start_pic = m_pic_index[num].i;
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_MONST, m_start_pic / 20);
	if(!from_gw) return;
	rectangle from_rect = calcDefMonstRect(m_start_pic % 20, animFrame);
	small_monst_rect.offset(to_rect.left,to_rect.top);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	m_start_pic = m_pic_index[num].i + 1;
	from_gw = getSheet(SHEET_MONST, m_start_pic / 20);
	if(!from_gw) return;
	from_rect = calcDefMonstRect(m_start_pic % 20, animFrame);
	small_monst_rect.offset(14,0);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	m_start_pic = m_pic_index[num].i + 2;
	from_gw = getSheet(SHEET_MONST, m_start_pic / 20);
	if(!from_gw) return;
	from_rect = calcDefMonstRect(m_start_pic % 20, animFrame);
	small_monst_rect.offset(-14,18);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	m_start_pic = m_pic_index[num].i + 3;
	from_gw = getSheet(SHEET_MONST, m_start_pic / 20);
	if(!from_gw) return;
	from_rect = calcDefMonstRect(m_start_pic % 20, animFrame);
	small_monst_rect.offset(14,0);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
}

void cPict::drawPresetDlog(short num, rectangle to_rect){
	to_rect.right = to_rect.left + 36;
	to_rect.bottom = to_rect.top + 36;
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_DLOG);
	rectangle from_rect = {0,0,36,36};
	from_rect.offset(36 * (num % 4),36 * (num / 4));
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawPresetDlogLg(short num, rectangle to_rect){
	to_rect.right = to_rect.left + (drawScaled ? getBounds().width() : 72);
	to_rect.bottom = to_rect.top + (drawScaled ? getBounds().height() : 72);
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_DLOG);
	rectangle from_rect = {0,0,72,72};
	from_rect.offset(36 * (num % 4),36 * (num / 4));
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawPresetTalk(short num, rectangle to_rect){
	to_rect.right = to_rect.left + 32;
	to_rect.bottom = to_rect.top + 32;
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_TALK);
	rectangle from_rect = {0,0,32,32};
	from_rect.offset(32 * (num % 10),32 * (num / 10));
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawPresetScen(short num, rectangle to_rect){
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_SCEN);
	rectangle from_rect = {0,0,32,32};
	from_rect.offset(32 * (num % 5),32 * (num / 5));
	to_rect.right = to_rect.left + 32;
	to_rect.bottom = to_rect.top + 32;
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawPresetScenLg(short num, rectangle to_rect){
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_SCEN_LG);
	to_rect.right = to_rect.left + (drawScaled ? getBounds().width() : 64);
	to_rect.bottom = to_rect.top + (drawScaled ? getBounds().height() : 64);
	rectangle from_rect = {0,0,64,64};
	from_rect.offset(num * 64, 0);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawPresetItem(short num, rectangle to_rect){
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	std::shared_ptr<sf::Texture> from_gw;
	rectangle from_rect = {0,0,18,18};
	if(num < 55) {
		from_gw = getSheet(SHEET_ITEM);
		from_rect = calc_rect(num % 5, num / 5);
	}else{
		from_gw = getSheet(SHEET_TINY_ITEM);
		to_rect.inset(5,9);
		from_rect.offset(18 * (num % 10), 18 * (num / 10));
	}
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawPresetTinyItem(short num, rectangle to_rect){
	to_rect.right = to_rect.left + 18;
	to_rect.bottom = to_rect.top + 18;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	std::shared_ptr<sf::Texture> from_gw;
	rectangle from_rect = {0,0,18,18};
	from_gw = getSheet(SHEET_TINY_ITEM);
	from_rect.offset(18 * (num % 10), 18 * (num / 10));
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawPresetPc(short num, rectangle to_rect){
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_PC);
	rectangle from_rect = calc_rect(2 * (num / 8), num % 8);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawPresetField(short num, rectangle to_rect){
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_FIELD);
	rectangle from_rect = calc_rect(num % 8, num / 8);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawPresetBoom(short num, rectangle to_rect){
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_BOOM);
	if(num >= 8)
		num = 8 * (num - 7) + animFrame % 8;
	rectangle from_rect = calc_rect(num % 8, num / 8);
	// TODO: Be smarter about this - we know the first row is static booms and subsequent rows are animated booms.
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawFullSheet(short num, rectangle to_rect){
	rectangle from_rect;
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_FULL, num);
	from_rect = rectangle(*from_gw);
	if(!drawScaled) {
		to_rect.right = to_rect.left + (from_rect.right - from_rect.left);
		to_rect.bottom = to_rect.top + (from_rect.bottom - from_rect.top);
	}
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawPresetMissile(short num, rectangle to_rect){
	rectangle from_rect = {0,0,18,18};
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_MISSILE);
	to_rect.right = to_rect.left + 18;
	to_rect.bottom = to_rect.top + 18;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	short i = animFrame % 8;
	from_rect.offset(18 * i, 18 * num);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawPresetTerMap(short num, rectangle to_rect){
	rectangle from_rect = {0,0,12,12};
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_TER_MAP);
	// TODO: Should probably fill black somewhere in here...?
	to_rect.right = to_rect.left + 24;
	to_rect.bottom = to_rect.top + 24;
	if(num >= 960)
		from_rect.offset(12 * 20, 12 * (num - 960));
	else from_rect.offset(12 * (num % 20), 12 * (num / 20));
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawStatusIcon(short num, rectangle to_rect){
	rectangle from_rect = {0,0,12,12};
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_STATUS);
	to_rect.right = to_rect.left + 12;
	to_rect.bottom = to_rect.top + 12;
	from_rect.offset(12 * (num % 3), 12 * (num / 3));
	if(getFormat(TXT_FRAME)) {
		rectangle pat_rect = to_rect;
		pat_rect.inset(-1,-1);
		tileImage(*inWindow, pat_rect, bg[6]);
	}
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawCustomTer(short num, rectangle to_rect){
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	rectangle from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawCustomTerAnim(short num, rectangle to_rect){
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	num += animFrame % 4;
	rectangle from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawCustomMonstSm(short num, rectangle to_rect){
	static const short adj[4] = {0, 2, 1, 3};
	num += adj[animFrame % 4];
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	
	rectangle from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawCustomMonstWide(short num, rectangle to_rect){
	static const short adj[4] = {0, 4, 2, 6};
	num += adj[animFrame % 4];
	rectangle small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	
	rectangle from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num);
	small_monst_rect.offset(to_rect.left,to_rect.top + 7);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num+1);
	small_monst_rect.offset(14,0);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
}

void cPict::drawCustomMonstTall(short num, rectangle to_rect){
	static const short adj[4] = {0, 4, 2, 6};
	num += adj[animFrame % 4];
	rectangle small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	
	rectangle from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num);
	small_monst_rect.offset(to_rect.left + 7,to_rect.top);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num+1);
	small_monst_rect.offset(0,18);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
}

void cPict::drawCustomMonstLg(short num, rectangle to_rect){
	static const short adj[4] = {0, 8, 4, 12};
	num += adj[animFrame % 4];
	rectangle small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	
	rectangle from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num);
	small_monst_rect.offset(to_rect.left,to_rect.top);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num+1);
	small_monst_rect.offset(14,0);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num+2);
	small_monst_rect.offset(-14,18);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num+3);
	small_monst_rect.offset(14,0);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
}

// This is a super-hacky way to wedge in scaled form, but at least it should work.
static int dlog_to_w = 18, dlog_to_h = 36;

void cPict::drawCustomDlog(short num, rectangle to_rect){
	rectangle from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num);
	to_rect.right = to_rect.left + dlog_to_w;
	to_rect.bottom = to_rect.top + dlog_to_h;
	from_rect.right = from_rect.left + 18;
	from_rect.bottom = from_rect.top + 36;
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num+1);
	to_rect.offset(dlog_to_w,0);
	from_rect.right = from_rect.left + 18;
	from_rect.bottom = from_rect.top + 36;
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawCustomDlogLg(short num, rectangle to_rect){
	if(drawScaled) {
		dlog_to_w = 9;
		dlog_to_h = 18;
	}
	drawCustomDlog(num,to_rect);
	to_rect.offset(dlog_to_h,0);
	drawCustomDlog(num + 2,to_rect);
	to_rect.offset(-dlog_to_h,dlog_to_h);
	drawCustomDlog(num + 4,to_rect);
	to_rect.offset(dlog_to_h,0);
	drawCustomDlog(num + 6,to_rect);
	if(drawScaled) {
		dlog_to_w = 18;
		dlog_to_h = 26;
	}
}

void cPict::drawCustomTalk(short num, rectangle to_rect){
	rectangle from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num);
	to_rect.right = to_rect.left + 16;
	to_rect.bottom = to_rect.top + 32;
	from_rect.right = from_rect.left + 16;
	from_rect.bottom = from_rect.top + 32;
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num+1);
	to_rect.offset(16,0);
	from_rect.right = from_rect.left + 16;
	from_rect.bottom = from_rect.top + 32;
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawCustomItem(short num, rectangle to_rect){
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	rectangle from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num);
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawCustomTinyItem(short num, rectangle to_rect){
	to_rect.right = to_rect.left + 18;
	to_rect.bottom = to_rect.top + 18;
	rectangle from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num);
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawCustomBoom(short num, rectangle to_rect){
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	rectangle from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num + animFrame % 8);
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawCustomMissile(short num, rectangle to_rect){
	num += animFrame % 8;
	rectangle from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num);
	from_rect.right = from_rect.left + 18;
	from_rect.bottom = from_rect.top + 18;
	if(animFrame >= 4) from_rect.offset(0, 18);
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	to_rect.inset(5,9);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawCustomTerMap(short num, rectangle to_rect){
	rectangle from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num % 1000);
	from_rect.right = from_rect.left + 12;
	from_rect.bottom = from_rect.top + 12;
	num /= 1000; num--;
	from_rect.offset((num / 3) * 12, (num % 3) * 12);
	to_rect.right = to_rect.left + 24;
	to_rect.bottom = to_rect.top + 24;
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawPartyMonstSm(short num, rectangle to_rect){
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	sf::Texture* from_gw;
	rectangle from_rect;
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num, true);
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawPartyMonstWide(short num, rectangle to_rect){
	rectangle small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	sf::Texture* from_gw;
	rectangle from_rect;
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num, true);
	small_monst_rect.offset(to_rect.left,to_rect.top + 7);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num+1, true);
	small_monst_rect.offset(14,0);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
}

void cPict::drawPartyMonstTall(short num, rectangle to_rect){
	rectangle small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	sf::Texture* from_gw;
	rectangle from_rect;
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num, true);
	small_monst_rect.offset(to_rect.left + 7,to_rect.top);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	small_monst_rect.offset(0,18);
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num+1, true);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
}

void cPict::drawPartyMonstLg(short num, rectangle to_rect){
	rectangle small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	sf::Texture* from_gw;
	rectangle from_rect;
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num, true);
	small_monst_rect.offset(to_rect.left,to_rect.top);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	small_monst_rect.offset(14,0);
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num+1, true);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	small_monst_rect.offset(-14,18);
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num+2, true);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	small_monst_rect.offset(14,0);
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num+3, true);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
}

void cPict::drawPartyScen(short num, rectangle to_rect){
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_HEADER);
	rectangle from_rect = {0,0,32,32};
	from_rect.offset(32 * (num % 5),32 * (num / 5));
	to_rect.right = to_rect.left + 32;
	to_rect.bottom = to_rect.top + 32;
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawPartyItem(short num, rectangle to_rect){
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	sf::Texture* from_gw;
	rectangle from_rect;
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num, true);
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawPartyPc(short num, rectangle to_rect){
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	sf::Texture* from_gw;
	rectangle from_rect;
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num, true);
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

cPict::~cPict() {}

void cPict::drawAt(sf::RenderWindow& win, rectangle dest, pic_num_t which_g, ePicType type_g, bool framed) {
	cPict pic(win);
	pic.frame = dest;
	pic.setPict(which_g, type_g);
	pic.setFormat(TXT_FRAME, framed);
	pic.draw();
}

cControl::storage_t cPict::store() {
	storage_t storage = cControl::store();
	storage["pic-num"] = picNum;
	storage["pic-type"] = picType;
	return storage;
}

void cPict::restore(storage_t to) {
	cControl::restore(to);
	if(to.find("pic-num") != to.end())
		picNum = boost::any_cast<pic_num_t>(to["pic-num"]);
	else picNum = -1;
	if(to.find("pic-type") != to.end())
		picType = boost::any_cast<ePicType>(to["pic-type"]);
	else picNum = PIC_DLOG;
}
