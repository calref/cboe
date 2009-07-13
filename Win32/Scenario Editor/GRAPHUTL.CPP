#define xmin(a,b)	((a) < (b) ?  (a) : (b))

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "global.h"

#include "KEYDLGS.h"
#include "edsound.h"

#include "graphutl.h"

extern HWND mainPtr;


extern HDC main_dc,main_dc2,main_dc3;
extern short ulx,uly;
extern HBITMAP mixed_gworld,dialog_pattern_gworld,pattern_gworld,status_pattern_gworld;
extern Boolean suppress_load_file_name;

extern char scen_strs[160][256];
extern char scen_strs2[110][256];
extern char talk_strs[170][256];
extern char town_strs[180][256];
extern char file_path_name[256];
extern char szFileName[128];


BOOL pal_ok = FALSE;
Boolean syscolors_stored = FALSE;
	int elements[5] = {COLOR_ACTIVEBORDER,COLOR_ACTIVECAPTION,
			COLOR_WINDOWFRAME,COLOR_SCROLLBAR,COLOR_BTNFACE};
	COLORREF store_element_colors[5];
short dlog_pat_placed = 0;
short current_pattern = -1;

/*void get_filename(char * scen_name){

    char buffer[128];
    int i=strlen(szFileName),j=0;
    
    while(szFileName[i] != '\\')
        i--;

    while(szFileName[i+j] != '.'){
       buffer[j]=szFileName[i+j+1];
       j++;
       }
    buffer[j]='b';
    buffer[j+1]='m';
    buffer[j+2]='p';
    buffer[j+3]='\0';
    sprintf(scen_name,"scenarios/%s",buffer);
}*/

HBITMAP load_pict(short pict_num)
{
	HBITMAP got_bitmap;
	HDC model_hdc;

	model_hdc = main_dc;

	switch(pict_num) {
		case 700: got_bitmap = ReadBMP("Images/STATAREA.BMP"); break;
		case 701: got_bitmap = ReadBMP("Images/STATAREA.BMP"); break;
		case 702: got_bitmap = ReadBMP("Images/STATAREA.BMP"); break;
		case 703: got_bitmap = ReadBMP("Images/TEXTBAR.BMP"); break;
		case 704: got_bitmap = ReadBMP("Images/BUTTONS.BMP"); break;
		case 705: got_bitmap = ReadBMP("Images/TERSCRN.BMP"); break;
		case 800: got_bitmap = ReadBMP("Images/TER1.BMP"); break;
		case 801: got_bitmap = ReadBMP("Images/TER2.BMP"); break;
		case 802: got_bitmap = ReadBMP("Images/TER3.BMP"); break;
		case 803: got_bitmap = ReadBMP("Images/TER4.BMP"); break;
		case 804: got_bitmap = ReadBMP("Images/TER5.BMP"); break;
		case 805: got_bitmap = ReadBMP("Images/TER6.BMP"); break;
		case 820: got_bitmap = ReadBMP("Images/TERANIM.BMP"); break;
		case 821: got_bitmap = ReadBMP("Images/FIELDS.BMP"); break;
		case 830: got_bitmap = ReadBMP("Images/STARTUP.BMP"); break;
		case 850: got_bitmap = ReadBMP("Images/DLOGPICS.BMP"); break;
		case 851: got_bitmap = ReadBMP("Images/SCENPICS.BMP"); break;
		case 860: got_bitmap = ReadBMP("Images/TALKPORT.BMP"); break;
		case 880: got_bitmap = ReadBMP("Images/MISSILES.BMP"); break;
		case 900: got_bitmap = ReadBMP("Images/TINYOBJ.BMP"); break;
		case 901: got_bitmap = ReadBMP("Images/OBJECTS.BMP"); break;
		case 902: got_bitmap = ReadBMP("Images/PCS.BMP"); break;
		case 905: got_bitmap = ReadBMP("Images/PCS.BMP"); break;
		case 903: got_bitmap = ReadBMP("Images/MIXED.BMP"); break;
		case 904: got_bitmap = ReadBMP("Images/MIXED.BMP"); break;
		case 906: got_bitmap = ReadBMP("Images/EDBTNS.BMP"); break;
		case 910: got_bitmap = ReadBMP("Images/BIGSCEN.BMP"); break;
		case 911: got_bitmap = ReadBMP("Images/BIGSCEN.BMP"); break;
		case 912: got_bitmap = ReadBMP("Images/BIGSCEN.BMP"); break;
		case 1100: case 1200: got_bitmap = ReadBMP("Images/MONST1.BMP"); break;
		case 1101: case 1201: got_bitmap = ReadBMP("Images/MONST2.BMP"); break;
		case 1102: case 1202: got_bitmap = ReadBMP("Images/MONST3.BMP"); break;
		case 1103: case 1203: got_bitmap = ReadBMP("Images/MONST4.BMP"); break;
		case 1104: case 1204: got_bitmap = ReadBMP("Images/MONST5.BMP"); break;
		case 1105: case 1205: got_bitmap = ReadBMP("Images/MONST6.BMP"); break;
		case 1106: case 1206: got_bitmap = ReadBMP("Images/MONST7.BMP"); break;
		case 1107: case 1207: got_bitmap = ReadBMP("Images/MONST8.BMP"); break;
		case 1108: case 1208: got_bitmap = ReadBMP("Images/MONST9.BMP"); break;
		case 1109: case 1209: got_bitmap = ReadBMP("Images/MONST10.BMP"); break;
		case 1400: got_bitmap = ReadBMP("Images/STSCICON.BMP"); break;
		case 1401: got_bitmap = ReadBMP("Images/HELPPICS.BMP"); break;
		case 1402: got_bitmap = ReadBMP("Images/APPIC.BMP"); break;
		case 1500: case 1501: case 1502: case 1503: case 1504: case 1505: case 1506: case 1507: 
			got_bitmap = ReadBMP("Images/BIGMAPS.BMP"); break;
		case 2000: got_bitmap = ReadBMP("Images/DLOGBTNS.BMP"); break;
		case 3000: got_bitmap = ReadBMP("Images/START.BMP"); break;
		case 3001: got_bitmap = ReadBMP("Images/SPIDLOGO.BMP"); break;
		case 3002: got_bitmap = ReadBMP("Images/EDSTART.BMP"); break;
		case 5000: got_bitmap = ReadBMP("Images/E3EDTITL.BMP"); break;
		
		default: got_bitmap = NULL;
		}
	return got_bitmap;
}

void rect_draw_some_item(HBITMAP src,RECT src_rect,HBITMAP dest,RECT dest_rect,
	short trans, short main_win) {
	HDC hdcMem,hdcMem2,hdcMem3,destDC,storeDC;
	HBITMAP transbmp;
	COLORREF white = RGB(255,255,255),black = RGB(0,0,0),oldcolor;
	RECT debug = {0,0,200,20};
	HBRUSH hbrush,old_brush;
	COLORREF x = RGB(17,17,17);
	HBITMAP store,store2;
	Boolean dlog_draw = FALSE;

	main_dc2 = CreateCompatibleDC(main_dc);
	SetMapMode(main_dc2,GetMapMode((HDC)mainPtr));
	main_dc3 = CreateCompatibleDC(main_dc);
	SetMapMode(main_dc3,GetMapMode((HDC)mainPtr));
	SetStretchBltMode(main_dc2,STRETCH_DELETESCANS);
	SetStretchBltMode(main_dc3,STRETCH_DELETESCANS);

	if (main_win == 2) {
		destDC = (HDC) dest;
		main_win = 1;
		dlog_draw = TRUE;
		hdcMem = CreateCompatibleDC(destDC);
		SelectObject(hdcMem, src);
		SetMapMode(hdcMem,GetMapMode((HDC)mainPtr));
		SetStretchBltMode(hdcMem,STRETCH_DELETESCANS);
		}
		else {
			destDC = main_dc;
			hdcMem = main_dc2;
			store = (HBITMAP)SelectObject(hdcMem,src);
			}


	SetStretchBltMode(destDC,STRETCH_DELETESCANS);
	SetStretchBltMode(hdcMem,STRETCH_DELETESCANS);
	if (trans != 1) {
		if (main_win == 0) { // Not transparent, into bitmap
			hdcMem2 = main_dc3;
			store2 = (HBITMAP)SelectObject(hdcMem2, dest);

			SetStretchBltMode(hdcMem2,STRETCH_DELETESCANS);

				StretchBlt(hdcMem2,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
				dest_rect.bottom - dest_rect.top,
				hdcMem,src_rect.left,src_rect.top,src_rect.right - src_rect.left,
				src_rect.bottom - src_rect.top,(trans >= 0) ? SRCCOPY : SRCAND);
			SelectObject(hdcMem2,store2);
			}

		else { // Not transparent, onto screen
		if (trans == 2) {
			hbrush = CreateSolidBrush(x);
			old_brush = (HBRUSH)SelectObject(destDC,hbrush);
			//SelectObject(hdcMem,hbrush);
			}
		if (dlog_draw == FALSE)
			SetViewportOrgEx(destDC,ulx,uly,NULL);

		SetStretchBltMode(destDC,STRETCH_DELETESCANS);
		SetStretchBltMode(hdcMem,STRETCH_DELETESCANS);
		StretchBlt(destDC,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
			dest_rect.bottom - dest_rect.top,
			hdcMem,src_rect.left,src_rect.top,src_rect.right - src_rect.left,
			src_rect.bottom - src_rect.top,(trans == 0) ? SRCCOPY : MERGECOPY);
			if (trans == 2) {
				SelectObject(destDC,old_brush);
				if (DeleteObject(hbrush) == 0)
					play_sound(1);
				}
		if (dlog_draw == FALSE)
			SetViewportOrgEx(destDC,0,0,NULL);


		}
		} // end of non-transparent draws
		else {
		if (main_win == 0) {
			hdcMem3 = CreateCompatibleDC(hdcMem);
			SelectObject(hdcMem3, dest);
			SetMapMode(hdcMem3,GetMapMode((HDC)mainPtr));
			transbmp = CreateBitmap(src_rect.right - src_rect.left,
						src_rect.bottom - src_rect.top,1,1,NULL);
		hdcMem2 = CreateCompatibleDC(destDC);
		SelectObject(hdcMem2, transbmp);
		oldcolor = SetBkColor(hdcMem, white);
		StretchBlt(hdcMem2,0,0,dest_rect.right - dest_rect.left,
			dest_rect.bottom - dest_rect.top,
			hdcMem,src_rect.left,src_rect.top,src_rect.right - src_rect.left,
			src_rect.bottom - src_rect.top,SRCCOPY);
		SetBkColor(hdcMem, oldcolor);

		StretchBlt(hdcMem3,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
			dest_rect.bottom - dest_rect.top,
			hdcMem,src_rect.left,src_rect.top,src_rect.right - src_rect.left,
			src_rect.bottom - src_rect.top,SRCINVERT);
		StretchBlt(hdcMem3,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
			dest_rect.bottom - dest_rect.top,
			hdcMem2,0,0,src_rect.right - src_rect.left,
			src_rect.bottom - src_rect.top,SRCAND);
		StretchBlt(hdcMem3,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
			dest_rect.bottom - dest_rect.top,
			hdcMem,src_rect.left,src_rect.top,src_rect.right - src_rect.left,
			src_rect.bottom - src_rect.top,SRCINVERT);
		DeleteDC(hdcMem3);
		DeleteDC(hdcMem2);

			DeleteObject(transbmp);
		}
		else {
			if (dlog_draw == FALSE)
				SetViewportOrgEx(destDC,ulx,uly,NULL);
			transbmp = CreateBitmap(src_rect.right - src_rect.left,
						src_rect.bottom - src_rect.top,1,1,NULL);
			hdcMem2 = CreateCompatibleDC(destDC);
			SelectObject(hdcMem2, transbmp);
			oldcolor = SetBkColor(hdcMem, white);
			StretchBlt(hdcMem2,0,0,dest_rect.right - dest_rect.left,
				dest_rect.bottom - dest_rect.top,
				hdcMem,src_rect.left,src_rect.top,src_rect.right - src_rect.left,
				src_rect.bottom - src_rect.top,SRCCOPY);

			SetBkColor(hdcMem, oldcolor);

			StretchBlt(destDC,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
				dest_rect.bottom - dest_rect.top,
				hdcMem,src_rect.left,src_rect.top,src_rect.right - src_rect.left,
				src_rect.bottom - src_rect.top,SRCINVERT);
			StretchBlt(destDC,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
				dest_rect.bottom - dest_rect.top,
				hdcMem2,0,0,src_rect.right - src_rect.left,
				src_rect.bottom - src_rect.top,SRCAND);
			StretchBlt(destDC,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
				dest_rect.bottom - dest_rect.top,
				hdcMem,src_rect.left,src_rect.top,src_rect.right - src_rect.left,
				src_rect.bottom - src_rect.top,SRCINVERT);
			if (dlog_draw == FALSE)
				SetViewportOrgEx(destDC,0,0,NULL);
			DeleteDC(hdcMem2);

				DeleteObject(transbmp);


			}
			}
	if (dlog_draw == TRUE)
		DeleteDC(hdcMem);
		else SelectObject(hdcMem,store);
	DeleteDC(main_dc2);
	DeleteDC(main_dc3);
}

 void fry_dc(HWND hwnd,HDC dc) {
	if (ReleaseDC(hwnd,dc) == 0)
	  //	add_string_to_buf("xx");
		PostQuitMessage(0);
 }

// which_mode is 0 ... dest is a bitmap
// is 1 ... ignore dest ... paint on mainPtr
// is 2 ... dest is a dialog, use the dialog pattern
// both pattern gworlds are 192 x 256
void paint_pattern(HBITMAP dest,short which_mode,RECT dest_rect,short which_pattern)
{
	HBITMAP source_pat;
	RECT pattern_source = {32,168,96,232}, pat_dest_orig = {0,0,64,64},pat_dest;
	short i,j;

	RECT draw_from_orig = {0,0,192,256},draw_from,draw_to;
	short store_ulx,store_uly;

	if (which_mode == 1)
		OffsetRect(&dest_rect,ulx, uly);
		else if (which_mode == 3)
      	which_mode = 1;
	if (which_mode == 2) {
		source_pat = dialog_pattern_gworld;
		if (dlog_pat_placed == 0) {
			dlog_pat_placed = 1;
			OffsetRect(&pattern_source, 64 * 2,0);
			for (i = 0; i < 3; i++)
				for (j = 0; j < 4; j++) {
					pat_dest = pat_dest_orig;
					OffsetRect(&pat_dest,64 * i, 64 * j);
					rect_draw_some_item(mixed_gworld,pattern_source,
						dialog_pattern_gworld,pat_dest,0,0);
					}
			}
		}
		else {
			source_pat = pattern_gworld;
			if (current_pattern != which_pattern) {
				current_pattern = which_pattern;
				OffsetRect(&pattern_source, 64 * (which_pattern % 5),
					64 * (which_pattern / 5));
				for (i = 0; i < 3; i++)
					for (j = 0; j < 4; j++) {
						pat_dest = pat_dest_orig;
						OffsetRect(&pat_dest,64 * i, 64 * j);
						rect_draw_some_item(mixed_gworld,pattern_source,
							pattern_gworld,pat_dest,0,0);
						}
				}
			}

	// now patterns are loaded, so have fun
	// first nullify ul shifting
	store_ulx = ulx;
	store_uly = uly;
	ulx = uly = 0;
	for (i = 0; i < (dest_rect.right / 192) + 1; i++)
		for (j = 0; j < (dest_rect.bottom / 256) + 1; j++) {
			draw_to = draw_from_orig;
			OffsetRect(&draw_to,192 * i, 256 * j);
			IntersectRect(&draw_to,&draw_to,&dest_rect);
			if (draw_to.right != 0) {
				draw_from = draw_to;
				OffsetRect(&draw_from, -192 * i, -256 * j);
				switch (which_mode) {
					case 0:
						rect_draw_some_item(source_pat,draw_from,
							dest,draw_to,0,0); break;
					case 1:
						rect_draw_some_item(source_pat,draw_from,
							source_pat,draw_to,0,1); break;
					case 2:
						rect_draw_some_item(source_pat,draw_from,
							dest,draw_to,0,2); break;
					}
				}
			}
	ulx = store_ulx;
   uly = store_uly;
}

HBITMAP ReadBMP(char * fileName)
{
	return (HBITMAP) LoadImage(0, fileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}
