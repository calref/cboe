
#include <cstring>
#include <cstdio>

#include "scen.global.hpp"
#include <array>
#include <string>
#include "scen.graphics.hpp"
#include <cmath>
#include "scen.btnmg.hpp"
#include "scrollbar.hpp"

extern rectangle right_sbar_rect;

extern rectangle right_buttons[NRSONPAGE];
rectangle right_scrollbar_rect;
extern short current_rs_top;

bool left_buttons_active = 1,right_buttons_active = 0;
extern std::array<lb_t,NLS> left_button_status;
extern std::vector<rb_t> right_button_status;
extern std::shared_ptr<cScrollbar> right_sbar;

// for following, lb stands for left button(s)

void init_lb() {
	for(short i = 0; i < NLS; i++) {
		left_button_status[i] = {LB_CLEAR, LB_NO_ACTION, ""};
	}
}

void reset_lb() {
	for(short i = 0; i < NLS; i++) {
		left_button_status[i] = {LB_CLEAR, LB_NO_ACTION, ""};
		draw_lb_slot(i,0);
	}
}

// is slot >= 0, force that slot
// if -1, use 1st free slot
void set_lb(short slot, eLBMode mode, eLBAction action, std::string label, bool do_draw) {
	if(slot < 0) {
		auto iter = std::find_if(left_button_status.begin(), left_button_status.end(), [](const lb_t& btn) {
			return btn.mode == LB_CLEAR;
		});
		if(iter == left_button_status.end())
			return;
		slot = iter - left_button_status.begin();
	}
	left_button_status[slot].mode = mode;
	left_button_status[slot].action = action;
	left_button_status[slot].label = label;
	if(do_draw)
		draw_lb_slot(slot,0);
	
}



void init_rb() {
	right_sbar->setPosition(0);
	right_button_status.clear();
}

void reset_rb() {
	right_button_status.clear();
	draw_rb();
	right_sbar->setMaximum(0);
	right_sbar->setPosition(0);
}

// is slot >= 0, force that slot
// if -1, use 1st free slot
void set_rb(short slot, eRBAction action, int n, std::string label, bool do_draw) {
	if(slot < 0) {
		for(short i = 0; i < NRS; i++)
			if(right_button_status[i].action == RB_CLEAR) {
				slot = i;
				break;
			}
	}
	if(slot >= NRS)
		right_button_status.resize(slot + 1);
	right_button_status[slot].action = action;
	right_button_status[slot].i = n;
	right_button_status[slot].label = label;
	for(char& c : right_button_status[slot].label) {
		if(c == '|')
			c = ' ';
	}
	if(do_draw)
		draw_rb_slot(slot,0);
}

