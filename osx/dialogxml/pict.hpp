/*
 *  pict.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef PICT_H
#define PICT_H

/// @file
/// Icon-related classes and types.

#include <SFML/Graphics.hpp>

#include <vector>
#include <map>
#include "graphtool.h" // for pic_num_t
#include "control.hpp"

/// Specifies an icon type.
enum ePicType {
	PIC_TER = 1,				///< 28x36 terrain graphic from the preset sheets
	PIC_TER_ANIM = 2,			///< 28x36 terrain graphic from the preset animated terrain sheet
	PIC_MONST = 3,				///< 28x36 monster graphic from the preset sheets
	PIC_DLOG = 4,				///< 36x36 dialog graphic from the preset sheet
	PIC_TALK = 5,				///< 32x32 talking portrait from the preset sheet
	PIC_SCEN = 6,				///< 32x32 scenario graphic from the scenario sheet
	PIC_ITEM = 7,				///< 28x36 item graphic from the large item sheet,
								///< or 18x18 item graphic from the small sheet centred in a 28x36 space
	PIC_PC = 8,					///< 28x36 pc graphic from the player sheet
	PIC_FIELD = 9,				///< 28x36 field graphic from the fields sheet
	PIC_BOOM = 10,				///< 28x36 boom graphic from the booms sheet
	PIC_FULL = 11,				///< entire sheet graphic; number is the resource ID
	PIC_MISSILE = 12,			///< 18x18 missile graphic from the missiles sheet
	PIC_DLOG_LG = 13,			///< 72x72 dialog graphic from the dialog sheet
	PIC_SCEN_LG = 14,			///< 64x64 scenario graphic (currently each is on its own sheet)
	PIC_TER_MAP = 15,			///< 12x12 map graphic... or should it be 6x6?
	PIC_STATUS = 16,			///< 12x12 status icon
	PIC_MONST_WIDE = 23,		///< 56x36 monster graphic from the preset sheets, resized to fit and centred in a 28x36 space
	PIC_MONST_TALL = 43,		///< 28x72 monster graphic from the preset sheets, resized to fit and centred in a 28x36 space
	PIC_MONST_LG = 63,			///< 56x72 monster graphic from the preset sheets, resized to fit in a 28x36 space
	PIC_CUSTOM_TER = 101,		///< 28x36 custom terrain graphic from the custom sheets
	PIC_CUSTOM_TER_ANIM = 102,	///< 28x36 custom animated terrain graphic from the custom sheets
	PIC_CUSTOM_MONST = 103,		///< 28x36 custom monster graphic from the custom sheets
	PIC_CUSTOM_DLOG = 104,		///< 36x36 dialog graphic drawn from two 18x26 halves in the custom sheets
	PIC_CUSTOM_TALK = 105,		///< 32x32 talking portrait drawn from two 16x32 halves in the custom sheets
	PIC_CUSTOM_SCEN = 106,		///< 32x32 scenario portrait loading from scenname.exr/scenario.png
	PIC_CUSTOM_ITEM = 107,		///< 28x36 custom item graphic from the custom sheets
	PIC_CUSTOM_FULL = 111,		///< entire sheet graphic, drawn from scenname.exr/sheetxxx.png where xxx is the number
	PIC_CUSTOM_MISSILE = 112,	///< 18x18 missile graphic drawn from the the custom sheets
	PIC_CUSTOM_DLOG_LG = 113,	///< 72x72 dialog graphic from the custom sheet, taken from 8 successive slots
	PIC_CUSTOM_TER_MAP = 115,	///< 12x12 map graphic (should it be 6x6?) taken from the custom sheet
	PIC_CUSTOM_MONST_WIDE = 123,///< 56x36 monster graphic from the custom sheets, resized to fit and centred in a 28x36 space
	PIC_CUSTOM_MONST_TALL = 143,///< 28x72 monster graphic from the custom sheets, resized to fit and centred in a 28x36 space
	PIC_CUSTOM_MONST_LG = 163,	///< 56x72 monster graphic from the custom sheets, resized to fit in a 28x36 space
	PIC_PARTY_MONST = 203,		///< 28x36 monster graphic drawn from the savegame sheet
	PIC_PARTY_SCEN = 206,		///< 32x32 graphic drawn from the scenario headers sheet
	PIC_PARTY_ITEM = 207,		///< 28x36 item graphic drawn from the savegame sheet
	PIC_PARTY_PC = 208,			///< 28x36 PC graphic drawn from the savegame sheet
	PIC_PARTY_MONST_WIDE = 223,	///< 56x36 monster graphic from the savegame sheet, resized to fit and centred in a 28x36 space
	PIC_PARTY_MONST_TALL = 243,	///< 28x72 monster graphic from the savegame sheet, resized to fit and centred in a 28x36 space
	PIC_PARTY_MONST_LG = 263,	///< 56x72 monster graphic from the savegame sheet, resized to fit in a 28x36 space
	NUM_PIC_TYPES
};

/// Flags that modify icon types.
/// Can be added to or subtracted from ePicType enums, returning an ePicType.
enum ePicTypeMod {
	PIC_PRESET = 0,		///< No mod, included just for good measure; can be added to cancel out all mods.
	PIC_WIDE = 20,		///< If applied to any derivative of PIC_MONST, makes the x-dimension 2 instead of 1.
	PIC_TALL = 40,		///< If applied to any derivative of PIC_MONST, makes the y-dimension 2 instead of 1.
	PIC_LARGE = PIC_WIDE + PIC_TALL, ///< A combination of PIC_WIDE and PIC_TALL.
	PIC_CUSTOM = 100,	///< If applied to any customizable graphic, makes it custom instead of preset.
	PIC_PARTY = 200,	///< If applied to any exportable graphic, makes it exported instead of preset.
};

/// Specifies a graphics sheet that icons are drawn from.
enum eSheetType {
	SHEET_TER,		///< The preset terrain sheets, terX.png
	SHEET_TER_ANIM,	///< The animated terrains sheet, teranim.png
	SHEET_MONST,	///< The preset monster sheets, monstX.png
	SHEET_DLOG,		///< The preset dialog icons sheet, dlogpics.png
	SHEET_TALK,		///< The preset talk icons sheet, talkportraits.png
	SHEET_SCEN,		///< The preset scenario icons sheet, scenpics.png
	SHEET_SCEN_LG,	///< The large scenario icons sheet, bigscenpics.png
	SHEET_ITEM,		///< The preset large items sheet, objects.png
	SHEET_TINY_ITEM,///< The small items sheet, tinyobj.png
	SHEET_PC,		///< The PC graphics sheet, pcs.png
	SHEET_FIELD,	///< The fields and objects sheet, fields.png
	SHEET_BOOM,		///< The special effects sheet, booms.png
	SHEET_MISSILE,	///< The missile animations sheet, missles.png
	SHEET_PARTY,	///< The exported graphics sheet stored in the saved game
	SHEET_HEADER,	///< The scenario header sheet
	SHEET_TER_MAP,	///< The terrain map icons sheet, termap.png
	SHEET_FULL,		///< Any full sheet
	SHEET_STATUS,	///< The status icons sheet, staticons.png
	SHEET_CUSTOM,	///< Any custom graphics sheet
	// TODO: Vehicle sheet is missing.
	// TODO: Documentation of full, custom, header, and exported sheets is still lacking.
	NUM_SHEET_TYPES
};

/// A simple icon.
/// This control can also be made clickable.
class cPict : public cControl {
public:
	/// @copydoc cDialog::init()
	static void init();
	void attachClickHandler(click_callback_t f) throw();
	void attachFocusHandler(focus_callback_t f) throw(xHandlerNotSupported);
	bool triggerClickHandler(cDialog& me, std::string id, eKeyMod mods);
	void setFormat(eFormat prop, short val) throw(xUnsupportedProp);
	short getFormat(eFormat prop) throw(xUnsupportedProp);
	void setColour(sf::Color clr) throw(xUnsupportedProp);
	sf::Color getColour() throw(xUnsupportedProp);
	/// @copydoc setPict(pic_num_t)
	/// @param type The type of the new icon
	/// @note Calling this function automatically adjusts the bounding rect so that the picture fits perfectly.
	/// It does not change the control's position.
	///
	/// This function applies a few automatic adjustments to its input:
	///
	/// - If type is PIC_MONST, it automatically looks up the chosen icon to determine
	/// whether it should apply the tall or wide modifiers.
	/// - If num is 1000 or greater and type is not PIC_FULL, it automatically subtracts 1000 and applies the custom modifier.
	void setPict(pic_num_t num, ePicType type);
	/// Set the pict's icon.
	/// @param num The new icon index.
	void setPict(pic_num_t num);
	/// Automatically recalculate the icon's bounding rect based on its current picture.
	void recalcRect();
	/// Get the current icon.
	/// @return The number of the current icon.
	pic_num_t getPicNum();
	/// Get the current icon's type.
	/// @return The type of the current icon.
	ePicType getPicType();
	/// Create a new icon.
	/// @param parent The parent dialog.
	explicit cPict(cDialog& parent);
	/// Create a new icon without a parent dialog.
	/// @param parent The parent window.
	explicit cPict(sf::RenderWindow& parent);
	bool isClickable();
	/// Advance the current animation frame.
	/// Should be called at predictable intervals if the dialog might contain an animated graphic.
	static void advanceAnim();
	virtual ~cPict();
	void draw();
	/// A utility function to draw an icon into an arbitrary window.
	/// @param win The window to draw in.
	/// @param dest The bounding rect to draw in (ignored for drawing the actual, but used for background fill and framing)
	/// @param which_g The icon to draw.
	/// @param type_g The type of icon to draw.
	/// @param framed Whether to draw a frame around the icon.
	static void drawAt(sf::RenderWindow& win, rectangle dest, pic_num_t which_g, ePicType type_g, bool framed);
	/// A convenience constant that can be passed as the pic number to setPict(pic_num_t num).
	/// It sets the icon to nothing, showing as just black.
	static const pic_num_t BLANK;
	cPict& operator=(cPict& other) = delete;
	cPict(cPict& other) = delete;
private:
	static std::shared_ptr<sf::Texture> getSheet(eSheetType type, size_t n = 0);
	static short animFrame;
	pic_num_t picNum;
	ePicType picType;
	bool clickable, drawFramed;
	void drawPresetTer(short num, rectangle to_rect);
	void drawPresetTerAnim(short num, rectangle to_rect);
	void drawPresetMonstSm(short num, rectangle to_rect);
	void drawPresetMonstWide(short num, rectangle to_rect);
	void drawPresetMonstTall(short num, rectangle to_rect);
	void drawPresetMonstLg(short num, rectangle to_rect);
	void drawPresetDlog(short num, rectangle to_rect);
	void drawPresetDlogLg(short num, rectangle to_rect);
	void drawPresetTalk(short num, rectangle to_rect);
	void drawPresetScen(short num, rectangle to_rect);
	void drawPresetScenLg(short num, rectangle to_rect);
	void drawPresetItem(short num, rectangle to_rect);
	void drawPresetPc(short num, rectangle to_rect);
	void drawPresetField(short num, rectangle to_rect);
	void drawPresetBoom(short num, rectangle to_rect);
	void drawPresetMissile(short num, rectangle to_rect);
	void drawPresetTerMap(short num, rectangle to_rect);
	void drawStatusIcon(short num, rectangle to_rect);
	void drawFullSheet(short num, rectangle to_rect);
	void drawCustomTer(short num, rectangle to_rect);
	void drawCustomTerAnim(short num, rectangle to_rect);
	void drawCustomMonstSm(short num, rectangle to_rect);
	void drawCustomMonstWide(short num, rectangle to_rect);
	void drawCustomMonstTall(short num, rectangle to_rect);
	void drawCustomMonstLg(short num, rectangle to_rect);
	void drawCustomDlog(short num, rectangle to_rect);
	void drawCustomDlogLg(short num, rectangle to_rect);
	void drawCustomTalk(short num, rectangle to_rect);
	void drawCustomItem(short num, rectangle to_rect);
	void drawCustomMissile(short num, rectangle to_rect);
	void drawCustomTerMap(short num, rectangle to_rect);
	void drawPartyMonstSm(short num, rectangle to_rect);
	void drawPartyMonstWide(short num, rectangle to_rect);
	void drawPartyMonstTall(short num, rectangle to_rect);
	void drawPartyMonstLg(short num, rectangle to_rect);
	void drawPartyScen(short num, rectangle to_rect);
	void drawPartyItem(short num, rectangle to_rect);
	void drawPartyPc(short num, rectangle to_rect);
	static std::map<ePicType,void(cPict::*)(short,rectangle)>& drawPict();
	click_callback_t onClick;
};

/// Apply a modifier to an icon type.
/// @param lhs The base icon type to modify.
/// @param rhs The modifier to apply.
/// @return The modified icon type.
/// @note As a special case, adding PIC_PRESET removes all modifiers.
ePicType operator + (ePicType lhs, ePicTypeMod rhs);
/// Remove a modifier from an icon type.
/// @param lhs The base icon type to modify.
/// @param rhs The modifier to remove.
/// @return The modified icon type.
ePicType operator - (ePicType lhs, ePicTypeMod rhs);
/// Apply a modifier to an icon type.
/// @param lhs The modifier to apply.
/// @param rhs The base icon type to modify.
/// @return The modified icon type.
/// @note As a special case, adding PIC_PRESET removes all modifiers.
ePicType operator + (ePicTypeMod lhs, ePicType rhs);
/// Remove a modifier from an icon type.
/// @param lhs The modifier to remove.
/// @param rhs The base icon type to modify.
/// @return The modified icon type.
ePicType operator - (ePicTypeMod lhs, ePicType rhs);
/// Apply a modifier to an icon type.
/// @param lhs The base icon type to modify.
/// @param rhs The modifier to apply.
/// @return lhs, now modified.
/// @note As a special case, adding PIC_PRESET removes all modifiers.
ePicType&operator +=(ePicType&lhs, ePicTypeMod rhs);
/// Remove a modifier from an icon type.
/// @param lhs The base icon type to modify.
/// @param rhs The modifier to remove.
/// @return lhs, now modified.
ePicType&operator -=(ePicType&lhs, ePicTypeMod rhs);
/// Check if an icon type has a modifier applied.
/// @param lhs The icon type to check.
/// @param rhs The modifier type to test for.
/// @return true if the modifier is present.
bool operator& (ePicType lhs, ePicTypeMod rhs);

#endif
