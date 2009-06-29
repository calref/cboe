#define xmin(a,b)	((a) < (b) ?  (a) : (b))

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <stdlib.h>
#include <stdio.h>
#include "global.h"
#include "text.h"
#include "string.h"
#include "exlsound.h"

#include "graphutl.h"
#include "globvar.h"

void rect_draw_some_item(HBITMAP src, RECT16 src_rect,HBITMAP dest, RECT16 dest_rect,
	short trans, short main_win)
{
	HDC hdcMem,hdcMem2,hdcMem3,destDC;
	HBITMAP transbmp;
	COLORREF white = RGB(255,255,255),oldcolor;
	HBRUSH hbrush,old_brush;
	COLORREF x = RGB(17,17,17);
	HBITMAP store,store2;
	Boolean dlog_draw = FALSE;

	if (main_win == 2)
	{
		destDC = (HDC) dest;
		main_win = 1;
		dlog_draw = TRUE;
		hdcMem = CreateCompatibleDC(destDC);
		SelectObject(hdcMem, src);
		SetMapMode(hdcMem,GetMapMode((HDC) mainPtr));
	}
	else
	{
		destDC = main_dc;
		hdcMem = main_dc2;
		store = (HBITMAP) SelectObject(hdcMem,src);
	}

	if (trans != 1)
	{
		if (main_win == 0)
		{ // Not transparent, into bitmap
			hdcMem2 = main_dc3;
			store2 = (HBITMAP) SelectObject(hdcMem2, dest);
			StretchBlt(hdcMem2,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
				dest_rect.bottom - dest_rect.top,
				hdcMem,src_rect.left,src_rect.top,src_rect.right - src_rect.left,
				src_rect.bottom - src_rect.top,(trans >= 0) ? SRCCOPY : SRCAND);
			SelectObject(hdcMem2,store2);
		}
		else
		{ // Not transparent, onto screen
		
			if (trans == 2)
			{
				hbrush = CreateSolidBrush(x);
				old_brush = (HBRUSH) SelectObject(destDC,hbrush);
			}
				
			if (dlog_draw == FALSE)	SetViewportOrgEx(destDC,ulx,uly,NULL);
	
			StretchBlt(destDC,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
				dest_rect.bottom - dest_rect.top,
				hdcMem,src_rect.left,src_rect.top,src_rect.right - src_rect.left,
				src_rect.bottom - src_rect.top,(trans == 0) ? SRCCOPY : MERGECOPY);
				if (trans == 2)
				{
					SelectObject(destDC,old_brush);
					if (DeleteObject(hbrush) == 0)	play_sound(1);
				}
				
			if (dlog_draw == FALSE)	SetViewportOrgEx(destDC,0,0,NULL);
		}
		
	} // end of non-transparent draws
	else
	{
		if (main_win == 0)
		{
			hdcMem3 = CreateCompatibleDC(hdcMem);
			SelectObject(hdcMem3, dest);
			SetMapMode(hdcMem3,GetMapMode((HDC) mainPtr));
			if ((src_rect.right - src_rect.left < 72) &&
				(src_rect.bottom - src_rect.top < 72))
					transbmp = bw_bitmap;
			else transbmp = CreateBitmap(src_rect.right - src_rect.left,
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
			hdcMem2,0,0,dest_rect.right - dest_rect.left,
			dest_rect.bottom - dest_rect.top,SRCAND);
		StretchBlt(hdcMem3,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
			dest_rect.bottom - dest_rect.top,
			hdcMem,src_rect.left,src_rect.top,src_rect.right - src_rect.left,
			src_rect.bottom - src_rect.top,SRCINVERT);
		DeleteDC(hdcMem3);
		DeleteDC(hdcMem2);
		if (transbmp != bw_bitmap) DeleteObject(transbmp);
		}
		else
		{
			if (dlog_draw == FALSE)	SetViewportOrgEx(destDC,ulx,uly,NULL);
			if ((src_rect.right - src_rect.left < 72) &&
				(src_rect.bottom - src_rect.top < 72))
					transbmp = bw_bitmap;
			else transbmp = CreateBitmap(src_rect.right - src_rect.left,
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
				hdcMem2,0,0,dest_rect.right - dest_rect.left,
				dest_rect.bottom - dest_rect.top,SRCAND);
				
			StretchBlt(destDC,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
				dest_rect.bottom - dest_rect.top,
				hdcMem,src_rect.left,src_rect.top,src_rect.right - src_rect.left,
				src_rect.bottom - src_rect.top,SRCINVERT);
				
			if (dlog_draw == FALSE)	SetViewportOrgEx(destDC,0,0,NULL);
			
			DeleteDC(hdcMem2);
			
			if (transbmp != bw_bitmap)	DeleteObject(transbmp);
			}
			}
	if (dlog_draw == TRUE) DeleteDC(hdcMem);
	else SelectObject(hdcMem,store);
}

void fry_dc(HWND hwnd,HDC dc)
{	
	if (ReleaseDC(hwnd,dc) == 0)
		DebugQuit("Cannot release DC in fry_dc");
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

HBITMAP load_pict(short pict_num,HDC)
{
	HBITMAP got_bitmap;

	switch(pict_num) {
		case 700:
		case 701:
		case 702: got_bitmap = ReadBMP("images/STATAREA.BMP"); break;
		case 703: got_bitmap = ReadBMP("images/TEXTBAR.BMP"); break;
		case 704: got_bitmap = ReadBMP("images/BUTTONS.BMP"); break;
		case 705: got_bitmap = ReadBMP("images/TERSCRN.BMP"); break;
		case 800: if(party.stuff_done[307][0] == 0)
                    got_bitmap = ReadBMP("images/TER1.BMP");
                  else
                    got_bitmap = ReadBMP("images/TER1_D.BMP");
                    break;
		case 801: if(party.stuff_done[307][0] == 0)
                    got_bitmap = ReadBMP("images/TER2.BMP");
                  else
                    got_bitmap = ReadBMP("images/TER2_D.BMP");
                    break;
		case 802: if(party.stuff_done[307][0] == 0)
                    got_bitmap = ReadBMP("images/TER3.BMP");
                  else
                    got_bitmap = ReadBMP("images/TER3_D.BMP");
                    break;
		case 803: if(party.stuff_done[307][0] == 0)
                    got_bitmap = ReadBMP("images/TER4.BMP");
                  else
                    got_bitmap = ReadBMP("images/TER4_D.BMP");
                    break;
		case 804: if(party.stuff_done[307][0] == 0)
                    got_bitmap = ReadBMP("images/TER5.BMP");
                  else
                    got_bitmap = ReadBMP("images/TER5_D.BMP");
                    break;
		case 805: if(party.stuff_done[307][0] == 0)
                    got_bitmap = ReadBMP("images/TER6.BMP");
                  else
                    got_bitmap = ReadBMP("images/TER6_D.BMP");
                    break;
		case 820: if(party.stuff_done[307][0] == 0)
                    got_bitmap = ReadBMP("images/TERANIM.BMP");
                  else
                    got_bitmap = ReadBMP("images/TERANIM_D.BMP");
                    break;
		case 821: if(party.stuff_done[307][0] == 0)
                    got_bitmap = ReadBMP("images/FIELDS.BMP");
                  else
                    got_bitmap = ReadBMP("images/FIELDS_D.BMP");
                    break;
		case 830: got_bitmap = ReadBMP("images/STARTUP.BMP"); break;
		case 831: got_bitmap = ReadBMP("images/STANIM.BMP"); break;
		case 832: got_bitmap = ReadBMP("images/STARTBUT.BMP"); break;
		case 850: got_bitmap = ReadBMP("images/DLOGPICS.BMP"); break;
		case 851: got_bitmap = ReadBMP("images/SCENPICS.BMP"); break;
		case 860: got_bitmap = ReadBMP("images/TALKPORT.BMP"); break;
//		case 875: got_bitmap = ReadBMP("images/DLOGMAPS.BMP"); break; => Exile III maps
		case 880: got_bitmap = ReadBMP("images/MISSILES.BMP"); break;
		case 900: got_bitmap = ReadBMP("images/TINYOBJ.BMP"); break;
		case 901: got_bitmap = ReadBMP("images/OBJECTS.BMP"); break;
		case 902:
        case 905: got_bitmap = ReadBMP("images/PCS.BMP"); break;
		case 903:
		case 904: if(party.stuff_done[307][0] == 0)
                    got_bitmap = ReadBMP("images/MIXED.BMP");
                  else
                    got_bitmap = ReadBMP("images/MIXED_D.BMP");
                    break;
		case 910:
		case 911:
		case 912: got_bitmap = ReadBMP("images/BIGSCEN.BMP"); break;
		case 1100: case 1200: got_bitmap = ReadBMP("images/MONST1.BMP"); break;
		case 1101: case 1201: got_bitmap = ReadBMP("images/MONST2.BMP"); break;
		case 1102: case 1202: got_bitmap = ReadBMP("images/MONST3.BMP"); break;
		case 1103: case 1203: got_bitmap = ReadBMP("images/MONST4.BMP"); break;
		case 1104: case 1204: got_bitmap = ReadBMP("images/MONST5.BMP"); break;
		case 1105: case 1205: got_bitmap = ReadBMP("images/MONST6.BMP"); break;
		case 1106: case 1206: got_bitmap = ReadBMP("images/MONST7.BMP"); break;
		case 1107: case 1207: got_bitmap = ReadBMP("images/MONST8.BMP"); break;
		case 1108: case 1208: got_bitmap = ReadBMP("images/MONST9.BMP"); break;
		case 1109: case 1209: got_bitmap = ReadBMP("images/MONST10.BMP"); break;
		case 1400: got_bitmap = ReadBMP("images/STSCICON.BMP"); break;
		case 1401: got_bitmap = ReadBMP("images/HELPPICS.BMP"); break;
		case 1402: got_bitmap = ReadBMP("images/APPIC.BMP"); break;
		case 1500: 
		case 1501: 
		case 1502: 
		case 1503: 
		case 1504: 
		case 1505: 
		case 1506: 
		case 1507:
			got_bitmap = ReadBMP("images/BIGMAPS.BMP"); break;
		case 2000: got_bitmap = ReadBMP("images/DLOGBTNS.BMP"); break;
		case 3000: got_bitmap = ReadBMP("images/START.BMP"); break;
		case 3001: got_bitmap = ReadBMP("images/SPIDLOGO.BMP"); break;
		case 4000: if(party.stuff_done[307][0] == 0)
                    got_bitmap = ReadBMP("images/TRIMS.BMP");
                  else
                    got_bitmap = ReadBMP("images/TRIMS_D.BMP");
                    break;

		default: got_bitmap = NULL;
		}
	return got_bitmap;
}

/* GK */

HBITMAP ReadBMP(char * fileName)
{
	return (HBITMAP) LoadImage(0, fileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}
