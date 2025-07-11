
#ifndef BOE_SCEN_GLOBAL_HPP
#define BOE_SCEN_GLOBAL_HPP

#include <vector>

const int BITMAP_WIDTH = 28;
const int BITMAP_HEIGHT = 36;

const int NUM_DLOG_B = 53;

const int NLS = 26; // number of left slots for buttons
#define	NRS	right_button_status.size() // number of right slots for scrolling list
const int NRSONPAGE = 33; // number of right slots for scrolling list on page at 1 time

const int RIGHT_AREA_UL_X = 287;
const int RIGHT_AREA_UL_Y = 5;
const int RIGHT_AREA_WIDTH = 290;
const int RIGHT_AREA_HEIGHT = 400;

const int TER_RECT_UL_X = 6;
const int TER_RECT_UL_Y = 19;

const int UI_LAYER_DEFAULT = 1000;
const int UI_LAYER_MENUBAR = 1200;

// When editing types, several more rows can fit because the tool palette isn't there
const int TYPE_ROWS_DRAWING = 16;
const int TYPE_ROWS_EDITING = 23;

enum eScenMode {
	MODE_DRAWING = 0,
	MODE_TOGGLE_ROAD = 1,
	// Deprecated. Use the location picker in the encounter editor dialog.
	MODE_SET_WANDER_POINTS = 2,
	MODE_ROOM_RECT = 3,
	MODE_PLACE_ITEM = 4,
	MODE_EDIT_ITEM = 5,
	MODE_TOGGLE_SPECIAL_DOT = 6,
	MODE_SET_TOWN_START = 7,
	MODE_COPY_CREATURE = 8,
	MODE_SET_TOWN_RECT = 9,
	MODE_PLACE_NORTH_ENTRANCE = 10,
	MODE_PLACE_EAST_ENTRANCE = 11,
	MODE_PLACE_SOUTH_ENTRANCE = 12,
	MODE_PLACE_WEST_ENTRANCE = 13,
	MODE_FLOOD_FILL = 14,
	MODE_PLACE_HORSE = 15,
	MODE_PLACE_BOAT = 16,
	MODE_STORAGE_RECT = 17,
	MODE_PLACE_FORCECAGE = 19,
	MODE_PLACE_WEB = 20,
	MODE_PLACE_CRATE = 21,
	MODE_PLACE_BARREL = 22,
	MODE_PLACE_FIRE_BARRIER = 23,
	MODE_PLACE_FORCE_BARRIER = 24,
	MODE_PLACE_QUICKFIRE = 25,
	MODE_CLEAR_FIELDS = 26,
	MODE_PLACE_STONE_BLOCK = 27,
	MODE_PLACE_CREATURE = 28,
	MODE_LARGE_PAINTBRUSH = 29, // uncertain
	MODE_SMALL_PAINTBRUSH = 30, // uncertain
	MODE_LARGE_SPRAYCAN = 31, // uncertain
	MODE_SMALL_SPRAYCAN = 32, // uncertain
	MODE_PLACE_SFX = 33, // eg bloodstain or ashes
	MODE_EYEDROPPER = 34,
	MODE_HOLLOW_RECT = 35,
	MODE_FILLED_RECT = 36,
	MODE_ERASER = 37,
	MODE_EDIT_SIGN = 38,
	MODE_EDIT_CREATURE = 39,
	MODE_EDIT_SPECIAL = 40, // or create a special if there's none there
	MODE_COPY_SPECIAL = 41,
	MODE_PASTE = 42,
	MODE_ERASE_SPECIAL = 43,
	MODE_PLACE_SPECIAL = 44, // or set the special to be called if there's already one there
	MODE_EDIT_TOWN_ENTRANCE = 45, // for outdoors
	MODE_COPY_ITEM = 46,
	MODE_SET_OUT_START = 47,
	MODE_ERASE_CREATURE = 48,
	MODE_ERASE_ITEM = 49,
	MODE_COPY_TERRAIN = 50,
	// Modes below this do not have the terrain area visible.
	MODE_MAIN_SCREEN = 60,
	MODE_INTRO_SCREEN = 61,
	MODE_EDIT_TYPES = 62, // currently only used for editing terrain, but I'd like to use it for editing monsters and items too.
	MODE_EDIT_SPECIALS = 63, // editing a LIST of specials, not to be confused with MODE_EDIT_SPECIAL singular
	MODE_EDIT_SPECIAL_ITEMS = 64,
	MODE_EDIT_QUESTS = 65,
	MODE_EDIT_SHOPS = 66,
	MODE_EDIT_STRINGS = 67,
	MODE_EDIT_DIALOGUE = 68,
};

enum eDrawMode {
	DRAW_TERRAIN = 0,
	DRAW_MONST = 1,
	DRAW_ITEM = 2,
};

enum eStrMode {
	STRS_SCEN, STRS_OUT, STRS_TOWN, STRS_JOURNAL,
	STRS_OUT_SIGN, STRS_TOWN_SIGN,
	STRS_OUT_RECT, STRS_TOWN_RECT,
};

enum ePalBtn {
	PAL_BLANK = -1,
	PAL_PENCIL = 0, PAL_BRUSH_LG = 1, PAL_BRUSH_SM = 2, PAL_SPRAY_LG = 3, PAL_SPRAY_SM = 4, PAL_DROPPER = 5, PAL_RECT_HOLLOW = 6, PAL_RECT_FILLED = 7, PAL_BUCKET = 8,
	PAL_ZOOM = 10, PAL_ERASER = 11, PAL_EDIT_SIGN = 12, PAL_TEXT_AREA = 13, PAL_WANDER = 14, PAL_CHANGE = 15, PAL_TOWN_BORDER = 16, PAL_EDIT_TOWN = 17, PAL_EDIT_STORAGE = 18,
	PAL_EDIT_ITEM = 20, PAL_COPY_ITEM = 21, PAL_ERASE_ITEM = 22, PAL_SPEC = 23, PAL_COPY_SPEC = 24, PAL_PASTE = 25, PAL_ERASE_SPEC = 26, PAL_EDIT_SPEC = 27, PAL_START = 28,
	PAL_EDIT_MONST = 30, PAL_COPY_MONST = 31, PAL_ERASE_MONST = 32, PAL_ERASE_FIELD = 33, PAL_ENTER_N = 34, PAL_ENTER_W = 35, PAL_ENTER_S = 36, PAL_ENTER_E = 37, PAL_COPY_TER = 38,
	PAL_WEB = 40, PAL_CRATE = 41, PAL_BARREL = 42, PAL_FIRE_BARR = 43, PAL_FORCE_BARR = 44, PAL_QUICKFIRE = 45, PAL_SPEC_SPOT = 46, PAL_BLOCK = 47, PAL_FORCECAGE = 48,
	PAL_SFX_SB = 50, PAL_SFX_MB = 51, PAL_SFX_LB = 52, PAL_SFX_SS = 53, PAL_SFX_LS = 54, PAL_SFX_ASH = 55, PAL_SFX_BONE = 56, PAL_SFX_ROCK = 57, PAL_ROAD = 58,
	PAL_ARROW_UP = 9, PAL_ARROW_DOWN = 69, PAL_TERRAIN = 29, PAL_ITEM = 39, PAL_MONST = 49,
	PAL_BOAT = 60, PAL_HORSE = 61,
};

extern std::string& fetch_str(eStrMode str_mode, size_t which);
extern std::string edit_string_action_name(std::string what, eStrMode str_mode);
extern std::vector<std::string>& fetch_str_list(eStrMode str_mode);

#endif
