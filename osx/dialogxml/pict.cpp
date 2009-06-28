/*
 *  pict.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include <Carbon/Carbon.h>
#include <vector>
#include <map>
#include <stdexcept>

#include "graphtool.h"
#include "dialog.h"

extern GWorldPtr bg_gworld;

void cPict::init(){
	// TODO: Load or otherwise acquire the various GWorlds.
	teranim = dlog = talk = scen = largeScen = item = tinyItem = pc = field = boom = missile = save = header = map = status = NULL;
	drawPict()[PIC_TER] = drawPresetTer;
	drawPict()[PIC_TER_ANIM] = drawPresetTerAnim;
	drawPict()[PIC_MONST] = drawPresetMonstSm;
	drawPict()[PIC_DLOG] = drawPresetDlog;
	drawPict()[PIC_TALK] = drawPresetTalk;
	drawPict()[PIC_SCEN] = drawPresetScen;
	drawPict()[PIC_ITEM] = drawPresetItem;
	drawPict()[PIC_PC] = drawPresetPc;
	drawPict()[PIC_FIELD] = drawPresetField;
	drawPict()[PIC_BOOM] = drawPresetBoom;
	drawPict()[PIC_FULL] = drawFullSheet;
	drawPict()[PIC_MISSILE] = drawPresetMissile;
	drawPict()[PIC_DLOG_LG] = drawPresetDlogLg;
	drawPict()[PIC_SCEN_LG] = drawPresetScenLg;
	drawPict()[PIC_TER_MAP] = drawPresetTerMap;
	drawPict()[PIC_STATUS] = drawStatusIcon;
	drawPict()[PIC_MONST_WIDE] = drawPresetMonstWide;
	drawPict()[PIC_MONST_TALL] = drawPresetMonstTall;
	drawPict()[PIC_MONST_LG] = drawPresetMonstLg;
	drawPict()[PIC_CUSTOM_TER] = drawCustomTer;
	drawPict()[PIC_CUSTOM_TER_ANIM] = drawCustomTerAnim;
	drawPict()[PIC_CUSTOM_MONST] = drawCustomMonstSm;
	drawPict()[PIC_CUSTOM_DLOG] = drawCustomDlog;
	drawPict()[PIC_CUSTOM_TALK] = drawCustomTalk;
	drawPict()[PIC_CUSTOM_SCEN] = drawCustomTalk;
	drawPict()[PIC_CUSTOM_ITEM] = drawCustomItem;
	drawPict()[PIC_CUSTOM_FULL] = drawFullSheet;
	drawPict()[PIC_CUSTOM_MISSILE] = drawCustomMissile;
	drawPict()[PIC_CUSTOM_DLOG_LG] = drawCustomDlogLg;
	drawPict()[PIC_CUSTOM_TER_MAP] = drawCustomTerMap;
	drawPict()[PIC_CUSTOM_MONST_WIDE] = drawCustomMonstWide;
	drawPict()[PIC_CUSTOM_MONST_TALL] = drawCustomMonstTall;
	drawPict()[PIC_CUSTOM_MONST_LG] = drawCustomMonstLg;
	drawPict()[PIC_PARTY_MONST] = drawPartyMonstSm;
	drawPict()[PIC_PARTY_SCEN] = drawPartyScen;
	drawPict()[PIC_PARTY_ITEM] = drawPartyItem;
	drawPict()[PIC_PARTY_PC] = drawPartyPc;
	drawPict()[PIC_PARTY_MONST_WIDE] = drawPartyMonstWide;
	drawPict()[PIC_PARTY_MONST_TALL] = drawPartyMonstTall;
	drawPict()[PIC_PARTY_MONST_LG] = drawPartyMonstLg;
}

std::map<ePicType,void(*)(short,Rect)>& cPict::drawPict(){
	static std::map<ePicType,void(*)(short,Rect)> f;
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

bool cPict::triggerClickHandler(cDialog& me, std::string id, eKeyMod mods, Point where){
	if(onClick != NULL) return onClick(me,id,mods);
	else return false;
}

void cPict::setFormat(eFormat prop, short val) throw(xUnsupportedProp){
	if(prop == TXT_FRAME) drawFramed = val;
	else throw xUnsupportedProp(prop);
	if(isVisible()) draw();
}

short cPict::getFormat(eFormat prop) throw(xUnsupportedProp){
	if(prop == TXT_FRAME) return drawFramed;
	else throw xUnsupportedProp(prop);
}

void cPict::setSheet(eSheetType type, GWorldPtr sheet, short n){
	// Step one: verify
	if(n < -1) throw std::out_of_range("Negative values for n, other than -1, are invalid.");
	if(n == -1 && type == SHEET_FULL)
		throw std::out_of_range("The value -1 for n is not valid in conjunction with SHEET_FULL.");
	// Step two: set
	size_t numSheets, num;
	switch(type){
		case SHEET_TER:
			numSheets = ter.size();
			if(n == -1) num = numSheets; else num = n;
			if(num < numSheets) ter[num] = sheet;
			else if(num == numSheets) ter.push_back(sheet);
			else{
				ter.reserve(num+1);
				while(num > ter.size()) ter.push_back(NULL);
				ter.push_back(sheet);
			}
			break;
		case SHEET_TER_ANIM:
			teranim = sheet;
			break;
		case SHEET_MONST:
			numSheets = monst.size();
			if(n == -1) num = numSheets; else num = n;
			if(num < numSheets) monst[num] = sheet;
			else if(num == numSheets) monst.push_back(sheet);
			else{
				monst.reserve(num+1);
				while(num > monst.size()) monst.push_back(NULL);
				monst.push_back(sheet);
			}
			break;
		case SHEET_DLOG:
			dlog = sheet;
			break;
		case SHEET_TALK:
			talk = sheet;
			break;
		case SHEET_SCEN:
			scen = sheet;
			break;
		case SHEET_ITEM:
			item = sheet;
			break;
		case SHEET_TINY_ITEM:
			tinyItem = sheet;
			break;
		case SHEET_PC:
			pc = sheet;
			break;
		case SHEET_FIELD:
			field = sheet;
			break;
		case SHEET_BOOM:
			boom = sheet;
			break;
		case SHEET_FULL:
			largeSheets[n] = sheet;
			break;
		case SHEET_MISSILE:
			missile = sheet;
			break;
		case SHEET_SCEN_LG:
			largeScen = sheet;
			break;
		case SHEET_TER_MAP:
			map = sheet;
			break;
		case SHEET_CUSTOM:
			numSheets = customSheets.size();
			if(n == -1) num = numSheets; else num = n;
			if(num < numSheets) customSheets[num] = sheet;
			else if(num == numSheets) customSheets.push_back(sheet);
			else{
				customSheets.reserve(num+1);
				while(num > customSheets.size()) customSheets.push_back(NULL);
				customSheets.push_back(sheet);
			}
			break;
		case SHEET_PARTY:
			save = sheet;
			break;
		case SHEET_HEADER:
			header = sheet;
			break;
		case SHEET_STATUS:
			status = sheet;
			break;
	}
}

bool cPict::isSheetSet(eSheetType type, size_t num){
	switch(type){
		case SHEET_TER:
			if(ter.size() <= num) return false;
			else return ter[num];
		case SHEET_TER_ANIM:
			return teranim;
		case SHEET_MONST:
			if(monst.size() <= num) return false;
			else return monst[num];
		case SHEET_DLOG:
			return dlog;
		case SHEET_TALK:
			return talk;
		case SHEET_SCEN:
			return scen;
		case SHEET_SCEN_LG:
			return largeScen;
		case SHEET_ITEM:
			return item;
		case SHEET_TINY_ITEM:
			return tinyItem;
		case SHEET_PC:
			return pc;
		case SHEET_FIELD:
			return field;
		case SHEET_BOOM:
			return boom;
		case SHEET_MISSILE:
			return missile;
		case SHEET_PARTY:
			return save;
		case SHEET_TER_MAP:
			return map;
		case SHEET_FULL:
			if(largeSheets.size() <= num) return false;
			else return largeSheets[num];
		case SHEET_CUSTOM:
			if(customSheets.size() <= num) return false;
			else return customSheets[num];
		case SHEET_HEADER:
			return header;
		case SHEET_STATUS:
			return status;
	}
	return false;
}

void cPict::setPict(pic_num_t num, ePicType type){
	picNum = num;
	picType = type;
	if(isVisible()) draw();
}

pic_num_t cPict::getPicNum(){
	return picNum;
}

ePicType cPict::getPicType(){
	return picType;
}

cPict::cPict(cDialog* parent) :
	cControl(parent,CTRL_PICT),
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

GWorldPtr cPict::teranim = NULL, cPict::dlog = NULL, cPict::talk = NULL, cPict::scen = NULL, cPict::largeScen = NULL;
GWorldPtr cPict::item = NULL, cPict::tinyItem = NULL, cPict::pc = NULL, cPict::field = NULL, cPict::boom = NULL;
GWorldPtr cPict::missile = NULL, cPict::save = NULL, cPict::header = NULL, cPict::map = NULL, cPict::status = NULL;
std::vector<GWorldPtr> cPict::ter, cPict::monst, cPict::customSheets;
std::map<size_t,GWorldPtr> cPict::largeSheets;
//std::map<ePicType,void(*)(short,GWorldPtr,Rect)> cPict::drawPict();
//void(* cPict::drawPict()[NUM_PIC_TYPES])(short,GWorldPtr,Rect);
short cPict::animFrame = 0;

void cPict::draw(){
	RGBColor store_color;
	Rect rect = frame;
	GrafPtr cur_port;
	GetPort(&cur_port);
	SetPortWindowPort(parent->win);
	
	if(!visible){ // Erase it
		InsetRect(&rect, -3, -3);
		tileImage(rect,bg_gworld,bg[parent->bg]);
		return;
	}
	if(picNum < 0) { // Just fill with black
		GetForeColor(&store_color);
		ForeColor(blackColor);
		PaintRect(&rect);
		RGBForeColor(&store_color);
		return;
	}
	GetBackColor(&store_color);
	BackColor(whiteColor);
	
	drawPict()[picType](picNum,rect);
	if(drawFramed) drawFrame(2,0);
	SetPort(cur_port);
}

void cPict::drawPresetTer(short num, Rect to_rect){
	printf("Getting terrain icon from sheet %i.\n",num / 50);
	if(!isSheetSet(SHEET_TER,num / 50)) throw xMissingSheet(SHEET_TER,num / 50);
	GWorldPtr from_gw = ter[num / 50];
	num = num % 50;
	Rect from_rect = calc_rect(num % 10, num / 10);
	if (to_rect.right - to_rect.left > 28) 
		InsetRect(&to_rect,4,0);
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0});
}

void cPict::drawPresetTerAnim(short num, Rect to_rect){
	Rect from_rect = calc_rect(4 * (num / 5) + animFrame, num % 5);
	if(!isSheetSet(SHEET_TER_ANIM)) throw xMissingSheet(SHEET_TER_ANIM);
	GWorldPtr from_gw = teranim;
	printf("Getting animated terrain graphic %i from sheet 20", num);
	if (to_rect.right - to_rect.left > 28) {
		InsetRect(&to_rect,4,0);
		to_rect.right = to_rect.left + 28;
	}
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0});
}

static Rect calcDefMonstRect(short i, short animFrame){
	Rect r = calc_rect(2 * (i / 10), i % 10);
	switch(animFrame){ // Sequence is right-facing, attack, left-facing, attack
		case 1:
			OffsetRect(&r,112,0);
			break;
		case 2:
			OffsetRect(&r,28,0);
			break;
		case 3:
			OffsetRect(&r,140,0);
			break;
	}
	return r;
}

void cPict::drawPresetMonstSm(short num, Rect to_rect){
	short m_start_pic = m_pic_index[num].i;
	if(!isSheetSet(SHEET_MONST,m_start_pic / 20)) throw xMissingSheet(SHEET_MONST,m_start_pic / 20);
	GWorldPtr from_gw = monst[m_start_pic / 20];
	m_start_pic = m_start_pic % 20;
	Rect from_rect = calcDefMonstRect(m_start_pic, animFrame);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	PaintRect(&to_rect);
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0}, transparent);
}

void cPict::drawPresetMonstWide(short num, Rect to_rect){
	Rect small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28; to_rect.bottom = to_rect.top + 36;
	PaintRect(&to_rect);
	
	short m_start_pic = m_pic_index[num].i;
	if(!isSheetSet(SHEET_MONST,m_start_pic / 20)) throw xMissingSheet(SHEET_MONST,m_start_pic / 20);
	GWorldPtr from_gw = monst[m_start_pic / 20];
	m_start_pic = m_start_pic % 20;
	Rect from_rect = calcDefMonstRect(num, animFrame);
	OffsetRect(&small_monst_rect,to_rect.left,to_rect.top + 7);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
	
	m_start_pic = m_pic_index[num].i + 1;
	if(!isSheetSet(SHEET_MONST,m_start_pic / 20)) throw xMissingSheet(SHEET_MONST,m_start_pic / 20);
	from_gw = monst[m_start_pic / 20];
	m_start_pic = m_start_pic % 20;
	from_rect = calcDefMonstRect(num, animFrame);
	OffsetRect(&small_monst_rect,14,0);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
}

void cPict::drawPresetMonstTall(short num, Rect to_rect){
	Rect small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	PaintRect(&to_rect);
	
	short m_start_pic = m_pic_index[num].i;
	if(!isSheetSet(SHEET_MONST,m_start_pic / 20)) throw xMissingSheet(SHEET_MONST,m_start_pic / 20);
	GWorldPtr from_gw = monst[m_start_pic / 20];
	m_start_pic = m_start_pic % 20;
	Rect from_rect = calcDefMonstRect(num, animFrame);
	OffsetRect(&small_monst_rect,to_rect.left + 7,to_rect.top);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
	
	m_start_pic = m_pic_index[num].i + 1;
	if(!isSheetSet(SHEET_MONST,m_start_pic / 20)) throw xMissingSheet(SHEET_MONST,m_start_pic / 20);
	from_gw = monst[m_start_pic / 20];
	m_start_pic = m_start_pic % 20;
	from_rect = calcDefMonstRect(num, animFrame);
	OffsetRect(&small_monst_rect,0,18);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
}

void cPict::drawPresetMonstLg(short num, Rect to_rect){
	Rect small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	PaintRect(&to_rect);
	
	short m_start_pic = m_pic_index[num].i;
	if(!isSheetSet(SHEET_MONST,m_start_pic / 20)) throw xMissingSheet(SHEET_MONST,m_start_pic / 20);
	GWorldPtr from_gw = monst[m_start_pic / 20];
	m_start_pic = m_start_pic % 20;
	Rect from_rect = calcDefMonstRect(num, animFrame);
	OffsetRect(&small_monst_rect,to_rect.left,to_rect.top);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
	
	m_start_pic = m_pic_index[num].i + 1;
	if(!isSheetSet(SHEET_MONST,m_start_pic / 20)) throw xMissingSheet(SHEET_MONST,m_start_pic / 20);
	from_gw = monst[m_start_pic / 20];
	m_start_pic = m_start_pic % 20;
	from_rect = calcDefMonstRect(num, animFrame);
	OffsetRect(&small_monst_rect,14,0);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
	
	m_start_pic = m_pic_index[num].i + 2;
	if(!isSheetSet(SHEET_MONST,m_start_pic / 20)) throw xMissingSheet(SHEET_MONST,m_start_pic / 20);
	from_gw = monst[m_start_pic / 20];
	m_start_pic = m_start_pic % 20;
	from_rect = calcDefMonstRect(num, animFrame);
	OffsetRect(&small_monst_rect,-14,18);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
	
	m_start_pic = m_pic_index[num].i + 3;
	if(!isSheetSet(SHEET_MONST,m_start_pic / 20)) throw xMissingSheet(SHEET_MONST,m_start_pic / 20);
	from_gw = monst[m_start_pic / 20];
	m_start_pic = m_start_pic % 20;
	from_rect = calcDefMonstRect(num, animFrame);
	OffsetRect(&small_monst_rect,14,0);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
}

void cPict::drawPresetDlog(short num, Rect to_rect){
	to_rect.right = to_rect.left + 36;
	to_rect.bottom = to_rect.top + 36;
	if(!isSheetSet(SHEET_DLOG)) throw xMissingSheet(SHEET_DLOG);
	GWorldPtr from_gw = dlog;
	Rect from_rect = {0,0,36,36};
	OffsetRect(&from_rect,36 * (num % 4),36 * (num / 4));
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0});
}

void cPict::drawPresetDlogLg(short num, Rect to_rect){
	to_rect.right = to_rect.left + 72;
	to_rect.bottom = to_rect.top + 72;
	if(!isSheetSet(SHEET_DLOG)) throw xMissingSheet(SHEET_DLOG);
	GWorldPtr from_gw = dlog;
	Rect from_rect = {0,0,72,72};
	OffsetRect(&from_rect,36 * (num % 4),36 * (num / 4));
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0});
}

void cPict::drawPresetTalk(short num, Rect to_rect){
	num--;
	to_rect.right = to_rect.left + 32;
	to_rect.bottom = to_rect.top + 32;
	if(!isSheetSet(SHEET_TALK)) throw xMissingSheet(SHEET_TALK);
	GWorldPtr from_gw = talk;
	Rect from_rect = {0,0,32,32};
	OffsetRect(&from_rect,32 * (num % 10),32 * (num / 10));
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0});
}

void cPict::drawPresetScen(short num, Rect to_rect){
	if(!isSheetSet(SHEET_SCEN)) throw xMissingSheet(SHEET_SCEN);
	GWorldPtr from_gw = scen;
	Rect from_rect = {0,0,32,32};
	OffsetRect(&from_rect,32 * (num % 5),32 * (num / 5));
	to_rect.right = to_rect.left + 32;
	to_rect.bottom = to_rect.top + 32;
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0});
}

void cPict::drawPresetScenLg(short num, Rect to_rect){
	if(!isSheetSet(SHEET_SCEN_LG)) throw xMissingSheet(SHEET_SCEN_LG);
	GWorldPtr from_gw = largeScen;
	to_rect.right = to_rect.left + 64;
	to_rect.bottom = to_rect.top + 64;
	Rect from_rect = {0,0,64,64};
	OffsetRect(&from_rect, num * 64, 0);
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0});
}

void cPict::drawPresetItem(short num, Rect to_rect){
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	PaintRect(&to_rect);
	GWorldPtr from_gw;
	Rect from_rect = {0,0,18,18};
	if (num < 45) {
		if(!isSheetSet(SHEET_ITEM)) throw xMissingSheet(SHEET_ITEM);
		from_gw = item;
		from_rect = calc_rect(num % 5, num / 5);
	}else{
		if(!isSheetSet(SHEET_TINY_ITEM)) throw xMissingSheet(SHEET_TINY_ITEM);
		from_gw = tinyItem;
		InsetRect(&to_rect,5,9);
		OffsetRect(&from_rect,18 * (num % 10), 18 * (num / 10));
	}
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0}, transparent);
}

void cPict::drawPresetPc(short num, Rect to_rect){
	if(!isSheetSet(SHEET_PC)) throw xMissingSheet(SHEET_PC);
	GWorldPtr from_gw = pc;
	Rect from_rect = calc_rect(2 * (num / 8), num % 8);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	PaintRect(&to_rect);
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0}, transparent);
}

void cPict::drawPresetField(short num, Rect to_rect){
	if(!isSheetSet(SHEET_FIELD)) throw xMissingSheet(SHEET_FIELD);
	GWorldPtr from_gw = field;
	Rect from_rect = calc_rect(num % 8, num / 8);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	PaintRect(&to_rect);
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0}, transparent);
}

void cPict::drawPresetBoom(short num, Rect to_rect){
	if(!isSheetSet(SHEET_BOOM)) throw xMissingSheet(SHEET_BOOM);
	GWorldPtr from_gw = boom;
	Rect from_rect = calc_rect(num % 8, num / 8);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	PaintRect(&to_rect);
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0}, transparent);
}

void cPict::drawFullSheet(short num, Rect to_rect){
	Rect from_rect;
	if(!isSheetSet(SHEET_FULL,num)) throw xMissingSheet(SHEET_FULL,num);
	GWorldPtr from_gw = largeSheets[num];
	GetPortBounds(from_gw, &from_rect);
	to_rect.right = to_rect.left + (from_rect.right - from_rect.left);
	to_rect.bottom = to_rect.top + (from_rect.bottom - from_rect.top);
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0});
}

void cPict::drawPresetMissile(short num, Rect to_rect){
	Rect from_rect = {0,0,18,18};
	if(!isSheetSet(SHEET_MISSILE)) throw xMissingSheet(SHEET_MISSILE);
	GWorldPtr from_gw = missile;
	to_rect.right = to_rect.left + 18;
	to_rect.bottom = to_rect.top + 18;
	PaintRect(&to_rect);
	short i = animFrame == 7 ? 0 : animFrame + 1;
	OffsetRect(&from_rect,18 * i, 18 * num);
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0}, transparent);
}

void cPict::drawPresetTerMap(short num, Rect to_rect){
	Rect from_rect = {0,0,12,12};
	if(!isSheetSet(SHEET_TER_MAP)) throw xMissingSheet(SHEET_TER_MAP);
	GWorldPtr from_gw = map;
	to_rect.right = to_rect.left + 24;
	to_rect.bottom = to_rect.top + 24;
	OffsetRect(&from_rect,12 * (num % 10), 12 * (num / 10));
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0});
}

void cPict::drawStatusIcon(short num, Rect to_rect){
	Rect from_rect = {0,0,12,12};
	if(!isSheetSet(SHEET_STATUS)) throw xMissingSheet(SHEET_STATUS);
	GWorldPtr from_gw = status;
	to_rect.right = to_rect.left + 12;
	to_rect.bottom = to_rect.top + 12;
	OffsetRect(&from_rect,12 * (num % 3), 12 * (num / 3));
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0}, transparent);
}

void cPict::drawCustomTer(short num, Rect to_rect){
	printf("Drawing graphic %i as a custom terrain pic.\n",num);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 32;
	Rect from_rect;
	short n = get_custom_rect(num,from_rect);
	if(!isSheetSet(SHEET_CUSTOM,n)) throw xMissingSheet(SHEET_CUSTOM,n);
	GWorldPtr from_gw = customSheets[n];
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0});
}

void cPict::drawCustomTerAnim(short num, Rect to_rect){
	printf("Drawing graphic %i as a custom animated terrain pic.\n",num);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 32;
	num += animFrame;
	Rect from_rect;
	short n = get_custom_rect(num,from_rect);
	if(!isSheetSet(SHEET_CUSTOM,n)) throw xMissingSheet(SHEET_CUSTOM,n);
	GWorldPtr from_gw = customSheets[n];
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0});
}

void cPict::drawCustomMonstSm(short num, Rect to_rect){
	static const short adj[4] = {0, 2, 1, 3};
	num += adj[animFrame];
	printf("Drawing graphic %i as a custom space pic.\n",num);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 32;
	PaintRect(&to_rect);
	
	Rect from_rect;
	short n = get_custom_rect(num,from_rect);
	if(!isSheetSet(SHEET_CUSTOM,n)) throw xMissingSheet(SHEET_CUSTOM,n);
	GWorldPtr from_gw = customSheets[n];
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0}, transparent);
}

void cPict::drawCustomMonstWide(short num, Rect to_rect){
	static const short adj[4] = {0, 4, 2, 6};
	num += adj[animFrame];
	Rect small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	PaintRect(&to_rect);
	
	Rect from_rect;
	short n = get_custom_rect(num,from_rect);
	if(!isSheetSet(SHEET_CUSTOM,n)) throw xMissingSheet(SHEET_CUSTOM,n);
	GWorldPtr from_gw = customSheets[n];
	OffsetRect(&small_monst_rect,to_rect.left,to_rect.top + 7);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
	
	n = get_custom_rect(num,from_rect);
	if(!isSheetSet(SHEET_CUSTOM,n)) throw xMissingSheet(SHEET_CUSTOM,n);
	from_gw = customSheets[n];
	OffsetRect(&small_monst_rect,14,0);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
}

void cPict::drawCustomMonstTall(short num, Rect to_rect){
	static const short adj[4] = {0, 4, 2, 6};
	num += adj[animFrame];
	Rect small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	PaintRect(&to_rect);
	
	Rect from_rect;
	short n = get_custom_rect(num,from_rect);
	if(!isSheetSet(SHEET_CUSTOM,n)) throw xMissingSheet(SHEET_CUSTOM,n);
	GWorldPtr from_gw = customSheets[n];
	OffsetRect(&small_monst_rect,to_rect.left + 7,to_rect.top);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
	
	n = get_custom_rect(num,from_rect);
	if(!isSheetSet(SHEET_CUSTOM,n)) throw xMissingSheet(SHEET_CUSTOM,n);
	from_gw = customSheets[n];
	OffsetRect(&small_monst_rect,0,18);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
}

void cPict::drawCustomMonstLg(short num, Rect to_rect){
	static const short adj[4] = {0, 8, 4, 12};
	num += adj[animFrame];
	Rect small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	PaintRect(&to_rect);
	
	Rect from_rect;
	short n = get_custom_rect(num,from_rect);
	if(!isSheetSet(SHEET_CUSTOM,n)) throw xMissingSheet(SHEET_CUSTOM,n);
	GWorldPtr from_gw = customSheets[n];
	OffsetRect(&small_monst_rect,to_rect.left,to_rect.top);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
	
	n = get_custom_rect(num,from_rect);
	if(!isSheetSet(SHEET_CUSTOM,n)) throw xMissingSheet(SHEET_CUSTOM,n);
	from_gw = customSheets[n];
	OffsetRect(&small_monst_rect,14,0);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
	
	n = get_custom_rect(num,from_rect);
	if(!isSheetSet(SHEET_CUSTOM,n)) throw xMissingSheet(SHEET_CUSTOM,n);
	from_gw = customSheets[n];
	OffsetRect(&small_monst_rect,-14,18);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
	
	n = get_custom_rect(num,from_rect);
	if(!isSheetSet(SHEET_CUSTOM,n)) throw xMissingSheet(SHEET_CUSTOM,n);
	from_gw = customSheets[n];
	OffsetRect(&small_monst_rect,14,0);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
}

void cPict::drawCustomDlog(short num, Rect to_rect){
	Rect from_rect;
	short n = get_custom_rect(num,from_rect);
	if(!isSheetSet(SHEET_CUSTOM,n)) throw xMissingSheet(SHEET_CUSTOM,n);
	GWorldPtr from_gw = customSheets[n];
	to_rect.right = to_rect.left + 18;
	to_rect.bottom = to_rect.top + 36;
	from_rect.right = from_rect.left + 18;
	from_rect.bottom = from_rect.top + 36;
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0});
	
	n = get_custom_rect(num,from_rect);
	if(!isSheetSet(SHEET_CUSTOM,n)) throw xMissingSheet(SHEET_CUSTOM,n);
	from_gw = customSheets[n];
	OffsetRect(&to_rect,18,0);
	from_rect.right = from_rect.left + 18;
	from_rect.bottom = from_rect.top + 36;
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0});
}

void cPict::drawCustomDlogLg(short num, Rect to_rect){
	drawCustomDlog(num,to_rect);
	OffsetRect(&to_rect,36,0);
	drawCustomDlog(num + 2,to_rect);
	OffsetRect(&to_rect,-36,36);
	drawCustomDlog(num + 4,to_rect);
	OffsetRect(&to_rect,36,0);
	drawCustomDlog(num + 6,to_rect);
}

void cPict::drawCustomTalk(short num, Rect to_rect){
	Rect from_rect;
	short n = get_custom_rect(num,from_rect);
	if(!isSheetSet(SHEET_CUSTOM,n)) throw xMissingSheet(SHEET_CUSTOM,n);
	GWorldPtr from_gw = customSheets[n];
	to_rect.right = to_rect.left + 16;
	to_rect.bottom = to_rect.top + 32;
	from_rect.right = from_rect.left + 16;
	from_rect.bottom = from_rect.top + 32;
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0});
	
	n = get_custom_rect(num,from_rect);
	if(!isSheetSet(SHEET_CUSTOM,n)) throw xMissingSheet(SHEET_CUSTOM,n);
	from_gw = customSheets[n];
	OffsetRect(&to_rect,16,0);
	from_rect.right = from_rect.left + 16;
	from_rect.bottom = from_rect.top + 32;
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0});
}

void cPict::drawCustomItem(short num, Rect to_rect){
	printf("Drawing graphic %i as a custom space pic.\n",num);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 32;
	Rect from_rect;
	short n = get_custom_rect(num,from_rect);
	if(!isSheetSet(SHEET_CUSTOM,n)) throw xMissingSheet(SHEET_CUSTOM,n);
	GWorldPtr from_gw = customSheets[n];
	PaintRect(&to_rect);
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0}, transparent);
}

void cPict::drawCustomMissile(short num, Rect to_rect){
	num += animFrame % 4;
	Rect from_rect;
	short n = get_custom_rect(num,from_rect);
	if(!isSheetSet(SHEET_CUSTOM,n)) throw xMissingSheet(SHEET_CUSTOM,n);
	GWorldPtr from_gw = customSheets[n];
	from_rect.right = from_rect.left + 18;
	from_rect.bottom = from_rect.top + 18;
	if(animFrame >= 4) OffsetRect(&from_rect, 0, 18);
	PaintRect(&to_rect);
	InsetRect(&to_rect,5,9);
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0}, transparent);
}

void cPict::drawCustomTerMap(short num, Rect to_rect){
	Rect from_rect;
	short n = get_custom_rect(num,from_rect);
	if(!isSheetSet(SHEET_CUSTOM,n)) throw xMissingSheet(SHEET_CUSTOM,n);
	GWorldPtr from_gw = customSheets[n];
	from_rect.right = from_rect.left + 12;
	from_rect.bottom = from_rect.top + 12;
	num /= 1000; num--;
	OffsetRect(&from_rect, (num / 3) * 12, (num % 3) * 12);
	to_rect.right = to_rect.left + 24;
	to_rect.bottom = to_rect.top + 24;
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0});
}

void cPict::drawPartyMonstSm(short num, Rect to_rect){
	printf("Drawing graphic %i as a custom space pic.\n",num);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 32;
	if(!isSheetSet(SHEET_PARTY)) throw xMissingSheet(SHEET_PARTY);
	GWorldPtr from_gw = save;
	Rect from_rect = get_custom_rect(num);
	PaintRect(&to_rect);
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0}, transparent);
}

void cPict::drawPartyMonstWide(short num, Rect to_rect){
	Rect small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	PaintRect(&to_rect);
	if(!isSheetSet(SHEET_PARTY)) throw xMissingSheet(SHEET_PARTY);
	GWorldPtr from_gw = save;
	
	Rect from_rect = get_custom_rect(num);
	OffsetRect(&small_monst_rect,to_rect.left,to_rect.top + 7);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
	
	from_rect = get_custom_rect(num+1);
	OffsetRect(&small_monst_rect,14,0);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
}

void cPict::drawPartyMonstTall(short num, Rect to_rect){
	Rect small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	PaintRect(&to_rect);
	if(!isSheetSet(SHEET_PARTY)) throw xMissingSheet(SHEET_PARTY);
	GWorldPtr from_gw = save;
	
	Rect from_rect = get_custom_rect(num);
	OffsetRect(&small_monst_rect,to_rect.left + 7,to_rect.top);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
	
	OffsetRect(&small_monst_rect,0,18);
	from_rect = get_custom_rect(num+1);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
}

void cPict::drawPartyMonstLg(short num, Rect to_rect){
	Rect small_monst_rect = {0,0,18,14};
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	PaintRect(&to_rect);
	if(!isSheetSet(SHEET_PARTY)) throw xMissingSheet(SHEET_PARTY);
	GWorldPtr from_gw = save;
	
	Rect from_rect = get_custom_rect(num);
	OffsetRect(&small_monst_rect,to_rect.left,to_rect.top);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
	
	OffsetRect(&small_monst_rect,14,0);
	from_rect = get_custom_rect(num+1);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
	
	OffsetRect(&small_monst_rect,-14,18);
	from_rect = get_custom_rect(num+2);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
	
	OffsetRect(&small_monst_rect,14,0);
	from_rect = get_custom_rect(num+3);
	rect_draw_some_item(from_gw, from_rect, small_monst_rect, (Point){0,0}, transparent);
}

void cPict::drawPartyScen(short num, Rect to_rect){
	if(!isSheetSet(SHEET_HEADER)) throw xMissingSheet(SHEET_HEADER);
	GWorldPtr from_gw = header;
	Rect from_rect = {0,0,32,32};
	OffsetRect(&from_rect,32 * (num % 5),32 * (num / 5));
	to_rect.right = to_rect.left + 32;
	to_rect.bottom = to_rect.top + 32;
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0});
}

void cPict::drawPartyItem(short num, Rect to_rect){
	printf("Drawing graphic %i as a custom space pic.\n",num);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 32;
	if(!isSheetSet(SHEET_PARTY)) throw xMissingSheet(SHEET_PARTY);
	GWorldPtr from_gw = save;
	Rect from_rect = get_custom_rect(num);
	PaintRect(&to_rect);
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0}, transparent);
}

void cPict::drawPartyPc(short num, Rect to_rect){
	printf("Drawing graphic %i as a custom space pic.\n",num);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 32;
	if(!isSheetSet(SHEET_PARTY)) throw xMissingSheet(SHEET_PARTY);
	GWorldPtr from_gw = save;
	Rect from_rect = get_custom_rect(num);
	PaintRect(&to_rect);
	rect_draw_some_item(from_gw, from_rect, to_rect, (Point){0,0}, transparent);
}

cPict::~cPict() {}

xMissingSheet::xMissingSheet(eSheetType t, size_t n) throw() : type(t), num(n) {}

xMissingSheet::~xMissingSheet() throw() {}

const char* xMissingSheet::what() throw(){
	const size_t len = strlen(messages[type]) + strlen(messages[NUM_SHEET_TYPES]) + 1;
	char*const msg = new char[len];
	sprintf(msg,messages[type],num);
	strcat(msg,messages[NUM_SHEET_TYPES]);
	return msg;
}

const char*const xMissingSheet::messages[] = {
	"Terrain sheet #%d has not been set!",
	"The animated terrain sheet has not been set!",
	"Monster sheet #%d has not been set!",
	"The dialog sheet has not been set!",
	"The talk portraits sheet has not been set!",
	"The scenario icon sheet has not been set!",
	"The large scenario icon sheet has not been set!",
	"The large items sheet has not been set!",
	"The tiny items sheet has not been set!",
	"The PC sheet has not been set!",
	"The fields sheet has not been set!",
	"The booms sheet has not been set!",
	"The missile sheet has not been set!",
	"The party sheet has not been set!",
	"The scenario header sheet has not been set!",
	"The terrain map sheet has not been set!",
	"Full sheet #%d has not been set!",
	"The status sheet has not been set!",
	"Custom sheet #%d has not been set!",
	"%s It was accessed before it was set."
};
