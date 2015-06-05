
#include <cstring>
#include <cstdio>

#include "scen.global.h"
#include <array>
#include <string>
#include "graphtool.hpp"
#include "scen.graphics.h"
#include <cmath>
#include "scen.btnmg.h"
#include "scrollbar.hpp"

extern rectangle right_sbar_rect;

rectangle left_button[NLS];
extern rectangle right_buttons[NRSONPAGE];
rectangle right_scrollbar_rect;
extern short current_rs_top;

bool left_buttons_active = 1,right_buttons_active = 0;
extern std::array<lb_t,NLS> left_button_status;
extern std::array<rb_t,NRS> right_button_status;
extern std::shared_ptr<cScrollbar> right_sbar;

// for following, lb stands for left button(s)

void init_lb() {
	short i;
	for(i = 0; i < NLS; i++) {
		left_button_status[i] = {LB_CLEAR, LB_NO_ACTION, ""};
	}
}

void reset_lb() {
	short i;
	for(i = 0; i < NLS; i++) {
		left_button_status[i] = {LB_CLEAR, LB_NO_ACTION, ""};
		draw_lb_slot(i,0);
	}
}

// is slot >= 0, force that slot
// if -1, use 1st free slot
void set_lb(short slot, eLBMode mode, eLBAction action, std::string label, bool do_draw) {
	short i;
	
	if(slot < 0) {
		for(i = 0; i < NLS; i++)
			if(left_button_status[i].mode == LB_CLEAR) {
				slot = i;
				i = NLS + 5000;
			}
		if(i < NLS + 5000)
			return;
	}
	left_button_status[slot].mode = mode;
	left_button_status[slot].action = action;
	left_button_status[slot].label = label;
	if(do_draw)
		draw_lb_slot(slot,0);
	
}



void init_rb() {
	short i;
	
	right_sbar->setPosition(0);
	for(i = 0; i < NRS; i++) {
		right_button_status[i] = {RB_CLEAR, 0, ""};
	}
}

void reset_rb() {
	short i;
	
	for(i = 0; i < NRS; i++) {
		right_button_status[i] = {RB_CLEAR, 0, ""};
	}
	draw_rb();
	right_sbar->setMaximum(0);
	right_sbar->setPosition(0);
}

// is slot >= 0, force that slot
// if -1, use 1st free slot
void set_rb(short slot, eRBAction action, int n, std::string label, bool do_draw) {
	short i;
	
	if(slot < 0) {
		for(i = 0; i < NRS; i++)
			if(right_button_status[i].action == RB_CLEAR) {
				slot = i;
				i = NRS + 5000;
			}
		if(i < NRS + 5000)
			return;
	}
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

