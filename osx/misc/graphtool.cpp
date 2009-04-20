/*
 *  graphtool.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#define _GRAPHTOOL_CPP
#include "graphtool.h"

//CursHandle arrow_curs[3][3], sword_curs, boot_curs, key_curs, target_curs,talk_curs,look_curs;
short arrow_curs[3][3] = {
	{8, 9, 10},
	{11,12,13},
	{14,15,16},
};
short sword_curs = 17, boot_curs = 18, drop_curs = 19, target_curs = 20;
short talk_curs = 21, key_curs = 22, look_curs = 23, current_cursor = 0;
CursHandle cursors[24] = {
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
};
void (*redraw_screen)();
Point offset;
PixPatHandle bg[21];
short geneva_font_num, dungeon_font_num;

void init_graph_tool(void (*redraw_callback)(), Point p){
	redraw_screen = redraw_callback;
	int i,j;
	if (cursors[0] == NULL) {
		for (i = 0; i < 8; i++)
			cursors[i] = GetCursor(130 + i);
		for (i = 0; i < 3; i++)
			for (j = 0; j < 3; j++)
				cursors[arrow_curs[i][j]] = GetCursor(100 + (i - 1) + 10 * (j - 1));
		cursors[sword_curs] = GetCursor(120);
		cursors[boot_curs] = GetCursor(121);
		cursors[drop_curs] = GetCursor(122);
		cursors[target_curs] = GetCursor(124);
		cursors[talk_curs] = GetCursor(126);
		cursors[key_curs] = GetCursor(127);
		cursors[look_curs] = GetCursor(129);
		set_cursor(sword_curs);
		current_cursor = sword_curs;
	}
	for (i = 0; i < 21; i++) 
	    bg[i] = GetPixPat(128 + i);
	Str255 fn1 = "\pGeneva";
	Str255 fn2 = "\pDungeon Bold";
	Str255 fn3 = "\pPalatino";
	GetFNum(fn1,&geneva_font_num);
	if (geneva_font_num == 0)
		GetFNum(fn3,&geneva_font_num);
	GetFNum(fn2,&dungeon_font_num);
	if (dungeon_font_num == 0)
		GetFNum(fn3,&dungeon_font_num);
	offset = p;
}

unsigned short readUShort(unsigned char ** ptr){
	unsigned short ret = CFSwapInt16LittleToHost(*(unsigned short*)*ptr);
	*ptr += 2;
	return ret;
}

unsigned int readUInt(unsigned char **ptr){
	unsigned int ret = CFSwapInt32LittleToHost(*(unsigned int*)*ptr);
	*ptr += 4;
	return ret;
}

GWorldPtr load_pict(int picture_to_get)
{
	PicHandle	current_pic_handle;
	Rect	pic_rect;
	short	pic_wd,pic_hgt;
	GWorldPtr	myGWorld;
	CGrafPtr	origPort;
	GDHandle	origDev;
	QDErr		check_error;
	PixMapHandle	offPMHandle;
	char good;
	printf("Loading PICT id %i...\n",picture_to_get);
    current_pic_handle = GetPicture (picture_to_get);
	if(ResError() != noErr){
		SysBeep(50);
		exit(1);
	}
	QDGetPictureBounds(current_pic_handle, &pic_rect);
	pic_wd = pic_rect.right - pic_rect.left;
	pic_hgt = pic_rect.bottom - pic_rect.top;  
	//printf("%i\t%i\n",pic_wd,pic_hgt);
	GetGWorld (&origPort, &origDev);
	check_error = NewGWorld (&myGWorld, 0, &pic_rect, NULL, NULL, kNativeEndianPixMap);
	if (check_error != noErr)  {
		SysBeep(50);
		printf("Error %i in load_pict()\n",check_error);
		ExitToShell();
	}
	
	SetGWorld(myGWorld, NULL);
	
	offPMHandle = GetGWorldPixMap (myGWorld);
	good = LockPixels (offPMHandle);
	if (good == FALSE)  {
		SysBeep(50);
	}
	SetRect (&pic_rect, 0, 0, pic_wd, pic_hgt);
	DrawPicture (current_pic_handle, &pic_rect);
	SetGWorld (origPort, origDev);
	UnlockPixels (offPMHandle);
	ReleaseResource ((Handle) current_pic_handle);
	
	return myGWorld;
}

GWorldPtr load_bmp(unsigned char *data, long length){
	
	if (length < 54) {
		return NULL; // Too short for headers
	}
	unsigned char * cur = data + 10;
	unsigned int offset = readUInt(&cur);
	cur+=4;
	unsigned int width = readUInt(&cur);
	unsigned int height = readUInt(&cur);
	cur += 2;
	unsigned short bitCount = readUShort(&cur);
	cur+=24;
	int indexed;
	unsigned int colourTable[256];
	int i;
	if (bitCount == 8) {
		if (length < 54 + 256 * 4) {
			return NULL;
		}
		for (i = 0; i<256; ++i) {
			unsigned char blue = *cur++;
			unsigned char green = *cur++;
			unsigned char red = *cur++;
			cur++;
			colourTable[i] = (red << 16) + (green << 8) + blue;
		}
		indexed = TRUE;
	}
	else if (bitCount == 24) {
		indexed = FALSE;
	}
	else {
		return NULL;
	}
	
	int bmppadding = (width * bitCount / 8) % 4;
	if (bmppadding != 0) {
		bmppadding = 4 - bmppadding;
	}
	
	if (length < offset + height * (bmppadding + width * bitCount / 8)) {
		return NULL;
	}
	
	cur = data + offset;
	
	GWorldPtr newGWorld;
	Rect picRext = {0, 0, height, width};
	NewGWorld(&newGWorld,32,&picRext,NULL,NULL,kNativeEndianPixMap);
	if (newGWorld == NULL) {
		return NULL;
	}
	
	PixMapHandle pixMap = GetGWorldPixMap(newGWorld);
	LockPixels(pixMap);
	unsigned int * picBuf = (unsigned int*) GetPixBaseAddr(pixMap);
	int pixrow = ((*pixMap)->rowBytes & 0x3FFF) / 4;
	
	int j;
	for (i = height - 1; i>= 0 ; --i) {
		for (j=0; j<width; ++j) {
			if (indexed) {
				picBuf[i * pixrow + j] = colourTable[*cur++];
			}
			else {
				unsigned char red = *cur++;
				unsigned char green = *cur++;
				unsigned char blue = *cur++;
				picBuf[i * pixrow + j]  = (red << 16) + (green << 8) + blue;
			}
			
		}
		cur+= bmppadding;
	}
	UnlockPixels(pixMap);
	return newGWorld;
}

void set_cursor(short which_c) {
	short i;
	
	current_cursor = which_c;
	HLock ((Handle) cursors[current_cursor]);
	SetCursor (*cursors[current_cursor]);
	HUnlock((Handle) cursors[current_cursor]);
}

void restore_cursor(){
	set_cursor(current_cursor);
}

/*
void set_cursor(CursHandle which_curs){
	HLock ((Handle) which_curs);
	SetCursor (*which_curs);
	HUnlock((Handle) which_curs);
}
*/
//	 masked; // if 10 - make AddOver
//   main_win; // if 2, drawing onto dialog
void rect_draw_some_item (GWorldPtr src_gworld,Rect src_rect,GWorldPtr targ_gworld,Rect targ_rect,
						  char masked,short main_win){
	Rect	destrec;
	PixMapHandle	test1, test2;
	const BitMap * store_dest;
	GrafPtr cur_port;
	RGBColor	store_color;
	
	if (main_win == 2) {
		GetBackColor(&store_color);
		BackColor(whiteColor);
	}
	
	GetPort(&cur_port);
	if (src_gworld == NULL) {
		if (main_win == 0) {
			SetPort ( targ_gworld);
			PaintRect(&targ_rect);
			SetPort (cur_port);
		}
		else PaintRect(&targ_rect);
		return;
	}
	
	store_dest = GetPortBitMapForCopyBits(cur_port);
	
	test1 = GetPortPixMap(src_gworld);
	
	if (main_win == 1) 	
		OffsetRect(&targ_rect,offset.h,offset.v); // would this cause problems for the pc editor?
	
	LockPixels(test1);
	if (main_win == 0) {
		test2 = GetPortPixMap(targ_gworld); 
		LockPixels(test2);
		if (masked == 1) 
			CopyBits ( (BitMap*)* test1 ,
					  (BitMap*)*test2 ,
					  &src_rect, &targ_rect, 
					  transparent , NULL);	
		else CopyBits ( (BitMap*)* test1 ,
					   (BitMap*)*test2 ,
					   &src_rect, &targ_rect, 
					   (masked == 10) ? addOver : 0, NULL);
		UnlockPixels(test2);
	}  
	else {
		if (masked == 1) 
			CopyBits ( (BitMap*)*test1 ,
					  store_dest ,
					  &src_rect, &targ_rect, 
					  transparent , NULL);
		else CopyBits ((BitMap*) *test1 ,
					   store_dest ,
					   &src_rect, &targ_rect, 
					   (masked == 10) ? addOver : 0, NULL);
	}
	UnlockPixels(test1);
	if (main_win == 2) 
		RGBBackColor(&store_color);
	SetPort(cur_port);
}

void char_win_draw_string(WindowPtr dest_window,Rect dest_rect,char *str,short mode,short line_height,bool main_win){
	char_port_draw_string(GetWindowPort(dest_window),dest_rect,str,mode,line_height, main_win);
}

void char_port_draw_string(GrafPtr dest_window,Rect dest_rect,char *str,short mode,short line_height,bool main_win){
	Str255 store_s;
	
	strcpy((char*) store_s,str);
	win_draw_string(dest_window, dest_rect,store_s, mode, line_height, main_win);
	
}

// mode: 0 - align up and left, 1 - center on one line
// str is a c string, 256 characters
// uses current font
void win_draw_string(GrafPtr dest_window,Rect dest_rect,Str255 str,short mode,short line_height,bool main_win){
	GrafPtr old_port;
	Str255 p_str,str_to_draw,str_to_draw2,c_str;
	static Str255 null_s = "8";
	if(null_s[0] == '8'){
		for(int i = 0;i < 256; i++)
			null_s[i] = ' ';
	}
	short str_len,i;
	short last_line_break = 0,last_word_break = 0,on_what_line = 0;
	short text_len[257];
	short total_width = 0;
	Boolean end_loop,force_skip = FALSE;
	KeyMap key_state;
	long dummy3;
	//RgnHandle current_clip;
	short adjust_x = 0,adjust_y = 0;
	printf("Drawing string \"%s\"\n",str);
	if (main_win) {
		adjust_x = offset.h; adjust_y = offset.v;
	}
	strcpy((char *) p_str,(char *) str);
	strcpy((char *) c_str,(char *) str);
	c2pstr((char*) p_str);	
	for (i = 0; i < 257; i++)
		text_len[i]= 0;
	MeasureText(256,p_str,text_len);
	str_len = (short) strlen((char *)str);
	if (str_len == 0) {
		return;
	}
	
	GetPort(&old_port);	
	SetPort( dest_window);
	
	//current_clip = NewRgn();
	//GetClip(current_clip);
	
	dest_rect.bottom += 5;
	dest_rect.bottom -= 5;
	
	for (i = 0; i < 257; i++)
		if ((text_len[i] > total_width) && (i <= str_len))
			total_width = text_len[i];
	if ((mode == 0) && (total_width < dest_rect.right - dest_rect.left))
		mode = 2;
	for (i = 0; i < 257; i++)
		if ((i <= str_len) && (c_str[i] == '|') && (mode == 2))
			mode = 0;
	
	switch (mode) {
		case 0: 
			MoveTo(dest_rect.left + 1 + adjust_x, dest_rect.top + 1 + line_height * on_what_line + adjust_y + 9);
			for (i = 0;text_len[i] != text_len[i + 1], i < str_len;i++) {
				if (((text_len[i] - text_len[last_line_break] > (dest_rect.right - dest_rect.left - 6)) 
					 && (last_word_break > last_line_break)) || (c_str[i] == '|')) {
				  	if (c_str[i] == '|') {
				  		c_str[i] = ' ';
				  		force_skip = TRUE;
					}
					sprintf((char *)str_to_draw,"%s",(char *)null_s);
					strncpy ((char *) str_to_draw,(char *) c_str + last_line_break,(size_t) (last_word_break - last_line_break - 1));
					//sprintf((char *)str_to_draw2," %s",str_to_draw);
					//str_to_draw2[0] = (char) strlen((char *)str_to_draw);
					//DrawString(str_to_draw2);
					c2pstr((char*)str_to_draw);
					DrawString(str_to_draw);
					on_what_line++;
					MoveTo(dest_rect.left + 1 + adjust_x, dest_rect.top + 1 + line_height * on_what_line + adjust_y + 9);
					last_line_break = last_word_break;
					if (force_skip == TRUE) {
						force_skip = FALSE;
						i++;
						last_line_break++;
						last_word_break++;
					}
				}
				if (c_str[i] == ' ')
					last_word_break = i + 1;
				if (on_what_line == LINES_IN_TEXT_WIN - 1)
					i = 10000;
			}
			
			if (i - last_line_break > 1) {
				strcpy((char *)str_to_draw,(char *)null_s);
				strncpy ((char *) str_to_draw,(char *) c_str + last_line_break,(size_t) (i - last_line_break));
				sprintf((char *)str_to_draw2," %s",str_to_draw);
				//if (strlen((char *) str_to_draw2) > 3) {
				//	str_to_draw2[0] = (char) strlen((char *)str_to_draw);
				//	DrawString(str_to_draw2);
				//}
				if(strlen((char*)str_to_draw) > 2){
					c2pstr((char*)str_to_draw);
					DrawString(str_to_draw);
				}
			}	
			break;
		case 1:
			MoveTo((dest_rect.right + dest_rect.left) / 2 - (4 * total_width) / 9 + adjust_x, 
				   (dest_rect.bottom + dest_rect.top - line_height) / 2 + 9 + adjust_y);	
			DrawString(p_str);
			break;
		case 2:
			MoveTo(dest_rect.left + 1 + adjust_x, 
				   dest_rect.top + 1 + adjust_y + 9);
			DrawString(p_str);					
			break;
		case 3:
			MoveTo(dest_rect.left + 1 + adjust_x, 
				   dest_rect.top + 1 + adjust_y + 9 + (dest_rect.bottom - dest_rect.top) / 6);
			DrawString(p_str);					
			break;
	}
	//SetClip(current_clip);
	//DisposeRgn(current_clip);
	SetPort(old_port);
}

short string_length(char *str){ // Why not just use strlen?
	short text_len[257];
	short total_width = 0,i,len;
	Str255 p_str;
	
	for (i = 0; i < 257; i++)
		text_len[i]= 0;
	
	strcpy((char *) p_str,str);
	c2pstr((char*) p_str);
	MeasureText(256,p_str,text_len);
	len = strlen((char *)str);
	
	for (i = 0; i < 257; i++)
		if ((text_len[i] > total_width) && (i <= len))
			total_width = text_len[i];
	return total_width;
}

/*OSStatus flip_pict(OSType domain, OSType type, short id, void *ptr, UInt32 size, Boolean isNative, void *refcon){
	//PicHandle
	PicPtr toFlip = (PicPtr) ptr;
	
	if(!isNative){
		printf("PICT %i Before: %i\t",id,toFlip->picFrame.bottom - toFlip->picFrame.top);
		toFlip->picSize = Endian16_Swap(toFlip->picSize);
		toFlip->picFrame.top = Endian16_Swap(toFlip->picFrame.top);
		toFlip->picFrame.left = Endian16_Swap(toFlip->picFrame.left);
		toFlip->picFrame.bottom = Endian16_Swap(toFlip->picFrame.bottom);
		toFlip->picFrame.right = Endian16_Swap(toFlip->picFrame.right);
		printf("After: %i\n",toFlip->picFrame.bottom - toFlip->picFrame.top);
	}
	
	return noErr;
}
*/
/*
void draw_terrain(){
	short q,r,x,y,i,small_i;
	location which_pt,where_draw;
	Rect draw_rect,clipping_rect = {8,8,332,260};	
	unsigned char t_to_draw;
	Rect source_rect,tiny_to,tiny_to_base = {37,29,44,36},tiny_from,from_rect,to_rect;
	Rect boat_rect[4] = {{0,0,36,28}, {0,28,36,56},{0,56,36,84},{0,84,36,112}};
	
	if (overall_mode >= 60)
		return;
	
	OffsetRect(&boat_rect[0],61,0);
	
	if (cur_viewing_mode == 0) {
		SetPort( ter_draw_gworld);
		FillCRect(&terrain_rect,bg[6]);
		FrameRect(&terrain_rect);
		SetPortWindowPort(mainPtr);
		for (q = 0; q < 9; q++) 
			for (r = 0; r < 9; r++)
			{
				where_draw.x = q; where_draw.y = r;
				if (editing_town == TRUE) {
					t_to_draw = t_d.terrain[cen_x + q - 4][cen_y + r - 4];		
				}
				else {
					if (cen_x + q - 4 == -1) 
						t_to_draw = borders[3][cen_y + r - 4];
					else if (cen_x + q - 4 == 48) 
						t_to_draw = borders[1][cen_y + r - 4];
					else if (cen_y + r - 4 == -1) 
						t_to_draw = borders[0][cen_x + q - 4];
					else if (cen_y + r - 4 == 48) 
						t_to_draw = borders[2][cen_x + q - 4];
					else t_to_draw = current_terrain.terrain[cen_x + q - 4][cen_y + r - 4];
				}
				draw_one_terrain_spot(q,r,t_to_draw);
				which_pt.x = cen_x + q - 4;
				which_pt.y =cen_y + r - 4;	
				
				tiny_to = tiny_to_base;	
				OffsetRect(&tiny_to,28 * q, 36 * r);
				
				// draw start icon, if starting point
				if ((editing_town == TRUE) && 
					(cur_town == scenario.which_town_start) && (scenario.where_start.x == cen_x + q - 4)
					&& (scenario.where_start.y == cen_y + r - 4)) {
					from_rect = start_button_from;
					to_rect = tiny_to;
					to_rect.left -= 14;
					rect_draw_some_item(editor_mixed,from_rect,ter_draw_gworld,to_rect,0,0);
					OffsetRect(&tiny_to,0,-7);
				}
				if ((editing_town == FALSE)
					&& (scenario.out_sec_start.x == cur_out.x)
					&& (scenario.out_sec_start.y == cur_out.y) 
					&& (scenario.out_start.x == cen_x + q - 4)
					&& (scenario.out_start.y == cen_y + r - 4)) {
					from_rect = start_button_from;
					to_rect = tiny_to;
					to_rect.left -= 14;
					rect_draw_some_item(editor_mixed,from_rect,ter_draw_gworld,to_rect,0,0);
					OffsetRect(&tiny_to,0,-7);
				}
				small_i = small_icons[scenario.ter_types[t_to_draw].special];
				if ((small_i == 30) && (scenario.ter_types[t_to_draw].flag2 >= 5))
					small_i = 31;
				if ((small_i == 31) && (scenario.ter_types[t_to_draw].flag2 == 10))
					small_i = 32;
				tiny_from = base_small_button_from;
				OffsetRect(&tiny_from,7 * (small_i % 10),7 * (small_i / 10));
				if (small_i > 0) {
					rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
					OffsetRect(&tiny_to,0,-7);
				}
				
				if (is_special(cen_x + q - 4,cen_y + r - 4) == TRUE) {
					tiny_from = base_small_button_from;
					OffsetRect(&tiny_from,7 * (7),7 * (0));
					rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
					OffsetRect(&tiny_to,0,-7);
				}	
				if ((t_to_draw == 7) || (t_to_draw == 10) || (t_to_draw == 13) || (t_to_draw == 16)) {
					tiny_from = base_small_button_from;
					OffsetRect(&tiny_from,7 * (3),7 * (2));
					rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
					OffsetRect(&tiny_to,0,-7);
				}	
				//if (is_s_d(cen_x + q - 4,cen_y + r - 4) == TRUE) {
				//	}	
				if (editing_town == FALSE) {
					for (i = 0; i < 4; i++)		
						if ((cen_x + q - 4 == current_terrain.wandering_locs[i].x) &&
							(cen_y + r - 4 == current_terrain.wandering_locs[i].y)) {
							tiny_from = base_small_button_from;
							OffsetRect(&tiny_from,7 * (2),7 * (1));
							rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
							OffsetRect(&tiny_to,0,-7);
							i = 4;
						}			
					
				}
				
				if (editing_town == TRUE) {
					for (i = 0; i < 30; i++) {
						if ((scenario.scen_boats[i].which_town == cur_town) &&
							(scenario.scen_boats[i].boat_loc.x == cen_x + q - 4) &&
							(scenario.scen_boats[i].boat_loc.y == cen_y + r - 4))
							Draw_Some_Item(mixed_gworld,boat_rect[0],ter_draw_gworld,where_draw,1,0);
						
					}	
					for (i = 0; i < 30; i++) {
						source_rect = boat_rect[0];
						OffsetRect(&source_rect,0,74);
						OffsetRect(&source_rect,56,36);
						if ((scenario.scen_horses[i].which_town == cur_town) &&
							(scenario.scen_horses[i].horse_loc.x == cen_x + q - 4) &&
							(scenario.scen_horses[i].horse_loc.y == cen_y + r - 4))
							Draw_Some_Item(mixed_gworld,source_rect,ter_draw_gworld,where_draw,1,0);
						
					}	
					for (i = 0; i < 4; i++)		
						if ((cen_x + q - 4 == town.start_locs[i].x) &&
							(cen_y + r - 4 == town.start_locs[i].y)) {
							tiny_from = base_small_button_from;
							OffsetRect(&tiny_from,7 * (6 + i),7 * (1));
							rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
							OffsetRect(&tiny_to,0,-7);
						}			
					for (i = 0; i < 4; i++)		
						if ((cen_x + q - 4 == town.wandering_locs[i].x) &&
							(cen_y + r - 4 == town.wandering_locs[i].y)) {
							tiny_from = base_small_button_from;
							OffsetRect(&tiny_from,7 * (2),7 * (1));
							rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
							OffsetRect(&tiny_to,0,-7);
							i = 4;
						}			
					if (is_web(cen_x + q - 4,cen_y + r - 4) == TRUE) {
						from_rect = calc_rect(5,0);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
					}	
					if (is_crate(cen_x + q - 4,cen_y + r - 4) == TRUE) {
						from_rect = calc_rect(6,0);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
					}	
					if (is_barrel(cen_x + q - 4,cen_y + r - 4) == TRUE) {
						from_rect = calc_rect(7,0);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
					}	
					if (is_fire_barrier(cen_x + q - 4,cen_y + r - 4) == TRUE) {
						from_rect = calc_rect(0,2);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
					}	
					if (is_quickfire(cen_x + q - 4,cen_y + r - 4) == TRUE) {
						from_rect = calc_rect(7,1);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
					}	
					if (is_force_barrier(cen_x + q - 4,cen_y + r - 4) == TRUE) {
						from_rect = calc_rect(2,2);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
					}	
					for (i = 0; i < 8; i++) 
						if (is_sfx(cen_x + q - 4,cen_y + r - 4,i)) {
							from_rect = calc_rect(i,3);
							Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);					
						}
					for (x = 0; x < 64; x++)
						if ((cen_x + q - 4 == town.preset_items[x].item_loc.x) &&
							(cen_y + r - 4 == town.preset_items[x].item_loc.y) && (town.preset_items[x].item_code >= 0)) {
						}
					for (x = 0; x < 60; x++)
						if ((cen_x + q - 4 == t_d.creatures[x].start_loc.x) &&
							(cen_y + r - 4 == t_d.creatures[x].start_loc.y) && (t_d.creatures[x].number != 0)) {
						}
					
				}
			}
		if (editing_town == TRUE) {
			draw_monsts();
			draw_items();
		}
		
		SetPort( ter_draw_gworld);
		ClipRect(&clipping_rect);
		
		if (editing_town == TRUE) {
			// draw info rects
			for (i = 0; i < 16; i++)
				if (t_d.room_rect[i].left > 0) {
					draw_rect.left = 22 + 28 * (t_d.room_rect[i].left - cen_x + 4);
					draw_rect.right = 22 + 28 * (t_d.room_rect[i].right - cen_x + 4);
					draw_rect.top = 24 + 36 * (t_d.room_rect[i].top - cen_y + 4);
					draw_rect.bottom = 24 + 36 * (t_d.room_rect[i].bottom - cen_y + 4);	
					ForeColor(redColor);
					FrameRect(&draw_rect);
					ForeColor(blackColor);
				}
			// draw border rect
			draw_rect.left = 21 + 28 * (town.in_town_rect.left - cen_x + 4);
			draw_rect.right = 21 + 28 * (town.in_town_rect.right - cen_x + 4);
			draw_rect.top = 25 + 36 * (town.in_town_rect.top - cen_y + 4);
			draw_rect.bottom = 25 + 36 * (town.in_town_rect.bottom - cen_y + 4);	
			ForeColor(whiteColor);
			FrameRect(&draw_rect);
			ForeColor(blackColor);
		}
		if (editing_town == FALSE) {
			// draw info rects
			for (i = 0; i < 8; i++)
				if (current_terrain.info_rect[i].left > 0) {
					draw_rect.left = 22 + 28 * (current_terrain.info_rect[i].left - cen_x + 4);
					draw_rect.right = 22 + 28 * (current_terrain.info_rect[i].right - cen_x + 4);
					draw_rect.top = 24 + 36 * (current_terrain.info_rect[i].top - cen_y + 4);
					draw_rect.bottom = 24 + 36 * (current_terrain.info_rect[i].bottom - cen_y + 4);	
					ForeColor(redColor);
					FrameRect(&draw_rect);
					ForeColor(blackColor);
				}
		}
		ClipRect(&terrain_rect);
		SetPortWindowPort(mainPtr);
		
		small_any_drawn = FALSE;
		//if (cur_viewing_mode == 0) 
		//	draw_frames();
	}
	
	if (cur_viewing_mode == 1) {
		SetPort( ter_draw_gworld);
		if (small_any_drawn == FALSE) {
		 	FillCRect(&terrain_rect,bg[6]);
			FrameRect(&terrain_rect);
		}
		for (q = 0; q < ((editing_town == TRUE) ? max_dim[town_type] : 48); q++) 
			for (r = 0; r < ((editing_town == TRUE) ? max_dim[town_type] : 48); r++) {
				t_to_draw = (editing_town == TRUE) ? t_d.terrain[q][r] :
				current_terrain.terrain[q][r];
				if ((small_what_drawn[q][r] != t_to_draw) || (small_any_drawn == FALSE)) {
					draw_one_tiny_terrain_spot(q,r,t_to_draw);
					small_what_drawn[q][r] = t_to_draw;
				}
			}
		SetPortWindowPort(mainPtr);
		small_any_drawn = TRUE;
	}
	
	//draw_cur_string();
	place_location();
	
	//to_rect = world_screen;
	//OffsetRect(&to_rect,TER_RECT_UL_X,TER_RECT_UL_Y);
	rect_draw_some_item(ter_draw_gworld,terrain_rect,ter_draw_gworld,world_screen,0,1);
}*/

Rect calc_rect(short i, short j){
	Rect base_rect = {0,0,36,28};
	
	OffsetRect(&base_rect,i * 28, j * 36);
	return base_rect;
}

Rect get_custom_rect (short which_rect){
	Rect store_rect = {0,0,36,28};
	
	OffsetRect(&store_rect,28 * (which_rect % 10),36 * (which_rect / 10));
	return store_rect;
}

m_pic_index_t m_pic_index[] = {
	{1, 1, 1},
	{2, 1, 1},
	{3, 1, 1},
	{4, 1, 1},
	{5, 1, 1},
	{6, 1, 1},
	{7, 1, 1},
	{8, 1, 1},
	{9, 1, 1},
	{10, 1, 1},
	//10
	{11, 1, 1},
	{12, 1, 1},
	{13, 1, 1},
	{14, 1, 1},
	{15, 1, 1},
	{16, 1, 1},
	{17, 1, 1},
	{18, 1, 1},
	{19, 1, 1},
	{20, 1, 1},
	//20
	{21, 1, 1},
	{22, 1, 1},
	{23, 1, 1},
	{24, 1, 1},
	{25, 1, 1},
	{26, 1, 1},
	{27, 1, 1},
	{28, 1, 1},
	{29, 1, 1},
	{30, 1, 1},
	//30
	{31, 1, 1},
	{32, 1, 1},
	{33, 1, 1},
	{34, 1, 1},
	{35, 1, 1},
	{36, 1, 1},
	{37, 1, 1},
	{38, 1, 1},
	{39, 1, 1},
	{40, 1, 1},
	//40
	{41, 1, 1},
	{42, 1, 1},
	{43, 1, 1},
	{44, 1, 1},
	{46, 1, 1},
	{47, 1, 1},
	{48, 1, 1},
	{49, 1, 1},
	{50, 1, 1},
	{51, 1, 2},
	//50
	{53, 1, 2},
	{55, 1, 2},
	{57, 1, 2},
	{59, 1, 1},
	{60, 1, 1},
	{61, 1, 1},
	{62, 1, 1},
	{63, 1, 1},
	{64, 1, 1},
	{65, 1, 1},
	//60
	{66, 1, 1},
	{67, 1, 1},
	{68, 1, 1},
	{69, 1, 1},
	{70, 1, 1},
	{71, 1, 1},
	{72, 1, 1},
	{73, 1, 1},
	{74, 1, 1},
	{75, 1, 1},
	//70
	{76, 1, 1},
	{77, 1, 1},
	{78, 1, 1},
	{79, 2, 1},
	{81, 1, 1},
	{82, 1, 1},
	{83, 1, 2},
	{85, 1, 1},
	{86, 1, 1},
	{87, 1, 1},
	//80
	{88, 1, 1},
	{89, 1, 1},
	{90, 1, 1},
	{91, 1, 1},
	{92, 1, 1},
	{93, 1, 1},
	{94, 1, 1},
	{95, 1, 1},
	{96, 1, 1},
	{97, 1, 1},
	//90
	{98, 1, 1},
	{99, 1, 1},
	{100, 1, 1},
	{101, 1, 1},
	{102, 1, 1},
	{103, 1, 1},
	{104, 1, 1},
	{105, 1, 1},
	{106, 1, 1},
	{107, 1, 1},
	//100
	{108, 1, 1},
	{109, 2, 1},
	{111, 1, 1},
	{112, 1, 1},
	{113, 1, 1},
	{114, 2, 1},
	{116, 1, 1},
	{117, 1, 1},
	{118, 1, 1},
	{119, 1, 1},
	//110
	{120, 2, 1},
	{122, 1, 1},
	{123, 1, 2},
	{125, 1, 2},
	{127, 1, 1},
	{128, 1, 1},
	{129, 1, 1},
	{130, 1, 1},
	{131, 2, 2},
	{135, 1, 1},
	//120
	{136, 1, 1},
	{137, 2, 1},
	{139, 1, 1},
	{140, 1, 1},
	{141, 1, 1},
	{142, 1, 1},
	{143, 1, 1},
	{144, 1, 1},
	{145, 1, 1},
	{146, 1, 1},
	//130
	{147, 1, 1},
	{148, 1, 1},
	{149, 1, 1},
	{150, 1, 1},
	{151, 1, 1},
	{152, 1, 1},
	{153, 1, 1},
	{154, 1, 1},
	{155, 2, 2},
	{159, 1, 1},
	//140
	{160, 2, 2},
	{164, 2, 1},
	{166, 2, 1},
	{168, 1, 2},
	{170, 1, 1},
	{171, 1, 1},
	{172, 1, 1},
	{173, 1, 1},
	{174, 1, 1},
	{175, 1, 1},
	//150
	{176, 1, 1},
	{177, 1, 1},
	{178, 1, 1},
	{179, 1, 1},
	{180, 1, 1},
	{181, 1, 1},
	{182, 1, 1},
	{183, 1, 1},
	{184, 1, 1},
	{185, 1, 1},
	//160
	{186, 1, 1},
	{187, 1, 1},
	{188, 1, 1},
	{189, 1, 1},
	{190, 1, 1},
	{191, 1, 1},
	{192, 1, 1},
	{193, 1, 1},
	{194, 1, 1},
	{195, 1, 1},
	//170
	{196, 1, 1},
	{197, 1, 1},
	{198, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	//180
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	//190
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	{0, 1, 1},
	//200
};