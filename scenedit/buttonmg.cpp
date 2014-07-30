#include <windows.h>
#include "string.h"
#include "stdio.h"

#include "global.h"
#include "graphics.h"
#include "math.h"
#include "buttonmg.h"

extern piles_of_stuff_dumping_type *data_store;
extern RECT right_sbar_rect;
extern RECT right_buttons[NRSONPAGE];
extern short current_rs_top;
extern short left_button_status[NLS]; // 0 - clear, 1 - text, 2 - title text, 3 - tabbed text, +10 - button
extern short right_button_status[NRS];
extern HWND right_sbar;
// 0 - clear
// 1000 + x - terrain type x
// 2000 + x - monster type x
// 3000 + x - item type x
// 4000 + x - global special node
// 5000 + x - out special node
// 6000 + x - town special node
// 7000 + x - global string x
// 8000 + x - out string x
// 9000 + x - town string x
// 10000 + x - scen. special item x
// 11000 + x - charter intro c
// 12000 + x - dialogue node x
// 13000 + x - basic dialogue node x


// for following, lb stands for left button(s)

void init_lb() {
	short i;
	for (i = 0; i < NLS; i++) {
		left_button_status[i] = 0;
		sprintf((char *) data_store->strings_ls[i], "");
		}
}

void reset_lb() {
	short i;
	for (i = 0; i < NLS; i++) {
		left_button_status[i] = 0;
		draw_lb_slot(i,0);
		}
}

// is slot >= 0, force that slot
// if -1, use 1st free slot
void set_lb(short slot, short mode, char const *label, short do_draw)
{
	short i;

	if (slot < 0) {
		for (i = 0; i < NLS; i++)
			if (left_button_status[i] == 0) {
				slot = i;
				i = NLS + 5000;
				}
		if (i < NLS + 5000)
			return;
		}
	left_button_status[slot] = mode;
	sprintf((char *)data_store->strings_ls[slot], "%-39.39s", label);
	data_store->strings_ls[slot][39] = 0;
	if (do_draw > 0)
		draw_lb_slot(slot,0);

}



void init_rb()
{
	short i;

    lpsi.fMask = SIF_POS;
    lpsi.nPos = 0;
    SetScrollInfo(right_sbar,SB_CTL,&lpsi,FALSE);
//	SetScrollPos(right_sbar,SB_CTL,0,FALSE);
    lpsi.fMask = SIF_RANGE;
    lpsi.nMax = 0;
    SetScrollInfo(right_sbar,SB_CTL,&lpsi,TRUE);
//	SetScrollRange(right_sbar,SB_CTL,0,0,TRUE);
	for (i = 0; i < NRS; i++) {
		right_button_status[i] = 0;
		sprintf((char *) data_store->strings_rs[i], "");
		}
}

void reset_rb()
{
	short i;

	for (i = 0; i < NRS; i++) {
		right_button_status[i] = 0;
		}
	draw_rb();
    lpsi.fMask = SIF_POS;
    lpsi.nPos = 0;
    SetScrollInfo(right_sbar,SB_CTL,&lpsi,FALSE);
//	SetScrollPos(right_sbar,SB_CTL,0,FALSE);
}

// is slot >= 0, force that slot
// if -1, use 1st free slot
void set_rb(short slot, short mode, char const *label, short do_draw)
{
	short i;

	if (slot < 0) {
		for (i = 0; i < NRS; i++)
			if (right_button_status[i] == 0) {
				slot = i;
				i = NRS + 5000;
				}
		if (i < NRS + 5000)
			return;
		}
	right_button_status[slot] = mode;
	sprintf((char *)data_store->strings_rs[slot], "%s", label);
	data_store->strings_rs[slot][39] = 0;
	for (i = 0; i < 39; i++)
		if (data_store->strings_rs[slot][i] == '|')
			data_store->strings_rs[slot][i] = ' ';
	if (do_draw > 0)
		draw_rb_slot(slot,0);

}
