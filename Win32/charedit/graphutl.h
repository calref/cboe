#ifndef _GRAPHUTL_H
	#define _GRAPHUTL_H

HBITMAP load_pict(short pict_num,HDC model_hdc);
void rect_draw_some_item(HBITMAP src,RECT src_rect,HBITMAP dest,RECT dest_rect,
	short trans, short main_win) ;
	void fry_dc(HWND hwnd,HDC dc) ;
	void DisposeGWorld(HBITMAP bitmap);
void SectRect(RECT *a, RECT *b, RECT *c); 
Boolean Button();
void paint_pattern(HBITMAP dest,short which_mode,RECT dest_rect,short which_pattern);

/* GK */
HBITMAP ReadBMP(char * fileName);

#endif
