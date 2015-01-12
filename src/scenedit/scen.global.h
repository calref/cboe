
#define DRAG_EDGE		15

#define	DISPLAY_LEFT	23
#define	DISPLAY_TOP		23
#define	BITMAP_WIDTH	28
#define	BITMAP_HEIGHT	36
#define	NUM_BUTTONS		15

#define	NUM_DLOG_B		53

#define NLS	25
// number of left slots for buttons
#define	NRS	400
// number of right slots for scrolling list
#define NRSONPAGE	33
// number of right slots for scrolling list on page at 1 time

#define	RIGHT_AREA_UL_X	287
#define	RIGHT_AREA_UL_Y	5
#define	RIGHT_AREA_WIDTH	290
#define	RIGHT_AREA_HEIGHT	400

#define	TER_RECT_UL_X	6
#define	TER_RECT_UL_Y	19 // Formerly 25

enum eScenMode {
	MODE_DRAWING = 0,
	MODE_SET_WANDER_POINTS = 2,
	MODE_ROOM_RECT = 3,
	MODE_PLACE_ITEM = 4,
	MODE_EDIT_ITEM = 5,
	MODE_TOGGLE_SPECIAL_DOT = 6, // I suspect that's what this was for, anyway – it currently does nothing
	MODE_SET_TOWN_START = 7, // unused; for something I'd like to add
	MODE_PLACE_SAME_CREATURE = 8,
	MODE_SET_TOWN_RECT = 9,
	MODE_PLACE_NORTH_ENTRANCE = 10,
	MODE_PLACE_EAST_ENTRANCE = 11,
	MODE_PLACE_SOUTH_ENTRANCE = 12,
	MODE_PLACE_WEST_ENTRANCE = 13,
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
	MODE_PASTE_SPECIAL = 42,
	MODE_ERASE_SPECIAL = 43,
	MODE_PLACE_SPECIAL = 44, // or set the special to be called if there's already one there
	MODE_EDIT_TOWN_ENTRANCE = 45, // for outdoors
	MODE_PLACE_SAME_ITEM = 46,
	MODE_SET_OUT_START = 47,
	MODE_ERASE_CREATURE = 48,
	MODE_ERASE_ITEM = 49,
	MODE_MAIN_SCREEN = 60,
	MODE_INTRO_SCREEN = 61,
	MODE_EDIT_TYPES = 62, // currently only used for editing terrain, but I'd like to use it for editing monsters and items too.
};

enum eDrawMode {
	DRAW_TERRAIN = 0,
	DRAW_MONST = 1,
	DRAW_ITEM = 2,
};
