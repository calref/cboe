#define xmin(a,b)	((a) < (b) ?  (a) : (b))

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <cstdlib>
#include <cstdio>
#include "global.h"
#include "boe.text.h"
#include <cstring>
#include "tools/soundtool.h"
#include "boe.infodlg.h"
#include "tools/dlogtool.h"
#include "tools/mathutil.h"
#include "boe.fields.h"
#include "boe.locutils.h"
#include "boe.monster.h"
#include "boe.graphics.h"

#include "boe.graphutil.h"
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
	Boolean dlog_draw = false;

	if (main_win == 2)
	{
		destDC = (HDC) dest;
		main_win = 1;
		dlog_draw = true;
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

			if (dlog_draw == false)	SetViewportOrgEx(destDC,ulx,uly,NULL);

			StretchBlt(destDC,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
				dest_rect.bottom - dest_rect.top,
				hdcMem,src_rect.left,src_rect.top,src_rect.right - src_rect.left,
				src_rect.bottom - src_rect.top,(trans == 0) ? SRCCOPY : MERGECOPY);
				if (trans == 2)
				{
					SelectObject(destDC,old_brush);
					if (DeleteObject(hbrush) == 0)	play_sound(1);
				}

			if (dlog_draw == false)	SetViewportOrgEx(destDC,0,0,NULL);
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
			if (dlog_draw == false)	SetViewportOrgEx(destDC,ulx,uly,NULL);
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

			if (dlog_draw == false)	SetViewportOrgEx(destDC,0,0,NULL);

			DeleteDC(hdcMem2);

			if (transbmp != bw_bitmap)	DeleteObject(transbmp);
			}
			}
	if (dlog_draw == true) DeleteDC(hdcMem);
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
		case 800: if(party.stuff_done[SDF_USE_DARKER_GRAPHICS] == 0)
                    got_bitmap = ReadBMP("images/TER1.BMP");
                  else
                    got_bitmap = ReadBMP("images/TER1_D.BMP");
                    break;
		case 801: if(party.stuff_done[SDF_USE_DARKER_GRAPHICS] == 0)
                    got_bitmap = ReadBMP("images/TER2.BMP");
                  else
                    got_bitmap = ReadBMP("images/TER2_D.BMP");
                    break;
		case 802: if(party.stuff_done[SDF_USE_DARKER_GRAPHICS] == 0)
                    got_bitmap = ReadBMP("images/TER3.BMP");
                  else
                    got_bitmap = ReadBMP("images/TER3_D.BMP");
                    break;
		case 803: if(party.stuff_done[SDF_USE_DARKER_GRAPHICS] == 0)
                    got_bitmap = ReadBMP("images/TER4.BMP");
                  else
                    got_bitmap = ReadBMP("images/TER4_D.BMP");
                    break;
		case 804: if(party.stuff_done[SDF_USE_DARKER_GRAPHICS] == 0)
                    got_bitmap = ReadBMP("images/TER5.BMP");
                  else
                    got_bitmap = ReadBMP("images/TER5_D.BMP");
                    break;
		case 805: if(party.stuff_done[SDF_USE_DARKER_GRAPHICS] == 0)
                    got_bitmap = ReadBMP("images/TER6.BMP");
                  else
                    got_bitmap = ReadBMP("images/TER6_D.BMP");
                    break;
		case 820: if(party.stuff_done[SDF_USE_DARKER_GRAPHICS] == 0)
                    got_bitmap = ReadBMP("images/TERANIM.BMP");
                  else
                    got_bitmap = ReadBMP("images/TERANIM_D.BMP");
                    break;
		case 821: if(party.stuff_done[SDF_USE_DARKER_GRAPHICS] == 0)
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
		case 904: if(party.stuff_done[SDF_USE_DARKER_GRAPHICS] == 0)
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
/*		case 1500:
		case 1501:
		case 1502:
		case 1503:
		case 1504:
		case 1505:
		case 1506:
		case 1507:
			got_bitmap = ReadBMP("images/BIGMAPS.BMP"); break;*/  // <= doesn't exist
		case 2000: got_bitmap = ReadBMP("images/DLOGBTNS.BMP"); break;
		case 3000: got_bitmap = ReadBMP("images/START.BMP"); break;
		case 3001: got_bitmap = ReadBMP("images/SPIDLOGO.BMP"); break;
		case 4000: if(party.stuff_done[SDF_USE_DARKER_GRAPHICS] == 0)
                    got_bitmap = ReadBMP("images/TRIMS.BMP");
                  else
                    got_bitmap = ReadBMP("images/TRIMS_D.BMP");
                    break;

		default: got_bitmap = NULL;
		}
	return got_bitmap;
}

/* GK */

HBITMAP ReadBMP(char const * fileName)
{
	return (HBITMAP) LoadImage(0, fileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

HBITMAP ReadScenFile(char const* filename){ //like ReadBMP but in the current scenario folder only.

        short store_val;
        char str[256]= "scenarios/";
           store_val= strlen(party.scen_name)-1;
            while(party.scen_name[store_val] != '.'){ // get rid of the .exs part
                store_val--;
                }
            strncat(str, party.scen_name, store_val + 1); // the scenario is in a subfolder so add that to the filepath
            strcat(str,"exr/");
            strcat(str,filename);

    return (HBITMAP) LoadImage(0, str, IMAGE_BITMAP, 0 ,0 , LR_LOADFROMFILE);
}

void draw_one_terrain_spot (short i,short j,short terrain_to_draw,short dest)
//short dest; // 0 - terrain gworld   1 - screen
// if terrain_to_draw is -1, do black
// if terrain_to_draw >= 10000, force to draw graphic which is terrain_to_draw - 10000
{
	RECT where_draw;
	RECT source_rect;

	HBITMAP source_gworld;
	short anim_type = 0;
	location l;
	HDC hdc;
	HBITMAP store_bmp;

	l.x = i; l.y = j;
	if ((supressing_some_spaces == true) &&
		(same_point(l,ok_space[0]) == false) &&
		(same_point(l,ok_space[1]) == false) &&
		(same_point(l,ok_space[2]) == false) &&
		(same_point(l,ok_space[3]) == false))
			return;

	where_draw = calc_rect(i,j);
	OffsetRect(&where_draw,13,13);
	if (terrain_to_draw == -1) {
		if (terrain_there[i][j] == 300) {
			return;
			}
		terrain_there[i][j] = 300;
		hdc = CreateCompatibleDC(main_dc);
		store_bmp = (HBITMAP) SelectObject(hdc,terrain_screen_gworld);
        SelectObject(hdc,GetStockObject(BLACK_BRUSH));
		Rectangle(hdc,where_draw.left,where_draw.top,where_draw.right,where_draw.bottom);
		SelectObject(hdc,store_bmp);
		if (!DeleteDC(hdc)) DebugQuit("Cannot release DC 24");
		return;
		}
	if (terrain_to_draw >= 10000) { // force using a specific graphic
 		if (terrain_there[i][j] == terrain_to_draw - 10000)
 			return;
 		source_gworld = terrains_gworld[(terrain_to_draw -10000) / 50];
 		terrain_there[i][j] = terrain_to_draw - 10000;
		source_rect = return_item_rect(terrain_to_draw - 10000);
		anim_type = -1;
		}
		else if (terrain_pic[terrain_to_draw] >= 2000) { // animated custom
			source_gworld = spec_scen_g;
            source_rect = get_custom_rect(terrain_pic[terrain_to_draw] - 2000 + (anim_ticks % 4));
			anim_type = 0;
			terrain_there[i][j] = -1;
			}
		else if (terrain_pic[terrain_to_draw] >= 1000) { // custom
			source_gworld = spec_scen_g;
			source_rect = get_custom_rect(terrain_pic[terrain_to_draw] - 1000);
			terrain_there[i][j] = -1;
			}
		else if (terrain_pic[terrain_to_draw] >= 400) { // animated
            source_gworld = ter_anim_gworld;
            source_rect = return_item_rect(terrain_pic[terrain_to_draw]);
			terrain_there[i][j] = -1;
			anim_type = 0;
			}
			else {
 				if (terrain_there[i][j] == terrain_pic[terrain_to_draw]) {
	 				return;
					}
				terrain_there[i][j] = terrain_pic[terrain_to_draw];

                source_gworld = terrains_gworld[(terrain_pic[terrain_to_draw]) / 50];
                source_rect = return_item_rect(terrain_pic[terrain_to_draw]);
				anim_type = -1;
				}

		if (anim_type >= 0)
		 {
			if ((is_town()) || (is_out()))
				anim_onscreen = true;
			}

	if (dest == 0)
		rect_draw_some_item(source_gworld, source_rect, terrain_screen_gworld, where_draw, (unsigned char) 0, 0);
		else rect_draw_some_item(source_gworld, source_rect, terrain_screen_gworld, where_draw, (unsigned char) 0, 1);
}

void draw_monsters()
{
	short i,j = 0,k;
	short width,height;
	RECT source_rect,to_rect;
	location where_draw,store_loc;
	HBITMAP source_gworld;

	short picture_wanted;
	unsigned char ter;
	RECT monst_rects[4][4] = {{{0,0,28,36},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
		{{7,0,21,18},{7,18,21,36},{0,0,0,0},{0,0,0,0}},
		{{0,9,14,27},{14,9,28,27},{0,0,0,0},{0,0,0,0}},
		{{0,0,14,18},{14,0,28,18},{0,18,14,36},{14,18,28,36}}};

	if (is_out())
		for (i = 0; i < 10; i++)
			if (party.out_c[i].exists == true) {
				if ((point_onscreen(party.p_loc, party.out_c[i].m_loc) == true) &&
					(can_see(party.p_loc, party.out_c[i].m_loc,0) < 5)) {
					where_draw.x = party.out_c[i].m_loc.x - party.p_loc.x + 4;
					where_draw.y = party.out_c[i].m_loc.y - party.p_loc.y + 4;
					terrain_there[where_draw.x][where_draw.y] = -1;

					j = 0;
					while ((party.out_c[i].what_monst.monst[j] == 0) && (j < 7))
						j++;

					if (j == 7) party.out_c[i].exists = false; // begin watch out
						else picture_wanted = get_monst_picnum(party.out_c[i].what_monst.monst[j]);
							// end watch out

					if (party.out_c[i].exists == true) {
						get_monst_dims(party.out_c[i].what_monst.monst[j],&width,&height);
						if (picture_wanted >= 1000) {
							for (k = 0; k < width * height; k++) {
								source_rect = get_custom_rect(picture_wanted % 1000 +
								  ((party.out_c[i].direction < 4) ? 0 : (width * height)) + k);
								to_rect = monst_rects[(width - 1) * 2 + height - 1][k];
								rect_draw_some_item(spec_scen_g, source_rect, small_temp_gworld,to_rect, 0, 0);
								source_rect = to_rect;
								OffsetRect(&to_rect,13 + 28 * where_draw.x,13 + 36 * where_draw.y);
								rect_draw_some_item(small_temp_gworld, source_rect, terrain_screen_gworld,to_rect, 1, 0);
								}
							}
						if (picture_wanted < 1000) {
							for (k = 0; k < width * height; k++) {
                                if (picture_wanted == 73 && k == 1){//drake special case (split on two sheets)
                                source_gworld = monsters_gworld[4];
                                source_rect = get_monster_template_rect(20,
						        (party.out_c[i].direction < 4) ? 0 : 1,0);
                                }
                                else if (picture_wanted == 101 && k == 1){//bear special case (split on two columns)
                                source_gworld = monsters_gworld[5];
                                source_rect = get_monster_template_rect(10,
						        (party.out_c[i].direction < 4) ? 0 : 1,0);
                                }
                                else{
                                source_gworld = monsters_gworld[m_pic_sheet[picture_wanted]];
								source_rect = get_monster_template_rect(party.out_c[i].what_monst.monst[j],
								  (party.out_c[i].direction < 4) ? 0 : 1,k);
                                }
								to_rect = monst_rects[(width - 1) * 2 + height - 1][k];
								rect_draw_some_item(source_gworld, source_rect, small_temp_gworld,to_rect, 0, 0);
								source_rect = to_rect;
								OffsetRect(&to_rect,13 + 28 * where_draw.x,13 + 36 * where_draw.y);
								rect_draw_some_item(small_temp_gworld, source_rect, terrain_screen_gworld,to_rect, 1, 0);
								}
							}
					}
				}
			}
	if (is_town())
		for (i = 0; i < T_M; i++)
		if ((c_town.monst.dudes[i].active != 0) && (c_town.monst.dudes[i].m_d.spec_skill != MONSTER_INVISIBLE))
			if (party_can_see_monst(i)) {
				where_draw.x = c_town.monst.dudes[i].m_loc.x - center.x + 4;
				where_draw.y = c_town.monst.dudes[i].m_loc.y - center.y + 4;
				get_monst_dims(c_town.monst.dudes[i].number,&width,&height);

				for (k = 0; k < width * height; k++) {
					store_loc = where_draw;
					store_loc.x += k % width;
					store_loc.y += k / width;
					// customize?
					if (c_town.monst.dudes[i].m_d.picture_num >= 1000) {
						source_rect = get_custom_rect((c_town.monst.dudes[i].m_d.picture_num % 1000) +
							 k + ((c_town.monst.dudes[i].m_d.direction < 4) ? 0 : width * height)
							 + ((combat_posing_monster == i + 100) ? (2 * width * height) : 0));
						Draw_Some_Item(spec_scen_g, source_rect, terrain_screen_gworld, store_loc, 1, 0);
						}
					if (c_town.monst.dudes[i].m_d.picture_num < 1000) {
                        if (c_town.monst.dudes[i].m_d.picture_num == 73 && k == 1){//drake special case (split on two sheets)
                        source_gworld = monsters_gworld[4];
                        source_rect = get_monster_template_rect(20,
						 ((c_town.monst.dudes[i].m_d.direction < 4) ? 0 : 1) + ((combat_posing_monster == i + 100) ? 10 : 0),0);
                        }
                        else if (c_town.monst.dudes[i].m_d.picture_num == 101 && k == 1){//bear special case (split on two columns)
                                source_gworld = monsters_gworld[5];
                                source_rect = get_monster_template_rect(10,

						 ((c_town.monst.dudes[i].m_d.direction < 4) ? 0 : 1) + ((combat_posing_monster == i + 100) ? 10 : 0),0);
                            }
                        else{
                        source_gworld = monsters_gworld[m_pic_sheet[c_town.monst.dudes[i].m_d.picture_num]];
						source_rect = get_monster_template_rect(c_town.monst.dudes[i].number,
						 ((c_town.monst.dudes[i].m_d.direction < 4) ? 0 : 1) + ((combat_posing_monster == i + 100) ? 10 : 0),k);
                        }
						ter = t_d.terrain[c_town.monst.dudes[i].m_loc.x][c_town.monst.dudes[i].m_loc.y];
						// in bed?
						if ((store_loc.x >= 0) && (store_loc.x < 9) && (store_loc.y >= 0) && (store_loc.y < 9) &&
							(scenario.ter_types[ter].picture == 143) &&
							((c_town.monst.dudes[i].m_d.m_type < MONSTER_TYPE_DEMON)//is humanoïd ?
							&& (c_town.monst.dudes[i].m_d.m_type != MONSTER_TYPE_REPTILE) && (c_town.monst.dudes[i].m_d.m_type != MONSTER_TYPE_BEAST))
							&& ((c_town.monst.dudes[i].active == 1) || (monst_target[i] == 6)) &&
							(width == 1) && (height == 1)) ////
							draw_one_terrain_spot((short) where_draw.x,(short) where_draw.y,10230,0);
							else Draw_Some_Item(source_gworld, source_rect, terrain_screen_gworld, store_loc, 1, 0);
							}
					}
				}
	if (is_combat()) {
			for (i = 0; i < T_M; i++)
				if ((c_town.monst.dudes[i].active != 0) && (c_town.monst.dudes[i].m_d.spec_skill != MONSTER_INVISIBLE))
					if ((point_onscreen(center,c_town.monst.dudes[i].m_loc) == true) && (party_can_see_monst(i) == true)) {
						where_draw.x = c_town.monst.dudes[i].m_loc.x - center.x + 4;
						where_draw.y = c_town.monst.dudes[i].m_loc.y - center.y + 4;
						get_monst_dims(c_town.monst.dudes[i].number,&width,&height);

						for (k = 0; k < width * height; k++) {
							store_loc = where_draw;
							store_loc.x += k % width;
							store_loc.y += k / width;
							// customize?
							if (c_town.monst.dudes[i].m_d.picture_num >= 1000) {
								source_rect = get_custom_rect((c_town.monst.dudes[i].m_d.picture_num % 1000) +
									 k + ((c_town.monst.dudes[i].m_d.direction < 4) ? 0 : width * height)
									 	+ ((combat_posing_monster == i + 100) ? (2 * width * height) : 0));
								Draw_Some_Item(spec_scen_g, source_rect, terrain_screen_gworld, store_loc, 1, 0);
								}
							if (c_town.monst.dudes[i].m_d.picture_num < 1000) {
                            if (c_town.monst.dudes[i].m_d.picture_num == 73 && k == 1){//drake special case (split on two sheets)
                                source_gworld = monsters_gworld[4];
                                source_rect = get_monster_template_rect(20,
						        ((c_town.monst.dudes[i].m_d.direction < 4) ? 0 : 1) + ((combat_posing_monster == i + 100) ? 10 : 0),0);
                                }
                                else if (c_town.monst.dudes[i].m_d.picture_num == 101 && k == 1){//bear special case (split on two columns)
                                source_gworld = monsters_gworld[5];
                                source_rect = get_monster_template_rect(10,
						 ((c_town.monst.dudes[i].m_d.direction < 4) ? 0 : 1) + ((combat_posing_monster == i + 100) ? 10 : 0),0);
                            }
                                else{
                                source_gworld = monsters_gworld[m_pic_sheet[c_town.monst.dudes[i].m_d.picture_num]];
								source_rect = get_monster_template_rect(c_town.monst.dudes[i].number,
								 ((c_town.monst.dudes[i].m_d.direction < 4) ? 0 : 1) + ((combat_posing_monster == i + 100) ? 10 : 0)
								 ,k);
                                }
								ter = t_d.terrain[c_town.monst.dudes[i].m_loc.x][c_town.monst.dudes[i].m_loc.y];
								if ((store_loc.x >= 0) && (store_loc.x < 9) && (store_loc.y >= 0) && (store_loc.y < 9) &&
									(scenario.ter_types[ter].picture == 143) &&
									((c_town.monst.dudes[i].m_d.m_type < MONSTER_TYPE_DEMON)//is humanoïd ?
										&& (c_town.monst.dudes[i].m_d.m_type != MONSTER_TYPE_REPTILE) && (c_town.monst.dudes[i].m_d.m_type != MONSTER_TYPE_BEAST))
									&& ((c_town.monst.dudes[i].active == 1) || (monst_target[i] == 6)) &&
									(width == 1) && (height == 1))
										draw_one_terrain_spot((short) where_draw.x,(short) where_draw.y,10230,0); ////
										else Draw_Some_Item(source_gworld, source_rect, terrain_screen_gworld, store_loc, 1, 0);
								}
							}
						}
	}
}


void draw_pcs(location center,short mode)
//short mode; // 0 - put pcs in gworld  1 - only rectangle around active pc
{
	short i;
	RECT source_rect;
	location where_draw;

	if (party_toast() == true) return;		// is party dead
	if (can_draw_pcs == false) return;

	for (i = 0; i < NUM_OF_PCS; i++)
		if (adven[i].isAlive())
			if ((point_onscreen(center, pc_pos[i]) == true) &&
				(party_can_see(pc_pos[i]) < 6)){
				where_draw.x = pc_pos[i].x - center.x + 4;
				where_draw.y = pc_pos[i].y - center.y + 4;
				source_rect = get_party_template_rect(i,(pc_dir[i] < 4) ? 0 : 1);
				if (combat_posing_monster == i)
					OffsetRect(&source_rect,280,0);

				if (mode == 0)
					Draw_Some_Item(pcs_gworld, source_rect, terrain_screen_gworld, where_draw, 1, 0);

				if ((current_pc == i) && (mode == 1) && (monsters_going == false))
					frame_space(pc_pos[current_pc],1,1,1);
			}

	if (current_pc == INVALID_PC) return;

	// Draw current pc on top
	if ((current_pc < 6) && ((point_onscreen(center, pc_pos[current_pc])) == true) && (adven[current_pc].isAlive())) {
		where_draw.x = pc_pos[current_pc].x - center.x + 4;
		where_draw.y = pc_pos[current_pc].y - center.y + 4;
		source_rect = get_party_template_rect(current_pc,(pc_dir[current_pc] < 4) ? 0 : 1);
				if (combat_posing_monster == current_pc)
					OffsetRect(&source_rect,280,0);

		if (mode == 0)
            Draw_Some_Item(pcs_gworld, source_rect, terrain_screen_gworld, where_draw, 1, 0);
		}
}

void draw_items()
{
	short i;
	RECT source_rect,dest_rect;
	location where_draw;

	for (i = 0; i < NUM_TOWN_ITEMS; i++) {
		if (t_i.items[i].variety != ITEM_TYPE_NO_ITEM) {
			where_draw.x = t_i.items[i].item_loc.x - center.x + 4;
			where_draw.y = t_i.items[i].item_loc.y - center.y + 4;

			if ((supressing_some_spaces == true) &&
				(same_point(where_draw,ok_space[0]) == false) &&
				(same_point(where_draw,ok_space[1]) == false) &&
				(same_point(where_draw,ok_space[2]) == false) &&
				(same_point(where_draw,ok_space[3]) == false))
				;
			else if ((point_onscreen(center, t_i.items[i].item_loc) == true) &&
				(t_i.items[i].isContained() == false) && (party_can_see(t_i.items[i].item_loc) < 6)) {
					if (t_i.items[i].graphic_num >= 150) {
						source_rect = get_custom_rect(t_i.items[i].graphic_num - 150);
						dest_rect = coord_to_rect(where_draw.x,where_draw.y);
						terrain_there[where_draw.x][where_draw.y] = -1;

						rect_draw_some_item(spec_scen_g,
						 source_rect, terrain_screen_gworld, dest_rect, 1, 0);
						}
						else {
							source_rect = get_item_template_rect(t_i.items[i].graphic_num);
							dest_rect = coord_to_rect(where_draw.x,where_draw.y);
							terrain_there[where_draw.x][where_draw.y] = -1;
							if (t_i.items[i].graphic_num >= 45) {
								dest_rect.top += 9;
								dest_rect.bottom -= 9;
								dest_rect.left += 5;
								dest_rect.right -= 5;
								}
							rect_draw_some_item((t_i.items[i].graphic_num < 45) ? items_gworld : tiny_obj_gworld,
							 source_rect, terrain_screen_gworld, dest_rect, 1, 0);
					 		}
					}
			}
		}
}

void draw_outd_boats(location center)
{
	location where_draw;
	RECT source_rect;
	short i;

	for (i = 0; i < 30; i++)
			if ((point_onscreen(center, party.boats[i].boat_loc) == true) && (party.boats[i].exists == true) &&
				(party.boats[i].which_town == INVALID_TOWN) &&
				(can_see(center, party.boats[i].boat_loc,0) < 5) && (party.in_boat != i)) {
				where_draw.x = party.boats[i].boat_loc.x - center.x + 4;
				where_draw.y = party.boats[i].boat_loc.y - center.y + 4;
				source_rect = boat_rects[0];
				OffsetRect(&source_rect,61,0);
				Draw_Some_Item(mixed_gworld, source_rect, terrain_screen_gworld, where_draw, 1, 0);
				}
	for (i = 0; i < 30; i++)
			if ((point_onscreen(center, party.horses[i].horse_loc) == true) && (party.horses[i].exists == true) &&
				(party.horses[i].which_town == INVALID_TOWN) &&
				(can_see(center, party.horses[i].horse_loc,0) < 5) && (party.in_horse != i)) {
				where_draw.x = party.horses[i].horse_loc.x - center.x + 4;
				where_draw.y = party.horses[i].horse_loc.y - center.y + 4;
				source_rect = boat_rects[0];
				OffsetRect(&source_rect,61,0);
				OffsetRect(&source_rect,0,74);
				OffsetRect(&source_rect,56,36);
				Draw_Some_Item(mixed_gworld, source_rect, terrain_screen_gworld, where_draw, 1, 0);
				}
}

void draw_town_boat(location center)
{
	location where_draw;
	RECT source_rect;
	short i;

	for (i = 0; i < 30; i++)
		if ((party.boats[i].which_town == c_town.town_num) &&
		((point_onscreen(center, party.boats[i].boat_loc) == true) &&
		(can_see(center, party.boats[i].boat_loc,0) < 5) && (party.in_boat != i)
		 && (pt_in_light(center,party.boats[i].boat_loc) == true))) {
		where_draw.x = party.boats[i].boat_loc.x - center.x + 4;
		where_draw.y = party.boats[i].boat_loc.y - center.y + 4;
		source_rect = boat_rects[0];
		OffsetRect(&source_rect,61,0);
		Draw_Some_Item(mixed_gworld, source_rect, terrain_screen_gworld, where_draw, 1, 0);
		}
	for (i = 0; i < 30; i++)
		if ((party.horses[i].which_town == c_town.town_num) &&
		((point_onscreen(center, party.horses[i].horse_loc) == true) &&
		(can_see(center, party.horses[i].horse_loc,0) < 5) && (party.in_horse != i)
		 && (pt_in_light(center,party.horses[i].horse_loc) == true))) {
		where_draw.x = party.horses[i].horse_loc.x - center.x + 4;
		where_draw.y = party.horses[i].horse_loc.y - center.y + 4;

		source_rect = boat_rects[0];
		OffsetRect(&source_rect,61,0);
		OffsetRect(&source_rect,0,74);
		OffsetRect(&source_rect,56,36);
		Draw_Some_Item(mixed_gworld, source_rect, terrain_screen_gworld, where_draw, 1, 0);
		}
}

void draw_sfx()
{
	short q,r,i,flag;
	location where_draw,loc;
	RECT orig_rect = {0,0,28,36},source_rect;

	if (PSD[SDF_NO_FRILLS] > 0)
		return;

		for (q = 0; q < 9; q++)
			for (r = 0; r < 9; r++)
				{
				where_draw = center;where_draw.x += q - 4;where_draw.y += r - 4;

				if ((where_draw.x < 0) || (where_draw.x > town_size[town_type] - 1)
						|| (where_draw.y < 0) || (where_draw.y > town_size[town_type] - 1))
							;
						else if (sfx[where_draw.x][where_draw.y] != 0) {
							for (i = 0; i < 8; i++) {
								flag = s_pow(2,i);
								if (sfx[where_draw.x][where_draw.y] & flag)
									if (spot_seen[q][r] > 0) {
									loc.x = q; loc.y = r;
									source_rect = orig_rect;
									OffsetRect(&source_rect,28 * i,36 * 3);
									Draw_Some_Item(fields_gworld,source_rect,terrain_screen_gworld,loc,
									 1,0);
									}
								}
							}
				}
}


void draw_one_field(unsigned char flag,short source_x,short source_y)
{
	short q,r;
	location where_draw,loc;
	RECT orig_rect = {0,0,28,36},source_rect;

		for (q = 0; q < 9; q++)
			for (r = 0; r < 9; r++)
				{
				where_draw = center;where_draw.x += q - 4;where_draw.y += r - 4;

				if ((where_draw.x < 0) || (where_draw.x > town_size[town_type] - 1)
						|| (where_draw.y < 0) || (where_draw.y > town_size[town_type] - 1))
							;
						else {
							if (misc_i[where_draw.x][where_draw.y] & flag)
								if (spot_seen[q][r] > 0) {
								loc.x = q; loc.y = r;
								source_rect = orig_rect;
								OffsetRect(&source_rect,28 * source_x,36 * source_y);
								Draw_Some_Item(fields_gworld,source_rect,terrain_screen_gworld,loc,
								 1,0);
								if ((is_town()) && ((flag == 32) || (flag == 64)))
									anim_onscreen = true;
								}
							}
				}
}

void draw_one_spec_item(unsigned char flag,short source_x,short source_y)
{
	short q,r;
	location where_draw,loc;
	RECT orig_rect = {0,0,28,36},source_rect;

		for (q = 0; q < 9; q++)
			for (r = 0; r < 9; r++)
				{
				where_draw = center;where_draw.x += q - 4;where_draw.y += r - 4;

				if ((where_draw.x < 0) || (where_draw.x > town_size[town_type] - 1)
						|| (where_draw.y < 0) || (where_draw.y > town_size[town_type] - 1))
							;
						else {
							if (c_town.explored[where_draw.x][where_draw.y] & flag)
								if (spot_seen[q][r] > 0) {
								loc.x = q; loc.y = r;
								source_rect = orig_rect;
								OffsetRect(&source_rect,28 * source_x,36 * source_y);
								Draw_Some_Item(fields_gworld,source_rect,terrain_screen_gworld,loc,
								 1,0);
								}
				}
			}
}



void draw_party_symbol(location center)
{
	RECT source_rect;
	location target = location(4,4);
	short i = 0;
	short dir_array[8] = {0,3,3,3,2,1,1,1};

	if (can_draw_pcs == false) return;
	if (party_toast() == true) return;
	if ((is_town()) && (c_town.p_loc.x > 70)) return;
	if (overall_mode == MODE_LOOK_TOWN)
	{
		target.x += c_town.p_loc.x - center.x;
		target.y += c_town.p_loc.y - center.y;
	}

	if ((party.in_boat < 0) && (party.in_horse < 0)) {
			i = first_active_pc();
			source_rect = get_party_template_rect(i,(party.direction < 4) ? 0 : 1);

			// now wedge in bed graphic
			if ((is_town()) && (scenario.ter_types[t_d.terrain[c_town.p_loc.x][c_town.p_loc.y]].picture == 143))
				draw_one_terrain_spot((short) target.x,(short) target.y,10230,0);
                else Draw_Some_Item(pcs_gworld, source_rect, terrain_screen_gworld, target, 1, 0);
		}
		else if (party.in_boat >= 0) {
				source_rect = boat_rects[dir_array[party.direction]];
				OffsetRect(&source_rect,61,0);
				Draw_Some_Item(mixed_gworld, source_rect, terrain_screen_gworld, target, 1, 0);
			}
			else {
				source_rect = boat_rects[(party.direction < 4) ? 0 : 1];
				OffsetRect(&source_rect,61,0);
				OffsetRect(&source_rect,0,74);
				Draw_Some_Item(mixed_gworld, source_rect, terrain_screen_gworld, target, 1, 0);
				}
}

RECT return_item_rect(short wanted)
{
	RECT orig_rect = {0,0,28,36};
	short ter;
	if (wanted >= 400) {
        ter = wanted - 400;
        OffsetRect(&orig_rect, 112 * (ter / 5) + 28 * (anim_ticks % 4), 36 * (ter % 5));
		return orig_rect;
		}
	else{
            ter = wanted % 50;
			OffsetRect(&orig_rect,28 * (ter % 10),36 * (ter / 10));
			return orig_rect;
		}
}

// Give the position of the monster graphic in the template in memory
RECT get_monster_template_rect (unsigned char type_wanted,short mode,short which_part)
//mode; // 0 - left  1 - right  +10 - combat mode
{
	RECT store_rect = {0,0,28,36};

	short picture_wanted;
//	short adj = 0;

//	if (mode >= 10) {adj = 2000; mode -= 10;}
	picture_wanted = get_monst_picnum(type_wanted);
	if (picture_wanted >= 1000)
		return store_rect;
	picture_wanted = m_pic_index[picture_wanted];// + which_part;
	picture_wanted = picture_wanted % 20;
    if(mode >= 10)
        OffsetRect(&store_rect, 112 + 56 * (picture_wanted / 10) + 28 * (mode-10), 36 * ((picture_wanted % 10) + which_part));
    else
        OffsetRect(&store_rect, 56 *  (picture_wanted / 10) + 28 * mode, 36 * ((picture_wanted % 10)+ which_part));

	return store_rect;
}


// Returns rect for drawing an item, if num < 25, rect is in big item template,
// otherwise in small item template
RECT get_item_template_rect (short type_wanted)
{
	RECT store_rect;

	if (type_wanted < 45) {
		store_rect.top = (type_wanted / 5) * BITMAP_HEIGHT;
		store_rect.bottom = store_rect.top + BITMAP_HEIGHT;
		store_rect.left = (type_wanted % 5) * BITMAP_WIDTH;
		store_rect.right = store_rect.left + BITMAP_WIDTH;
		}
		else {
			store_rect.top = (type_wanted / 10) * 18;
			store_rect.bottom = store_rect.top + 18;
			store_rect.left = (type_wanted % 10) * 18;
			store_rect.right = store_rect.left + 18;
			}

	return store_rect;
}


unsigned char get_t_t(int x, int y)  // returns terrain type at where
{
	if (is_out())
		return out[x][y];
		else if (is_town())
			return t_d.terrain[x][y];
			else return combat_terrain[x][y];
}

// Is this is subterranean fluid that gets shore plopped down on it?
Boolean is_fluid(unsigned char ter_type)////
{
	if (((ter_type >= 71) && (ter_type <= 76)) || (ter_type == 90))
		return true;
	return false;
}

// Is this is subterranean beach that gets shore plopped down next to it?
Boolean is_shore(unsigned char ter_type)////
{
	if (is_fluid(ter_type) == true)	return false;
	if (ter_type == 77)	return false;
	if (ter_type == 90)	return false;
	return true;
}

// These two functions used to determine wall round-cornering
Boolean is_wall(unsigned char ter_type)////
{
	short pic = scenario.ter_types[ter_type].picture;

	if ((pic >= 88) && (pic <= 120))
		return true;

	return false;
}
Boolean is_ground(unsigned char ter_type)
{
	short pic = scenario.ter_types[ter_type].picture;

	if ((pic >= 0) && (pic <= 87)) return true;
	if ((pic >= 121) && (pic <= 122)) return true;
	if ((pic >= 179) && (pic <= 208)) return true;
	if ((pic >= 211) && (pic <= 212)) return true;
	if ((pic >= 215) && (pic <= 245)) return true;

	return false;
}

void make_town_trim(short mode)
//mode; // 0 - town 1 - outdoor combat
{
	short store_mode;

	store_mode = overall_mode;
	overall_mode = (mode == 0) ? MODE_TOWN : MODE_COMBAT;
	for (int x = 0; x < town_size[town_type]; x++)
		for (int y = 0; y < town_size[town_type]; y++)
			town_trim[x][y] = add_trim_to_array(x, y,
			 (mode == 0) ? t_d.terrain[x][y] : combat_terrain[x][y]);

	for (int x = 0; x < town_size[town_type]; x++)
		for (int y = 0; y < town_size[town_type]; y++)
		{
			if (town_trim[x][y] & 1) town_trim[x][y] &= 125;
			if (town_trim[x][y] & 4) town_trim[x][y] &= 245;
			if (town_trim[x][y] & 10) town_trim[x][y] &= 215;
			if (town_trim[x][y] & 64) town_trim[x][y] &= 95;
		}
	overall_mode = store_mode;
}

void make_out_trim()
{
	short store_mode;

	store_mode = overall_mode;
	overall_mode = MODE_OUTDOORS;

	for (int x = 0; x < 96; x++)
		for (int y = 0; y < 96; y++)
			out_trim[x][y] = add_trim_to_array(x, y, out[x][y]);

	for (int x = 0; x < 96; x++)
		for (int y = 0; y < 96; y++)
		{
			if (out_trim[x][y] & 1)	out_trim[x][y] &= 125;
			if (out_trim[x][y] & 4)	out_trim[x][y] &= 245;
			if (out_trim[x][y] & 10) out_trim[x][y] &= 215;
			if (out_trim[x][y] & 64) out_trim[x][y] &= 95;
		}
	overall_mode = store_mode;

}

/* fixup */
char add_trim_to_array(location where,unsigned char ter_type)
{
	return add_trim_to_array(where.x, where.y, ter_type);
}

char add_trim_to_array(int x, int y, unsigned char ter_type)
{
	Boolean at_top = false,at_bot = false,at_left = false,at_right = false;
	unsigned char store;
	char to_return = 0;

	if (x == 0)	at_left = true;
	if (y == 0)	at_top = true;
	if ((overall_mode == MODE_OUTDOORS) || (overall_mode == MODE_LOOK_OUTDOORS))
	{
		if (x == 95) at_right = true;
		if (y == 95) at_bot = true;
	}
	else
	{
		if (x == town_size[town_type] - 1) at_right = true;
		if (y == town_size[town_type] - 1) at_bot = true;
	}

	// Set up trim for fluids
	if (is_fluid(ter_type) == true) {
		if (at_left == false) {
			store = get_t_t(x - 1,y);
			if (is_shore(store) == true) to_return |= 64;
			}
		if (at_right == false) {
			store = get_t_t(x + 1,y);
			if (is_shore(store) == true) to_return |= 4;
			}
		if (at_top == false) {
			store = get_t_t(x,y - 1);
			if (is_shore(store) == true) to_return |= 1;
			}
		if (at_bot == false) {
			store = get_t_t(x,y + 1);
			if (is_shore(store) == true) to_return |= 16;
			}
		if ((at_left == false) && (at_top == false)) {
			store = get_t_t(x - 1,y - 1);
			if (is_shore(store) == true) to_return |= 128;
			}
		if ((at_right == false) && (at_top == false)) {
			store = get_t_t(x + 1,y + 1);
			if (is_shore(store) == true) to_return |= 8;
			}
		if ((at_right == false) && (at_bot == false)) {
			store = get_t_t(x + 1,y - 1);
			if (is_shore(store) == true) to_return |= 2;
			}
		if ((at_left == false) && (at_bot == false)) {
			store = get_t_t(x - 1,y + 1);
			if (is_shore(store) == true) to_return |= 32;
			}
		}

	return to_return;
}

/* this is NOT the same as PtInRect() function */
Boolean pt_in_rect(location loc, RECT16 rect)
{
	if ((loc.x >= rect.left) && (loc.x <= rect.right) && (loc.y >= rect.top) && (loc.y <= rect.bottom))
		return true;
	return false;
}

// Time for some chicanery
// The how to item for monsters will be 599
// item 600 + i will mean monster i.
void adjust_monst_menu()
{
	short i,monst_pos = 0;
	char monst_name[256];
	HMENU menu,big_menu;
	short total_added = 0;

	if (in_startup_mode == true)
		return;

	big_menu = GetMenu(mainPtr);
	menu = GetSubMenu(big_menu,5);
	if (menu == NULL)
		return;
	for (i = 0; i < 256; i++)
		on_monst_menu[i] = -1;

	for (i = 1; i < 256; i++)
		if ((i == 1) || (party.m_seen[i] > 0)) {
			on_monst_menu[monst_pos] = i;
			monst_pos++;
			}

	for (i = 0; i < 256; i++)
		DeleteMenu(menu,600 + i,MF_BYCOMMAND);

	for (i = 0; i < 256; i++)
		if (on_monst_menu[i] >= 0) {
			//GetIndString(monst_name, 2,on_monst_menu[i]);
			sprintf((char *) monst_name,"%s",scen_item_list->monst_names[on_monst_menu[i]]);			if ((total_added % 24 == 0) && (total_added > 0))
				InsertMenu(menu,599,MF_MENUBREAK | MF_BYCOMMAND | MF_ENABLED | MF_STRING, 600 + i, monst_name);
				else InsertMenu(menu,599,MF_BYCOMMAND | MF_ENABLED | MF_STRING, 600 + i, monst_name);
			total_added++;
			}
}

void frame_space(location where,short mode,short width,short height)
//mode;  // 0 - red   1 - green
{
	location where_put;
	RECT to_frame;
	HDC hdc;
	HPEN hpen,old_pen;
	COLORREF x[3] = {RGB(200,0,0),RGB(102,255,0),RGB(200,0,200)};//RGB(204,204,204);

	if (point_onscreen(center,where) == false)
		return;

	where_put.x = 4 + where.x - center.x;
	where_put.y = 4 + where.y - center.y;

	to_frame.top = 18 + where_put.y * 36;
	to_frame.left = 18 + where_put.x * 28;
	to_frame.bottom = 54 + where_put.y * 36 + 36 * (height - 1);
	to_frame.right = 46 + where_put.x * 28 + 28 * (width - 1);

	hdc = GetDC(mainPtr);
	SetViewportOrgEx(hdc,ulx,uly,NULL);
	hpen = CreatePen(PS_SOLID,1,x[mode]);
	old_pen = (HPEN) SelectObject(hdc,hpen);
	MoveToEx(hdc,to_frame.left,to_frame.top, NULL);

	LineTo(hdc,to_frame.right,to_frame.top);
	LineTo(hdc,to_frame.right,to_frame.bottom);
	LineTo(hdc,to_frame.left,to_frame.bottom);
	LineTo(hdc,to_frame.left,to_frame.top);

	SelectObject(hdc,old_pen);
	ReleaseDC(mainPtr,hdc);
	DeleteObject(hpen);
}
