//
//  pictypes.hpp
//  BoE
//
//  Created by Celtic Minstrel on 15-01-06.
//
//

#ifndef BoE_pictypes_hpp
#define BoE_pictypes_hpp

/// @file
/// Constants to specify the type of an icon

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
	PIC_TER_MAP = 15,			///< 12x12 map graphic from the terrain map sheet, expanded to 24x24
	PIC_STATUS = 16,			///< 12x12 status icon
	PIC_TINY_ITEM = 17,			///< 18x18 item graphic from the small item sheet
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
	PIC_CUSTOM_BOOM = 110,		///< 28x36 custom explosion graphic (8 frames)
	PIC_CUSTOM_FULL = 111,		///< entire sheet graphic, drawn from scenname.exr/sheetxxx.png where xxx is the number
	PIC_CUSTOM_MISSILE = 112,	///< 18x18 missile graphic drawn from the the custom sheets
	PIC_CUSTOM_DLOG_LG = 113,	///< 72x72 dialog graphic from the custom sheet, taken from 8 successive slots
	PIC_CUSTOM_TER_MAP = 115,	///< 12x12 map graphic taken from the custom sheet and expanded to 24x24
	PIC_CUSTOM_TINY_ITEM = 117,	///< 28x36 custom item graphic shrunk down into an 18x18 space
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
