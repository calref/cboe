
#include <cstdio>
#include <sstream>
#include <cstring>
#include "scen.global.h"
#include "classes.h"
#include "graphtool.h"
#include "scen.graphics.h"
#include <cmath>
#include "scen.keydlgs.h"
#include "soundtool.h"
#include "mathutil.h"
#include "boe.consts.h" // TODO: Put these constants in a global file

#include "dialog.h"

#include "scen.core.h"
#include "scen.townout.h"
#include "scrollbar.h"
#include "restypes.hpp"

void load_main_screen();
void load_terrain_template();
short terrain_in_index();
void put_terrain_in_template();
void place_location();
//void draw_cur_string();
void undo_clip();

short find_index_spot();
bool is_s_d();
void sort_specials();

extern cOutdoors current_terrain;
extern sf::RenderWindow mainPtr;
extern cTown* current_town;
extern short cen_x, cen_y,current_terrain_type,cur_town;
extern cTown* town;
//extern big_tr_type t_d;
//extern template_town_type town_template;
extern short cur_viewing_mode;
extern eScenMode overall_mode;
eDrawMode draw_mode = DRAW_TERRAIN;
extern short available_dlog_buttons[NUM_DLOG_B];
extern bool editing_town;
//extern short max_dim[3];
//extern piles_of_stuff_dumping_type *data_store;
extern cScenario scenario;
extern RECT world_screen;
extern sf::Texture bg_gworld;
extern RECT left_button[NLS];
extern RECT right_buttons[NRSONPAGE];
extern RECT right_scrollbar_rect;
extern RECT right_area_rect;
extern ter_num_t borders[4][50];
extern std::shared_ptr<cScrollbar> right_sbar;

extern bool left_buttons_active,right_buttons_active;
extern short left_button_status[NLS]; // 0 - clear, 1 - text, 2 - title text, +10 - button
extern short right_button_status[NRS];
//extern unsigned char m_pic_index[200];
// TODO: What is this for?
//extern btn_t buttons[];
extern location cur_out;

short num_ir[3] = {12,10,4};

sf::Texture monst_gworld[NUM_MONST_SHEETS];
sf::Texture terrain_gworld[NUM_TER_SHEETS];
sf::Texture editor_mixed;
sf::RenderTexture terrain_buttons_gworld;
sf::Texture anim_gworld;
sf::Texture fields_gworld;
sf::Texture dialog_gworld;
sf::Texture items_gworld;
sf::Texture tiny_obj_gworld;
sf::Texture small_ter_gworld;
sf::Texture boom_gworld;
cCustomGraphics spec_scen_g;
sf::Texture vehicle_gworld;
//GWorldPtr dlg_buttons_gworld[NUM_BUTTONS][2]
sf::RenderTexture ter_draw_gworld;
sf::Texture dlogpics_gworld;
sf::Texture talkfaces_gworld;
sf::Texture roads_gworld;
sf::Texture missiles_gworld;
sf::Texture status_gworld;
sf::Texture pc_gworld;
extern tessel_ref_t map_pat[];

// begin new stuff
RECT blue_button_from = {120,91,134,107};
RECT start_button_from = {120,70,127,91};
RECT base_small_button_from = {120,0,127,7};
extern RECT palette_buttons[10][6];
extern RECT palette_buttons_from[71];
extern short town_buttons[6][10], out_buttons[6][10];
extern bool good_palette_buttons[2][6][10];
RECT palette_button_base = {0,0,18,25};
RECT terrain_buttons_rect = {0,0,410,294};
extern RECT left_buttons[NLS][2]; // 0 - whole, 1 - blue button
RECT left_button_base = {5,5,21,200};
RECT right_button_base = {RIGHT_AREA_UL_Y,RIGHT_AREA_UL_X,17,RIGHT_AREA_UL_Y};
RECT terrain_rect = {0,0,340,272};
char current_string[256] = "";
char current_string2[256] = "";
extern RECT terrain_rects[256];
extern char strings_ls[NLS][40];
extern char strings_rs[NRS][40];

short map_pats[220] = {50,50,1,1,1,6,6,6,6,6,
	6,6,6,6,6,6,6,6,2,2,
	2,2,2,2,2,2,2,2,2,2,
	2,2,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,3,3,3,3,
	3,3,3,3,3,3,3,3,3,0, // 50
	0,0,0,0,0,0,0,23,23,23,
	15,15,50,50,0,0,0,0,0,7,
	7,8,8,14,14,9,9,9,5,5,
	0,0,0,0,0,0,0,0,0,0,
	18,18,0,0,0,0,0,0,0,0, // 100
	22,22,0,0,0,0,0,0,0,0,
	0,0,0,10,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,11,0,0, // 150
	0,0,0,12,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0, // 200
	0,0,0,0,0,0,0,0,0,0
};


unsigned char small_what_drawn[64][64];
extern bool small_any_drawn;

static short get_small_icon(ter_num_t ter){
	short icon = -1;
	switch(scenario.ter_types[ter].special){
		case eTerSpec::NONE:
			icon = scenario.ter_types[ter].flag1.s;
			break;
		case eTerSpec::CHANGE_WHEN_STEP_ON:
			icon = 23;
			break;
		case eTerSpec::DAMAGING:
			switch(scenario.ter_types[ter].flag3.u){
				case DAMAGE_WEAPON:
					icon = 40;
					break;
				case DAMAGE_FIRE:
					icon = 37;
					break;
				case DAMAGE_POISON:
					icon = 43;
					break;
				case DAMAGE_MAGIC:
					icon = 39;
					break;
				case DAMAGE_UNBLOCKABLE:
					icon = 39;
					break;
				case DAMAGE_COLD:
					icon = 38;
					break;
				case DAMAGE_UNDEAD:
					icon = 18;
					break;
				case DAMAGE_DEMON:
					icon = 19;
					break;
			}
			break;
		case eTerSpec::BRIDGE:
			icon = 42;
			break;
		case eTerSpec::BED:
			icon = -1;
			break;
		case eTerSpec::DANGEROUS:
			switch((eStatus)scenario.ter_types[ter].flag3.u){
				case eStatus::POISONED_WEAPON: // TODO: Do something here
					break;
				case eStatus::BLESS_CURSE: // TODO: Do something here (check flag1 to determine whether bless or curse)
					break;
				case eStatus::POISON:
					icon = 35;
					break;
				case eStatus::HASTE_SLOW: // TODO: Do something here (check flag1 to determine whether haste or slow)
					break;
				case eStatus::INVULNERABLE: // TODO: Do something here
					break;
				case eStatus::MAGIC_RESISTANCE: // TODO: Do something here
					break;
				case eStatus::WEBS: // TODO: Do something here
					break;
				case eStatus::DISEASE:
					icon = 33;
					break;
				case eStatus::INVISIBLE: // TODO: Do something here
					break;
				case eStatus::DUMB: // TODO: Do something here
					break;
				case eStatus::MARTYRS_SHIELD: // TODO: Do something here
					break;
				case eStatus::ASLEEP:
					icon = 44;
					break;
				case eStatus::PARALYZED: // TODO: Do something here
					break;
				case eStatus::ACID:
					icon = 41;
					break;
				case eStatus::MAIN: case eStatus::CHARM:
					break; // Nothing to do here; these values are "magic" and should not be used
			}
			break;
		case eTerSpec::CRUMBLING:
			icon = 34;
			break;
		case eTerSpec::LOCKABLE:
			icon = 30;
			break;
		case eTerSpec::UNLOCKABLE:
			if (scenario.ter_types[ter].flag2.u >= 5)
				icon = (scenario.ter_types[ter].flag2.u == 10) ? 32 : 31;
			else icon = 30;
			break;
		case eTerSpec::IS_A_SIGN:
			icon = 26;
			break;
		case eTerSpec::CALL_SPECIAL:
			icon = scenario.ter_types[ter].flag3.s;
			break;
		case eTerSpec::IS_A_CONTAINER:
			icon = 36;
			break;
		case eTerSpec::WATERFALL:
			icon = -1;
			break;
		case eTerSpec::CONVEYOR:
			switch(scenario.ter_types[ter].flag1.u){ // TODO: Consider the other four possible directions
				case DIR_N:
					icon = 27;
					break;
				case DIR_E:
					icon = 28;
					break;
				case DIR_S:
					icon = 29;
					break;
				case DIR_W:
					icon = 20;
					break;
			}
			break;
		case eTerSpec::BLOCKED_TO_MONSTERS:
			icon = 21;
			break;
		case eTerSpec::TOWN_ENTRANCE:
			icon = 22;
			break;
		case eTerSpec::CHANGE_WHEN_USED:
			icon = -1;
			break;
		case eTerSpec::CALL_SPECIAL_WHEN_USED:
			icon = scenario.ter_types[ter].flag3.s;
			break;
		default:
			icon = -1;
	}
	if(icon == 255) icon = -1;
	return icon;
}

void Set_up_win () {
	short i,j;
	for (i = 0; i < 70; i++){
		palette_buttons_from[i] = palette_button_base;
		palette_buttons_from[i].offset((i%10) * 25, (i/10) * 17);
		palette_buttons_from[i].right++;
	}
	for (i = 0; i < 10; i++)
		for (j = 0; j < 6; j++) {
			palette_buttons[i][j] = palette_button_base;
			palette_buttons[i][j].offset(i * 25, j * 17);
			if (i == 10 || !good_palette_buttons[1][j][i+1])
				palette_buttons[i][j].right++;
		}
	for (i = 0; i < 10; i++)
		for (j = /*2*/0; j < 6; j++)
			palette_buttons[i][j].offset(0,3);
	for (i = 0; i < 10; i++)
		for (j = /*3*/0; j < 6; j++)
			palette_buttons[i][j].offset(0,3);
	for (i = 0; i < 10; i++)
		for (j = /*4*/0; j < 6; j++)
			palette_buttons[i][j].offset(0,3);
	
	for (i = 0; i < NLS; i++) {
		left_buttons[i][0] = left_button_base;
		left_buttons[i][0].offset(0,i * 16);
		left_buttons[i][1] = left_buttons[i][0];
		left_buttons[i][1].top += 1;
		left_buttons[i][1].bottom -= 1;
		left_buttons[i][1].left += 0;
		left_buttons[i][1].right = left_buttons[i][1].left + 16;
	}
	right_button_base.left = RIGHT_AREA_UL_X + 1;
	right_button_base.top = RIGHT_AREA_UL_Y + 1;
	right_button_base.bottom = right_button_base.top + 12;
	right_button_base.right = right_button_base.left + RIGHT_AREA_WIDTH - 20;
	for (i = 0; i < NRSONPAGE; i++) {
		right_buttons[i] = right_button_base;
		right_buttons[i].offset(0,i * 12);
	}
	load_main_screen();
}

void run_startup_g() {
	RECT pict_rect = {0,0,480,640}, dest_rect;
	sf::Texture pict_to_draw;
	dest_rect = pict_rect;
	
	dest_rect.offset(-25,-25);
	pict_to_draw.loadFromImage(*ResMgr::get<ImageRsrc>("edsplash"));
	rect_draw_some_item(pict_to_draw, pict_rect, mainPtr, dest_rect, sf::BlendAlpha);
	mainPtr.display();
	play_sound(-95);
	sf::Clock timer;
	sf::Event event;
	while(timer.getElapsedTime() < time_in_ticks(120)) {
		mainPtr.pollEvent(event);
		if(event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed)
			break;
	}
	
}

void load_graphics(){
	int i;
	//for (i = 0; i < NUM_BUTTONS; i++)
	//	for (j = 0; j < 2; j++)
	//		dlg_buttons_gworld[i][j] = load_pict(2000 + (2 * i) + j); // move to dlogtool
//	for (i = 0; i < 14; i++)
//	    bg[i] = GetPixPat (128 + i);
//	for (i = 0; i < 25; i++)
//	    map_pat[i] = GetPixPat (200 + i);
	
	for (i = 0; i < 11; i++){
		std::ostringstream sout;
		sout << "monst" << i + 1;
		monst_gworld[i].loadFromImage(*ResMgr::get<ImageRsrc>(sout.str()));
	}
	for (i = 0; i < 7; i++){
		std::ostringstream sout;
		sout << "ter" << i + 1;
		terrain_gworld[i].loadFromImage(*ResMgr::get<ImageRsrc>(sout.str()));
	}
	editor_mixed.loadFromImage(*ResMgr::get<ImageRsrc>("edbuttons"));
	anim_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("teranim"));
	fields_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("fields"));
	roads_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("trim"));
	talkfaces_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("talkportraits"));
	items_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("objects"));
	tiny_obj_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("tinyobj"));
	dlogpics_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("dlogpics"));
	small_ter_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("termap"));
	//mixed_gworld = load_pict(903);
	vehicle_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("vehicle"));
	missiles_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("missiles"));
	status_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("staticons"));
	pc_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("pcs"));
}

void load_main_screen() {
	
	terrain_buttons_gworld.create(terrain_buttons_rect.width(), terrain_buttons_rect.height());
	ter_draw_gworld.create(terrain_rect.width(), terrain_rect.height());
	world_screen = terrain_rect;
	world_screen.offset(TER_RECT_UL_X,TER_RECT_UL_Y);
	
	//draw_main_screen();
}

void redraw_screen() {
	RECT windRect(mainPtr);
	tileImage(mainPtr,windRect,bg[20]);
	draw_main_screen();
	if (overall_mode < MODE_MAIN_SCREEN);
	draw_terrain();
	mainPtr.display();
}

void draw_main_screen() {
	RECT draw_rect;
	
	
	// draw left buttons (always active)
	//for (i = 0; i < NLS; i++) {
	//	draw_lb_slot(i);
	//}
	draw_lb();
	
	// draw right buttons (only when not editing terrain)
	if (overall_mode >= MODE_MAIN_SCREEN) {
		draw_rect.left = RIGHT_AREA_UL_X;
		draw_rect.top = RIGHT_AREA_UL_Y;
		draw_rect.right = RIGHT_AREA_UL_X + RIGHT_AREA_WIDTH - 16;
		draw_rect.bottom = RIGHT_AREA_UL_Y + RIGHT_AREA_HEIGHT;
		
		frame_rect(mainPtr, draw_rect, sf::Color::Black);
		draw_rect.inset(1,1);
		tileImage(mainPtr,draw_rect,bg[17]);
		
		draw_rb();
		right_sbar->draw();
	}
	
	// draw terrain palette
	if ((overall_mode < MODE_MAIN_SCREEN) || (overall_mode == MODE_EDIT_TYPES)) {
		//draw_rect = terrain_buttons_rect;
		//OffsetRect(&draw_rect,RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
		//rect_draw_some_item(terrain_buttons_gworld,terrain_buttons_rect,
		//	terrain_buttons_gworld,draw_rect,0,1);
		place_location();
	}
	
	
}

void draw_lb() {
	RECT temp_rect(mainPtr);
	short i;
	
	temp_rect.right = RIGHT_AREA_UL_X - 2;
	tileImage(mainPtr,temp_rect,bg[20]);
	for (i = 0; i < NLS; i++)
		draw_lb_slot(i,0);
}

// mode 0 normal 1 click
void draw_lb_slot (short which,short mode)  {
	RECT text_rect,from_rect;
	
 	tileImage(mainPtr,left_buttons[which][0],bg[20]);
	if (left_button_status[which] == 0)
		return;
	text_rect = left_buttons[which][0];
	if (left_button_status[which] >= 10) {
		text_rect.left += 18;
		from_rect = blue_button_from;
		if (mode > 0)
			from_rect.offset(from_rect.right - from_rect.left,0);
		rect_draw_some_item(editor_mixed,from_rect,left_buttons[which][1],(location){0,0});
	}
	if (left_button_status[which] % 10 == 3)
		text_rect.left += 16;
	TextStyle style;
	if (left_button_status[which] % 10 == 2) {
		style.pointSize = 14;
	}
	else text_rect.offset(0,2);
	if (mode > 0)
		style.colour = sf::Color::Blue;
	style.lineHeight = 12;
	win_draw_string(mainPtr,text_rect,strings_ls[which],eTextMode::WRAP,style);
}

void draw_rb() {
	short i,pos;
	
 	pos = right_sbar->getPosition();
	for (i = pos; i < pos + NRSONPAGE; i++)
		draw_rb_slot(i,0);
}

// mode 0 normal 1 pressed
void draw_rb_slot (short which,short mode)  {
	RECT text_rect;
 	short pos;
 	
 	pos = right_sbar->getPosition();
	if ((which < pos) || (which >= pos + NRSONPAGE))
		return;
	
 	tileImage(mainPtr,right_buttons[which - pos],bg[17]);
	if (right_button_status[which] == 0)
		return;
	text_rect = right_buttons[which - pos];
	
	TextStyle style;
	if (mode > 0)
		style.colour = sf::Color::Red;
	style.lineHeight = 12;
	win_draw_string(mainPtr,text_rect,(char *)strings_rs[which],eTextMode::WRAP,style);
}

void set_up_terrain_buttons() {
	short i,j,pic,small_i;
	RECT ter_from,ter_from_base = {0,0,36,28};
	RECT tiny_from,tiny_to;
	
	RECT palette_from,palette_to = palette_button_base;
	
	tileImage(terrain_buttons_gworld,terrain_buttons_rect,bg[17]);
	frame_rect(terrain_buttons_gworld, terrain_buttons_rect, sf::Color::Black);
 	
	// first make terrain buttons
	switch(draw_mode){
		case DRAW_TERRAIN:
			for (i = 0; i < 256; i++) {
				ter_from = ter_from_base;
				pic = scenario.ter_types[i].picture;
				if (pic >= 1000) {
					sf::Texture* source_gworld;
					graf_pos_ref(source_gworld, ter_from) = spec_scen_g.find_graphic(pic % 1000);
					rect_draw_some_item(*source_gworld,
										ter_from,terrain_buttons_gworld,terrain_rects[i]);
				}
				else if (pic < 400)	{
					pic = pic % 50;
					ter_from.offset(28 * (pic % 10), 36 * (pic / 10));
					rect_draw_some_item(terrain_gworld[scenario.ter_types[i].picture/50],
										ter_from,terrain_buttons_gworld,terrain_rects[i]);
				}
				else {
					pic = pic % 50;
					ter_from.left = 112 * (pic / 5);
					ter_from.right = ter_from.left + 28;
					ter_from.top = 36 * (pic % 5);
					ter_from.bottom = ter_from.top + 36;
					rect_draw_some_item(anim_gworld,
										ter_from,terrain_buttons_gworld,terrain_rects[i]);
					
				}
				small_i = get_small_icon(i);
//				if (i == 82)
//					small_i = 3;
//				if (i == 83)
//					small_i = 2;
//				if ((i == 7) || (i == 10) || (i == 13) || (i == 16))
//					small_i = 23;
				tiny_from = base_small_button_from;
				tiny_from.offset(7 * (small_i % 10),7 * (small_i / 10));
				tiny_to = terrain_rects[i];
				tiny_to.top = tiny_to.bottom - 7;
				tiny_to.left = tiny_to.right - 7;
				if (small_i > 0 && small_i < 255)
					rect_draw_some_item(editor_mixed,tiny_from,terrain_buttons_gworld,tiny_to);
			}
			break;
		case DRAW_MONST:
			break;
		case DRAW_ITEM:
			break;
	}
	
	if (overall_mode < MODE_MAIN_SCREEN) {
//		palette_to.left = 5;
//		palette_to.top = terrain_rects[255].bottom + 5;
//		if (editing_town) {
//			palette_from.bottom = palette_buttons[0][5].bottom;
//			palette_from.right = palette_buttons[7][5].right;
//			}
//			else {
//				palette_from.bottom = palette_buttons[0][2].bottom;
//				palette_from.right = palette_buttons[7][2].right;
//				}
//		palette_to.right = palette_to.left + palette_from.right;
//		palette_to.bottom = palette_to.top + palette_from.bottom;
//		rect_draw_some_item(editor_mixed,
//			palette_from,terrain_buttons_gworld,palette_to,1,0);
		palette_to.offset(5,terrain_rects[255].bottom + 14);
		palette_to.right++;
		//printf("terrain_rects[255].bottom = %i\n", terrain_rects[255].bottom);
		for(i = 0; i < 10; i++){
			for(j = 0; j < 6; j++){
				if(editing_town && town_buttons[j][i] >= 0)
					palette_from = palette_buttons_from[town_buttons[j][i]];
				else if(!editing_town && out_buttons[j][i] >= 0)
					palette_from = palette_buttons_from[out_buttons[j][i]];
				else palette_from = palette_button_base;
				//printf("palette_from = {top = %i, left = %i, bottom = %i, right = %i\n",
				//	   palette_from.top, palette_from.left, palette_from.bottom, palette_from.right);
				//printf("palette_to = {top = %i, left = %i, bottom = %i, right = %i\n",
				//	   palette_to.top, palette_to.left, palette_to.bottom, palette_to.right);
				if(good_palette_buttons[editing_town][j][i]){
					//printf("Drawing button %i at col %i, row %i\n",editing_town?town_buttons[j][i]:out_buttons[j][i],i,j);
					rect_draw_some_item(editor_mixed,palette_from,terrain_buttons_gworld,palette_to,sf::BlendAlpha);
				}
				palette_to.offset(0,17);
			}
			palette_to.offset(25,-6*17);
		}
	}
}

void draw_terrain(){
	short q,r,x,i,small_i;
	location which_pt,where_draw;
	RECT draw_rect,clipping_rect = {8,8,332,260};
	unsigned char t_to_draw;
	RECT source_rect,tiny_to,tiny_to_base = {37,29,44,36},tiny_from,from_rect,to_rect;
	RECT boat_rect = {0,0,36,28};
	
	if (overall_mode >= MODE_MAIN_SCREEN)
		return;
	
	if (cur_viewing_mode == 0) {
		tileImage(ter_draw_gworld,terrain_rect,bg[17]);
		frame_rect(ter_draw_gworld, terrain_rect, sf::Color::Black);
		for (q = 0; q < 9; q++)
			for (r = 0; r < 9; r++)
			{
				where_draw.x = q;
				where_draw.y = r;
				if (editing_town) {
					t_to_draw = town->terrain(cen_x + q - 4,cen_y + r - 4);
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
				
				if(is_spot(cen_x + q - 4,cen_y + r - 4))
					Draw_Some_Item(fields_gworld, calc_rect(4, 0), ter_draw_gworld, where_draw, sf::BlendAlpha);
				
				which_pt.x = cen_x + q - 4;
				which_pt.y =cen_y + r - 4;
				
				tiny_to = tiny_to_base;
				tiny_to.offset(28 * q, 36 * r);
				
				// draw start icon, if starting point
				if ((editing_town) &&
					(cur_town == scenario.which_town_start) && (scenario.where_start.x == cen_x + q - 4)
					&& (scenario.where_start.y == cen_y + r - 4)) {
					from_rect = start_button_from;
					to_rect = tiny_to;
					to_rect.left -= 14;
					rect_draw_some_item(editor_mixed,from_rect,ter_draw_gworld,to_rect);
					tiny_to.offset(0,-7);
				}
				if (!editing_town
					&& (scenario.out_sec_start.x == cur_out.x)
					&& (scenario.out_sec_start.y == cur_out.y)
					&& (scenario.out_start.x == cen_x + q - 4)
					&& (scenario.out_start.y == cen_y + r - 4)) {
					from_rect = start_button_from;
					to_rect = tiny_to;
					to_rect.left -= 14;
					rect_draw_some_item(editor_mixed,from_rect,ter_draw_gworld,to_rect);
					tiny_to.offset(0,-7);
				}
				small_i = get_small_icon(t_to_draw);
				tiny_from = base_small_button_from;
				tiny_from.offset(7 * (small_i % 10),7 * (small_i / 10));
				if (small_i > 0) {
					rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to);
					tiny_to.offset(0,-7);
				}
				
				if (is_special(cen_x + q - 4,cen_y + r - 4)) {
					tiny_from = base_small_button_from;
					tiny_from.offset(7 * (7),7 * (0));
					rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to);
					tiny_to.offset(0,-7);
				}
				if ((t_to_draw == 7) || (t_to_draw == 10) || (t_to_draw == 13) || (t_to_draw == 16)) {
					tiny_from = base_small_button_from;
					tiny_from.offset(7 * (3),7 * (2));
					rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to);
					tiny_to.offset(0,-7);
				}
				//if (is_s_d(cen_x + q - 4,cen_y + r - 4)) {
				//}
				if (!editing_town) {
					for (i = 0; i < 4; i++)
						if ((cen_x + q - 4 == current_terrain.wandering_locs[i].x) &&
							(cen_y + r - 4 == current_terrain.wandering_locs[i].y)) {
							tiny_from = base_small_button_from;
							tiny_from.offset(7 * (2),7 * (1));
							rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to);
							tiny_to.offset(0,-7);
							i = 4;
						}
					
				}
				
				if (editing_town) {
					for (i = 0; i < 30; i++) {
						if ((scenario.boats[i].which_town == cur_town) &&
							(scenario.boats[i].loc.x == cen_x + q - 4) &&
							(scenario.boats[i].loc.y == cen_y + r - 4))
							Draw_Some_Item(vehicle_gworld,boat_rect,ter_draw_gworld,where_draw,sf::BlendAlpha);
						
					}
					for (i = 0; i < 30; i++) {
						source_rect = boat_rect;
						source_rect.offset(0,36);
						if ((scenario.horses[i].which_town == cur_town) &&
							(scenario.horses[i].loc.x == cen_x + q - 4) &&
							(scenario.horses[i].loc.y == cen_y + r - 4))
							Draw_Some_Item(vehicle_gworld,source_rect,ter_draw_gworld,where_draw,sf::BlendAlpha);
						
					}
					for (i = 0; i < 4; i++)
						if ((cen_x + q - 4 == town->start_locs[i].x) &&
							(cen_y + r - 4 == town->start_locs[i].y)) {
							tiny_from = base_small_button_from;
							tiny_from.offset(7 * (6 + i),7 * (1));
							rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to);
							tiny_to.offset(0,-7);
						}
					for (i = 0; i < 4; i++)
						if ((cen_x + q - 4 == town->wandering_locs[i].x) &&
							(cen_y + r - 4 == town->wandering_locs[i].y)) {
							tiny_from = base_small_button_from;
							tiny_from.offset(7 * (2),7 * (1));
							rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to);
							tiny_to.offset(0,-7);
							i = 4;
						}
					if (is_web(cen_x + q - 4,cen_y + r - 4)) {
						from_rect = calc_rect(5,0);
						Draw_Some_Item(fields_gworld,from_rect,ter_draw_gworld,where_draw,sf::BlendAlpha);
					}
					if (is_crate(cen_x + q - 4,cen_y + r - 4)) {
						from_rect = calc_rect(6,0);
						Draw_Some_Item(fields_gworld,from_rect,ter_draw_gworld,where_draw,sf::BlendAlpha);
					}
					if (is_barrel(cen_x + q - 4,cen_y + r - 4)) {
						from_rect = calc_rect(7,0);
						Draw_Some_Item(fields_gworld,from_rect,ter_draw_gworld,where_draw,sf::BlendAlpha);
					}
					if (is_fire_barrier(cen_x + q - 4,cen_y + r - 4)) {
						from_rect = calc_rect(0,2);
						Draw_Some_Item(fields_gworld,from_rect,ter_draw_gworld,where_draw,sf::BlendAlpha);
					}
					if (is_quickfire(cen_x + q - 4,cen_y + r - 4)) {
						from_rect = calc_rect(7,1);
						Draw_Some_Item(fields_gworld,from_rect,ter_draw_gworld,where_draw,sf::BlendAlpha);
					}
					if (is_force_barrier(cen_x + q - 4,cen_y + r - 4)) {
						from_rect = calc_rect(2,2);
						Draw_Some_Item(fields_gworld,from_rect,ter_draw_gworld,where_draw,sf::BlendAlpha);
					}
					for (i = 0; i < 8; i++)
						if (is_sfx(cen_x + q - 4,cen_y + r - 4,i)) {
							from_rect = calc_rect(i,3);
							Draw_Some_Item(fields_gworld,from_rect,ter_draw_gworld,where_draw,sf::BlendAlpha);
						}
					for (x = 0; x < town->max_items(); x++)
						if ((cen_x + q - 4 == town->preset_items[x].loc.x) &&
							(cen_y + r - 4 == town->preset_items[x].loc.y) && (town->preset_items[x].code >= 0)) {
						}
					for (x = 0; x < town->max_monst(); x++)
						if ((cen_x + q - 4 == town->creatures(x).start_loc.x) &&
							(cen_y + r - 4 == town->creatures(x).start_loc.y) && (town->creatures(x).number != 0)) {
						}
					
				}
			}
		if (editing_town) {
			draw_monsts();
			draw_items();
		}
		
		clip_rect(ter_draw_gworld,clipping_rect);
		
		if (editing_town) {
			// draw info rects
			for (i = 0; i < 16; i++)
				if (town->room_rect(i).left > 0) {
					draw_rect.left = 22 + 28 * (town->room_rect(i).left - cen_x + 4);
					draw_rect.right = 22 + 28 * (town->room_rect(i).right - cen_x + 4);
					draw_rect.top = 24 + 36 * (town->room_rect(i).top - cen_y + 4);
					draw_rect.bottom = 24 + 36 * (town->room_rect(i).bottom - cen_y + 4);
					frame_rect(ter_draw_gworld, draw_rect, sf::Color::Red);
				}
			// draw border rect
			draw_rect.left = 21 + 28 * (town->in_town_rect.left - cen_x + 4);
			draw_rect.right = 21 + 28 * (town->in_town_rect.right - cen_x + 4);
			draw_rect.top = 25 + 36 * (town->in_town_rect.top - cen_y + 4);
			draw_rect.bottom = 25 + 36 * (town->in_town_rect.bottom - cen_y + 4);
			frame_rect(ter_draw_gworld, draw_rect, sf::Color::White);
		}
		if (!editing_town) {
			// draw info rects
			for (i = 0; i < 8; i++)
				if (current_terrain.info_rect[i].left > 0) {
					draw_rect.left = 22 + 28 * (current_terrain.info_rect[i].left - cen_x + 4);
					draw_rect.right = 22 + 28 * (current_terrain.info_rect[i].right - cen_x + 4);
					draw_rect.top = 24 + 36 * (current_terrain.info_rect[i].top - cen_y + 4);
					draw_rect.bottom = 24 + 36 * (current_terrain.info_rect[i].bottom - cen_y + 4);
					frame_rect(ter_draw_gworld, draw_rect, sf::Color::Red);
				}
		}
		clip_rect(ter_draw_gworld, terrain_rect);
		
		small_any_drawn = false;
		//if (cur_viewing_mode == 0)
		//	draw_frames();
	}
	
	if (cur_viewing_mode == 1) {
		if (!small_any_drawn) {
		 	tileImage(ter_draw_gworld, terrain_rect,bg[17]);
			frame_rect(ter_draw_gworld, terrain_rect, sf::Color::Black);
		}
		for (q = 0; q < (editing_town ? town->max_dim() : 48); q++)
			for (r = 0; r < (editing_town ? town->max_dim() : 48); r++) {
				t_to_draw = editing_town ? town->terrain(q,r) : current_terrain.terrain[q][r];
				if (!small_what_drawn[q][r] != t_to_draw || small_any_drawn) {
					draw_one_tiny_terrain_spot(q,r,t_to_draw);
					small_what_drawn[q][r] = t_to_draw;
				}
			}
		small_any_drawn = true;
	}
	ter_draw_gworld.display();
	
	//to_rect = world_screen;
	//OffsetRect(&to_rect,TER_RECT_UL_X,TER_RECT_UL_Y);
	rect_draw_some_item(ter_draw_gworld.getTexture(),terrain_rect,mainPtr,world_screen);
}

void draw_monsts() {
	short i,k,width,height,m_start_pic;
	sf::Texture* from_gworld;
	RECT source_rect;
	location where_draw,store_loc;
	
	for (i = 0; i < town->max_monst(); i++)
		if (town->creatures(i).number != 0) {
			where_draw.x = town->creatures(i).start_loc.x - cen_x + 4;
			where_draw.y = town->creatures(i).start_loc.y - cen_y + 4;
			width = scenario.scen_monsters[town->creatures(i).number].x_width;
			height = scenario.scen_monsters[town->creatures(i).number].y_width;
			
			for (k = 0; k < width * height; k++) {
				store_loc = where_draw;
				if ((where_draw.x == minmax(0,8,where_draw.x)) &&
					(where_draw.y == minmax(0,8,where_draw.y)) &&
					(scenario.scen_monsters[town->creatures(i).number].picture_num >= 1000)) {
					graf_pos_ref(from_gworld, source_rect) = spec_scen_g.find_graphic((scenario.scen_monsters[town->creatures(i).number].picture_num + k) % 1000);
					store_loc.x += k % width;
					store_loc.y += k / width;
					Draw_Some_Item(*from_gworld, source_rect, ter_draw_gworld, store_loc, sf::BlendAlpha);
				}
				else if (scenario.scen_monsters[town->creatures(i).number].picture_num < 1000) {
					m_start_pic = m_pic_index[scenario.scen_monsters[town->creatures(i).number].picture_num].i + k;
					from_gworld = &monst_gworld[m_start_pic / 20];
					m_start_pic = m_start_pic % 20;
					source_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
					store_loc.x += k % width;
					store_loc.y += k / width;
					Draw_Some_Item(*from_gworld, source_rect, ter_draw_gworld, store_loc, sf::BlendAlpha);
				}
			}
		}
}

// Returns rect for drawing an item, if num < 25, rect is in big item template,
// otherwise in small item template
// TODO: I have another function that does the same thing but also returns the texture containing the item along with the RECT
RECT get_item_template_rect (short type_wanted) {
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

void draw_items() {
	short i;
	RECT source_rect,dest_rect;
	location where_draw;
	short pic_num;
	
	for (i = 0; i < town->max_items(); i++) {
		if (town->preset_items[i].code >= 0) {
			where_draw.x = town->preset_items[i].loc.x - cen_x + 4;
			where_draw.y = town->preset_items[i].loc.y - cen_y + 4;
			pic_num = scenario.scen_items[town->preset_items[i].code].graphic_num;
			if ((where_draw.x >= 0) && (where_draw.x <= 8) &&
				(where_draw.y >= 0) && (where_draw.y <= 8))  {
				
				// TODO: Item pics start at 1000 now instead of 150
				if (pic_num >= 150) {
					sf::Texture* source_gworld;
					graf_pos_ref(source_gworld, source_rect) = spec_scen_g.find_graphic(pic_num - 150);
					dest_rect = calc_rect(where_draw.x,where_draw.y);
					rect_draw_some_item(*source_gworld, source_rect, ter_draw_gworld, dest_rect, sf::BlendAlpha);
				}
				else {
					source_rect = get_item_template_rect(pic_num);
					dest_rect = calc_rect(where_draw.x,where_draw.y);
					dest_rect.offset(8,8);
					if (pic_num >= 45) {
						dest_rect.top += 9;
						dest_rect.bottom -= 9;
						dest_rect.left += 5;
						dest_rect.right -= 5;
					}
					rect_draw_some_item((pic_num < 55) ? items_gworld : tiny_obj_gworld,
										source_rect, ter_draw_gworld, dest_rect,sf::BlendAlpha);
				}
			}
		}
	}
}


void force_tiny_redraw() {
//	short q,r;
//	for (q = 0; q < 8; q++)
//		for (r = 0; r < 64; r++)
//			ter_changed[q][r] = 255;
	
}

void redraw_selected_ter() {
	//	draw_one_terrain_spot(18,8,(unsigned char) current_terrain_type);
}

void draw_one_terrain_spot (short i,short j,ter_num_t terrain_to_draw) {
	location where_draw;
	RECT source_rect;
	short picture_wanted;
	sf::Texture* source_gworld;
	
	picture_wanted = scenario.ter_types[terrain_to_draw].picture;
//	if (picture_wanted >= 1000) {
//		terrain_to_draw = 90;
//		picture_wanted = 74;
//	}
	
	where_draw.x = (char) i;
	where_draw.y = (char) j;
	
	if(picture_wanted >= 1000 && spec_scen_g) {
		graf_pos_ref(source_gworld, source_rect) = spec_scen_g.find_graphic(picture_wanted % 1000);
	}
	else if (picture_wanted >= 400)	{
		source_gworld = &anim_gworld;
		picture_wanted -= 400;
		source_rect.left = 112 * (picture_wanted / 5);
		source_rect.right = source_rect.left + 28;
		source_rect.top = 36 * (picture_wanted % 5);
		source_rect.bottom = source_rect.top + 36;
	}
	else {
		source_rect = get_template_rect(terrain_to_draw);
		source_gworld = &terrain_gworld[picture_wanted / 50];
	}
	
	Draw_Some_Item(*source_gworld, source_rect, ter_draw_gworld, where_draw);
}

void draw_one_tiny_terrain_spot (short i,short j,ter_num_t terrain_to_draw) {
	
	location where_draw;
	// TODO: Update for new 12x12 map graphics, rather than 4x4
	RECT dest_rect = {0,0,4,4},from_rect = {0,0,4,4};
	short picture_wanted;
	sf::Texture* source_gworld;
	
	picture_wanted = scenario.ter_types[terrain_to_draw].picture;
	
	where_draw.x = (char) i;
	where_draw.y = (char) j;
	dest_rect.offset(8 + 4 * i,8 + 4 * j);
	switch (picture_wanted) {
			
		case 0: case 1: case 73: case 72:
			tileImage(ter_draw_gworld, dest_rect,map_pat[0]);
			break;
		case 2: case 3: case 4:
			tileImage(ter_draw_gworld, dest_rect,map_pat[1]);
			break;
			
		default:
			if ((picture_wanted < 170) && (map_pats[picture_wanted] > 0)) {
				tileImage(ter_draw_gworld, dest_rect,map_pat[map_pats[picture_wanted]]);
			}
			else if (picture_wanted >= 1000)	{
				graf_pos_ref(source_gworld, from_rect) = spec_scen_g.find_graphic(picture_wanted % 1000);
				rect_draw_some_item(*source_gworld, from_rect, ter_draw_gworld, dest_rect);
			}
			else if (picture_wanted >= 400)	{
				source_gworld = &anim_gworld;
				picture_wanted -= 400;
				if (picture_wanted == 0) tileImage(ter_draw_gworld, dest_rect,map_pat[13]);
				else if (picture_wanted == 4) tileImage(ter_draw_gworld, dest_rect,map_pat[21]);
				else if (picture_wanted == 7) tileImage(ter_draw_gworld, dest_rect,map_pat[20]);
				else if (picture_wanted == 8) tileImage(ter_draw_gworld, dest_rect,map_pat[19]);
				else if (picture_wanted == 9) tileImage(ter_draw_gworld, dest_rect,map_pat[20]);
				else if (picture_wanted == 10) tileImage(ter_draw_gworld, dest_rect,map_pat[19]);
				else {
					//source_rect.left = 112 * (picture_wanted / 5);
					//source_rect.right = source_rect.left + 28;
					//source_rect.top = 36 * (picture_wanted % 5);
					//source_rect.bottom = source_rect.top + 36;
					from_rect.offset((picture_wanted / 5) * 24 + 1,(picture_wanted % 5) * 6 + 1 + 156);
					rect_draw_some_item(small_ter_gworld, from_rect, ter_draw_gworld, dest_rect);
					//rect_draw_some_item(source_gworld, source_rect, ter_draw_gworld, dest_rect, 0, 0);
				}
			}
			else {
				//source_rect = get_template_rect(terrain_to_draw);
				//source_gworld = terrain_gworld[picture_wanted / 50];
				from_rect.offset((picture_wanted % 10) * 6 + 1,(picture_wanted / 10) * 6 + 1);
				rect_draw_some_item(small_ter_gworld, from_rect, ter_draw_gworld, dest_rect);
			}
			break;
	}
}

/* Draw a bitmap in the world window. hor in 0 .. 8, vert in 0 .. 8,
 object is ptr. to bitmap to be drawn, and masking is for Copybits. */
// TODO: Wait, why is targ_gworld unused?
void Draw_Some_Item(sf::Texture& src_gworld,RECT src_rect,sf::RenderTarget& /*targ_gworld*/,location target,sf::BlendMode mode) {
	RECT	destrec;
	
	if ((target.x < 0) || (target.x > 8) || (target.y < 0) || (target.y > 8))
		return;
	destrec.left = 8 + BITMAP_WIDTH * target.x;
	destrec.right = destrec.left + BITMAP_WIDTH;
	destrec.top = 8 + BITMAP_HEIGHT * target.y;
	destrec.bottom = destrec.top + BITMAP_HEIGHT;
	
	destrec.left = destrec.right - (src_rect.right - src_rect.left);
	destrec.top = destrec.bottom - (src_rect.bottom - src_rect.top);
	
	rect_draw_some_item(src_gworld,src_rect,ter_draw_gworld,destrec,mode);
}

/* Input terrain currently trying to draw. Get back RECT in terrain template containing
 desired pixmap, or RECT to darkness if desired map not present */
RECT get_template_rect (unsigned short type_wanted) {
	RECT store_rect;
	short picture_wanted;
	
	picture_wanted = scenario.ter_types[type_wanted].picture;
	if (picture_wanted >= 1000)
		picture_wanted = 0;
	picture_wanted = picture_wanted % 50;
	store_rect.top = 0 + (picture_wanted / 10) * BITMAP_HEIGHT;
	store_rect.bottom = store_rect.top + BITMAP_HEIGHT;
	store_rect.left = 0 + (picture_wanted % 10) * BITMAP_WIDTH;
	store_rect.right = store_rect.left + BITMAP_WIDTH;
	
	return store_rect;
}

void draw_frames() {
	char q,r,i;
	location which_pt;
	RECT draw_rect;
	
	for (q = 0; q < 9; q++) {
		for (r = 0; r < 9; r++)
		{
			which_pt.x = cen_x + q - 4;
			which_pt.y = cen_y + r - 4;
			draw_rect.top = 23 + r * 36;
			draw_rect.bottom = 58 + r * 36;
			draw_rect.left = 23 + q * 28;
			draw_rect.right = 50 + q * 28;
			for (i = 0; i < 4; i++)
				if ((which_pt.x == town->wandering_locs[i].x) &&
					(which_pt.y == town->wandering_locs[i].y)) {
					
					frame_rect(ter_draw_gworld, draw_rect, sf::Color::Red);
				}
			for (i = 0; i < 4; i++)
				if ((which_pt.x == town->start_locs[i].x) &&
					(which_pt.y == town->start_locs[i].y)) {
					frame_rect(ter_draw_gworld, draw_rect, sf::Color::Magenta);
				}
			
			
		}
	}
	
}


void place_location() {
	char draw_str[256];
	RECT draw_rect,source_rect,erase_rect;
	RECT text_rect = {0,0,12,100};
	short picture_wanted;
	
	/*
	 from_rect = terrain_buttons_rect;
	 from_rect.top = from_rect.bottom - 150;
	 draw_rect = from_rect;
	 OffsetRect(&draw_rect,RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
	 rect_draw_some_item(terrain_buttons_gworld,from_rect,
	 terrain_buttons_gworld,draw_rect,0,1);
	 */
	
	//EraseRect(&text_rect);
	
	erase_rect.left = 2;//terrain_rects[255].left + 17;
	erase_rect.right = RIGHT_AREA_WIDTH - 1;
	erase_rect.top = terrain_rects[255].top + 26 - 9;//12 - 9;
	erase_rect.bottom = erase_rect.top + 12;
	tileImage(terrain_buttons_gworld, erase_rect,bg[17]);
	
	//MoveTo(terrain_rects[255].left + 20 ,terrain_rects[255].top + 12);
	location moveTo(5 ,terrain_rects[255].top + 26);
	draw_rect = text_rect;
	draw_rect.offset(moveTo);
	if (overall_mode < MODE_MAIN_SCREEN)
		sprintf((char *) draw_str,"Center: x = %d, y = %d  ",cen_x,cen_y);
	else {
		//MoveTo(5 ,terrain_rects[255].top + 28);
		sprintf((char *) draw_str,"Click terrain to edit. ");
	}
	TextStyle style;
	style.lineHeight = 12;
	win_draw_string(terrain_buttons_gworld, draw_rect, draw_str, eTextMode::LEFT_TOP, style);
	
	moveTo = location(260 ,terrain_rects[255].top + 26);
	draw_rect = text_rect;
	draw_rect.offset(moveTo);
	sprintf((char*)draw_str,"%i",current_terrain_type);
	win_draw_string(terrain_buttons_gworld, draw_rect, draw_str, eTextMode::LEFT_TOP, style);
	
	erase_rect.left = 2;
	erase_rect.right = RIGHT_AREA_WIDTH - 1;
	erase_rect.top = terrain_rects[255].bottom + 117;
	erase_rect.bottom = RIGHT_AREA_HEIGHT + 6;
	tileImage(terrain_buttons_gworld, erase_rect,bg[17]);
	
	if (overall_mode < MODE_MAIN_SCREEN) {
		moveTo = location(5,terrain_rects[255].bottom + 129);
		draw_rect = text_rect;
		draw_rect.offset(moveTo);
		win_draw_string(terrain_buttons_gworld, draw_rect, current_string, eTextMode::LEFT_TOP, style);
		moveTo = location(RIGHT_AREA_WIDTH / 2,terrain_rects[255].bottom + 129);
		draw_rect = text_rect;
		draw_rect.offset(moveTo);
		win_draw_string(terrain_buttons_gworld, draw_rect, current_string2, eTextMode::LEFT_TOP, style);
	}
	
	draw_rect.top = palette_buttons[0][0].top + terrain_rects[255].bottom + 5;
	draw_rect.left = palette_buttons[9][0].right + 10; // + 17;
	draw_rect.bottom = draw_rect.top + 36;
	draw_rect.right = draw_rect.left + 28;
	picture_wanted = scenario.ter_types[current_terrain_type].picture;
	
	if (overall_mode < MODE_MAIN_SCREEN) {
		if (picture_wanted >= 1000)	{
			sf::Texture* source_gworld;
			graf_pos_ref(source_gworld, source_rect) = spec_scen_g.find_graphic(picture_wanted % 1000);
			rect_draw_some_item(*source_gworld,
								source_rect,terrain_buttons_gworld,draw_rect);
		}
		else if (picture_wanted >= 400)	{
			picture_wanted -= 400;
			source_rect.left = 112 * (picture_wanted / 5);
			source_rect.right = source_rect.left + 28;
			source_rect.top = 36 * (picture_wanted % 5);
			source_rect.bottom = source_rect.top + 36;
			rect_draw_some_item(anim_gworld,source_rect,terrain_buttons_gworld,draw_rect);
		}
		else {
			source_rect = get_template_rect(current_terrain_type);
			rect_draw_some_item(terrain_gworld[picture_wanted / 50],source_rect,
								terrain_buttons_gworld,draw_rect);
		}
		short small_i = get_small_icon(current_terrain_type);
		RECT tiny_to = draw_rect;
		tiny_to.top = tiny_to.bottom - 7;
		tiny_to.left = tiny_to.right - 7;
		RECT tiny_from = base_small_button_from;
		tiny_from.offset(7 * (small_i % 10),7 * (small_i / 10));
		if (small_i > 0 && small_i < 255)
			rect_draw_some_item(editor_mixed,tiny_from,terrain_buttons_gworld,tiny_to);
	}
	
	terrain_buttons_gworld.display();
	draw_rect = terrain_buttons_rect;
	draw_rect.offset(RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
	rect_draw_some_item(terrain_buttons_gworld.getTexture(),terrain_buttons_rect,mainPtr,draw_rect);
}

// klugde for speed ...exactly like place location above, but just writes location
void place_just_location() {
	char draw_str[256];
	RECT from_rect,draw_rect,erase_rect;
	
	erase_rect.left = terrain_rects[255].left + 17;
	erase_rect.right = RIGHT_AREA_WIDTH - 1;
	erase_rect.top = terrain_rects[255].top + 12 - 9;
	erase_rect.bottom = erase_rect.top + 12;
	tileImage(terrain_buttons_gworld, erase_rect,bg[17]);
	
	//MoveTo(terrain_rects[255].left + 20 ,terrain_rects[255].top + 12);
	location moveTo(5 ,terrain_rects[255].top + 26);
	draw_rect = {0,0,12,100};
	draw_rect.offset(moveTo);
	if (overall_mode < MODE_MAIN_SCREEN)
		sprintf((char *) draw_str,"Center: x = %d, y = %d  ",cen_x,cen_y);
	else {
		//MoveTo(5 ,terrain_rects[255].top + 28);
		sprintf((char *) draw_str,"Click terrain to edit. ");
	}
	TextStyle style;
	style.lineHeight = 12;
	win_draw_string(terrain_buttons_gworld, draw_rect, draw_str, eTextMode::LEFT_TOP, style);
	
	from_rect = terrain_buttons_rect;
	from_rect.top = erase_rect.top;
	from_rect.bottom = erase_rect.bottom;
	draw_rect = from_rect;
	draw_rect.offset(RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
	rect_draw_some_item(terrain_buttons_gworld.getTexture(),from_rect,mainPtr,draw_rect);
}

void set_string(const char *string,const char *string2) {
	strcpy((char *)current_string,string);
//	if (strlen(string2) == 0)
//		current_string2[0] = 0;
//	else
//		sprintf((char *)current_string2,"Bob");
	strcpy((char *)current_string2,string2);
}

/*
void draw_cur_string() {
	RECT from_rect,draw_rect;
	
	from_rect = terrain_buttons_rect;
	from_rect.top = from_rect.bottom - 40;
	draw_rect = from_rect;
	OffsetRect(&draw_rect,RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
	rect_draw_some_item(terrain_buttons_gworld,from_rect,
						terrain_buttons_gworld,draw_rect,0,1);
	MoveTo(RIGHT_AREA_UL_X + 5,terrain_rects[255].bottom + 120);
	DrawString(current_string);
	MoveTo(RIGHT_AREA_UL_X + 5,terrain_rects[255].bottom + 132);
	DrawString(current_string2);
} */

bool is_special(short i,short j) {
	short k;
	
	if (editing_town)
		for (k = 0; k < 50; k++)
			if ((town->special_locs[k].x == i) && (town->special_locs[k].y == j))
				return true;
	if (!editing_town)
		for (k = 0; k < 18; k++)
			if ((current_terrain.special_locs[k].x == i) && (current_terrain.special_locs[k].y == j))
				return true;
	
	return false;
}

void take_special(short /*i*/,short /*j*/) {
}

void make_special(short /*i*/,short /*j*/) {
}

void sort_specials() {
}

bool is_spot(short i,short j){
	if(editing_town)
		return is_field_type(i,j,2);
	else return current_terrain.special_spot[i][j];
	return false;
}

bool is_field_type(short i,short j,short field_type) {
	unsigned short k;
	
	for (k = 0; k < town->preset_fields.size(); k++)
		if ((town->preset_fields[k].type == field_type) &&
			(town->preset_fields[k].loc.x == i) &&
			(town->preset_fields[k].loc.y == j))
			return true;
	return false;
}

void make_field_type(short i,short j,short field_type) {
	unsigned short k;
	
	if (is_field_type(i,j,field_type))
		return;
	for (k = 0; k < town->preset_fields.size(); k++)
		if (town->preset_fields[k].type == 0) {
			town->preset_fields[k].loc.x = i;
			town->preset_fields[k].loc.y = j;
			town->preset_fields[k].type = field_type;
			return;
		}
	//give_error("Each town can have at most 50 fields and special effects (webs, barrels, blood stains, etc.). To place more, use the eraser first.","",0);
	cTown::cField the_field;
	the_field.loc.x = i;
	the_field.loc.y = j;
	the_field.type = field_type;
	town->preset_fields.push_back(the_field);
}


void take_field_type(short i,short j,short field_type) {
	unsigned short k;
	
	for (k = 0; k < town->preset_fields.size(); k++)
		if ((town->preset_fields[k].type == field_type) &&
			(town->preset_fields[k].loc.x == i) &&
			(town->preset_fields[k].loc.y == j)) {
			town->preset_fields[k].type = 0;
			return;
		}
}

bool is_web(short i,short j) {
	return is_field_type(i,j,3);
	
}
void make_web(short i,short j) {
	make_field_type(i,j,3);
}
void take_web(short i,short j) {
	take_field_type(i,j,3);
}

bool is_crate(short i,short j) {
	return is_field_type(i,j,4);
}
void make_crate(short i,short j) {
	make_field_type(i,j,4);
	
}
void take_crate(short i,short j) {
	take_field_type(i,j,4);
}

bool is_barrel(short i,short j) {
	return is_field_type(i,j,5);
}
void make_barrel(short i,short j) {
	make_field_type(i,j,5);
}
void take_barrel(short i,short j) {
	take_field_type(i,j,5);
}

bool is_fire_barrier(short i,short j) {
	return is_field_type(i,j,6);
}
void make_fire_barrier(short i,short j) {
	make_field_type(i,j,6);
}
void take_fire_barrier(short i,short j) {
	take_field_type(i,j,6);
}

bool is_force_barrier(short i,short j) {
	return is_field_type(i,j,7);
}
void make_force_barrier(short i,short j) {
	make_field_type(i,j,7);
}
void take_force_barrier(short i,short j) {
	take_field_type(i,j,7);
	
}

bool is_sfx(short i,short j,short type) {
	return is_field_type(i,j,type + 14);
}
void make_sfx(short i,short j,short type) {
	make_field_type(i,j,type + 14);
}
void take_sfx(short i,short j,short type) {
	take_field_type(i,j,type + 14);
}


bool is_quickfire(short i,short j) {
	return is_field_type(i,j,8);
}
void make_quickfire(short i,short j) {
	make_field_type(i,j,8);
}
void take_quickfire(short i,short j) {
	take_field_type(i,j,8);
}

bool container_there(location l) {
	if (!editing_town)
		return false;
	if(scenario.ter_types[town->terrain(l.x,l.y)].special == eTerSpec::IS_A_CONTAINER)
		return true;
	if (is_barrel(l.x,l.y))
		return true;
	if (is_crate(l.x,l.y))
		return true;
	return 0;
}

//void get_str(const char* str,short i, short j)
//{
//	if (i == -1) {
//		strcpy((char *) str,data_store->scen_item_list.monst_names[j]);
//		return;
//	}
//	if (i == -2) {
//		strcpy((char *) str,data_store->scen_item_list.scen_items[j].full_name);
//		return;
//	}
//	if (i == -3) {
//		strcpy((char *) str,buttons[available_dlog_buttons[j]].str);
//		return;
//	}
//	if (i == -4) {
//		strcpy((char *) str,data_store->scen_item_list.ter_names[j]);
//		return;
//	}
//	if (i == -5) {
//		get_str(str,40,j * 7 + 1);
//		return;
//	}
//	GetIndString(str, i, j);
//	p2cstr(str);
//}
void record_display_strings(){}
