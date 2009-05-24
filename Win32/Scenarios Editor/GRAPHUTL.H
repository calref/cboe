HBITMAP load_pict(short pict_num);
void rect_draw_some_item(HBITMAP src,RECT src_rect,HBITMAP dest,RECT dest_rect,
	short trans, short main_win);
void fry_dc(HWND hwnd,HDC dc);
HBITMAP load_pict(short pict_num,HDC model_hdc);
void DisposeGWorld(HBITMAP bitmap);
void paint_pattern(HBITMAP dest,short which_mode,RECT dest_rect,short which_pattern);
HBITMAP ReadBMP(char * fileName);
