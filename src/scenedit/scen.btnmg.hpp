
enum eLBAction {
	LB_NO_ACTION,
	LB_NEW_SCEN,
	LB_LOAD_SCEN,
	LB_LOAD_LAST,
	LB_EDIT_TER,
	LB_EDIT_MONST,
	LB_EDIT_ITEM,
	LB_NEW_TOWN,
	LB_EDIT_TEXT,
	LB_EDIT_SPECITEM,
	LB_EDIT_QUEST,
	LB_EDIT_SHOPS,
	LB_LOAD_OUT,
	LB_EDIT_OUT,
	LB_LOAD_TOWN,
	LB_EDIT_TOWN,
	LB_EDIT_TALK,
	LB_RETURN,
};

enum eRBAction {
	RB_CLEAR = 0,
	RB_SCEN_SPEC = 4,
	RB_OUT_SPEC = 5,
	RB_TOWN_SPEC = 6,
	RB_SCEN_STR = 7,
	RB_OUT_STR = 8,
	RB_TOWN_STR = 9,
	RB_SPEC_ITEM = 10,
	RB_JOURNAL = 11,
	RB_DIALOGUE = 12,
	RB_PERSONALITY = 13,
	RB_OUT_SIGN = 14,
	RB_TOWN_SIGN = 15,
	RB_QUEST = 16,
	RB_SHOP = 17,
	RB_OUT_RECT = 18,
	RB_TOWN_RECT = 19,
};

enum eLBMode {
	LB_CLEAR = 0,
	LB_TEXT = 1,
	LB_TITLE = 2,
	LB_INDENT = 3
};

struct lb_t {
	eLBMode mode;
	eLBAction action;
	std::string label;
};

struct rb_t {
	eRBAction action;
	int i;
	std::string label;
};

void init_lb();
void reset_lb();
void set_lb(short slot, eLBMode mode, eLBAction action, std::string label, bool do_draw = false);
void init_rb() ;
void reset_rb() ;
void set_rb(short slot, eRBAction action, int i, std::string label, bool do_draw = false);
