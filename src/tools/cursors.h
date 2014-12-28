//
//  cursors.h
//  BoE
//
//  Created by Celtic Minstrel on 03/06/09.
//  Wrappers for NSCursor, based on an Apple example
//

#ifndef BOE_CURSORS_H
#define BOE_CURSORS_H

enum cursor_type {
	wand_curs = 0,
	eyedropper_curs = 1,
	brush_curs = 2,
	spray_curs = 3,
	eraser_curs = 4,
	topleft_curs = 5,
	bottomright_curs = 6,
	hand_curs = 7,
	NW_curs = 8,
	N_curs = 9,
	NE_curs = 10,
	W_curs = 11,
	wait_curs = 12,
	E_curs = 13,
	SW_curs = 14,
	S_curs = 15,
	SE_curs = 16,
	sword_curs = 17,
	boot_curs = 18,
	drop_curs = 19,
	target_curs = 20,
	talk_curs = 21,
	key_curs = 22,
 	look_curs = 23,
	watch_curs,
	text_curs, // Keep this one last
};

class Cursor {
	void* ptr;
public:
	Cursor(const char* imgPath, float hotSpotX, float hotSpotY);
	~Cursor();
	void apply();
};

// This hides the cursor until it moves
void obscureCursor();

void set_cursor(cursor_type which_curs);
void restore_cursor();

#endif
