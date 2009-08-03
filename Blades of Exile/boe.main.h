#ifndef _BLADES_H
	#define _BLADES_H
	
#include "classes/pc.h"			// party_record_type
#include "global.h"		// structs

void move_sound(unsigned char ter,short step);
void check_game_done();
Boolean handle_menu (short item, HMENU menu);
void load_cursors();
void change_cursor(POINT where_curs);
void cursor_go();
void cursor_stay();
long CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

#endif
