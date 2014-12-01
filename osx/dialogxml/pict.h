/*
 *  pict.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef PICT_H
#define PICT_H

#include <SFML/Graphics.hpp>

#include <vector>
#include <map>
#include "graphtool.h" // for pic_num_t
#include "control.h"

enum ePicType {
	PIC_TER = 1, // 28x36 terrain graphic from the preset sheets
	PIC_TER_ANIM = 2, // 28x36 terrain graphic from the preset animated terrain sheet
	PIC_MONST = 3, // 28x36 monster graphic from the preset sheets
	PIC_DLOG = 4, // 36x36 dialog graphic from the preset sheet
	PIC_TALK = 5, // 32x32 talking portrait from the preset sheet
	PIC_SCEN = 6, // 32x32 scenario graphic from the scenario sheet
	PIC_ITEM = 7, // 28x36 item graphic from the large item sheet, or 18x18 item graphic from the small sheet centred in a 28x36 space
	PIC_PC = 8, // 28x36 pc graphic from the player sheet
	PIC_FIELD = 9, // 28x36 field graphic from the fields sheet
	PIC_BOOM = 10, // 28x36 boom graphic from the booms sheet
	PIC_FULL = 11, // entire sheet graphic; number is the resource ID
	PIC_MISSILE = 12, // 18x18 missile graphic from the missiles sheet
	PIC_DLOG_LG = 13, // 72x72 dialog graphic from the dialog sheet
	PIC_SCEN_LG = 14, // 64x64 scenario graphic (currently each is on its own sheet)
	PIC_TER_MAP = 15, // 12x12 map graphic... or should it be 6x6?
	PIC_STATUS = 16, // 12x12 status icon
	PIC_MONST_WIDE = 23, // 56x36 monster graphic from the preset sheets, resized to fit and centred in a 28x36 space
	PIC_MONST_TALL = 43, // 28x72 monster graphic from the preset sheets, resized to fit and centred in a 28x36 space
	PIC_MONST_LG = 63, // 56x72 monster graphic from the preset sheets, resized to fit in a 28x36 space
	PIC_CUSTOM_TER = 101, // 28x36 custom graphic from the custom sheets
	PIC_CUSTOM_TER_ANIM = 102,
	PIC_CUSTOM_MONST = 103,
	PIC_CUSTOM_DLOG = 104, // 36x36 dialog graphic drawn from two 18x26 halves in the custom sheets
	PIC_CUSTOM_TALK = 105, // 32x32 talking portrait drawn from two 16x32 halves in the custom sheets
	PIC_CUSTOM_SCEN = 106, // 32x32 scenario portrait loading from scenname.exr/scenario.png
	PIC_CUSTOM_ITEM = 107,
	PIC_CUSTOM_FULL = 111, // entire sheet graphic, drawn from scenname.exr/sheetxxx.png where xxx is the number
	PIC_CUSTOM_MISSILE = 112, // 18x18 missile graphic drawn from the the custom sheets
	PIC_CUSTOM_DLOG_LG = 113, // 72x72 dialog graphic from the custom sheet, taken from 8 successive slots
	PIC_CUSTOM_TER_MAP = 115, // 12x12 map graphic (should it be 6x6?) taken from the custom sheet
	PIC_CUSTOM_MONST_WIDE = 123, // 56x36 monster graphic from the custom sheets, resized to fit and centred in a 28x36 space
	PIC_CUSTOM_MONST_TALL = 143, // 28x72 monster graphic from the custom sheets, resized to fit and centred in a 28x36 space
	PIC_CUSTOM_MONST_LG = 163, // 56x72 monster graphic from the custom sheets, resized to fit in a 28x36 space
	PIC_PARTY_MONST = 203, // 28x36 graphic drawn from the savegame sheet
	PIC_PARTY_SCEN = 206, // 32x32 graphic drawn from the scenario headers sheet
	PIC_PARTY_ITEM = 207,
	PIC_PARTY_PC = 208,
	PIC_PARTY_MONST_WIDE = 223, // 56x36 monster graphic from the savegame sheet, resized to fit and centred in a 28x36 space
	PIC_PARTY_MONST_TALL = 243, // 28x72 monster graphic from the savegame sheet, resized to fit and centred in a 28x36 space
	PIC_PARTY_MONST_LG = 263, // 56x72 monster graphic from the savegame sheet, resized to fit in a 28x36 space
	NUM_PIC_TYPES
};

enum ePicTypeMod {
	// Can be added to or subtracted from ePicType enums, returning an ePicType.
	PIC_PRESET = 0, // just for good measure
	PIC_WIDE = 20, // if applied to any derivative of PIC_MONST, makes the x-dimension 2 instead of 1
	PIC_TALL = 40, // if applied to any derivative of PIC_MONST, makes the y-dimension 2 instead of 1
	PIC_LARGE = PIC_WIDE + PIC_TALL,
	PIC_CUSTOM = 100, // if applied to any customizable graphic, makes it custom instead of preset
	PIC_PARTY = 200, // similar to above
};

enum eSheetType {
	SHEET_TER,
	SHEET_TER_ANIM,
	SHEET_MONST,
	SHEET_DLOG,
	SHEET_TALK,
	SHEET_SCEN,
	SHEET_SCEN_LG,
	SHEET_ITEM,
	SHEET_TINY_ITEM,
	SHEET_PC,
	SHEET_FIELD,
	SHEET_BOOM,
	SHEET_MISSILE,
	SHEET_PARTY,
	SHEET_HEADER,
	SHEET_TER_MAP,
	SHEET_FULL,
	SHEET_STATUS,
	SHEET_CUSTOM,
	NUM_SHEET_TYPES
};

class cPict : public cControl {
public:
	static void init();
	void attachClickHandler(click_callback_t f) throw();
	void attachFocusHandler(focus_callback_t f) throw(xHandlerNotSupported);
	bool triggerClickHandler(cDialog& me, std::string id, eKeyMod mods, location where);
	void setFormat(eFormat prop, short val) throw(xUnsupportedProp);
	short getFormat(eFormat prop) throw(xUnsupportedProp);
	void setColour(sf::Color clr) throw(xUnsupportedProp);
	sf::Color getColour() throw(xUnsupportedProp);
	void setPict(pic_num_t num, ePicType type);
	void setPict(pic_num_t num);
	pic_num_t getPicNum();
	ePicType getPicType();
	explicit cPict(cDialog& parent);
	explicit cPict(sf::RenderWindow& parent);
	bool isClickable();
	static void advanceAnim();
	virtual ~cPict();
	void draw();
	static void drawAt(sf::RenderWindow& win, RECT dest, pic_num_t which_g, ePicType type_g, bool framed);
	cPict& operator=(cPict& other) = delete;
	cPict(cPict& other) = delete;
private:
	static std::shared_ptr<sf::Texture> getSheet(eSheetType type, size_t n = 0);
	static short animFrame;
	short picNum;
	ePicType picType;
	bool clickable, drawFramed;
	void drawPresetTer(short num, RECT to_rect);
	void drawPresetTerAnim(short num, RECT to_rect);
	void drawPresetMonstSm(short num, RECT to_rect);
	void drawPresetMonstWide(short num, RECT to_rect);
	void drawPresetMonstTall(short num, RECT to_rect);
	void drawPresetMonstLg(short num, RECT to_rect);
	void drawPresetDlog(short num, RECT to_rect);
	void drawPresetDlogLg(short num, RECT to_rect);
	void drawPresetTalk(short num, RECT to_rect);
	void drawPresetScen(short num, RECT to_rect);
	void drawPresetScenLg(short num, RECT to_rect);
	void drawPresetItem(short num, RECT to_rect);
	void drawPresetPc(short num, RECT to_rect);
	void drawPresetField(short num, RECT to_rect);
	void drawPresetBoom(short num, RECT to_rect);
	void drawPresetMissile(short num, RECT to_rect);
	void drawPresetTerMap(short num, RECT to_rect);
	void drawStatusIcon(short num, RECT to_rect);
	void drawFullSheet(short num, RECT to_rect);
	void drawCustomTer(short num, RECT to_rect);
	void drawCustomTerAnim(short num, RECT to_rect);
	void drawCustomMonstSm(short num, RECT to_rect);
	void drawCustomMonstWide(short num, RECT to_rect);
	void drawCustomMonstTall(short num, RECT to_rect);
	void drawCustomMonstLg(short num, RECT to_rect);
	void drawCustomDlog(short num, RECT to_rect);
	void drawCustomDlogLg(short num, RECT to_rect);
	void drawCustomTalk(short num, RECT to_rect);
	void drawCustomItem(short num, RECT to_rect);
	void drawCustomMissile(short num, RECT to_rect);
	void drawCustomTerMap(short num, RECT to_rect);
	void drawPartyMonstSm(short num, RECT to_rect);
	void drawPartyMonstWide(short num, RECT to_rect);
	void drawPartyMonstTall(short num, RECT to_rect);
	void drawPartyMonstLg(short num, RECT to_rect);
	void drawPartyScen(short num, RECT to_rect);
	void drawPartyItem(short num, RECT to_rect);
	void drawPartyPc(short num, RECT to_rect);
	static std::map<ePicType,void(cPict::*)(short,RECT)>& drawPict();
	click_callback_t onClick;
};

ePicType operator + (ePicType lhs, ePicTypeMod rhs);
ePicType operator - (ePicType lhs, ePicTypeMod rhs);
ePicType operator + (ePicTypeMod lhs, ePicType rhs);
ePicType operator - (ePicTypeMod lhs, ePicType rhs);
ePicType&operator +=(ePicType&lhs, ePicTypeMod rhs);
ePicType&operator -=(ePicType&lhs, ePicTypeMod rhs);

#endif
