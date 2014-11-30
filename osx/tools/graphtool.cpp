/*
 *  graphtool.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#define GRAPHTOOL_CPP
#include "graphtool.h"

#ifdef __APPLE__
#include <OpenGl/GL.h>
#else
#include <GL/GL.h>
#endif

#include <typeinfo>
#include <boost/filesystem.hpp>
#include <boost/math/constants/constants.hpp>

#include "restypes.hpp"

using boost::math::constants::pi;

RECT bg[21];
RECT map_pat[30];
RECT bw_pats[6];
sf::Texture bg_gworld;
bool use_win_graphics = false;
sf::Shader maskShader;
extern fs::path progDir;
// TODO: Shouldn't need this
extern sf::RenderWindow mainPtr;

void init_graph_tool(){
	fs::path shaderPath = progDir/"data"/"shaders";
	fs::path fragPath = shaderPath/"mask.frag", vertPath = shaderPath/"mask.vert";
	std::ifstream fin;
	
	fin.open(fragPath.c_str());
	if(!fin.good()) perror("Error loading fragment shader");
	fin.seekg(0, std::ios::end);
	int size = fin.tellg();
	fin.seekg(0);
	char* fbuf = new char[size + 1];
	fbuf[size] = 0;
	fin.read(fbuf, size);
	fin.close();
	
	fin.open(vertPath.c_str());
	if(!fin.good()) perror("Error loading vertex shader");
	fin.seekg(0, std::ios::end);
	size = fin.tellg();
	fin.seekg(0);
	char* vbuf = new char[size + 1];
	vbuf[size] = 0;
	fin.read(vbuf, size);
	
	if(!maskShader.loadFromMemory(vbuf, fbuf)) {
		fprintf(stderr,"Error: Failed to load shaders from %s\nVertex:\n%s\nFragment:\n%s",shaderPath.c_str(),vbuf,fbuf);
	}
	delete[] fbuf;
	delete[] vbuf;
	int i,j;
	// TODO: The duplication of location here shouldn't be necessary
	static const location pat_offs[17] = {
		location{0,3}, location{1,1}, location{2,1}, location{2,0},
		location{3,0}, location{3,1}, location{1,3}, location{0,0},
		location{0,2}, location{1,2}, location{0,1}, location{2,2},
		location{2,3}, location{3,2}, location{1,0}, location{4,0}, location{3,3}
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
//	for (i = 0; i < 21; i++) 
//	    bg[i] = GetPixPat(128 + i);
	for(i = 0; i < 17; i++){
		bg[pat_i[i]] = {0,0,64,64};
		bg[pat_i[i]].offset(64 * pat_offs[i].x,64 * pat_offs[i].y);
	}
	RECT tmp_rect = bg[19];
	tmp_rect.offset(0, 64);
	bg[0] = bg[1] = bg[18] = map_pat[7] = tmp_rect;
	bg[0].right -= 32;
	bg[0].bottom -= 32;
	bg[1].left -= 32;
	bg[1].bottom -= 32;
	bg[18].right -= 32;
	bg[18].top -= 32;
	map_pat[7].left -= 32;
	map_pat[7].top -= 32;
	tmp_rect.offset(0, 64);
	map_pat[8] = map_pat[23] = map_pat[26] = tmp_rect;
	map_pat[8].right -= 32;
	map_pat[8].bottom -= 32;
	map_pat[23].left -= 32;
	map_pat[23].right -= 16;
	map_pat[23].bottom -= 32;
	map_pat[26].left -= 32 + 16;
	map_pat[26].bottom -= 32;
	tmp_rect.offset(0, 64);
	bg[7] = tmp_rect;
	bg[7].bottom = bg[7].top + 16;
	tmp_rect.offset(0, -32);
	tmp_rect.right = tmp_rect.left + 8;
	tmp_rect.bottom = tmp_rect.top + 8;
	for(i = 0; i < 26; i++){
		map_pat[map_i[i]] = tmp_rect;
		map_pat[map_i[i]].offset(8 * (i % 8),8 * (i / 8));
		// Note: 8 * (i / 8) != i, despite appearances, due to integer rounding
	}
	tmp_rect = map_pat[29];
	for(i = 0; i < 6; i++) {
		tmp_rect.offset(8, 0);
		bw_pats[i] = tmp_rect;
	}
	bg_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("pixpats"));
}

static void rect_draw_some_item(const sf::Texture& src_gworld,RECT src_rect,sf::RenderTarget& targ_gworld,RECT targ_rect,sf::RenderStates mode);

void rect_draw_some_item(sf::RenderTarget& targ_gworld,RECT targ_rect) {
	fill_rect(targ_gworld, targ_rect, sf::Color::Black);
}

void rect_draw_some_item(const sf::Texture& src_gworld,RECT src_rect,sf::RenderTarget& targ_gworld,RECT targ_rect,sf::BlendMode mode){
	rect_draw_some_item(src_gworld, src_rect, targ_gworld, targ_rect, sf::RenderStates(mode));
}

void rect_draw_some_item(const sf::Texture& src_gworld,RECT src_rect,sf::RenderTarget& targ_gworld,RECT targ_rect,sf::RenderStates mode) {
	setActiveRenderTarget(targ_gworld);
	sf::Sprite tile(src_gworld, src_rect);
	tile.setPosition(targ_rect.left, targ_rect.top);
	double xScale = targ_rect.width(), yScale = targ_rect.height();
	xScale /= src_rect.width();
	yScale /= src_rect.height();
	tile.setScale(xScale, yScale);
	targ_gworld.draw(tile, mode);
}

void rect_draw_some_item(const sf::Texture& src_gworld,RECT src_rect,RECT targ_rect,location offset, sf::BlendMode mode) {
	targ_rect.offset(offset);
	rect_draw_some_item(src_gworld,src_rect,mainPtr,targ_rect,mode);
}

void rect_draw_some_item(const sf::Texture& src_gworld,RECT src_rect,const sf::Texture& mask_gworld,RECT mask_rect,sf::RenderTarget& targ_gworld,RECT targ_rect) {
	sf::RenderTexture src;
	src.create(src_rect.width(), src_rect.height());
	RECT dest_rect = src_rect;
	dest_rect.offset(-dest_rect.left,-dest_rect.top);
	rect_draw_some_item(src_gworld, src_rect, src, dest_rect);
	src.display();
	sf::RenderTexture mask;
	mask.create(mask_rect.width(), mask_rect.height());
	dest_rect = mask_rect;
	dest_rect.offset(-dest_rect.left,-dest_rect.top);
	rect_draw_some_item(mask_gworld, mask_rect, mask, dest_rect);
	mask.display();
	
	maskShader.setParameter("texture", sf::Shader::CurrentTexture);
	maskShader.setParameter("mask", mask.getTexture());
	rect_draw_some_item(src.getTexture(), dest_rect, targ_gworld, targ_rect, &maskShader);
}

void TextStyle::applyTo(sf::Text& text) {
	switch(font) {
		case FONT_PLAIN:
			text.setFont(*ResMgr::get<FontRsrc>("plain"));
			break;
		case FONT_BOLD:
			text.setFont(*ResMgr::get<FontRsrc>("bold"));
			break;
		case FONT_DUNGEON:
			text.setFont(*ResMgr::get<FontRsrc>("dungeon"));
			break;
		case FONT_MAIDWORD:
			text.setFont(*ResMgr::get<FontRsrc>("maidenword"));
			break;
	}
	text.setCharacterSize(pointSize);
	int style = sf::Text::Regular;
	if(italic) style |= sf::Text::Italic;
	if(underline) style |= sf::Text::Underlined;
	text.setStyle(style);
	text.setColor(colour);
}

struct text_params_t {
	TextStyle style;
	eTextMode mode;
	location offset = {0,0};
};

struct snippet_t {
	std::string text;
	location at;
};

void win_draw_string(sf::RenderTarget& dest_window,RECT dest_rect,std::string str,text_params_t options);

void win_draw_string(sf::RenderTarget& dest_window,RECT dest_rect,std::string str,eTextMode mode,TextStyle style, location offset) {
	text_params_t params;
	params.mode = mode;
	params.style = style;
	params.offset = offset;
	win_draw_string(dest_window, dest_rect, str, params);
}

void win_draw_string(sf::RenderTarget& dest_window,RECT dest_rect,std::string str,text_params_t options) {
	short line_height = options.style.lineHeight;
	sf::Text str_to_draw;
	options.style.applyTo(str_to_draw);
	short str_len,i;
	short last_line_break = 0,last_word_break = 0;
	short total_width = 0;
	short adjust_x = 0,adjust_y = 0;
	
	adjust_x = options.offset.x;
	adjust_y = options.offset.y;
	str_to_draw.setString("fj"); // Something that has both an ascender and a descender
	adjust_y -= str_to_draw.getLocalBounds().height;
	
	str_to_draw.setString(str);
	str_len = str.length();
	if (str_len == 0) {
		return;
	}
	
	eTextMode mode = options.mode;
	total_width = str_to_draw.getLocalBounds().width;
	if(mode == eTextMode::WRAP && total_width < dest_rect.width())
		mode = eTextMode::LEFT_TOP;
	if(mode == eTextMode::LEFT_TOP && str.find('|') != std::string::npos)
		mode = eTextMode::WRAP;
	
	auto text_len = [&str_to_draw](size_t i) -> int {
		return str_to_draw.findCharacterPos(i).x;
	};
	
	location moveTo;
	std::vector<snippet_t> snippets;
	line_height -= 2;
	
	if(mode == eTextMode::WRAP) {
		moveTo = location(dest_rect.left + 1 + adjust_x, dest_rect.top + 1 + adjust_y + 9);
		for(i = 0; text_len(i) != text_len(i + 1) && i < str_len;i++) {
			if(((text_len(i) - text_len(last_line_break) > (dest_rect.width() - 6))
				 && (last_word_break > last_line_break)) || (str[i] == '|')) {
				if(str[i] == '|') {
					str[i] = ' ';
					last_word_break = i + 1;
				}
				size_t amount = last_word_break - last_line_break - 1;
				snippets.push_back({str.substr(last_line_break,amount), moveTo});
				moveTo.y += line_height;
				last_line_break = last_word_break;
			}
			if(str[i] == ' ')
				last_word_break = i + 1;
		}
		
		if(i - last_line_break > 1) {
			std::string snippet = str.substr(last_line_break);
			if(snippet.size() > 2)
				snippets.push_back({snippet, moveTo});
		}
	} else {
		switch(mode) {
			case eTextMode::CENTRE:
				moveTo = location((dest_rect.right + dest_rect.left) / 2 - (4 * total_width) / 9 + adjust_x,
						(dest_rect.bottom + dest_rect.top - line_height) / 2 + 9 + adjust_y);
				break;
			case eTextMode::LEFT_TOP:
				moveTo = location(dest_rect.left + 1 + adjust_x, dest_rect.top + 1 + adjust_y + 9);
				break;
			case eTextMode::LEFT_BOTTOM:
				moveTo = location(dest_rect.left + 1 + adjust_x, dest_rect.top + 1 + adjust_y + 9 + dest_rect.height() / 6);
				break;
			case eTextMode::WRAP:
				break; // Never happens, but put this here to silence warning
		}
		snippets.push_back({str, moveTo});
	}
	
	for(auto& snippet : snippets) {
		str_to_draw.setString(snippet.text);
		str_to_draw.setPosition(snippet.at);
		dest_window.draw(str_to_draw);
	}
}

short string_length(std::string str, TextStyle style){
	short total_width = 0;
	
	sf::Text text;
	style.applyTo(text);
	text.setString(str);
	total_width = text.getLocalBounds().width;
	return total_width;
}

/*
void draw_terrain(){
	short q,r,x,y,i,small_i;
	location which_pt,where_draw;
	RECT draw_rect,clipping_rect = {8,8,332,260};	
	unsigned char t_to_draw;
	RECT source_rect,tiny_to,tiny_to_base = {37,29,44,36},tiny_from,from_rect,to_rect;
	RECT boat_rect[4] = {{0,0,36,28}, {0,28,36,56},{0,56,36,84},{0,84,36,112}};
	
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

RECT calc_rect(short i, short j){
	RECT base_rect = {0,0,36,28};
	
	base_rect.offset(i * 28, j * 36);
	return base_rect;
}

graf_pos cCustomGraphics::find_graphic(pic_num_t which_rect, bool party) {
	short sheet = which_rect / 100;
	if(is_old) sheet = 0;
	else which_rect %= 100;
	RECT store_rect = {0,0,28,36};
	
	store_rect.offset(28 * (which_rect % 10),36 * (which_rect / 10));
	return std::make_pair(party ? this->party : &sheets[sheet],store_rect);
}

size_t cCustomGraphics::count() {
	if(sheets == NULL) return 0;
	else if(is_old) {
		RECT bounds(sheets[0]);
		if(bounds.width() < 280) return bounds.width() / 28;
		return bounds.height() / 36;
	} else {
		size_t count = 100 * (numSheets - 1);
		RECT bounds(sheets[numSheets - 1]);
		if(bounds.width() < 280) count += bounds.width() / 28;
		else count += bounds.height() / 36;
		return count;
	}
}

// TODO: This doesn't belong in this file
std::string get_str(std::string list, short j){
	if(j == 0) return list;
	StringRsrc& strings = *ResMgr::get<StringRsrc>(list);
	return strings[j - 1];
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

// TODO: Put these classes in a header?
class Ellipse : public sf::Shape {
	float divSz;
	int points;
	float a, b;
public:
	explicit Ellipse(sf::Vector2f size, unsigned int points = 30) : points(points) {
		a = size.x / 2.0f;
		b = size.y / 2.0f;
		divSz = 2 * pi<float>() / points;
		update();
	}
	
	unsigned int getPointCount() const override {
		return points;
	}
	
	sf::Vector2f getPoint(unsigned int i) const override {
		float t = i * divSz;
		return sf::Vector2f(a + a*sin(t), b + b*cos(t));
	}
	
	// TODO: Additional functions?
};

class RoundRect : public sf::Shape {
	float divSz;
	int points;
	float w,h,r;
public:
	RoundRect(sf::Vector2f size, float cornerRadius, unsigned int points = 32) : points(points / 4) {
		w = size.x;
		h = size.y;
		r = cornerRadius;
		divSz = 2 * pi<float>() / points;
		update();
	}
	
	unsigned int getPointCount() const override {
		return points * 4;
	}
	
	sf::Vector2f getPoint(unsigned int i) const override {
		const float pi = ::pi<float>();
		const float half_pi = 0.5 * pi;
		float t = i * divSz;
		switch(i / points) {
			case 0: // top left corner
				return {r + r*sin(t + pi), r + r*cos(t + pi)};
			case 1: // bottom left corner
				return {r + r*cos(t + half_pi), h - r + r*sin(t - half_pi)};
			case 2: // bottom right corner
				return {w - r + r*cos(t + half_pi), h - r - r*sin(t + half_pi)};
			case 3: // top right corner
				return {w - r - r*cos(t - half_pi), r + r*sin(t - half_pi)};
		}
		// Unreachable
		printf("Whoops, rounded rectangle had bad point!");
		return {0,0};
	}
	
	// TODO: Additional functions?
};

void draw_line(sf::RenderTarget& target, location from, location to, int thickness, sf::Color colour, sf::BlendMode mode) {
	sf::VertexArray line(sf::LinesStrip, 2);
	line[0].position = from;
	line[0].color = colour;
	line[1].position = to;
	line[1].color = colour;
	setActiveRenderTarget(target);
	float saveThickness;
	glGetFloatv(GL_LINE_WIDTH, &saveThickness);
	glLineWidth(thickness);
	target.draw(line, mode);
	glLineWidth(saveThickness);
}

static void fill_shape(sf::RenderTarget& target, sf::Shape& shape, int x, int y, sf::Color colour) {
	shape.setPosition(x, y);
	shape.setFillColor(colour);
	setActiveRenderTarget(target);
	target.draw(shape);
	
}

static void frame_shape(sf::RenderTarget& target, sf::Shape& shape, int x, int y, sf::Color colour) {
	shape.setPosition(x, y);
	shape.setOutlineColor(colour);
	shape.setFillColor(sf::Color::Transparent);
	// TODO: Determine the correct outline value; should be one pixel, not sure if it should be negative
	shape.setOutlineThickness(1.0);
	target.draw(shape);
}

void fill_rect(sf::RenderTarget& target, RECT rect, sf::Color colour) {
	sf::RectangleShape fill(sf::Vector2f(rect.width(), rect.height()));
	fill_shape(target, fill, rect.left, rect.top, colour);
}

void frame_rect(sf::RenderTarget& target, RECT rect, sf::Color colour) {
	sf::RectangleShape frame(sf::Vector2f(rect.width(), rect.height()));
	frame_shape(target, frame, rect.left, rect.top, colour);
}

void fill_roundrect(sf::RenderTarget& target, RECT rect, int rad, sf::Color colour) {
	RoundRect fill(sf::Vector2f(rect.width(), rect.height()), rad);
	fill_shape(target, fill, rect.left, rect.top, colour);
}

void frame_roundrect(sf::RenderTarget& target, RECT rect, int rad, sf::Color colour) {
	RoundRect frame(sf::Vector2f(rect.width(), rect.height()), rad);
	frame_shape(target, frame, rect.left, rect.top, colour);
}

void fill_circle(sf::RenderTarget& target, RECT rect, sf::Color colour) {
	Ellipse fill(sf::Vector2f(rect.width(), rect.height()));
	fill_shape(target, fill, rect.left, rect.top, colour);
}

void frame_circle(sf::RenderTarget& target, RECT rect, sf::Color colour) {
	Ellipse frame(sf::Vector2f(rect.width(), rect.height()));
	frame_shape(target, frame, rect.left, rect.top, colour);
}

void fill_region(sf::RenderWindow& target, Region& region, sf::Color colour) {
	clip_region(target, region);
	fill_rect(target, RECT(target), colour);
	undo_clip(target);
}

void frame_region(sf::RenderWindow& target, Region& region, sf::Color colour) {
	// TODO: Uh, actually, this won't do what it says. Eh, I'll fix it if I ever use it.
	clip_region(target, region);
	frame_rect(target, RECT(target), colour);
	undo_clip(target);
}

void Region::addEllipse(RECT frame) {
	Ellipse* ellipse = new Ellipse(sf::Vector2f(frame.width(), frame.height()));
	ellipse->setFillColor(sf::Color::Black);
	shapes.push_back(std::shared_ptr<sf::Shape>(ellipse));
}

void Region::addRect(RECT rect){
	sf::RectangleShape* frame = new sf::RectangleShape(sf::Vector2f(rect.width(), rect.height()));
	frame->setPosition(rect.left, rect.top);
	frame->setFillColor(sf::Color::Black);
	shapes.push_back(std::shared_ptr<sf::Shape>(frame));
}

void Region::clear() {
	shapes.clear();
}

void Region::offset(int x, int y) {
	for(auto shape : shapes) {
		shape->move(x,y);
	}
}

void Region::offset(location off) {
	offset(off.x, off.y);
}

// We can only use stencil buffer in the main window
// Could request it in dialogs, but currently don't
// SFML does not appear to allow requesting it for render textures
void Region::setStencil(sf::RenderWindow& where) {
	where.setActive();
	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 1);
	for(auto shape : shapes) {
		// Save the colour in case we need to reuse this region
		sf::Color colour = shape->getFillColor();
		if(colour == sf::Color::Black)
			glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		else glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
		// Make transparent so we don't overwrite important stuff
		shape->setFillColor(sf::Color::Transparent);
		where.draw(*shape);
		shape->setFillColor(colour);
	}
	glStencilFunc(GL_EQUAL, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

void clip_rect(sf::RenderTarget& where, RECT rect) {
	rect |= RECT(where); // Make sure we don't draw out of bounds
	// TODO: Make sure this works for the scissor test...
	setActiveRenderTarget(where);
	glEnable(GL_SCISSOR_TEST);
	glScissor(rect.left, RECT(where).height() - rect.bottom, rect.width(), rect.height());
}

void clip_region(sf::RenderWindow& where, Region& region) {
	region.setStencil(where);
}

void undo_clip(sf::RenderTarget& where) {
	setActiveRenderTarget(where);
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_STENCIL_TEST);
}

void setActiveRenderTarget(sf::RenderTarget& where) {
	const std::type_info& type = typeid(where);
	if(type == typeid(sf::RenderWindow&))
		dynamic_cast<sf::RenderWindow&>(where).setActive();
	else if(type == typeid(sf::RenderTexture&))
		dynamic_cast<sf::RenderTexture&>(where).setActive();
	else throw std::bad_cast();
}

Region& Region::operator-=(Region& other) {
	for(auto shape : other.shapes) {
		// TODO: Shouldn't this be done to a copy of the shape instead?
		if(shape->getFillColor() == sf::Color::Black)
			shape->setFillColor(sf::Color::White);
		else shape->setFillColor(sf::Color::Black);
		shapes.push_back(shape);
	}
	return *this;
}

void tileImage(sf::RenderTarget& target, RECT area, sf::Texture& img, sf::BlendMode mode){
	bool saveRep = img.isRepeated();
	img.setRepeated(true);
	sf::Vector2u imgSz = img.getSize();
	RECT clipArea = area;
	area.left -= area.left % imgSz.x;
	area.top -= area.top % imgSz.y;
	area |= RECT(target); // Make sure we don't draw out of bounds

	unsigned int hrep = int((double(area.width())/imgSz.x)+0.5);
	unsigned int vrep = int((double(area.height())/imgSz.y)+0.5);
	sf::RectangleShape tessel(sf::Vector2f(hrep*imgSz.x,vrep*imgSz.y));
	tessel.setTexture(&img);
	tessel.setPosition(area.left, area.top);
	sf::RenderStates renderMode(mode);
	setActiveRenderTarget(target);
	clip_rect(target, clipArea);
	target.draw(tessel, renderMode);
	undo_clip(target);
	img.setRepeated(saveRep);
}

void tileImage(sf::RenderTarget& target, RECT area, sf::Texture& img, RECT srcRect, sf::BlendMode mode){
	sf::RenderTexture temp;
	temp.create(srcRect.width(), srcRect.height());
	temp.setRepeated(true);
	RECT tesselRect(temp);
	temp.setActive();
	rect_draw_some_item(img, srcRect, temp, tesselRect);
	temp.display();
	RECT clipArea = area;
	area.left -= area.left % tesselRect.width();
	area.top -= area.top % tesselRect.height();
	area |= RECT(target); // Make sure we don't draw out of bounds
	
	sf::RectangleShape tessel(sf::Vector2f(area.width(),area.height()));
	tessel.setTexture(&temp.getTexture());
	tessel.setTextureRect(area);
	tessel.setPosition(area.left, area.top);
	sf::RenderStates renderMode(mode);
	setActiveRenderTarget(target);
	clip_rect(target, clipArea);
	target.draw(tessel, renderMode);
	undo_clip(target);
}

#if 0

void tileImage(RgnHandle area, GWorldPtr img, RECT srcRect, short mode){
	GrafPtr cur_port;
	GetPort(&cur_port);
	const BitMap* drawDest = GetPortBitMapForCopyBits(cur_port);
	PixMapHandle drawSource = GetPortPixMap(img);
	
	int srcWidth=srcRect.right-srcRect.left;
	int srcHeight=srcRect.bottom-srcRect.top;
	int x,y;
	RECT bounds;
	GetRegionBounds(area, &bounds);
	bounds.left -= bounds.left % srcWidth;
	bounds.top -= bounds.top % srcHeight;
	unsigned int hrep = (int)((double(bounds.right-bounds.left)/srcWidth)+0.5);
	unsigned int vrep = (int)((double(bounds.bottom-bounds.top)/srcHeight)+0.5);
	for(unsigned int i=0; i<=hrep; i++){
		for(unsigned int j=0; j<=vrep; j++){
			x=bounds.left+i*srcWidth;
			y=bounds.top+j*srcHeight;
			RECT targetRect={y,x,y+srcHeight,x+srcWidth};
			CopyBits((BitMap*)*drawSource, drawDest,&srcRect,&targetRect,mode,area);
		}
	}
}
#endif
