#ifndef _GRAPHUTIL_H
	#define _GRAPHUTIL_H

void rect_draw_some_item(HBITMAP src, RECT16 src_rect, HBITMAP dest, RECT16 dest_rect,
	short trans, short main_win);
void fry_dc(HWND hwnd,HDC dc);

void DisposeGWorld(HBITMAP bitmap);
HBITMAP load_pict(short pict_num,HDC);
void paint_pattern(HBITMAP dest,short which_mode,RECT dest_rect,short which_pattern);

/* GK */
extern HBITMAP ReadBMP(char * fileName);
extern HBITMAP ReadScenFile(char* filename);

#endif
