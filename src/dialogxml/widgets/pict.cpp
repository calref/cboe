/*
 *  pict.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include "pict.hpp"

#include <functional>
#include <map>
#include <stdexcept>
#include <vector>

#include "gfxsheets.hpp"
#include "render_shapes.hpp"
#include "render_image.hpp"
#include "render_text.hpp"
#include "tiling.hpp"
#include "location.hpp"
#include "dialog.hpp"
#include "res_image.hpp"

extern cCustomGraphics spec_scen_g;
const pic_num_t cPict::BLANK = -1;

void cPict::init(){
	using namespace std::placeholders;
	
	// item
	for (auto type : { PIC_ITEM, PIC_TINY_ITEM, PIC_CUSTOM_ITEM, PIC_PARTY_ITEM, PIC_CUSTOM_TINY_ITEM})
		drawPict()[type] = std::bind(&cPict::drawItem,_1,_2,type, type==PIC_TINY_ITEM || type==PIC_CUSTOM_TINY_ITEM);

	// monster
	for (auto type : { PIC_MONST, PIC_MONST_WIDE, PIC_MONST_TALL, PIC_MONST_LG,
		PIC_CUSTOM_MONST, PIC_CUSTOM_MONST_WIDE, PIC_CUSTOM_MONST_TALL, PIC_CUSTOM_MONST_LG,
		PIC_PARTY_MONST, PIC_PARTY_MONST_WIDE, PIC_PARTY_MONST_TALL, PIC_PARTY_MONST_LG,
	})
		drawPict()[type] = std::bind(&cPict::drawMonster,_1,_2,type);
	
	// pc
	for (auto type : { PIC_PC, PIC_CUSTOM_PC, PIC_PARTY_PC})
		drawPict()[type] = std::bind(&cPict::drawPc,_1,_2,type);

	// scenario
	for (auto type : { PIC_SCEN, PIC_SCEN_LG})
		drawPict()[type] = std::bind(&cPict::drawScenario,_1,_2,type);
	drawPict()[PIC_CUSTOM_SCEN] = &cPict::drawCustomTalk;
	drawPict()[PIC_PARTY_SCEN] = &cPict::drawPartyScen; // checkme, I am not sure that this can happen

	// terrain
	for (auto type : { PIC_TER, PIC_TER_ANIM, PIC_TER_MAP, PIC_CUSTOM_TER, PIC_CUSTOM_TER_ANIM, PIC_CUSTOM_TER_MAP})
		drawPict()[type] = std::bind(&cPict::drawTerrain,_1,_2,type);


	drawPict()[PIC_DLOG] = &cPict::drawPresetDlog;
	drawPict()[PIC_TALK] = &cPict::drawPresetTalk;
	drawPict()[PIC_FIELD] = &cPict::drawPresetField;
	drawPict()[PIC_BOOM] = &cPict::drawPresetBoom;
	drawPict()[PIC_FULL] = &cPict::drawFullSheet;
	drawPict()[PIC_MISSILE] = &cPict::drawPresetMissile;
	drawPict()[PIC_DLOG_LG] = &cPict::drawPresetDlogLg;
	drawPict()[PIC_STATUS] = &cPict::drawStatusIcon;
	drawPict()[PIC_CUSTOM_DLOG] = &cPict::drawCustomDlog;
	drawPict()[PIC_CUSTOM_TALK] = &cPict::drawCustomTalk;
	drawPict()[PIC_CUSTOM_FULL] = &cPict::drawFullSheet;
	drawPict()[PIC_CUSTOM_BOOM] = &cPict::drawCustomBoom;
	drawPict()[PIC_CUSTOM_MISSILE] = &cPict::drawCustomMissile;
	drawPict()[PIC_CUSTOM_DLOG_LG] = &cPict::drawCustomDlogLg;
}

std::map<ePicType,std::function<void(cPict *, rectangle)>>& cPict::drawPict(){
	static std::map<ePicType,std::function<void(cPict *, rectangle)> > f;
	return f;
}

bool cPict::manageFormat(eFormat prop, bool set, boost::any* val) {
	switch(prop) {
		case TXT_FRAME:
			if(val) {
				if(set) frameStyle = boost::any_cast<eFrameStyle>(*val);
				else *val = frameStyle;
			}
			break;
		case TXT_WRAP:
			if(val) {
				if(set) drawScaled = !boost::any_cast<bool>(*val);
				else *val = !drawScaled;
			}
			break;
		// TODO: Color is not supported
		default: return false;
	}
	return true;
}

void cPict::updateResultType(ePicType type)
{
	if(type == PIC_MONST && resultType == PIC_MONST && picture.num < m_pic_index.size()) {
		if(m_pic_index[picture.num].x == 2) resultType += PIC_WIDE;
		if(m_pic_index[picture.num].y == 2) resultType += PIC_TALL;
	}
	if(resultType != PIC_FULL && picture.num >= 1000) {
		if(picture.num >= 10000) {
			picture.num -= 10000;
			resultType += PIC_PARTY;
		} else {
			resultType += PIC_CUSTOM;
			picture.num %= 1000;
		}
	}
	recalcRect();
}

pic_num_t cPict::getPicNum(){
	return picture.num;
}

ePicType cPict::getPicType(){
	return resultType;
}

// AsanU: unset
cPict::cPict(cDialog& parent) :
	cControl(CTRL_PICT,parent), picture(), drawScaled(false) {
	setFormat(TXT_FRAME, FRM_SOLID);
}

cPict::cPict(sf::RenderWindow& parent) :
	cControl(CTRL_PICT, parent), picture(), drawScaled(false) {
	setFormat(TXT_FRAME, FRM_SOLID);
}

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


bool cPict::parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) {
	std::string name = attr.Name();
	if(name == "type") {
		std::string val = attr.Value();
		if(val == "blank")
			blank = true;
		else if(val == "ter")
			resultType = PIC_TER;
		else if(val == "teranim")
			resultType = PIC_TER_ANIM;
		else if(val == "monst")
			resultType = PIC_MONST;
		else if(val == "dlog")
			resultType = PIC_DLOG;
		else if(val == "talk")
			resultType = PIC_TALK;
		else if(val == "scen")
			resultType = PIC_SCEN;
		else if(val == "item")
			resultType = PIC_ITEM;
		else if(val == "pc")
			resultType = PIC_PC;
		else if(val == "field")
			resultType = PIC_FIELD;
		else if(val == "boom")
			resultType = PIC_BOOM;
		else if(val == "missile")
			resultType = PIC_MISSILE;
		else if(val == "full")
			resultType = PIC_FULL;
		else if(val == "map")
			resultType = PIC_TER_MAP;
		else if(val == "status")
			resultType = PIC_STATUS;
		else throw xBadVal(tagName, name, val, attr.Row(), attr.Column(), fname);
		return true;
	} else if(name == "num") {
		try {
			attr.GetValue(&picture.num);
		} catch(ticpp::Exception&) {
			throw xBadVal(tagName, name, attr.Value(), attr.Row(), attr.Column(), fname);
		}
		return true;
	} else if(name == "custom") {
		custom = true;
		return true;
	} else if(name == "size") {
		std::string val = attr.Value();
		if(val == "wide") wide = true;
		else if(val == "tall") tall = true;
		else if(val == "large") wide = tall = true;
		else if(val == "small") tiny = true;
		else throw xBadVal(tagName, name, val, attr.Row(), attr.Column(), fname);
		return true;
	} else if(name == "scaled") {
		std::string val = attr.Value();
		if(val == "true") setFormat(TXT_WRAP, false);
		else if(val == "false") setFormat(TXT_WRAP, true);
		else throw xBadVal(tagName, name, val, attr.Row(), attr.Column(), fname);
		return true;
	} else if(name == "def-key") {
		try {
			attachKey(parseKey(attr.Value()));
		} catch(int) {
			throw xBadVal(tagName, name, attr.Value(), attr.Row(), attr.Column(), fname);
		}
		return true;
	} else if(name == "wrap") {
		return false;
	}
	return cControl::parseAttribute(attr, tagName, fname);
}

void cPict::validatePostParse(ticpp::Element& who, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& nodes) {
	if(!attrs.count("type")) throw xMissingAttr(who.Value(), "type", who.Row(), who.Column(), fname);
	if(blank && attrs.count("num")) throw xBadAttr(who.Value(), "num", who.Row(), who.Column(), fname);
	else if(!blank && !attrs.count("num")) throw xMissingAttr(who.Value(), "num", who.Row(), who.Column(), fname);
	
	if(blank) resultType = PIC_MONST, picture.num = BLANK;
	else if(tiny && resultType == PIC_ITEM) resultType = PIC_TINY_ITEM;
	else if(custom) resultType += PIC_CUSTOM;
	
	if(wide && tall) resultType += PIC_LARGE;
	else if(wide) resultType += PIC_WIDE;
	else if(tall) resultType += PIC_TALL;
	
	setPict(picture.num, resultType);
	return cControl::validatePostParse(who, fname, attrs, nodes);
}

void cPict::recalcRect() {
	rectangle bounds = getBounds();
	switch(resultType) {
		case NUM_PIC_TYPES: case PIC_NONE: break;
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
			auto sheet = getSheet(SHEET_FULL, picture.num);
			bounds.width() = sheet->dimension.x;
			bounds.height() = sheet->dimension.y;
			break;
	}
	setBounds(bounds);
}

std::shared_ptr<const Texture> cPict::getSheet(eSheetType type, size_t n) {
	std::ostringstream sout;
	bool purgeable = false;
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
			purgeable = true;
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
					purgeable = false;
					sout << "sheet" << n;
			}
	}
	return &ResMgr::textures.get(sout.str(), purgeable);
}

void cPict::draw(){
	sf::Color store_color;
	rectangle rect = frame;
	inWindow->setActive();
	
	if(!visible) return;
	
	if(picture.num == BLANK) // Just fill with black
		fill_rect(*inWindow, rect, sf::Color::Black);
	else if (drawPict().count(resultType)==0) // internal problem, fill with violet
		fill_rect(*inWindow, rect, sf::Color{155,38,182});
	else
		drawPict()[resultType](this, rect);
	
	drawFrame(2, frameStyle);
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

bool cPict::get_picture(cPictNum const &pict, Texture& source, rectangle &from_rect, int anim, int which_part)
try {
	source=Texture();
	ePicType type=pict.type;
	if (pict.num<0)
		type=ePicType::PIC_NONE;
	switch (type) {
		// terrain
		case ePicType::PIC_TER: {
			source = *ResMgr::textures.get("ter" + std::to_string(1 + pict.num / 50));
			int picture_wanted = pict.num%50;
			from_rect = calc_rect(picture_wanted % 10, picture_wanted / 10);
			break;
		}
		case ePicType::PIC_TER_ANIM:
			source = *ResMgr::textures.get("teranim");
			from_rect = calc_rect(4*(pict.num/5)+(anim%4),pict.num%5);
			break;
		case ePicType::PIC_TER_MAP:
			source = *ResMgr::textures.get("termap");
			if (pict.num<960) {
				from_rect.left = 12*(pict.num%20);
				from_rect.top = 12*(pict.num/20);
			}
			else {
				from_rect.left = 12*20;
				from_rect.top = 12*(pict.num-960);
			}
			from_rect.right = from_rect.left+12;
			from_rect.bottom = from_rect.top+12;
			break;
		case ePicType::PIC_CUSTOM_TER:
			if (!spec_scen_g)
				break;
			std::tie(source,from_rect) = spec_scen_g.find_graphic(pict.num);
			break;
		case ePicType::PIC_CUSTOM_TER_ANIM:
			if (!spec_scen_g)
				break;
			std::tie(source,from_rect) = spec_scen_g.find_graphic(pict.num+(anim%4));
			break;
			// item
		case ePicType::PIC_ITEM: // 0-54
		case ePicType::PIC_TINY_ITEM: // 0-139
			if (pict.num<55 && pict.type != ePicType::PIC_TINY_ITEM) {
				source = *ResMgr::textures.get("objects");
				from_rect = calc_rect(pict.num % 5, pict.num / 5);
			}
			else {
				source = *ResMgr::textures.get("tinyobj");
				from_rect.left=18 * (pict.num % 10);
				from_rect.top=18 * (pict.num / 10);
				from_rect.right = from_rect.left+18;
				from_rect.bottom = from_rect.top+18;
			}
			break;
		case ePicType::PIC_CUSTOM_ITEM:
		case ePicType::PIC_CUSTOM_TINY_ITEM:
			if (!spec_scen_g)
				break;
			std::tie(source,from_rect) = spec_scen_g.find_graphic(pict.num);
			break;
		case ePicType::PIC_PARTY_ITEM:
			if (!spec_scen_g)
				break;
			std::tie(source,from_rect) = spec_scen_g.find_graphic(pict.num, true);
			break;

		// TODO finish monster code
		case ePicType::PIC_MONST:
		case ePicType::PIC_MONST_LG:
		case ePicType::PIC_MONST_TALL:
		case ePicType::PIC_MONST_WIDE: {
			if (pict.num>=m_pic_index.size())
				throw "bad index";
			auto pictId=m_pic_index[pict.num].i+which_part;
			from_rect = calcDefMonstRect((pictId%20), (anim%4));
			source = *ResMgr::textures.get("monst"+std::to_string((pictId/20)+1));
			break;
		}
		case ePicType::PIC_CUSTOM_MONST:
		case ePicType::PIC_CUSTOM_MONST_LG:
		case ePicType::PIC_CUSTOM_MONST_TALL:
		case ePicType::PIC_CUSTOM_MONST_WIDE: {
			if (!spec_scen_g)
				break;
			static const short adj[4] = {0, 2, 1, 3};
			static const std::map<ePicType, int> typeToFactor=
				{ {ePicType::PIC_CUSTOM_MONST, 1}, {ePicType::PIC_CUSTOM_MONST_TALL, 2},
					{ePicType::PIC_CUSTOM_MONST_WIDE, 2}, {ePicType::PIC_CUSTOM_MONST_LG, 4},
				};
			std::tie(source,from_rect) = spec_scen_g.find_graphic(pict.num+typeToFactor.find(type)->second*adj[anim%4]+which_part);
			break;
		}
		case ePicType::PIC_PARTY_MONST:
		case ePicType::PIC_PARTY_MONST_LG:
		case ePicType::PIC_PARTY_MONST_TALL:
		case ePicType::PIC_PARTY_MONST_WIDE:
			if (!spec_scen_g)
				break;
			std::tie(source,from_rect) = spec_scen_g.find_graphic(pict.num+which_part, true);
			break;
			
		case ePicType::PIC_PC: {
			if (pict.num>=100)
				throw "bad index";
			from_rect = calc_rect(2*(pict.num/8)+((anim&2) ? 1 : 0), (pict.num%8) + ((anim&1) ? 8 : 0));
			source = *ResMgr::textures.get("pcs");
			break;
		}
		case ePicType::PIC_CUSTOM_PC:
			std::tie(source,from_rect) = spec_scen_g.find_graphic(pict.num+(anim&2) ? 1 : 0, false);
			break;
		case ePicType::PIC_PARTY_PC:
			std::tie(source,from_rect) = spec_scen_g.find_graphic(pict.num+(anim&2) ? 1 : 0, true);
			break;

		// dialog
		case ePicType::PIC_DLOG:
			from_rect = {0,0,36,36};
			from_rect.offset(36 * (pict.num % 4),36 * (pict.num / 4));
			source = *ResMgr::textures.get("dlogpics");
			break;
		case ePicType::PIC_DLOG_LG:
			from_rect = {0,0,72,72};
			from_rect.offset(36 * (pict.num % 4),36 * (pict.num / 4));
			source = *ResMgr::textures.get("dlogpics");
			break;
		case ePicType::PIC_CUSTOM_DLOG:
		case ePicType::PIC_CUSTOM_DLOG_LG:
			if (!spec_scen_g)
				break;
			std::tie(source,from_rect) = spec_scen_g.find_graphic(pict.num);
			from_rect.right = from_rect.left + 18;
			from_rect.bottom = from_rect.top + 36;
			break;

		// scene
		case ePicType::PIC_SCEN:
			from_rect = {0,0,32,32};
			from_rect.offset(32 * (pict.num % 5),32 * (pict.num / 5));
			source = *ResMgr::textures.get("scenpics");
			break;
		case ePicType::PIC_SCEN_LG:
			from_rect = {0,0,64,64};
			from_rect.offset(64 * pict.num,0);
			source = *ResMgr::textures.get("bigscenpics");
			break;

		// talk
		case ePicType::PIC_TALK:
			from_rect = {0,0,32,32};
			from_rect.offset(32 * (pict.num % 10),32 * (pict.num / 10));
			source = *ResMgr::textures.get("talkportraits");
			break;
		default:
			break;
	}
	if (!source)
		throw "can not find image";
	return true;
}
catch (...) {
	if (pict.num==-1) // ok no picture
		return false;
	std::cerr << "Error[get_picture]: can not find picture id=" << pict.num << ", type=" << int(pict.type)<< "\n";
	source = *ResMgr::textures.get("errors");
	from_rect={0,0,40,40};
	return true;
}

void cPict::drawTerrain(rectangle to_rect, ePicType defType)
{
	if (defType==PIC_TER_MAP || defType==PIC_CUSTOM_TER_MAP) {
		to_rect.right = to_rect.left + 24;
		to_rect.bottom = to_rect.top + 24;
	}
	else if(to_rect.right - to_rect.left > 28)
		to_rect.inset(4,0);
	Texture source;
	rectangle source_rect;
	cPictNum pict=getSourcePicture(defType);
	if (!get_picture(pict, source, source_rect, animFrame)) {
		fill_rect(*inWindow, to_rect, sf::Color::Black);
		return;
	}
	rect_draw_some_item(source, source_rect, *inWindow, to_rect, RenderState(sf::BlendNone,pict.tint));
}

void cPict::draw_monster(sf::RenderTarget &target, rectangle to_rect, cPictNum const &pict, int anim)
{
	Texture source;
	rectangle source_rect;
	struct WH { int w, h;} dim={1,1};
	location origin{0,0};
	switch(pict.type) {
		case PIC_MONST_WIDE: case PIC_CUSTOM_MONST_WIDE: case PIC_PARTY_MONST_WIDE:
			dim={2,1}; origin={0,to_rect.height()/4}; break;
		case PIC_MONST_TALL: case PIC_CUSTOM_MONST_TALL: case PIC_PARTY_MONST_TALL:
			dim={1,2}; origin={to_rect.width()/4,0}; break;
		case PIC_MONST_LG: case PIC_CUSTOM_MONST_LG: case PIC_PARTY_MONST_LG:
			dim={2,2}; break;
		default: break;
	}
	to_rect.left+=origin.x; to_rect.right-=origin.x;
	to_rect.top+=origin.y; to_rect.bottom-=origin.y;
	struct WH decal={ to_rect.width()/dim.w, to_rect.height()/dim.h};
	rectangle small_monst_rect;
	for (int h=0; h<dim.h; ++h) {
		small_monst_rect.top=to_rect.top+h*decal.h;
		small_monst_rect.bottom=small_monst_rect.top+decal.h;
		for (int w=0; w<dim.w; ++w) {
			Texture source;
			rectangle source_rect;
			if (!get_picture(pict, source, source_rect, anim, h*dim.w+w))
				continue;
			small_monst_rect.left=to_rect.left+w*decal.w;
			small_monst_rect.right=small_monst_rect.left+decal.w;
			rect_draw_some_item(source, source_rect, target, small_monst_rect, RenderState(sf::BlendAlpha, pict.tint));
		}
	}
}

void cPict::drawMonster(rectangle to_rect, ePicType defType)
{
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	
	draw_monster(*inWindow, to_rect, getSourcePicture(defType), animFrame);
}

void cPict::drawPresetDlog(rectangle to_rect){
	to_rect.right = to_rect.left + 36;
	to_rect.bottom = to_rect.top + 36;

	Texture source;
	rectangle source_rect;
	cPictNum pict=getSourcePicture(ePicType::PIC_DLOG);
	if (!get_picture(pict, source, source_rect, animFrame))
		return;
	rect_draw_some_item(source, source_rect, *inWindow, to_rect, RenderState(sf::BlendNone,pict.tint));
}

void cPict::drawPresetDlogLg(rectangle to_rect){
	to_rect.right = to_rect.left + (drawScaled ? getBounds().width() : 72);
	to_rect.bottom = to_rect.top + (drawScaled ? getBounds().height() : 72);
	
	Texture source;
	rectangle source_rect;
	cPictNum pict=getSourcePicture(ePicType::PIC_DLOG_LG);
	if (!get_picture(pict, source, source_rect, animFrame))
		return;
	rect_draw_some_item(source, source_rect, *inWindow, to_rect, RenderState(sf::BlendNone,pict.tint));
}

void cPict::drawPresetTalk(rectangle to_rect){
	to_rect.right = to_rect.left + 32;
	to_rect.bottom = to_rect.top + 32;
	
	Texture source;
	rectangle source_rect;
	cPictNum pict=getSourcePicture(ePicType::PIC_TALK);
	if (!get_picture(pict, source, source_rect, animFrame))
		return;
	rect_draw_some_item(source, source_rect, *inWindow, to_rect, RenderState(sf::BlendNone,pict.tint));
}


void cPict::drawScenario(rectangle to_rect, ePicType defType)
{
	if (!drawScaled || defType!=ePicType::PIC_SCEN_LG) {
		int const w=defType==ePicType::PIC_SCEN_LG ? 64 : 32;
		to_rect.right = to_rect.left + w;
		to_rect.bottom = to_rect.top + w;
	}
	Texture source;
	rectangle source_rect;
	cPictNum pict=getSourcePicture(defType);
	if (!get_picture(pict, source, source_rect, animFrame))
		return;
	rect_draw_some_item(source, source_rect, *inWindow, to_rect, RenderState(sf::BlendNone,pict.tint));
}

void cPict::drawPartyScen(rectangle to_rect){
	auto from_gw = getSheet(SHEET_HEADER);
	rectangle from_rect = {0,0,32,32};
	from_rect.offset(32 * (picture.num % 5),32 * (picture.num / 5));
	to_rect.right = to_rect.left + 32;
	to_rect.bottom = to_rect.top + 32;
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, RenderState(sf::BlendNone,picture.tint));
}

void cPict::drawItem(rectangle to_rect, ePicType defType, bool inTinyRect)
{
	if (inTinyRect) {
		to_rect.right = to_rect.left + 18;
		to_rect.bottom = to_rect.top + 18;
	}
	else {
		to_rect.right = to_rect.left + 28;
		to_rect.bottom = to_rect.top + 36;
	}
	fill_rect(*inWindow, to_rect, sf::Color::Black);

	Texture source;
	rectangle source_rect;	auto pict=getSourcePicture(defType);
	if (!get_picture(pict, source, source_rect, animFrame))
		return;
	if (!inTinyRect) {
		if (pict.type==ePicType::PIC_TINY_ITEM || pict.type==ePicType::PIC_CUSTOM_TINY_ITEM || (pict.type==ePicType::PIC_ITEM && pict.num>=55))
			to_rect.inset(5,9);
	}
	rect_draw_some_item(source, source_rect, *inWindow, to_rect, RenderState(sf::BlendAlpha,pict.tint));
}

void cPict::drawPc(rectangle to_rect, ePicType defType)
{
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	
	Texture source;
	rectangle source_rect;
	cPictNum pict=getSourcePicture(defType);
	if (!get_picture(pict, source, source_rect, animFrame))
		return;
	rect_draw_some_item(source, source_rect, *inWindow, to_rect, RenderState(sf::BlendAlpha,pict.tint));
}

void cPict::drawPresetField(rectangle to_rect){
	auto from_gw = getSheet(SHEET_FIELD);
	rectangle from_rect = calc_rect(picture.num % 8, picture.num / 8);
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, RenderState(sf::BlendAlpha,picture.tint));
}

void cPict::drawPresetBoom(rectangle to_rect){
	auto from_gw = getSheet(SHEET_BOOM);
	int num=picture.num;
	if(num >= 8)
		num = 8 * (num - 7) + animFrame % 8;
	rectangle from_rect = calc_rect(num % 8, num / 8);
	// TODO: Be smarter about this - we know the first row is static booms and subsequent rows are animated booms.
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, RenderState(sf::BlendAlpha,picture.tint));
}

void cPict::drawFullSheet(rectangle to_rect){
	auto from_gw = getSheet(SHEET_FULL, picture.num);
	if (!from_gw) return;
	rectangle from_rect = rectangle(*from_gw);
	if(!drawScaled) {
		to_rect.right = to_rect.left + (from_rect.right - from_rect.left);
		to_rect.bottom = to_rect.top + (from_rect.bottom - from_rect.top);
	}
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, RenderState(sf::BlendNone,picture.tint));
}

void cPict::drawPresetMissile(rectangle to_rect){
	rectangle from_rect = {0,0,18,18};
	auto from_gw = getSheet(SHEET_MISSILE);
	to_rect.right = to_rect.left + 18;
	to_rect.bottom = to_rect.top + 18;
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	short i = animFrame % 8;
	from_rect.offset(18 * i, 18 * picture.num);
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, RenderState(sf::BlendAlpha,picture.tint));
}

void cPict::drawStatusIcon(rectangle to_rect){
	rectangle from_rect = {0,0,12,12};
	auto from_gw = getSheet(SHEET_STATUS);
	to_rect.right = to_rect.left + 12;
	to_rect.bottom = to_rect.top + 12;
	from_rect.offset(12 * (picture.num % 3), 12 * (picture.num / 3));
	if(getFormat(TXT_FRAME)) {
		rectangle pat_rect = to_rect;
		pat_rect.inset(-1,-1);
		tileImage(*inWindow, pat_rect, bg[6]);
	}
	rect_draw_some_item(*from_gw, from_rect, *inWindow, to_rect, RenderState(sf::BlendAlpha,picture.tint));
}

// This is a super-hacky way to wedge in scaled form, but at least it should work.
static int dlog_to_w = 18, dlog_to_h = 36;

void cPict::drawCustomNumDlog(int num, rectangle to_rect){
	Texture source;
	rectangle source_rect;
	cPictNum pict=getSourcePicture(ePicType::PIC_CUSTOM_DLOG);
	pict.num = num;
	if (pict.type==ePicType::PIC_CUSTOM_DLOG || pict.type==ePicType::PIC_CUSTOM_DLOG_LG) {
		to_rect.right = to_rect.left + dlog_to_w;
		to_rect.bottom = to_rect.top + dlog_to_h;
		if (get_picture(pict, source, source_rect, animFrame))
			rect_draw_some_item(source, source_rect, *inWindow, to_rect, RenderState(sf::BlendNone,pict.tint));
	
		to_rect.offset(dlog_to_w,0);
		++pict.num;
		if (get_picture(pict, source, source_rect, animFrame))
			rect_draw_some_item(source, source_rect, *inWindow, to_rect, RenderState(sf::BlendNone,pict.tint));
	}
	else {
		to_rect.right = to_rect.left + 36;
		to_rect.bottom = to_rect.top + 36;
		if (get_picture(pict, source, source_rect, animFrame))
			rect_draw_some_item(source, source_rect, *inWindow, to_rect, RenderState(sf::BlendNone,pict.tint));
	}
}

void cPict::drawCustomDlog(rectangle to_rect){
	drawCustomNumDlog(picture.num, to_rect);
}

void cPict::drawCustomDlogLg(rectangle to_rect){
	// CHECKME: this does not make any sense
	if(drawScaled) {
		dlog_to_w = 9;
		dlog_to_h = 18;
	}
	drawCustomNumDlog(picture.num,to_rect);
	to_rect.offset(dlog_to_h,0);
	drawCustomNumDlog(picture.num + 2,to_rect);
	to_rect.offset(-dlog_to_h,dlog_to_h);
	drawCustomNumDlog(picture.num + 4,to_rect);
	to_rect.offset(dlog_to_h,0);
	drawCustomNumDlog(picture.num + 6,to_rect);
	if(drawScaled) {
		dlog_to_w = 18;
		dlog_to_h = 36;
	}
}

void cPict::drawCustomTalk(rectangle to_rect){
	Texture from_gw;
	rectangle from_rect;
	std::tie(from_gw,from_rect) = spec_scen_g.find_graphic(picture.num);
	to_rect.right = to_rect.left + 16;
	to_rect.bottom = to_rect.top + 32;
	from_rect.right = from_rect.left + 16;
	from_rect.bottom = from_rect.top + 32;
	rect_draw_some_item(from_gw, from_rect, *inWindow, to_rect, RenderState(sf::BlendNone,picture.tint));
	
	std::tie(from_gw,from_rect) = spec_scen_g.find_graphic(picture.num+1);
	to_rect.offset(16,0);
	from_rect.right = from_rect.left + 16;
	from_rect.bottom = from_rect.top + 32;
	rect_draw_some_item(from_gw, from_rect, *inWindow, to_rect, RenderState(sf::BlendNone,picture.tint));
}

void cPict::drawCustomBoom(rectangle to_rect){
	to_rect.right = to_rect.left + 28;
	to_rect.bottom = to_rect.top + 36;
	Texture from_gw;
	rectangle from_rect;
	std::tie(from_gw,from_rect) = spec_scen_g.find_graphic(picture.num + animFrame % 8);
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	rect_draw_some_item(from_gw, from_rect, *inWindow, to_rect, RenderState(sf::BlendAlpha,picture.tint));
}

void cPict::drawCustomMissile(rectangle to_rect){
	Texture from_gw;
	rectangle from_rect;
	std::tie(from_gw,from_rect) = spec_scen_g.find_graphic(picture.num + animFrame % 8);
	from_rect.right = from_rect.left + 18;
	from_rect.bottom = from_rect.top + 18;
	if(animFrame >= 4) from_rect.offset(0, 18);
	fill_rect(*inWindow, to_rect, sf::Color::Black);
	to_rect.inset(5,9);
	rect_draw_some_item(from_gw, from_rect, *inWindow, to_rect, RenderState(sf::BlendAlpha,picture.tint));
}

cPict::~cPict() {}

void cPict::drawAt(sf::RenderWindow& win, rectangle dest, pic_num_t which_g, ePicType type_g, bool framed) {
	cPict pic(win);
	pic.frame = dest;
	pic.setPict(which_g, type_g);
	pic.setFormat(TXT_FRAME, framed ? FRM_SOLID : FRM_NONE);
	pic.draw();
}

cControl::storage_t cPict::store() {
	storage_t storage = cControl::store();
	storage["pic-num"] = picture;
	storage["pic-type"] = resultType;
	return storage;
}

void cPict::restore(storage_t to) {
	cControl::restore(to);
	if(to.find("pic-num") != to.end())
		picture = boost::any_cast<cPictNum>(to["pic-num"]);
	if(to.find("pic-type") != to.end())
		resultType = boost::any_cast<ePicType>(to["pic-type"]);
}
