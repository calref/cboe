#include <array>
#include <memory>
#include <unordered_map>
#include <vector>

#include "scen.global.hpp"
#include "scrollbar.hpp"

enum eLBAction {
	LB_NO_ACTION,
	LB_NEW_SCEN,
	LB_LOAD_SCEN,
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
	RB_MONST = 2,
	RB_ITEM = 3,
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
	
	lb_t(eLBMode newMode=LB_CLEAR, eLBAction newAction=LB_NO_ACTION, std::string const &newLabel="")
	: mode(newMode)
	, action(newAction)
	, label(newLabel) {
	}
};

struct rb_t {
	eRBAction action;
	int i;
	std::string label;
};

class iEventListener;
class cScenButtonsBars {
public:
	void init();
	void show_palette_bar(bool show=true) {
		if (!palette_bar)
			return;
		if (show)
			palette_bar->show();
		else
			palette_bar->hide();
	}
	void show_right_bar(bool show=true) {
		if (!right_bar)
			return;
		if (show)
			right_bar->show();
		else
			right_bar->hide();
	}

	void draw_left_buttons();
	void draw_left_slot(short which,short mode);
	void set_left_button(short slot, eLBMode mode, eLBAction action, std::string const &label, bool do_draw = false);
	void reset_left_buttons();

	void draw_right_buttons() const;
	void draw_right_slot(short which,short mode) const ;
	void set_right_button(short slot, eRBAction action, int i, std::string const &label, bool do_draw = false);
	void reset_right_bar_and_buttons();
	void reset_right(long newMaximum);
	
	void draw_palette(bool editing_town) const;
	
	void set_startup_screen();
	void set_main_screen(location const &cur_out, int cur_town, std::string const &town_name);
	bool handle_one_event(const sf::Event& event);

protected:
	void init_bar(std::shared_ptr<cScrollbar>& sbar, const std::string& name, rectangle const &rect, rectangle const &events_rect, int pgSz);

public:
	std::array<lb_t,NLS> left_buttons;
	std::array<rectangle[2],NLS> left_buttons_rectangles;  // 0 - whole, 1 - blue button
	rectangle terrain_rectangle; // the terrain main rectangle
	rectangle terrain_border_rects[4]; // border rects order: top, left, bottom, right
	rectangle terrain_rects[256];
	rectangle terrain_buttons_rect;
	rectangle const terrain_base_small_button_from = {120,0,127,7};

	// right: one big part
	std::vector<rb_t> right_buttons;
	std::array<rectangle,NRSONPAGE> right_buttons_rectangles;
	std::shared_ptr<cScrollbar> right_bar;
	
	// right: two part
	// ---- top -------
	std::shared_ptr<cScrollbar> palette_bar; // big and half size right scrollbar

	// ---- bottom -------
	rectangle palette_buttons[10][6];
	static ePalBtn out_buttons[6][10], town_buttons[6][10];
protected:
	rectangle palette_button_base;

protected:
	std::unordered_map<std::string, std::shared_ptr<iEventListener>> event_listeners;
};

extern cScenButtonsBars scen_controls;
