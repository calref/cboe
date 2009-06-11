/*
 *  graphtool.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#define GRAPHTOOL_CPP
#include <Carbon/Carbon.h>
#include <Quicktime/Quicktime.h>
#include "graphtool.h"
#include "cursors.h"

cursor_type arrow_curs[3][3] = {
	{NW_curs, N_curs, NE_curs},
	{W_curs,wait_curs,E_curs},
	{SW_curs, S_curs, SE_curs},
};
cursor_type current_cursor = sword_curs;
CursorRef cursors[24] = {
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
};
void (*redraw_screen)();
Rect bg[21];
Rect map_pat[30];
GWorldPtr bg_gworld;
short geneva_font_num, dungeon_font_num;
bool use_win_graphics = false;
CursorRef GetCursorFromPath(std::string filename, Point hotspot);

void clean_up_graphtool(){
	for(int i = 0; i < 24; i++)
		DisposeNSCursor(cursors[i]);
	CleanUp();
}

void init_graph_tool(void (*redraw_callback)()){
	redraw_screen = redraw_callback;
	int i,j;
	static const Point cursor_hs[24] = {
		{ 1, 4}, {14, 1}, {13, 5}, {8,8}, {8,8}, {8,8}, {8,8}, {0,14},
		{ 3,12}, { 7,13}, { 3,12},
		{ 3, 9}, { 8, 8}, { 8, 3},
		{ 3,12}, { 7,13}, { 3,12},
		{ 1, 1}, { 7, 3}, { 0,14}, {8,8}, {6,7}, {3,2}, {7,6}
	};
	static const Point pat_offs[17] = {
		{3,0}, {1,1}, {1,2}, {0,2},
		{0,3}, {1,3}, {3,1}, {0,0},
		{2,0}, {2,1}, {1,0}, {2,2},
		{3,2}, {2,3}, {0,1}, {0,4}, {3,3}
	};
	static const int pat_i[17] = {
		2, 3, 4, 5, 6, 8, 9, 10,
		11,12,13,14,15,16,17,19,20
	};
	static const int map_i[26] = {
		0, 1, 2, 3, 4, 5, 6, 9,
		10,11,12,13,14,15,16,17,
		18,19,20,21,22,24,25,27,
		28,29
	};
	static const char*const edit_cursor_files[8] = {
		"wand.gif","eyedropper.gif","brush.gif","spraycan.gif",
		"eraser.gif","topleft.gif","bottomright.gif","hand.gif"
	};
	static const char*const arrow_files[3][3] = {
		{"NW.gif","W.gif","SW.gif"},
		{"N.gif","wait.gif","S.gif"},
		{"NE.gif","E.gif","SE.gif"}
	};
	static const char*const game_cursor_files[7] = {
		"sword.gif","boot.gif","drop.gif","target.gif",
		"talk.gif","key.gif","look.gif"
	};
	if (cursors[0] == NULL) {
		for (i = 0; i < 8; i++)
			cursors[i] = GetCursorFromPath(edit_cursor_files[i],cursor_hs[i]);
		for (i = 0; i < 3; i++)
			for (j = 0; j < 3; j++)
				cursors[arrow_curs[i][j]] = GetCursorFromPath(arrow_files[i][j],cursor_hs[arrow_curs[i][j]]);
		for (i = 17; i < 24; i++)
			cursors[i] = GetCursorFromPath(game_cursor_files[i - 17],cursor_hs[i]);
	}
//	for (i = 0; i < 21; i++) 
//	    bg[i] = GetPixPat(128 + i);
	for(i = 0; i < 17; i++){
		SetRect(&bg[pat_i[i]],0,0,64,64);
		OffsetRect(&bg[pat_i[i]],64 * pat_offs[i].h,64 * pat_offs[i].v);
	}
	Rect tmp_rect = bg[19];
	OffsetRect(&tmp_rect, 0, 64);
	bg[0] = bg[1] = bg[18] = map_pat[7] = tmp_rect;
	bg[0].right -= 32;
	bg[0].bottom -= 32;
	bg[1].left -= 32;
	bg[1].bottom -= 32;
	bg[18].right -= 32;
	bg[18].top -= 32;
	map_pat[7].left -= 32;
	map_pat[7].top -= 32;
	OffsetRect(&tmp_rect, 0, 64);
	map_pat[8] = map_pat[23] = map_pat[26] = tmp_rect;
	map_pat[8].right -= 32;
	map_pat[8].bottom -= 32;
	map_pat[23].left -= 32;
	map_pat[23].right -= 16;
	map_pat[23].bottom -= 32;
	map_pat[26].left -= 32 + 16;
	map_pat[26].bottom -= 32;
	OffsetRect(&tmp_rect, 0, 64);
	bg[7] = tmp_rect;
	bg[7].bottom = bg[7].top + 16;
	OffsetRect(&tmp_rect, 0, -32);
	tmp_rect.right = tmp_rect.left + 8;
	tmp_rect.bottom = tmp_rect.top + 8;
	for(i = 0; i < 26; i++){
		map_pat[map_i[i]] = tmp_rect;
		OffsetRect(&map_pat[map_i[i]],8 * (i % 8),8 * (i / 8));
		// Note: 8 * (i / 8) != i, despite appearances, due to integer rounding
	}
	bg_gworld = load_pict("pixpats.png");
	// TODO: Try to deprecate these font things
	Str255 fn1 = "\pGeneva";
	Str255 fn2 = "\pDungeon Bold";
	Str255 fn3 = "\pPalatino";
	GetFNum(fn1,&geneva_font_num);
	if (geneva_font_num == 0)
		GetFNum(fn3,&geneva_font_num);
	GetFNum(fn2,&dungeon_font_num);
	if (dungeon_font_num == 0)
		GetFNum(fn3,&dungeon_font_num);
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

extern std::string progDir;
GWorldPtr load_pict(std::string picture_to_get, std::string scen_name){
	std::string filePath;
	FSRef ref;
	FSSpec spec;
	if(scen_name != ""){
		filePath = progDir + "/Blades of Exile Scenarios/" + scen_name;
		filePath.replace(filePath.find_last_of('.'),filePath.size(),".exr");
		filePath += '/' + picture_to_get;
		printf(filePath.c_str());
		printf("\n\n");
		FSPathMakeRef((UInt8*)filePath.c_str(), &ref, NULL);
		if(FSGetCatalogInfo(&ref, kFSCatInfoNone, NULL, NULL, &spec, NULL) != fnfErr)
			return importPictureFileToGWorld(&spec);
	}
	filePath = progDir + "/Scenario Editor/graphics.exd/";
	if(use_win_graphics) filePath += "win/";
	else filePath += "mac/";
	filePath += picture_to_get;
	printf("Loading graphics sheet from: %s\n\n",filePath.c_str());
	FSPathMakeRef((UInt8*)filePath.c_str(), &ref, NULL);
	if(FSGetCatalogInfo(&ref, kFSCatInfoNone, NULL, NULL, &spec, NULL) != fnfErr)
		return importPictureFileToGWorld(&spec);
	throw std::string("Graphic ") + picture_to_get + std::string(" not found."); // TODO: Create an exception class for this
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
	//printf("Loading PICT id %i...\n",picture_to_get);
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
	if (good == false)  {
		SysBeep(50);
	}
	SetRect (&pic_rect, 0, 0, pic_wd, pic_hgt);
	DrawPicture (current_pic_handle, &pic_rect);
	SetGWorld (origPort, origDev);
	UnlockPixels (offPMHandle);
	ReleaseResource ((Handle) current_pic_handle);
	
	return myGWorld;
}

GWorldPtr load_bmp(unsigned char *data, unsigned long length){
	
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
	unsigned int i;
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
		indexed = true;
	}
	else if (bitCount == 24) {
		indexed = false;
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
	
	unsigned int j;
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

void set_cursor(cursor_type which_c) {
	current_cursor = which_c;
	SetNSCursor(cursors[current_cursor]);
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
void rect_draw_some_item (GWorldPtr src_gworld,Rect src_rect,GWorldPtr targ_gworld,Rect targ_rect,char mode){
	PixMapHandle pix1, pix2;
	GrafPtr cur_port;
	RGBColor store_color;
	
	GetBackColor(&store_color);
	BackColor(whiteColor);
	GetPort(&cur_port);
	
	if (src_gworld == NULL) {
		SetPort ( targ_gworld);
		PaintRect(&targ_rect);
		SetPort (cur_port);
		return;
	}
	
	pix1 = GetPortPixMap(src_gworld);
	
	LockPixels(pix1);
	pix2 = GetPortPixMap(targ_gworld); 
	LockPixels(pix2);
	CopyBits((BitMap*)*pix1, (BitMap*)*pix2, &src_rect, &targ_rect, mode, NULL);
	UnlockPixels(pix2);
	UnlockPixels(pix1);
	
	RGBBackColor(&store_color);
	SetPort(cur_port);
}

void rect_draw_some_item (GWorldPtr src_gworld,Rect src_rect,Rect targ_rect,Point offset,char mode){
	PixMapHandle pix1;
	const BitMap* pix2;
	GrafPtr cur_port;
	RGBColor	store_color;
	
	GetBackColor(&store_color);
	BackColor(whiteColor);
	GetPort(&cur_port);
	
	if (src_gworld == NULL) {
		PaintRect(&targ_rect);
		return;
	}
	
	OffsetRect(&targ_rect,offset.h,offset.v);
	
	pix2 = GetPortBitMapForCopyBits(cur_port);
	pix1 = GetPortPixMap(src_gworld);
	
	LockPixels(pix1);
	CopyBits((BitMap*)*pix1, pix2, &src_rect, &targ_rect, mode, NULL);
	UnlockPixels(pix1);
	
	RGBBackColor(&store_color);
	SetPort(cur_port);
}

void char_win_draw_string(WindowPtr dest_window,Rect dest_rect,const char *str,short mode,short line_height,Point offset){
	char_port_draw_string(GetWindowPort(dest_window),dest_rect,str,mode,line_height, offset);
}

void char_port_draw_string(GrafPtr dest_window,Rect dest_rect,const char *str,short mode,short line_height,Point offset){
	Str255 store_s;
	
	strcpy((char*) store_s,str);
	win_draw_string(dest_window, dest_rect,store_s, mode, line_height, offset);
	
}

// mode: 0 - align up and left, 1 - center on one line
// str is a c string, 256 characters
// uses current font
void win_draw_string(GrafPtr dest_window,Rect dest_rect,Str255 str,short mode,short line_height,Point offset){
	GrafPtr old_port;
	Str255 p_str,str_to_draw/*,str_to_draw2*/,c_str;
	static Str255 null_s = "8";
	if(null_s[0] == '8'){
		for(int i = 0;i < 256; i++)
			null_s[i] = ' ';
	}
	short str_len,i;
	short last_line_break = 0,last_word_break = 0,on_what_line = 0;
	short text_len[257];
	short total_width = 0;
	//bool /*end_loop,*/force_skip = false;
	//KeyMap key_state;
	//RgnHandle current_clip;
	short adjust_x = 0,adjust_y = 0;
	
	adjust_x = offset.h;
	adjust_y = offset.v;
	
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
			for (i = 0;text_len[i] != text_len[i + 1] && i < str_len;i++) {
				if (((text_len[i] - text_len[last_line_break] > (dest_rect.right - dest_rect.left - 6)) 
					 && (last_word_break > last_line_break)) || (c_str[i] == '|')) {
				  	if (c_str[i] == '|') {
				  		c_str[i] = ' ';
				  		//force_skip = true;
						last_word_break = i + 1;
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
					//if (force_skip) {
						//force_skip = false;
						//i++;
						//last_line_break++;
						//last_word_break++;
					//}
				}
				if (c_str[i] == ' ')
					last_word_break = i + 1;
//				if (on_what_line == LINES_IN_TEXT_WIN - 1)
//					i = 10000;
			}
			
			if (i - last_line_break > 1) {
				strcpy((char *)str_to_draw,(char *)null_s);
				strncpy ((char *) str_to_draw,(char *) c_str + last_line_break,(size_t) (i - last_line_break));
				//sprintf((char *)str_to_draw2," %s",str_to_draw);
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
	SetPort(old_port);//printf("String drawn.\n");
}

short string_length(const char *str){ // Why not just use strlen?
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
				if (editing_town == true) {
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
				if ((editing_town == true) && 
					(cur_town == scenario.which_town_start) && (scenario.where_start.x == cen_x + q - 4)
					&& (scenario.where_start.y == cen_y + r - 4)) {
					from_rect = start_button_from;
					to_rect = tiny_to;
					to_rect.left -= 14;
					rect_draw_some_item(editor_mixed,from_rect,ter_draw_gworld,to_rect,0,0);
					OffsetRect(&tiny_to,0,-7);
				}
				if ((editing_town == false)
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
				
				if (is_special(cen_x + q - 4,cen_y + r - 4) == true) {
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
				//if (is_s_d(cen_x + q - 4,cen_y + r - 4) == true) {
				//	}	
				if (editing_town == false) {
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
				
				if (editing_town == true) {
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
					if (is_web(cen_x + q - 4,cen_y + r - 4) == true) {
						from_rect = calc_rect(5,0);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
					}	
					if (is_crate(cen_x + q - 4,cen_y + r - 4) == true) {
						from_rect = calc_rect(6,0);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
					}	
					if (is_barrel(cen_x + q - 4,cen_y + r - 4) == true) {
						from_rect = calc_rect(7,0);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
					}	
					if (is_fire_barrier(cen_x + q - 4,cen_y + r - 4) == true) {
						from_rect = calc_rect(0,2);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
					}	
					if (is_quickfire(cen_x + q - 4,cen_y + r - 4) == true) {
						from_rect = calc_rect(7,1);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
					}	
					if (is_force_barrier(cen_x + q - 4,cen_y + r - 4) == true) {
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
		if (editing_town == true) {
			draw_monsts();
			draw_items();
		}
		
		SetPort( ter_draw_gworld);
		ClipRect(&clipping_rect);
		
		if (editing_town == true) {
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
		if (editing_town == false) {
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
		
		small_any_drawn = false;
		//if (cur_viewing_mode == 0) 
		//	draw_frames();
	}
	
	if (cur_viewing_mode == 1) {
		SetPort( ter_draw_gworld);
		if (small_any_drawn == false) {
		 	FillCRect(&terrain_rect,bg[6]);
			FrameRect(&terrain_rect);
		}
		for (q = 0; q < ((editing_town == true) ? max_dim[town_type] : 48); q++) 
			for (r = 0; r < ((editing_town == true) ? max_dim[town_type] : 48); r++) {
				t_to_draw = (editing_town == true) ? t_d.terrain[q][r] :
				current_terrain.terrain[q][r];
				if ((small_what_drawn[q][r] != t_to_draw) || (small_any_drawn == false)) {
					draw_one_tiny_terrain_spot(q,r,t_to_draw);
					small_what_drawn[q][r] = t_to_draw;
				}
			}
		SetPortWindowPort(mainPtr);
		small_any_drawn = true;
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

short get_custom_rect (short which_rect, Rect& store_rect){ // returns the number of the sheet to use
	short sheet = which_rect / 100;
	which_rect %= 100;
	SetRect(&store_rect,0,0,28,36);
	
	OffsetRect(&store_rect,28 * (which_rect % 10),36 * (which_rect / 10));
	return sheet;
}

void get_str(Str255 str,short i, short j){
	GetIndString(str, i, j);
	p2cstr(str);
}

void writeGWorldToPNGFile(GWorldPtr gw, const FSSpec *fileSpec){
    GraphicsExportComponent ge = 0; 
    OpenADefaultComponent(GraphicsExporterComponentType, kQTFileTypePNG, &ge ); 
    GraphicsExportSetInputGWorld(ge, gw); 
    GraphicsExportSetOutputFile(ge, fileSpec); 
    GraphicsExportDoExport(ge,nil); 
    CloseComponent(ge); 
}

GWorldPtr importPictureFileToGWorld(const FSSpec *fileSpec){
	GraphicsImportComponent gi;
	GetGraphicsImporterForFile (fileSpec, &gi );
	Rect naturalBounds;
	GraphicsImportGetNaturalBounds (gi, &naturalBounds);
	GWorldPtr temp;
	NewGWorld(&temp,0,&naturalBounds,NULL,NULL,kNativeEndianPixMap);
	GraphicsImportSetGWorld (gi, temp, nil);
	GraphicsImportDraw (gi);
	CloseComponent(gi);
	return(temp);
}

extern std::string progDir;
CursorRef GetCursorFromPath(std::string filename, Point hotspot){
	std::string fullpath = progDir + "/Scenario Editor/graphics.exd/";
	if(use_win_graphics) fullpath += "win";
	else fullpath += "mac";
	fullpath += "/cursors/";
	fullpath += filename;
	printf("Loading cursor from: %s\n\n",fullpath.c_str());
	FSRef ref;
	OSStatus err = FSPathMakeRef((UInt8*)fullpath.c_str(), &ref, NULL);
	CFURLRef url = CFURLCreateFromFSRef(NULL, &ref);
	return CreateCursorFromFile(url, hotspot.h, hotspot.v);
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
	{45, 1, 1},
	{199, 1, 1},
	{200, 1, 1},
	{201, 1, 1},
	{202, 1, 1},
	{203, 1, 1},
	//180
	{204, 1, 1},
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

void tileImage(Rect area, GWorldPtr img, short mode){
	RgnHandle clip= NewRgn();
	RectRgn(clip,&area);
	
	GrafPtr cur_port;
	GetPort(&cur_port);
	const BitMap* drawDest = GetPortBitMapForCopyBits(cur_port);
	PixMapHandle drawSource = GetPortPixMap(img);
	
	Rect imgRect;
	GetPortBounds(img, &imgRect);
	
	int imgWidth=imgRect.right-imgRect.left;
	int imgHeight=imgRect.bottom-imgRect.top;
	area.left -= area.left % imgWidth;
	area.top -= area.top % imgHeight;
	int x,y;
	unsigned int hrep = (int)((double(area.right-area.left)/imgWidth)+0.5);
	unsigned int vrep = (int)((double(area.bottom-area.top)/imgHeight)+0.5);
	for(unsigned int i=0; i<=hrep; i++){
		for(unsigned int j=0; j<=vrep; j++){
			x=area.left+i*imgWidth;
			y=area.top+j*imgHeight;
			Rect targetRect={y,x,y+imgHeight,x+imgWidth};
			CopyBits((BitMap*)*drawSource, drawDest,&imgRect,&targetRect,mode,clip);
		}
	}
	DisposeRgn(clip);
}

void tileImage(Rect area, GWorldPtr img, Rect srcRect, short mode){
	RgnHandle clip= NewRgn();
	RectRgn(clip,&area);
	
	GrafPtr cur_port;
	GetPort(&cur_port);
	const BitMap* drawDest = GetPortBitMapForCopyBits(cur_port);
	PixMapHandle drawSource = GetPortPixMap(img);
	
	int srcWidth=srcRect.right-srcRect.left;
	int srcHeight=srcRect.bottom-srcRect.top;
	area.left -= area.left % srcWidth;
	area.top -= area.top % srcHeight;
	int x,y;
	unsigned int hrep = (int)((double(area.right-area.left)/srcWidth)+0.5);
	unsigned int vrep = (int)((double(area.bottom-area.top)/srcHeight)+0.5);
	for(unsigned int i=0; i<=hrep; i++){
		for(unsigned int j=0; j<=vrep; j++){
			x=area.left+i*srcWidth;
			y=area.top+j*srcHeight;
			Rect targetRect={y,x,y+srcHeight,x+srcWidth};
			CopyBits((BitMap*)*drawSource, drawDest,&srcRect,&targetRect,mode,clip);
		}
	}
	DisposeRgn(clip);
}

void tileImage(RgnHandle area, GWorldPtr img, short mode){
	GrafPtr cur_port;
	GetPort(&cur_port);
	const BitMap* drawDest = GetPortBitMapForCopyBits(cur_port);
	PixMapHandle drawSource = GetPortPixMap(img);
	
	Rect imgRect;
	GetPortBounds(img, &imgRect);
	
	int imgWidth=imgRect.right-imgRect.left;
	int imgHeight=imgRect.bottom-imgRect.top;
	int x,y;
	Rect bounds;
	GetRegionBounds(area, &bounds);
	bounds.left -= bounds.left % imgWidth;
	bounds.top -= bounds.top % imgHeight;
	unsigned int hrep = (int)((double(bounds.right-bounds.left)/imgWidth)+0.5);
	unsigned int vrep = (int)((double(bounds.bottom-bounds.top)/imgHeight)+0.5);
	for(unsigned int i=0; i<=hrep; i++){
		for(unsigned int j=0; j<=vrep; j++){
			x=bounds.left+i*imgWidth;
			y=bounds.top+j*imgHeight;
			Rect targetRect={y,x,y+imgHeight,x+imgWidth};
			CopyBits((BitMap*)*drawSource, drawDest,&imgRect,&targetRect,mode,area);
		}
	}
}

void tileImage(RgnHandle area, GWorldPtr img, Rect srcRect, short mode){
	GrafPtr cur_port;
	GetPort(&cur_port);
	const BitMap* drawDest = GetPortBitMapForCopyBits(cur_port);
	PixMapHandle drawSource = GetPortPixMap(img);
	
	int srcWidth=srcRect.right-srcRect.left;
	int srcHeight=srcRect.bottom-srcRect.top;
	int x,y;
	Rect bounds;
	GetRegionBounds(area, &bounds);
	bounds.left -= bounds.left % srcWidth;
	bounds.top -= bounds.top % srcHeight;
	unsigned int hrep = (int)((double(bounds.right-bounds.left)/srcWidth)+0.5);
	unsigned int vrep = (int)((double(bounds.bottom-bounds.top)/srcHeight)+0.5);
	for(unsigned int i=0; i<=hrep; i++){
		for(unsigned int j=0; j<=vrep; j++){
			x=bounds.left+i*srcWidth;
			y=bounds.top+j*srcHeight;
			Rect targetRect={y,x,y+srcHeight,x+srcWidth};
			CopyBits((BitMap*)*drawSource, drawDest,&srcRect,&targetRect,mode,area);
		}
	}
}
