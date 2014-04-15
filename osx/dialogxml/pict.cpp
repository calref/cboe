/*
 *  pict.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include "pict.h"
#include <vector>
#include <map>
#include <stdexcept>

#include "graphtool.h"
#include "dialog.h"
#include "restypes.hpp"

extern sf::Texture bg_gworld;
extern cCustomGraphics spec_scen_g;

void cPict::init(){
	drawPict()[PIC_TER] = &cPict::drawPresetTer;
	drawPict()[PIC_TER_ANIM] = &cPict::drawPresetTerAnim;
	drawPict()[PIC_MONST] = &cPict::drawPresetMonstSm;
	drawPict()[PIC_DLOG] = &cPict::drawPresetDlog;
	drawPict()[PIC_TALK] = &cPict::drawPresetTalk;
	drawPict()[PIC_SCEN] = &cPict::drawPresetScen;
	drawPict()[PIC_ITEM] = &cPict::drawPresetItem;
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
	drawPict()[PIC_CUSTOM_FULL] = &cPict::drawFullSheet;
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

std::map<ePicType,void(cPict::*)(short,RECT)>& cPict::drawPict(){
	static std::map<ePicType,void(cPict::*)(short,RECT)> f;
	return f;
}

void cPict::attachClickHandler(click_callback_t f) throw(){
	if(f == NULL){
		onClick = NULL;
		clickable = false;
	}else{
		onClick = f;
		clickable = true;
	}
}

void cPict::attachFocusHandler(focus_callback_t f __attribute__((unused))) throw(xHandlerNotSupported){
	throw xHandlerNotSupported(true);
}

bool cPict::triggerClickHandler(cDialog& me, std::string id, eKeyMod mods, location where){
	if(onClick != NULL) return onClick(me,id,mods);
	else return false;
}

void cPict::setFormat(eFormat prop, short val) throw(xUnsupportedProp){
	if(prop == TXT_FRAME) drawFramed = val;
	else throw xUnsupportedProp(prop);
}

short cPict::getFormat(eFormat prop) throw(xUnsupportedProp){
	if(prop == TXT_FRAME) return drawFramed;
	else throw xUnsupportedProp(prop);
}

void cPict::setColour(sf::Color clr) throw(xUnsupportedProp) {
	// TODO: Colour is not supported
}

sf::Color cPict::getColour() throw(xUnsupportedProp) {
	// TODO: Colour is not supported
	return sf::Color();
}

void cPict::setPict(pic_num_t num, ePicType type){
	picNum = num;
	picType = type;
}

void cPict::setPict(pic_num_t num) {
	picNum = num;
}

pic_num_t cPict::getPicNum(){
	return picNum;
}

ePicType cPict::getPicType(){
	return picType;
}

cPict::cPict(cDialog& parent) :
	cControl(CTRL_PICT,parent),
	drawFramed(true), 
	clickable(false) {}

cPict::cPict(sf::RenderWindow& parent) :
	cControl(CTRL_PICT, parent),
	drawFramed(true),
	clickable(false) {}

bool cPict::isClickable(){
	return clickable;
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
				case PIC_FULL:
					return PIC_CUSTOM_FULL;
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
				case PIC_CUSTOM_FULL:
					return PIC_FULL;
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

short cPict::animFrame = 0;

std::shared_ptr<sf::Texture> cPict::getSheet(eSheetType type, size_t n) {
	std::ostringstream sout;
	switch(type) {
		case SHEET_TER:
			sout << "ter" << n;
			break;
		case SHEET_TER_ANIM:
			sout << "teranim";
			break;
		case SHEET_MONST:
			sout << "monst" << n;
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
				case 1500:
					sout << "staticonhelp";
					break;
			}
	}
	std::shared_ptr<sf::Texture> sheet(new sf::Texture);
	sheet->loadFromImage(*ResMgr::get<ImageRsrc>(sout.str()));
	return sheet;
}

void cPict::draw(){
	sf::Color store_color;
	RECT rect = frame;
	inWindow->setActive();
	
	if(!visible){ // Erase it
		rect.inset(-3, -3);
		tileImage(*inWindow,rect,bg_gworld,bg[parent->bg]);
		return;
	}
	if(picNum < 0) { // Just fill with black
		fill_rect(*inWindow, rect, sf::Color::Black);
		return;
	}
	
	(this->*drawPict()[picType])(picNum,rect);
	// TODO: When should we pass 1 as the second parameter?
	if(drawFramed) drawFrame(2,0);
}

void cPict::drawPresetTer(short num, RECT to_rect){
	printf("Getting terrain icon from sheet %i.\n",num / 50);
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_TER, num / 50);
	num = num % 50;
	RECT from_rect = calc_rect(num % 10, num / 10);
	if (to_rect.right - to_rect.left > 28) 
		to_rect.inset(4,0);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawPresetTerAnim(short num, RECT to_rect){
	RECT from_rect = calc_rect(4 * (num / 5) + animFrame, num % 5);
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_TER_ANIM);
	printf("Getting animated terrain graphic %i from sheet 20", num);
	if (to_rect.right - to_rect.left > 28) {
		to_rect.inset(4,0);
		to_rect.right = to_rect.left + 28;
	}
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

static RECT calcDefMonstRect(short i, short animFrame){
	RECT r = calc_rect(2 * (i / 10), i % 10);
	switch(animFrame){ // Sequence is right-facing, attack, left-facing, attack
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

void cPict::drawPresetMonstSm(short num, RECT to_rect){
	short m_start_pic = m_pic_index[num].i;
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_MONST, m_start_pic / 20);
	m_start_pic = m_start_pic % 20;
	RECT from_rect = calcDefMonstRect(m_start_pic, animFrame);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawPresetMonstWide(short num, RECT to_rect){
	RECT small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28; to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	
	short m_start_pic = m_pic_index[num].i;
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_MONST, m_start_pic / 20);
	m_start_pic = m_start_pic % 20;
	RECT from_rect = calcDefMonstRect(num, animFrame);
	small_monst_rect.offset(to_rect.left,to_rect.top + 7);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	m_start_pic = m_pic_index[num].i + 1;
	from_gw = getSheet(SHEET_MONST, m_start_pic / 20);
	m_start_pic = m_start_pic % 20;
	from_rect = calcDefMonstRect(num, animFrame);
	small_monst_rect.offset(14,0);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
}

void cPict::drawPresetMonstTall(short num, RECT to_rect){
	RECT small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	
	short m_start_pic = m_pic_index[num].i;
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_MONST, m_start_pic / 20);
	m_start_pic = m_start_pic % 20;
	RECT from_rect = calcDefMonstRect(num, animFrame);
	small_monst_rect.offset(to_rect.left + 7,to_rect.top);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	m_start_pic = m_pic_index[num].i + 1;
	from_gw = getSheet(SHEET_MONST, m_start_pic / 20);
	m_start_pic = m_start_pic % 20;
	from_rect = calcDefMonstRect(num, animFrame);
	small_monst_rect.offset(0,18);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
}

void cPict::drawPresetMonstLg(short num, RECT to_rect){
	RECT small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	
	short m_start_pic = m_pic_index[num].i;
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_MONST, m_start_pic / 20);
	m_start_pic = m_start_pic % 20;
	RECT from_rect = calcDefMonstRect(num, animFrame);
	small_monst_rect.offset(to_rect.left,to_rect.top);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	m_start_pic = m_pic_index[num].i + 1;
	from_gw = getSheet(SHEET_MONST, m_start_pic / 20);
	m_start_pic = m_start_pic % 20;
	from_rect = calcDefMonstRect(num, animFrame);
	small_monst_rect.offset(14,0);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	m_start_pic = m_pic_index[num].i + 2;
	from_gw = getSheet(SHEET_MONST, m_start_pic / 20);
	m_start_pic = m_start_pic % 20;
	from_rect = calcDefMonstRect(num, animFrame);
	small_monst_rect.offset(-14,18);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	m_start_pic = m_pic_index[num].i + 3;
	from_gw = getSheet(SHEET_MONST, m_start_pic / 20);
	m_start_pic = m_start_pic % 20;
	from_rect = calcDefMonstRect(num, animFrame);
	small_monst_rect.offset(14,0);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
}

void cPict::drawPresetDlog(short num, RECT to_rect){
	to_rect.right = to_rect.left + 36;
	to_rect.bottom = to_rect.top + 36;
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_DLOG);
	RECT from_rect = {0,0,36,36};
	from_rect.offset(36 * (num % 4),36 * (num / 4));
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawPresetDlogLg(short num, RECT to_rect){
	to_rect.right = to_rect.left + 72;
	to_rect.bottom = to_rect.top + 72;
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_DLOG);
	RECT from_rect = {0,0,72,72};
	from_rect.offset(36 * (num % 4),36 * (num / 4));
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawPresetTalk(short num, RECT to_rect){
	num--;
	to_rect.right = to_rect.left + 32;
	to_rect.bottom = to_rect.top + 32;
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_TALK);
	RECT from_rect = {0,0,32,32};
	from_rect.offset(32 * (num % 10),32 * (num / 10));
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawPresetScen(short num, RECT to_rect){
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_SCEN);
	RECT from_rect = {0,0,32,32};
	from_rect.offset(32 * (num % 5),32 * (num / 5));
	to_rect.right = to_rect.left + 32;
	to_rect.bottom = to_rect.top + 32;
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawPresetScenLg(short num, RECT to_rect){
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_SCEN_LG);
	to_rect.right = to_rect.left + 64;
	to_rect.bottom = to_rect.top + 64;
	RECT from_rect = {0,0,64,64};
	from_rect.offset(num * 64, 0);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawPresetItem(short num, RECT to_rect){
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	std::shared_ptr<sf::Texture> from_gw;
	RECT from_rect = {0,0,18,18};
	if (num < 45) {
		from_gw = getSheet(SHEET_ITEM);
		from_rect = calc_rect(num % 5, num / 5);
	}else{
		from_gw = getSheet(SHEET_TINY_ITEM);
		to_rect.inset(5,9);
		from_rect.offset(18 * (num % 10), 18 * (num / 10));
	}
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawPresetPc(short num, RECT to_rect){
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_PC);
	RECT from_rect = calc_rect(2 * (num / 8), num % 8);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawPresetField(short num, RECT to_rect){
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_FIELD);
	RECT from_rect = calc_rect(num % 8, num / 8);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawPresetBoom(short num, RECT to_rect){
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_BOOM);
	RECT from_rect = calc_rect(num % 8, num / 8);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawFullSheet(short num, RECT to_rect){
	RECT from_rect;
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_FULL, num);
	from_rect = RECT(*from_gw);
	to_rect.right = to_rect.left + (from_rect.right - from_rect.left);
	to_rect.bottom = to_rect.top + (from_rect.bottom - from_rect.top);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawPresetMissile(short num, RECT to_rect){
	RECT from_rect = {0,0,18,18};
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_MISSILE);
	to_rect.right = to_rect.left + 18;
	to_rect.bottom = to_rect.top + 18;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	short i = animFrame == 7 ? 0 : animFrame + 1;
	from_rect.offset(18 * i, 18 * num);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawPresetTerMap(short num, RECT to_rect){
	RECT from_rect = {0,0,12,12};
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_TER_MAP);
	// TODO: Should probably fill black somewhere in here...?
	to_rect.right = to_rect.left + 24;
	to_rect.bottom = to_rect.top + 24;
	from_rect.offset(12 * (num % 10), 12 * (num / 10));
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawStatusIcon(short num, RECT to_rect){
	RECT from_rect = {0,0,12,12};
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_STATUS);
	to_rect.right = to_rect.left + 12;
	to_rect.bottom = to_rect.top + 12;
	from_rect.offset(12 * (num % 3), 12 * (num / 3));
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawCustomTer(short num, RECT to_rect){
	printf("Drawing graphic %i as a custom terrain pic.\n",num);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 32;
	RECT from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawCustomTerAnim(short num, RECT to_rect){
	printf("Drawing graphic %i as a custom animated terrain pic.\n",num);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 32;
	num += animFrame;
	RECT from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawCustomMonstSm(short num, RECT to_rect){
	static const short adj[4] = {0, 2, 1, 3};
	num += adj[animFrame];
	printf("Drawing graphic %i as a custom space pic.\n",num);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 32;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	
	RECT from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawCustomMonstWide(short num, RECT to_rect){
	static const short adj[4] = {0, 4, 2, 6};
	num += adj[animFrame];
	RECT small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	
	RECT from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num);
	small_monst_rect.offset(to_rect.left,to_rect.top + 7);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num+1);
	small_monst_rect.offset(14,0);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
}

void cPict::drawCustomMonstTall(short num, RECT to_rect){
	static const short adj[4] = {0, 4, 2, 6};
	num += adj[animFrame];
	RECT small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	
	RECT from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num);
	small_monst_rect.offset(to_rect.left + 7,to_rect.top);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num+1);
	small_monst_rect.offset(0,18);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
}

void cPict::drawCustomMonstLg(short num, RECT to_rect){
	static const short adj[4] = {0, 8, 4, 12};
	num += adj[animFrame];
	RECT small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	
	RECT from_rect;
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

void cPict::drawCustomDlog(short num, RECT to_rect){
	RECT from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num);
	to_rect.right = to_rect.left + 18;
	to_rect.bottom = to_rect.top + 36;
	from_rect.right = from_rect.left + 18;
	from_rect.bottom = from_rect.top + 36;
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num+1);
	to_rect.offset(18,0);
	from_rect.right = from_rect.left + 18;
	from_rect.bottom = from_rect.top + 36;
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawCustomDlogLg(short num, RECT to_rect){
	drawCustomDlog(num,to_rect);
	to_rect.offset(36,0);
	drawCustomDlog(num + 2,to_rect);
	to_rect.offset(-36,36);
	drawCustomDlog(num + 4,to_rect);
	to_rect.offset(36,0);
	drawCustomDlog(num + 6,to_rect);
}

void cPict::drawCustomTalk(short num, RECT to_rect){
	RECT from_rect;
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

void cPict::drawCustomItem(short num, RECT to_rect){
	printf("Drawing graphic %i as a custom space pic.\n",num);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 32;
	RECT from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num);
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawCustomMissile(short num, RECT to_rect){
	num += animFrame % 4;
	RECT from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num);
	from_rect.right = from_rect.left + 18;
	from_rect.bottom = from_rect.top + 18;
	if(animFrame >= 4) from_rect.offset(0, 18);
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	to_rect.inset(5,9);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawCustomTerMap(short num, RECT to_rect){
	RECT from_rect;
	sf::Texture* from_gw;
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num);
	from_rect.right = from_rect.left + 12;
	from_rect.bottom = from_rect.top + 12;
	num /= 1000; num--;
	from_rect.offset((num / 3) * 12, (num % 3) * 12);
	to_rect.right = to_rect.left + 24;
	to_rect.bottom = to_rect.top + 24;
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawPartyMonstSm(short num, RECT to_rect){
	printf("Drawing graphic %i as a custom space pic.\n",num);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 32;
	sf::Texture* from_gw;
	RECT from_rect;
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num, true);
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawPartyMonstWide(short num, RECT to_rect){
	RECT small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	sf::Texture* from_gw;
	RECT from_rect;
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num, true);
	small_monst_rect.offset(to_rect.left,to_rect.top + 7);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num+1, true);
	small_monst_rect.offset(14,0);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
}

void cPict::drawPartyMonstTall(short num, RECT to_rect){
	RECT small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	sf::Texture* from_gw;
	RECT from_rect;
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num, true);
	small_monst_rect.offset(to_rect.left + 7,to_rect.top);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
	
	small_monst_rect.offset(0,18);
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num+1, true);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, small_monst_rect, sf::BlendAlpha);
}

void cPict::drawPartyMonstLg(short num, RECT to_rect){
	RECT small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	sf::Texture* from_gw;
	RECT from_rect;
	
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

void cPict::drawPartyScen(short num, RECT to_rect){
	std::shared_ptr<sf::Texture> from_gw = getSheet(SHEET_HEADER);
	RECT from_rect = {0,0,32,32};
	from_rect.offset(32 * (num % 5),32 * (num / 5));
	to_rect.right = to_rect.left + 32;
	to_rect.bottom = to_rect.top + 32;
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect);
}

void cPict::drawPartyItem(short num, RECT to_rect){
	printf("Drawing graphic %i as a custom space pic.\n",num);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 32;
	sf::Texture* from_gw;
	RECT from_rect;
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num, true);
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

void cPict::drawPartyPc(short num, RECT to_rect){
	printf("Drawing graphic %i as a custom space pic.\n",num);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 32;
	sf::Texture* from_gw;
	RECT from_rect;
	
	graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(num, true);
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, sf::BlendAlpha);
}

cPict::~cPict() {}
