
#include <cstdio>
#include <sstream>
#include <cstring>
#include "scen.global.hpp"
#include "scenario.hpp"
#include "graphtool.hpp"
#include "scen.graphics.hpp"
#include <cmath>
#include "scen.keydlgs.hpp"
#include "soundtool.hpp"
#include "mathutil.hpp"
#include "boe.consts.hpp" // TODO: Put these constants in a global file

#include "dialog.hpp"

#include "scen.core.hpp"
#include "scen.townout.hpp"
#include "scrollbar.hpp"
#include "restypes.hpp"

#include "scen.btnmg.hpp"

void load_main_screen();
void load_terrain_template();
short terrain_in_index();
void put_terrain_in_template();
void undo_clip();

short find_index_spot();
bool is_s_d();
void sort_specials();

extern cOutdoors* current_terrain;
extern sf::RenderWindow mainPtr;
extern cTown* current_town;
extern short cen_x, cen_y,current_terrain_type,cur_town;
extern cTown* town;
extern short cur_viewing_mode;
extern eScenMode overall_mode;
eDrawMode draw_mode = DRAW_TERRAIN;
extern short available_dlog_buttons[NUM_DLOG_B];
extern bool editing_town;
extern cScenario scenario;
extern rectangle world_screen;
extern sf::Texture bg_gworld;
extern rectangle right_buttons[NRSONPAGE];
extern rectangle right_scrollbar_rect;
extern std::shared_ptr<cScrollbar> right_sbar, pal_sbar;

extern bool left_buttons_active,right_buttons_active;
extern std::array<lb_t,NLS> left_button_status;
extern std::vector<rb_t> right_button_status;
short mini_map_scales[3] = {12, 6, 4};
// TODO: What is this for?
//extern btn_t buttons[];
extern location cur_out, mouse_spot;
extern ter_num_t current_ground;

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
sf::RenderTexture ter_draw_gworld;
sf::Texture dlogpics_gworld;
sf::Texture talkfaces_gworld;
sf::Texture roads_gworld;
sf::Texture missiles_gworld;
sf::Texture status_gworld;
sf::Texture pc_gworld;
const sf::Color hilite_colour = {0xff, 0x00, 0x80, 0x40};
extern tessel_ref_t map_pat[];

// begin new stuff
rectangle blue_button_from = {120,235,134,251};
rectangle start_button_from = {120,70,127,91};
rectangle base_small_button_from = {120,0,127,7};
extern rectangle palette_buttons[10][6];
extern rectangle palette_buttons_from[71];
extern ePalBtn town_buttons[6][10], out_buttons[6][10];
rectangle palette_button_base = {0,0,18,25};
rectangle terrain_buttons_rect = {0,0,410,294};
extern rectangle left_buttons[NLS][2]; // 0 - whole, 1 - blue button
rectangle left_button_base = {5,5,21,280};
rectangle right_button_base = {RIGHT_AREA_UL_Y,RIGHT_AREA_UL_X,17,RIGHT_AREA_UL_Y};
rectangle terrain_rect = {0,0,340,272};
std::string current_string[2];
extern rectangle terrain_rects[256];

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
			icon = scenario.ter_types[ter].flag1;
			break;
		case eTerSpec::CHANGE_WHEN_STEP_ON:
			icon = 87;
			break;
		case eTerSpec::DAMAGING:
			switch(eDamageType(scenario.ter_types[ter].flag3)) {
				case eDamageType::WEAPON:
					icon = 16;
					break;
				case eDamageType::FIRE:
					icon = 18;
					break;
				case eDamageType::POISON:
					icon = 17;
					break;
				case eDamageType::MAGIC:
					icon = 20;
					break;
				case eDamageType::SPECIAL:
					icon = 22;
					break;
				case eDamageType::UNBLOCKABLE:
					icon = 21;
					break;
				case eDamageType::COLD:
					icon = 19;
					break;
				case eDamageType::UNDEAD:
					icon = 25;
					break;
				case eDamageType::DEMON:
					icon = 23;
					break;
				case eDamageType::MARKED: // Invalid
					break;
			}
			break;
		case eTerSpec::BRIDGE:
			icon = 82;
			break;
		case eTerSpec::BED:
			icon = -1;
			break;
		case eTerSpec::DANGEROUS:
			icon = 12;
			switch((eStatus)scenario.ter_types[ter].flag3){
				case eStatus::POISONED_WEAPON: // TODO: Do something here
					break;
				case eStatus::BLESS_CURSE:
					icon = scenario.ter_types[ter].flag1 > 0 ? 4 : 5;
					break;
				case eStatus::POISON:
					if(scenario.ter_types[ter].flag1 > 0)
						icon = 1;
					break;
				case eStatus::HASTE_SLOW:
					icon = scenario.ter_types[ter].flag1 > 0 ? 6 : 7;
					break;
				case eStatus::INVULNERABLE: // TODO: Do something here
					break;
				case eStatus::MAGIC_RESISTANCE: // TODO: Do something here
					break;
				case eStatus::WEBS:
					if(scenario.ter_types[ter].flag1 > 0)
						icon = 52;
					break;
				case eStatus::DISEASE:
					if(scenario.ter_types[ter].flag1 > 0)
						icon = 0;
					break;
				case eStatus::INVISIBLE: // TODO: Do something here
					break;
				case eStatus::DUMB:
					icon = scenario.ter_types[ter].flag1 > 0 ? 8 : 9;
					break;
				case eStatus::MARTYRS_SHIELD: // TODO: Do something here
					break;
				case eStatus::ASLEEP:
					if(scenario.ter_types[ter].flag1 > 0)
						icon = 3;
					break;
				case eStatus::PARALYZED: // TODO: Do something here
					break;
				case eStatus::ACID:
					if(scenario.ter_types[ter].flag1 > 0)
						icon = 2;
					break;
				case eStatus::FORCECAGE:
					if(scenario.ter_types[ter].flag1 > 0)
						icon = 43;
					break;
				case eStatus::MAIN: case eStatus::CHARM:
					icon = -1;
					break; // Nothing to do here; these values are "magic" and should not be used
			}
			break;
		case eTerSpec::CRUMBLING:
			icon = 98;
			break;
		case eTerSpec::LOCKABLE:
			icon = 94;
			break;
		case eTerSpec::UNLOCKABLE:
			if(scenario.ter_types[ter].flag2 >= 5)
				icon = (scenario.ter_types[ter].flag2 == 10) ? 96 : 95;
			else icon = 94;
			break;
		case eTerSpec::IS_A_SIGN:
			icon = 92;
			break;
		case eTerSpec::CALL_SPECIAL:
		case eTerSpec::CALL_SPECIAL_WHEN_USED:
			icon = scenario.ter_types[ter].flag3;
			break;
		case eTerSpec::IS_A_CONTAINER:
			icon = 93;
			break;
		case eTerSpec::WATERFALL_CAVE:
		case eTerSpec::WATERFALL_SURFACE:
			icon = 91;
			break;
		case eTerSpec::CONVEYOR:
			switch(scenario.ter_types[ter].flag1){
				case DIR_N:
					icon = 78;
					break;
				case DIR_NE:
					icon = 79;
					break;
				case DIR_NW:
					icon = 77;
					break;
				case DIR_E:
					icon = 80;
					break;
				case DIR_S:
					icon = 74;
					break;
				case DIR_SE:
					icon = 81;
					break;
				case DIR_SW:
					icon = 75;
					break;
				case DIR_W:
					icon = 76;
					break;
			}
			break;
		case eTerSpec::BLOCKED_TO_MONSTERS:
			icon = 28;
			break;
		case eTerSpec::TOWN_ENTRANCE:
			icon = 84;
			break;
		case eTerSpec::CHANGE_WHEN_USED:
			icon = 97;
			break;
		case eTerSpec::WILDERNESS_CAVE:
		case eTerSpec::WILDERNESS_SURFACE:
			icon = 90;
			break;
		default:
			icon = -1;
	}
	if(icon == 255) icon = -1;
	return icon;
}

static std::vector<short> get_small_icons(location at, ter_num_t t_to_draw) {
	std::vector<short> icons;
	// draw start icon, if starting point
	if(editing_town && cur_town == scenario.which_town_start && scenario.where_start == at) {
		icons.push_back(-1);
	}
	if(!editing_town && scenario.out_sec_start == cur_out && scenario.out_start == at) {
		icons.push_back(-1);
	}
	short ter_small_i = get_small_icon(t_to_draw);
	// Special case for towns
	if(ter_small_i == 84 && !editing_town) {
		bool have_town = false;
		for(size_t i = 0; i < current_terrain->city_locs.size(); i++) {
			if(current_terrain->city_locs[i] == at)
				have_town = true;
		}
		if(!have_town) ter_small_i++;
	}
	if(ter_small_i >= 0)
		icons.push_back(ter_small_i);
	
	if(is_special(at.x, at.y)) {
		std::vector<spec_loc_t>& spec_list = editing_town ? town->special_locs : current_terrain->special_locs;
		int num_spec = std::count_if(spec_list.begin(), spec_list.end(), [at](spec_loc_t which) {
			return which.spec >= 0 && which.x == at.x && which.y == at.y;
		});
		if(num_spec > 1)
			icons.push_back(89);
		else icons.push_back(88);
	}
	if(editing_town) {
		if(scenario.ter_types[t_to_draw].light_radius > 0)
			icons.push_back(83);
		for(size_t i = 0; i < 4; i++)
			if(at == town->start_locs[i]) {
				icons.push_back(70 + i);
			}
		for(size_t i = 0; i < 4; i++)
			if(at == town->wandering_locs[i]) {
				icons.push_back(86);
			}
		if(is_field_type(at.x, at.y, BARRIER_FIRE)) {
			icons.push_back(33);
		}
		if(is_field_type(at.x, at.y, BARRIER_FORCE)) {
			icons.push_back(34);
		}
	} else {
		for(size_t i = 0; i < 4; i++)
			if(at == current_terrain->wandering_locs[i]) {
				icons.push_back(86);
			}
	}
	return icons;
}

void Set_up_win () {
	short i,j;
	for(i = 0; i < 70; i++){
		palette_buttons_from[i] = palette_button_base;
		palette_buttons_from[i].offset((i%10) * 25, (i/10) * 17);
		palette_buttons_from[i].right++;
	}
	for(i = 0; i < 10; i++)
		for(j = 0; j < 6; j++) {
			palette_buttons[i][j] = palette_button_base;
			palette_buttons[i][j].offset(i * 25, j * 17);
		}
	for(i = 0; i < 10; i++)
		for(j = /*2*/0; j < 6; j++)
			palette_buttons[i][j].offset(0,3);
	for(i = 0; i < 10; i++)
		for(j = /*3*/0; j < 6; j++)
			palette_buttons[i][j].offset(0,3);
	for(i = 0; i < 10; i++)
		for(j = /*4*/0; j < 6; j++)
			palette_buttons[i][j].offset(0,3);
	
	for(i = 0; i < NLS; i++) {
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
	for(i = 0; i < NRSONPAGE; i++) {
		right_buttons[i] = right_button_base;
		right_buttons[i].offset(0,i * 12);
	}
	load_main_screen();
}

void run_startup_g() {
	sf::Event event;
	rectangle pict_rect = {0,0,480,640}, dest_rect;
	sf::Texture pict_to_draw;
	dest_rect = pict_rect;
	
	dest_rect.offset(-25,-25);
	pict_to_draw.loadFromImage(*ResMgr::get<ImageRsrc>("edsplash"));
	for(int i = 0; i < 10; i++) {
		mainPtr.clear(sf::Color::Black);
		rect_draw_some_item(pict_to_draw, pict_rect, mainPtr, dest_rect, sf::BlendAlpha);
		mainPtr.display();
		mainPtr.pollEvent(event);
	}
	play_sound(-95);
	sf::Clock timer;
	while(timer.getElapsedTime() < time_in_ticks(120)) {
		mainPtr.pollEvent(event);
		if(event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed)
			break;
	}
	
}

void load_graphics(){
	int i;
	
	for(i = 0; i < NUM_MONST_SHEETS; i++){
		std::ostringstream sout;
		sout << "monst" << i + 1;
		monst_gworld[i].loadFromImage(*ResMgr::get<ImageRsrc>(sout.str()));
	}
	for(i = 0; i < NUM_TER_SHEETS; i++){
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
	rectangle windRect(mainPtr);
	tileImage(mainPtr,windRect,bg[20]);
	draw_main_screen();
	if(overall_mode < MODE_MAIN_SCREEN)
		draw_terrain();
	mainPtr.display();
}

void draw_main_screen() {
	rectangle draw_rect;
	
	draw_lb();
	
	// draw right buttons (only when not editing terrain)
	if(overall_mode >= MODE_MAIN_SCREEN) {
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
	if((overall_mode < MODE_MAIN_SCREEN) || (overall_mode == MODE_EDIT_TYPES)) {
		//draw_rect = terrain_buttons_rect;
		//OffsetRect(&draw_rect,RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
		//rect_draw_some_item(terrain_buttons_gworld,terrain_buttons_rect,
		//	terrain_buttons_gworld,draw_rect,0,1);
		place_location();
		pal_sbar->draw();
	}
	
	
}

void draw_lb() {
	short i;
	
	for(i = 0; i < NLS; i++)
		draw_lb_slot(i,0);
}

// mode 0 normal 1 click
void draw_lb_slot (short which,short mode)  {
	rectangle text_rect,from_rect;
	
 	tileImage(mainPtr,left_buttons[which][0],bg[20]);
	if(left_button_status[which].mode == LB_CLEAR)
		return;
	text_rect = left_buttons[which][0];
	if(left_button_status[which].action != LB_NO_ACTION) {
		text_rect.left += 18;
		from_rect = blue_button_from;
		if(mode > 0)
			from_rect.offset(0,from_rect.height());
		rect_draw_some_item(editor_mixed,from_rect,left_buttons[which][1],location{0,0});
	}
	if(left_button_status[which].mode == LB_INDENT)
		text_rect.left += 16;
	TextStyle style;
	if(left_button_status[which].mode == LB_TITLE) {
		style.pointSize = 14;
	}
	else text_rect.offset(0,2);
	if(mode > 0)
		style.colour = sf::Color::Blue;
	style.lineHeight = 12;
	win_draw_string(mainPtr,text_rect,left_button_status[which].label,eTextMode::WRAP,style);
}

void draw_rb() {
	long pos = right_sbar->getPosition();
	for(long i = pos; i < pos + NRSONPAGE && i < NRS; i++)
		draw_rb_slot(i,0);
}

// mode 0 normal 1 pressed
void draw_rb_slot (short which,short mode)  {
	rectangle text_rect;
 	long pos;
 	
 	pos = right_sbar->getPosition();
	if(which < pos || which >= pos + NRSONPAGE || which >= NRS)
		return;
	
 	tileImage(mainPtr,right_buttons[which - pos],bg[17]);
	if(right_button_status[which].action == RB_CLEAR)
		return;
	text_rect = right_buttons[which - pos];
	
	TextStyle style;
	if(mode > 0)
		style.colour = sf::Color::Red;
	style.lineHeight = 12;
	win_draw_string(mainPtr,text_rect,right_button_status[which].label,eTextMode::WRAP,style);
}

void set_up_terrain_buttons(bool reset) {
	short i,j,pic,small_i;
	rectangle ter_from,ter_from_base = {0,0,36,28}, ter_plus_from = {148,235,164,251};
	rectangle tiny_from,tiny_to;
	
	rectangle palette_from,palette_to = palette_button_base;
	int max;
	switch(draw_mode) {
		case DRAW_TERRAIN: max = scenario.ter_types.size(); break;
		case DRAW_ITEM: max = scenario.scen_items.size(); break;
		case DRAW_MONST: max = scenario.scen_monsters.size(); max--; break;
		default: return;
	}
	if(overall_mode == MODE_EDIT_TYPES) max++;
	
	if(reset) pal_sbar->setPosition(0);
	pal_sbar->setMaximum((max - 241) / 16);
	
	tileImage(terrain_buttons_gworld,terrain_buttons_rect,bg[17]);
	frame_rect(terrain_buttons_gworld, terrain_buttons_rect, sf::Color::Black);
	int first = pal_sbar->getPosition() * 16;
	if(draw_mode == DRAW_MONST) first++, max++;
	int end = min(first + 256, max);
 	
	// first make terrain buttons
	for(i = first; i < end; i++) {
		switch(draw_mode){
			case DRAW_TERRAIN:
				if(i == scenario.ter_types.size()) {
					rect_draw_some_item(editor_mixed, ter_plus_from, terrain_buttons_gworld, terrain_rects[i - first]);
					break;
				}
				ter_from = ter_from_base;
				pic = scenario.ter_types[i].picture;
				if(pic >= 1000) {
					sf::Texture* source_gworld;
					graf_pos_ref(source_gworld, ter_from) = spec_scen_g.find_graphic(pic % 1000);
					rect_draw_some_item(*source_gworld,
										ter_from,terrain_buttons_gworld,terrain_rects[i - first]);
				}
				else if(pic < 960)	{
					pic = pic % 50;
					ter_from.offset(28 * (pic % 10), 36 * (pic / 10));
					rect_draw_some_item(terrain_gworld[scenario.ter_types[i].picture/50],
										ter_from,terrain_buttons_gworld,terrain_rects[i - first]);
				}
				else {
					pic = (pic - 560) % 50;
					ter_from.left = 112 * (pic / 5);
					ter_from.right = ter_from.left + 28;
					ter_from.top = 36 * (pic % 5);
					ter_from.bottom = ter_from.top + 36;
					rect_draw_some_item(anim_gworld,
										ter_from,terrain_buttons_gworld,terrain_rects[i - first]);
					
				}
				small_i = get_small_icon(i);
				tiny_from = base_small_button_from;
				tiny_from.offset(7 * (small_i % 30),7 * (small_i / 30));
				tiny_to = terrain_rects[i - first];
				tiny_to.top = tiny_to.bottom - 7;
				tiny_to.left = tiny_to.right - 7;
				if(small_i >= 0 && small_i < 255)
					rect_draw_some_item(editor_mixed,tiny_from,terrain_buttons_gworld,tiny_to);
				break;
			case DRAW_MONST:
				pic = scenario.scen_monsters[i].picture_num;
				tiny_to = terrain_rects[i - first];
				frame_rect(terrain_buttons_gworld, tiny_to, sf::Color::Black);
				if(pic >= 4000) {
					pic %= 1000;
					tiny_to.width() = tiny_to.width() / 2;
					tiny_to.height() = tiny_to.height() / 2;
					sf::Texture* source_gworld;
					graf_pos_ref(source_gworld, ter_from) = spec_scen_g.find_graphic(pic);
					rect_draw_some_item(*source_gworld, ter_from, terrain_buttons_gworld, tiny_to, sf::BlendAlpha);
					pic++;
					tiny_to.offset(tiny_to.width(), 0);
					graf_pos_ref(source_gworld, ter_from) = spec_scen_g.find_graphic(pic);
					rect_draw_some_item(*source_gworld, ter_from, terrain_buttons_gworld, tiny_to, sf::BlendAlpha);
					pic++;
					tiny_to.offset(-tiny_to.width(), tiny_to.height());
					graf_pos_ref(source_gworld, ter_from) = spec_scen_g.find_graphic(pic);
					rect_draw_some_item(*source_gworld, ter_from, terrain_buttons_gworld, tiny_to, sf::BlendAlpha);
					pic++;
					tiny_to.offset(tiny_to.width(), 0);
					graf_pos_ref(source_gworld, ter_from) = spec_scen_g.find_graphic(pic);
					rect_draw_some_item(*source_gworld, ter_from, terrain_buttons_gworld, tiny_to, sf::BlendAlpha);
				} else if(pic >= 3000) {
					pic %= 1000;
					tiny_to.width() = tiny_to.width() / 2;
					tiny_to.height() = tiny_to.height() / 2;
					tiny_to.offset(tiny_to.width() / 2, 0);
					sf::Texture* source_gworld;
					graf_pos_ref(source_gworld, ter_from) = spec_scen_g.find_graphic(pic);
					rect_draw_some_item(*source_gworld, ter_from, terrain_buttons_gworld, tiny_to, sf::BlendAlpha);
					pic++;
					tiny_to.offset(0, tiny_to.height());
					graf_pos_ref(source_gworld, ter_from) = spec_scen_g.find_graphic(pic);
					rect_draw_some_item(*source_gworld, ter_from, terrain_buttons_gworld, tiny_to, sf::BlendAlpha);
				} else if(pic >= 2000) {
					pic %= 1000;
					tiny_to.width() = tiny_to.width() / 2;
					tiny_to.height() = tiny_to.height() / 2;
					tiny_to.offset(0, tiny_to.height() / 2);
					sf::Texture* source_gworld;
					graf_pos_ref(source_gworld, ter_from) = spec_scen_g.find_graphic(pic);
					rect_draw_some_item(*source_gworld, ter_from, terrain_buttons_gworld, tiny_to, sf::BlendAlpha);
					pic++;
					tiny_to.offset(tiny_to.width(), 0);
					graf_pos_ref(source_gworld, ter_from) = spec_scen_g.find_graphic(pic);
					rect_draw_some_item(*source_gworld, ter_from, terrain_buttons_gworld, tiny_to, sf::BlendAlpha);
				} else if(pic >= 1000) {
					pic %= 1000;
					sf::Texture* source_gworld;
					graf_pos_ref(source_gworld, ter_from) = spec_scen_g.find_graphic(pic);
					rect_draw_some_item(*source_gworld, ter_from, terrain_buttons_gworld, tiny_to, sf::BlendAlpha);
				} else {
					auto pic_info = m_pic_index[pic];
					pic = pic_info.i;
					if(pic_info.x == 2 && pic_info.y == 2) {
						tiny_to.width() = tiny_to.width() / 2;
						tiny_to.height() = tiny_to.height() / 2;
						ter_from = calc_rect(2 * ((pic % 20) / 10), (pic % 20) % 10);
						rect_draw_some_item(monst_gworld[pic / 20], ter_from, terrain_buttons_gworld, tiny_to, sf::BlendAlpha);
						pic++;
						tiny_to.offset(tiny_to.width(), 0);
						ter_from = calc_rect(2 * ((pic % 20) / 10), (pic % 20) % 10);
						rect_draw_some_item(monst_gworld[pic / 20], ter_from, terrain_buttons_gworld, tiny_to, sf::BlendAlpha);
						pic++;
						tiny_to.offset(-tiny_to.width(), tiny_to.height());
						ter_from = calc_rect(2 * ((pic % 20) / 10), (pic % 20) % 10);
						rect_draw_some_item(monst_gworld[pic / 20], ter_from, terrain_buttons_gworld, tiny_to, sf::BlendAlpha);
						pic++;
						tiny_to.offset(tiny_to.width(), 0);
						ter_from = calc_rect(2 * ((pic % 20) / 10), (pic % 20) % 10);
						rect_draw_some_item(monst_gworld[pic / 20], ter_from, terrain_buttons_gworld, tiny_to, sf::BlendAlpha);
					} else if(pic_info.y == 2) {
						tiny_to.width() = tiny_to.width() / 2;
						tiny_to.height() = tiny_to.height() / 2;
						tiny_to.offset(tiny_to.width() / 2, 0);
						ter_from = calc_rect(2 * ((pic % 20) / 10), (pic % 20) % 10);
						rect_draw_some_item(monst_gworld[pic / 20], ter_from, terrain_buttons_gworld, tiny_to, sf::BlendAlpha);
						pic++;
						tiny_to.offset(0, tiny_to.height());
						ter_from = calc_rect(2 * ((pic % 20) / 10), (pic % 20) % 10);
						rect_draw_some_item(monst_gworld[pic / 20], ter_from, terrain_buttons_gworld, tiny_to, sf::BlendAlpha);
					} else if(pic_info.x == 2) {
						tiny_to.width() = tiny_to.width() / 2;
						tiny_to.height() = tiny_to.height() / 2;
						tiny_to.offset(0, tiny_to.height() / 2);
						ter_from = calc_rect(2 * ((pic % 20) / 10), (pic % 20) % 10);
						rect_draw_some_item(monst_gworld[pic / 20], ter_from, terrain_buttons_gworld, tiny_to, sf::BlendAlpha);
						pic++;
						tiny_to.offset(tiny_to.width(), 0);
						ter_from = calc_rect(2 * ((pic % 20) / 10), (pic % 20) % 10);
						rect_draw_some_item(monst_gworld[pic / 20], ter_from, terrain_buttons_gworld, tiny_to, sf::BlendAlpha);
					} else {
						ter_from = calc_rect(2 * ((pic % 20) / 10), (pic % 20) % 10);
						rect_draw_some_item(monst_gworld[pic / 20], ter_from, terrain_buttons_gworld, tiny_to, sf::BlendAlpha);
					}
				}
				break;
			case DRAW_ITEM:
				pic = scenario.scen_items[i].graphic_num;
				tiny_to = terrain_rects[i - first];
				frame_rect(terrain_buttons_gworld, tiny_to, sf::Color::Black);
				if(pic >= 1000) {
					sf::Texture* source_gworld;
					graf_pos_ref(source_gworld, ter_from) = spec_scen_g.find_graphic(pic % 1000);
					rect_draw_some_item(*source_gworld,ter_from,terrain_buttons_gworld,tiny_to,sf::BlendAlpha);
				} else {
					tiny_from = {0,0,18,18};
					tiny_from.offset((pic % 10) * 18,(pic / 10) * 18);
					rect_draw_some_item(tiny_obj_gworld,tiny_from,terrain_buttons_gworld,tiny_to,sf::BlendAlpha);
				}
				break;
		}
	}
	
	if(overall_mode < MODE_MAIN_SCREEN) {
//		palette_to.left = 5;
//		palette_to.top = terrain_rects[255].bottom + 5;
//		if(editing_town) {
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
				auto cur_palette_buttons = editing_town ? town_buttons : out_buttons;
				if(cur_palette_buttons[j][i] != PAL_BLANK)
					palette_from = palette_buttons_from[cur_palette_buttons[j][i]];
				//printf("palette_from = {top = %i, left = %i, bottom = %i, right = %i\n",
				//	   palette_from.top, palette_from.left, palette_from.bottom, palette_from.right);
				//printf("palette_to = {top = %i, left = %i, bottom = %i, right = %i\n",
				//	   palette_to.top, palette_to.left, palette_to.bottom, palette_to.right);
				if(cur_palette_buttons[j][i] != PAL_BLANK){
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
	short q,r,i;
	location which_pt,where_draw;
	rectangle draw_rect,clipping_rect = {8,8,332,260};
	ter_num_t t_to_draw;
	rectangle source_rect,tiny_to,tiny_to_base = {37,29,44,36},from_rect,to_rect;
	rectangle boat_rect = {0,0,36,28};
	
	if(overall_mode >= MODE_MAIN_SCREEN)
		return;
	
	if(cur_viewing_mode == 0) {
		tileImage(ter_draw_gworld,terrain_rect,bg[17]);
		frame_rect(ter_draw_gworld, terrain_rect, sf::Color::Black);
		for(q = 0; q < 9; q++)
			for(r = 0; r < 9; r++) {
				where_draw.x = q;
				where_draw.y = r;
				if(editing_town) {
					t_to_draw = town->terrain(cen_x + q - 4,cen_y + r - 4);
				}
				else {
					short ter_x = cen_x + q - 4, ter_y = cen_y + r - 4;
					if(ter_x == -1 && ter_y == -1 && cur_out.x > 0 && cur_out.y > 0)
						t_to_draw = scenario.outdoors[cur_out.x - 1][cur_out.y - 1]->terrain[47][47];
					else if(ter_x == -1 && ter_y == 48 && cur_out.x > 0 && cur_out.y < scenario.outdoors.height() - 1)
						t_to_draw = scenario.outdoors[cur_out.x - 1][cur_out.y + 1]->terrain[47][0];
					else if(ter_x == 48 && ter_y == -1 && cur_out.x < scenario.outdoors.width() - 1 && cur_out.y > 0)
						t_to_draw = scenario.outdoors[cur_out.x + 1][cur_out.y - 1]->terrain[0][47];
					else if(ter_x == 48 && ter_y == 48 && cur_out.x < scenario.outdoors.width() - 1 && cur_out.y < scenario.outdoors.height() - 1)
						t_to_draw = scenario.outdoors[cur_out.x + 1][cur_out.y + 1]->terrain[0][0];
					else if(ter_x == -1 && ter_y >= 0 && ter_y < 48 && cur_out.x > 0)
						t_to_draw = scenario.outdoors[cur_out.x - 1][cur_out.y]->terrain[47][ter_y];
					else if(ter_y == -1 && ter_x >= 0 && ter_x < 48 && cur_out.y > 0)
						t_to_draw = scenario.outdoors[cur_out.x][cur_out.y - 1]->terrain[ter_x][47];
					else if(ter_x == 48 && ter_y >= 0 && ter_y < 48 && cur_out.x < scenario.outdoors.width() - 1)
						t_to_draw = scenario.outdoors[cur_out.x + 1][cur_out.y]->terrain[0][ter_y];
					else if(ter_y == 48 && ter_x >= 0 && ter_x < 48 && cur_out.y < scenario.outdoors.height() - 1)
						t_to_draw = scenario.outdoors[cur_out.x][cur_out.y + 1]->terrain[ter_x][0];
					else if(ter_x == -1 || ter_x == 48 || ter_y == -1 || ter_y == 48)
						t_to_draw = 90;
					else t_to_draw = current_terrain->terrain[ter_x][ter_y];
				}
				draw_one_terrain_spot(q,r,t_to_draw);
				
				rectangle destrec;
				
				destrec.left = 8 + BITMAP_WIDTH * where_draw.x;
				destrec.right = destrec.left + BITMAP_WIDTH;
				destrec.top = 8 + BITMAP_HEIGHT * where_draw.y;
				destrec.bottom = destrec.top + BITMAP_HEIGHT;
				
				if(is_spot(cen_x + q - 4,cen_y + r - 4))
					rect_draw_some_item(fields_gworld, calc_rect(4, 0), ter_draw_gworld, destrec, sf::BlendAlpha);
				
				which_pt.x = cen_x + q - 4;
				which_pt.y =cen_y + r - 4;
				
				// TODO: I'm not quite sure whether these should be testing loc or loc_in_sec
				if(!editing_town) {
					for(i = 0; i < scenario.boats.size(); i++) {
						if(scenario.boats[i].which_town == 200 &&
						   scenario.boats[i].sector == cur_out &&
						   scenario.boats[i].loc == which_pt)
							rect_draw_some_item(vehicle_gworld,boat_rect,ter_draw_gworld,destrec,sf::BlendAlpha);
						
					}
					for(i = 0; i < scenario.horses.size(); i++) {
						source_rect = boat_rect;
						source_rect.offset(0,36);
						if(scenario.horses[i].which_town == 200 &&
						   scenario.horses[i].sector == cur_out &&
						   scenario.horses[i].loc == which_pt)
							rect_draw_some_item(vehicle_gworld,source_rect,ter_draw_gworld,destrec,sf::BlendAlpha);
						
					}
				}
				
				if(editing_town) {
					for(i = 0; i < scenario.boats.size(); i++) {
						if(scenario.boats[i].which_town == cur_town &&
						   scenario.boats[i].loc == which_pt)
							rect_draw_some_item(vehicle_gworld,boat_rect,ter_draw_gworld,destrec,sf::BlendAlpha);
						
					}
					for(i = 0; i < scenario.horses.size(); i++) {
						source_rect = boat_rect;
						source_rect.offset(0,36);
						if(scenario.horses[i].which_town == cur_town &&
						   scenario.horses[i].loc == which_pt)
							rect_draw_some_item(vehicle_gworld,source_rect,ter_draw_gworld,destrec,sf::BlendAlpha);
						
					}
					if(is_field_type(cen_x + q - 4,cen_y + r - 4, FIELD_WEB)) {
						from_rect = calc_rect(5,0);
						rect_draw_some_item(fields_gworld,from_rect,ter_draw_gworld,destrec,sf::BlendAlpha);
					}
					if(is_field_type(cen_x + q - 4,cen_y + r - 4, OBJECT_CRATE)) {
						from_rect = calc_rect(6,0);
						rect_draw_some_item(fields_gworld,from_rect,ter_draw_gworld,destrec,sf::BlendAlpha);
					}
					if(is_field_type(cen_x + q - 4,cen_y + r - 4, OBJECT_BARREL)) {
						from_rect = calc_rect(7,0);
						rect_draw_some_item(fields_gworld,from_rect,ter_draw_gworld,destrec,sf::BlendAlpha);
					}
					if(is_field_type(cen_x + q - 4,cen_y + r - 4, BARRIER_FIRE)) {
						from_rect = calc_rect(8,4);
						rect_draw_some_item(anim_gworld,from_rect,ter_draw_gworld,destrec,sf::BlendAlpha);
					}
					if(is_field_type(cen_x + q - 4,cen_y + r - 4, FIELD_QUICKFIRE)) {
						from_rect = calc_rect(7,1);
						rect_draw_some_item(fields_gworld,from_rect,ter_draw_gworld,destrec,sf::BlendAlpha);
					}
					if(is_field_type(cen_x + q - 4,cen_y + r - 4, BARRIER_FORCE)) {
						from_rect = calc_rect(10,4);
						rect_draw_some_item(anim_gworld,from_rect,ter_draw_gworld,destrec,sf::BlendAlpha);
					}
					if(is_field_type(cen_x + q - 4,cen_y + r - 4, OBJECT_BLOCK)) {
						from_rect = calc_rect(3,0);
						rect_draw_some_item(fields_gworld,from_rect,ter_draw_gworld,destrec,sf::BlendAlpha);
					}
					if(is_field_type(cen_x + q - 4,cen_y + r - 4, BARRIER_CAGE)) {
						from_rect = calc_rect(0,0);
						rect_draw_some_item(fields_gworld,from_rect,ter_draw_gworld,destrec,sf::BlendAlpha);
					}
					for(i = 0; i < 8; i++) {
						eFieldType sfx = eFieldType(SFX_SMALL_BLOOD + i);
						if(is_field_type(cen_x + q - 4,cen_y + r - 4,sfx)) {
							from_rect = calc_rect(i,3);
							rect_draw_some_item(fields_gworld,from_rect,ter_draw_gworld,destrec,sf::BlendAlpha);
						}
					}
				}
				
				tiny_to = tiny_to_base;
				tiny_to.offset(28 * q, 36 * r);
				
				// Tiny icons
				std::vector<short> icons = get_small_icons(loc(cen_x + q - 4, cen_y + r - 4), t_to_draw);
				
				if(!icons.empty()) {
					bool has_start = icons[0] == -1;
					rectangle tiny_from_base = {120, 0, 127, 7};
					for(short icon : icons) {
						rectangle tiny_from = tiny_from_base;
						if(icon == -1) {
							tiny_from.offset(30 * 7, 0);
							tiny_from.right += 14;
							tiny_to.left -= 14;
						} else {
							tiny_from.offset((icon % 30) * 7, (icon / 30) * 7);
						}
						rect_draw_some_item(editor_mixed, tiny_from, ter_draw_gworld, tiny_to);
						if(icon == -1) tiny_to.left += 14;
						tiny_to.offset(0, -7);
						// Now check to see if it's overflowing our space
						if(tiny_to.top < destrec.top) {
							tiny_to.offset(-7, 7 * (has_start ? 4 : 5));
						}
					}
				}
				
				if(mouse_spot.x >= 0 && mouse_spot.y >= 0) {
					bool need_hilite = false, large_hilite = false;
					int d = dist(where_draw, mouse_spot);
					if(overall_mode == MODE_SMALL_PAINTBRUSH && d <= 1) {
						need_hilite = true;
						large_hilite = true;
					} else if((overall_mode == MODE_ERASER || overall_mode == MODE_SMALL_SPRAYCAN) && d <= 2) {
						need_hilite = true;
						large_hilite = true;
					} else if((overall_mode == MODE_LARGE_PAINTBRUSH || overall_mode == MODE_LARGE_SPRAYCAN) && d <= 4) {
						need_hilite = true;
						large_hilite = true;
					} else if(where_draw == mouse_spot)
						need_hilite = true;
					if(need_hilite) {
						fill_rect(ter_draw_gworld, destrec, hilite_colour);
						if(large_hilite && where_draw == mouse_spot)
							fill_rect(ter_draw_gworld, destrec, hilite_colour);
					}
				}
			}
		if(editing_town) {
			draw_monsts();
			draw_items();
		}
		
		clip_rect(ter_draw_gworld,clipping_rect);
		
		if(editing_town) {
			// draw info rects
			for(i = 0; i < town->room_rect.size(); i++)
				if(town->room_rect[i].left > 0) {
					draw_rect.left = 22 + 28 * (town->room_rect[i].left - cen_x + 4);
					draw_rect.right = 22 + 28 * (town->room_rect[i].right - cen_x + 4);
					draw_rect.top = 24 + 36 * (town->room_rect[i].top - cen_y + 4);
					draw_rect.bottom = 24 + 36 * (town->room_rect[i].bottom - cen_y + 4);
					draw_rect.inset(-10, -13);
					frame_rect(ter_draw_gworld, draw_rect, sf::Color::Red);
				}
			// draw border rect
			draw_rect.left = 21 + 28 * (town->in_town_rect.left - cen_x + 4);
			draw_rect.right = 21 + 28 * (town->in_town_rect.right - cen_x + 4);
			draw_rect.top = 25 + 36 * (town->in_town_rect.top - cen_y + 4);
			draw_rect.bottom = 25 + 36 * (town->in_town_rect.bottom - cen_y + 4);
			draw_rect.inset(10, 13);
			frame_rect(ter_draw_gworld, draw_rect, sf::Color::White);
		}
		if(!editing_town) {
			// draw info rects
			for(i = 0; i < current_terrain->info_rect.size(); i++)
				if(current_terrain->info_rect[i].left > 0) {
					draw_rect.left = 22 + 28 * (current_terrain->info_rect[i].left - cen_x + 4);
					draw_rect.right = 22 + 28 * (current_terrain->info_rect[i].right - cen_x + 4);
					draw_rect.top = 24 + 36 * (current_terrain->info_rect[i].top - cen_y + 4);
					draw_rect.bottom = 24 + 36 * (current_terrain->info_rect[i].bottom - cen_y + 4);
					draw_rect.inset(-10, -13);
					frame_rect(ter_draw_gworld, draw_rect, sf::Color::Red);
				}
		}
		clip_rect(ter_draw_gworld, terrain_rect);
		
		small_any_drawn = false;
		//if(cur_viewing_mode == 0)
		//	draw_frames();
	}
	
	else {
		tileImage(ter_draw_gworld, terrain_rect,bg[17]);
		frame_rect(ter_draw_gworld, terrain_rect, sf::Color::Black);
		// Width available:  64 4x4 tiles, 42 6x6 tiles, or 21 12x12 tiles -- 256 pixels
		// Height available: 81 4x4 tiles, 54 6x6 tiles, or 27 12x12 tiles -- 324 pixels
		short size = mini_map_scales[cur_viewing_mode - 1];
		int max_dim = (editing_town ? town->max_dim() : 48);
		int xMin = 0, yMin = 0, xMax = max_dim, yMax = max_dim;
		if(cen_x + 5 > 256 / size) {
			xMin = cen_x + 5 - (256 / size);
			xMax = cen_x + 5;
		} else xMax = std::min(xMax, 256 / size);
		if(cen_y + 5 > 324 / size) {
			yMin = cen_y + 5 - (324 / size);
			yMax = cen_y + 5;
		} else yMax = std::min(yMax, 324 / size);
		std::cout << "Drawing map for x = " << xMin << "..." << xMax << " and y = " << yMin << "..." << yMax << std::endl;
		for(q = xMin; q < xMax; q++)
			for(r = yMin; r < yMax; r++) {
				if(q - xMin < 0 || q - xMin >= max_dim || r - yMin < 0 || r - yMin >= max_dim)
					t_to_draw = 90;
				else t_to_draw = editing_town ? town->terrain(q,r) : current_terrain->terrain[q][r];
				draw_one_tiny_terrain_spot(q-xMin,r-yMin,t_to_draw,size);
				small_what_drawn[q][r] = t_to_draw;
			}
		small_any_drawn = true;
	}
	ter_draw_gworld.display();
	
	rect_draw_some_item(ter_draw_gworld.getTexture(),terrain_rect,mainPtr,world_screen);
}

void draw_monsts() {
	short i,k,width,height,m_start_pic;
	sf::Texture* from_gworld;
	rectangle source_rect;
	location where_draw,store_loc;
	
	for(i = 0; i < town->creatures.size(); i++)
		if(town->creatures[i].number != 0) {
			where_draw.x = town->creatures[i].start_loc.x - cen_x + 4;
			where_draw.y = town->creatures[i].start_loc.y - cen_y + 4;
			width = scenario.scen_monsters[town->creatures[i].number].x_width;
			height = scenario.scen_monsters[town->creatures[i].number].y_width;
			
			for(k = 0; k < width * height; k++) {
				store_loc = where_draw;
				if((where_draw.x == minmax(0,8,where_draw.x)) &&
					(where_draw.y == minmax(0,8,where_draw.y)) &&
					(scenario.scen_monsters[town->creatures[i].number].picture_num >= 1000)) {
					graf_pos_ref(from_gworld, source_rect) = spec_scen_g.find_graphic((scenario.scen_monsters[town->creatures[i].number].picture_num + k) % 1000);
					store_loc.x += k % width;
					store_loc.y += k / width;
				}
				else if(scenario.scen_monsters[town->creatures[i].number].picture_num < 1000) {
					m_start_pic = m_pic_index[scenario.scen_monsters[town->creatures[i].number].picture_num].i + k;
					from_gworld = &monst_gworld[m_start_pic / 20];
					m_start_pic = m_start_pic % 20;
					source_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
					store_loc.x += k % width;
					store_loc.y += k / width;
				}
				
				if(store_loc.x < 0 || store_loc.x > 8 || store_loc.y < 0 || store_loc.y > 8)
					continue;
				
				rectangle destrec;
				destrec.left = 8 + BITMAP_WIDTH * store_loc.x;
				destrec.right = destrec.left + BITMAP_WIDTH;
				destrec.top = 8 + BITMAP_HEIGHT * store_loc.y;
				destrec.bottom = destrec.top + BITMAP_HEIGHT;
				
				destrec.left = destrec.right - (source_rect.right - source_rect.left);
				destrec.top = destrec.bottom - (source_rect.bottom - source_rect.top);
				
				rect_draw_some_item(*from_gworld, source_rect, ter_draw_gworld, destrec, sf::BlendAlpha);
			}
		}
}

// Returns rect for drawing an item, if num < 25, rect is in big item template,
// otherwise in small item template
// TODO: I have another function that does the same thing but also returns the texture containing the item along with the rectangle
rectangle get_item_template_rect (short type_wanted) {
	rectangle store_rect;
	
	if(type_wanted < 45) {
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
	rectangle source_rect,dest_rect;
	location where_draw;
	short pic_num;
	
	for(i = 0; i < town->preset_items.size(); i++) {
		if(town->preset_items[i].code >= 0) {
			where_draw.x = town->preset_items[i].loc.x - cen_x + 4;
			where_draw.y = town->preset_items[i].loc.y - cen_y + 4;
			pic_num = scenario.scen_items[town->preset_items[i].code].graphic_num;
			if((where_draw.x >= 0) && (where_draw.x <= 8) &&
				(where_draw.y >= 0) && (where_draw.y <= 8))  {
				
				if(pic_num >= 1000) {
					sf::Texture* source_gworld;
					graf_pos_ref(source_gworld, source_rect) = spec_scen_g.find_graphic(pic_num - 1000);
					dest_rect = calc_rect(where_draw.x,where_draw.y);
					dest_rect.offset(8,8);
					rect_draw_some_item(*source_gworld, source_rect, ter_draw_gworld, dest_rect, sf::BlendAlpha);
				}
				else {
					source_rect = get_item_template_rect(pic_num);
					dest_rect = calc_rect(where_draw.x,where_draw.y);
					dest_rect.offset(8,8);
					if(pic_num >= 45) {
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
//	for(q = 0; q < 8; q++)
//		for(r = 0; r < 64; r++)
//			ter_changed[q][r] = 255;
	
}

void draw_one_terrain_spot (short i,short j,ter_num_t terrain_to_draw) {
	location where_draw;
	rectangle source_rect;
	short picture_wanted;
	sf::Texture* source_gworld;
	
	if(i < 0 || i > 8 || j < 0 || j > 8)
		return;
	
	picture_wanted = scenario.ter_types[terrain_to_draw].picture;
	
	where_draw.x = (char) i;
	where_draw.y = (char) j;
	
	if(picture_wanted >= 1000 && spec_scen_g) {
		graf_pos_ref(source_gworld, source_rect) = spec_scen_g.find_graphic(picture_wanted % 1000);
	}
	else if(picture_wanted >= 960)	{
		source_gworld = &anim_gworld;
		picture_wanted -= 960;
		source_rect.left = 112 * (picture_wanted / 5);
		source_rect.right = source_rect.left + 28;
		source_rect.top = 36 * (picture_wanted % 5);
		source_rect.bottom = source_rect.top + 36;
	}
	else {
		source_rect = get_template_rect(terrain_to_draw);
		source_gworld = &terrain_gworld[picture_wanted / 50];
	}
	
	rectangle destrec;
	destrec.left = 8 + BITMAP_WIDTH * where_draw.x;
	destrec.right = destrec.left + BITMAP_WIDTH;
	destrec.top = 8 + BITMAP_HEIGHT * where_draw.y;
	destrec.bottom = destrec.top + BITMAP_HEIGHT;
	
	rect_draw_some_item(*source_gworld, source_rect, ter_draw_gworld, destrec);
}

void draw_one_tiny_terrain_spot (short i,short j,ter_num_t terrain_to_draw,short size) {
	rectangle dest_rect = {0,0,size,size},from_rect = {0,0,12,12};
	short picture_wanted;
	bool drawLargeIcon = false;
	sf::Texture* source_gworld;
	
	picture_wanted = scenario.ter_types[terrain_to_draw].map_pic;
	if(picture_wanted == NO_PIC) {
		drawLargeIcon = true;
		picture_wanted = scenario.ter_types[terrain_to_draw].picture;
	}
	
	dest_rect.offset(8 + size * i,8 + size * j);
	if(drawLargeIcon) {
		if(picture_wanted >= 1000)	{
			graf_pos_ref(source_gworld, from_rect) = spec_scen_g.find_graphic(picture_wanted % 1000);
		} else if(picture_wanted >= 960) {
			source_gworld = &anim_gworld;
			from_rect = calc_rect(4 * ((picture_wanted - 960) / 5),(picture_wanted - 960) % 5);
		} else {
			source_gworld = &terrain_gworld[picture_wanted / 50];
			picture_wanted %= 50;
			from_rect = calc_rect(picture_wanted % 10, picture_wanted / 10);
		}
		rect_draw_some_item(*source_gworld, from_rect, ter_draw_gworld, dest_rect);
	} else switch(picture_wanted) {
			
		case 0: case 1: case 73: case 72:
			tileImage(ter_draw_gworld, dest_rect,map_pat[0]);
			break;
		case 2: case 3: case 4:
			tileImage(ter_draw_gworld, dest_rect,map_pat[1]);
			break;
			
		default:
			if((picture_wanted < 170) && (map_pats[picture_wanted] > 0)) {
				tileImage(ter_draw_gworld, dest_rect,map_pat[map_pats[picture_wanted]]);
			}
			else if(picture_wanted >= 1000)	{
				sf::Texture* from_gw;
				graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(picture_wanted % 1000);
				from_rect.right = from_rect.left + 12;
				from_rect.bottom = from_rect.top + 12;
				picture_wanted /= 1000; picture_wanted--;
				from_rect.offset((picture_wanted / 3) * 12, (picture_wanted % 3) * 12);
				rect_draw_some_item(*from_gw, from_rect, ter_draw_gworld, dest_rect);
			}
			else if(picture_wanted >= 960)	{
				picture_wanted -= 960;
				if(picture_wanted == 0) tileImage(ter_draw_gworld, dest_rect,map_pat[13]);
				else if(picture_wanted == 4) tileImage(ter_draw_gworld, dest_rect,map_pat[21]);
				else if(picture_wanted == 7) tileImage(ter_draw_gworld, dest_rect,map_pat[20]);
				else if(picture_wanted == 8) tileImage(ter_draw_gworld, dest_rect,map_pat[19]);
				else if(picture_wanted == 9) tileImage(ter_draw_gworld, dest_rect,map_pat[20]);
				else if(picture_wanted == 10) tileImage(ter_draw_gworld, dest_rect,map_pat[19]);
				else {
					from_rect.offset(12 * 20, (picture_wanted - 960) * 12);
					rect_draw_some_item(small_ter_gworld, from_rect, ter_draw_gworld, dest_rect);
				}
			}
			else {
				from_rect.offset((picture_wanted % 20) * 12,(picture_wanted / 20) * 12);
				rect_draw_some_item(small_ter_gworld, from_rect, ter_draw_gworld, dest_rect);
			}
			break;
	}
	if(mouse_spot.x >= 0 && mouse_spot.y >= 0) {
		location where_draw(i,j);
		bool need_hilite = false, large_hilite = false;
		int d = dist(where_draw, mouse_spot);
		if(overall_mode == MODE_SMALL_PAINTBRUSH && d <= 1) {
			need_hilite = true;
			large_hilite = true;
		} else if((overall_mode == MODE_ERASER || overall_mode == MODE_SMALL_SPRAYCAN) && d <= 2) {
			need_hilite = true;
			large_hilite = true;
		} else if((overall_mode == MODE_LARGE_PAINTBRUSH || overall_mode == MODE_LARGE_SPRAYCAN) && d <= 4) {
			need_hilite = true;
			large_hilite = true;
		} else if(where_draw == mouse_spot)
			need_hilite = true;
		if(need_hilite) {
			fill_rect(ter_draw_gworld, dest_rect, hilite_colour);
			if(large_hilite && where_draw == mouse_spot)
				fill_rect(ter_draw_gworld, dest_rect, hilite_colour);
		}
	}
}

/* Input terrain currently trying to draw. Get back rectangle in terrain template containing
 desired pixmap, or rectangle to darkness if desired map not present */
rectangle get_template_rect (unsigned short type_wanted) {
	rectangle store_rect;
	short picture_wanted;
	
	picture_wanted = scenario.ter_types[type_wanted].picture;
	if(picture_wanted >= 1000)
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
	rectangle draw_rect;
	
	for(q = 0; q < 9; q++) {
		for(r = 0; r < 9; r++) {
			which_pt.x = cen_x + q - 4;
			which_pt.y = cen_y + r - 4;
			draw_rect.top = 23 + r * 36;
			draw_rect.bottom = 58 + r * 36;
			draw_rect.left = 23 + q * 28;
			draw_rect.right = 50 + q * 28;
			for(i = 0; i < 4; i++)
				if((which_pt.x == town->wandering_locs[i].x) &&
					(which_pt.y == town->wandering_locs[i].y)) {
					
					frame_rect(ter_draw_gworld, draw_rect, sf::Color::Red);
				}
			for(i = 0; i < 4; i++)
				if((which_pt.x == town->start_locs[i].x) &&
					(which_pt.y == town->start_locs[i].y)) {
					frame_rect(ter_draw_gworld, draw_rect, sf::Color::Magenta);
				}
			
			
		}
	}
	
}

static void place_selected_terrain(ter_num_t ter, rectangle draw_rect) {
	pic_num_t picture_wanted = scenario.ter_types[ter].picture;
	rectangle source_rect;
	if(picture_wanted >= 1000)	{
		sf::Texture* source_gworld;
		graf_pos_ref(source_gworld, source_rect) = spec_scen_g.find_graphic(picture_wanted % 1000);
		rect_draw_some_item(*source_gworld,
							source_rect,terrain_buttons_gworld,draw_rect);
	}
	else if(picture_wanted >= 960)	{
		picture_wanted -= 960;
		source_rect.left = 112 * (picture_wanted / 5);
		source_rect.right = source_rect.left + 28;
		source_rect.top = 36 * (picture_wanted % 5);
		source_rect.bottom = source_rect.top + 36;
		rect_draw_some_item(anim_gworld,source_rect,terrain_buttons_gworld,draw_rect);
	}
	else {
		source_rect = get_template_rect(ter);
		rect_draw_some_item(terrain_gworld[picture_wanted / 50],source_rect,
							terrain_buttons_gworld,draw_rect);
	}
	short small_i = get_small_icon(ter);
	rectangle tiny_to = draw_rect;
	tiny_to.top = tiny_to.bottom - 7;
	tiny_to.left = tiny_to.right - 7;
	rectangle tiny_from = base_small_button_from;
	tiny_from.offset(7 * (small_i % 30),7 * (small_i / 30));
	if(small_i >= 0 && small_i < 255)
		rect_draw_some_item(editor_mixed,tiny_from,terrain_buttons_gworld,tiny_to);
}

void place_location() {
	std::ostringstream sout;
	rectangle draw_rect,source_rect,erase_rect;
	rectangle text_rect = {0,0,12,100};
	short picture_wanted;
	
	erase_rect.left = 2;
	erase_rect.right = RIGHT_AREA_WIDTH - 1;
	erase_rect.top = terrain_rects[255].top + 26 - 9;
	erase_rect.bottom = erase_rect.top + 12;
	tileImage(terrain_buttons_gworld, erase_rect,bg[17]);
	
	location moveTo(5, terrain_rects[255].top + 18);
	draw_rect = text_rect;
	draw_rect.offset(moveTo);
	if(overall_mode < MODE_MAIN_SCREEN)
		sout << "Center: x = " << cen_x << ", y = " << cen_y;
	else {
		moveTo.y += 13; // TODO: Not sure how important this is.
		sout << "Click terrain to edit. ";
	}
	TextStyle style;
	style.lineHeight = 12;
	win_draw_string(terrain_buttons_gworld, draw_rect, sout.str(), eTextMode::LEFT_TOP, style);
	sout.str("");
	
	moveTo = location(260 ,terrain_rects[255].top + 18);
	draw_rect = text_rect;
	draw_rect.offset(moveTo);
	sout << current_terrain_type;
	win_draw_string(terrain_buttons_gworld, draw_rect, sout.str(), eTextMode::LEFT_TOP, style);
	sout.str("");
	
	erase_rect.left = 2;
	erase_rect.right = RIGHT_AREA_WIDTH - 1;
	erase_rect.top = terrain_rects[255].bottom + 117;
	erase_rect.bottom = RIGHT_AREA_HEIGHT + 6;
	tileImage(terrain_buttons_gworld, erase_rect,bg[17]);
	
	if(overall_mode < MODE_MAIN_SCREEN) {
		moveTo = location(5,terrain_rects[255].bottom + 121);
		draw_rect = text_rect;
		draw_rect.offset(moveTo);
		win_draw_string(terrain_buttons_gworld, draw_rect, current_string[0], eTextMode::LEFT_TOP, style);
		moveTo = location(RIGHT_AREA_WIDTH / 2,terrain_rects[255].bottom + 121);
		draw_rect = text_rect;
		draw_rect.offset(moveTo);
		win_draw_string(terrain_buttons_gworld, draw_rect, current_string[1], eTextMode::LEFT_TOP, style);
	}
	
	draw_rect.top = palette_buttons[0][0].top + terrain_rects[255].bottom + 5;
	draw_rect.left = palette_buttons[9][0].right + 10; // + 17;
	draw_rect.bottom = draw_rect.top + 36;
	draw_rect.right = draw_rect.left + 28;
	
	if(overall_mode < MODE_MAIN_SCREEN) {
		place_selected_terrain(current_terrain_type, draw_rect);
		if(overall_mode == MODE_PLACE_CREATURE || overall_mode == MODE_PLACE_SAME_CREATURE) {
			rectangle to_rect = draw_rect;
			extern short mode_count;
			picture_wanted = scenario.scen_monsters[mode_count].picture_num;
			if(picture_wanted >= 4000) {
				picture_wanted %= 1000;
				to_rect.width() = to_rect.width() / 2;
				to_rect.height() = to_rect.height() / 2;
				sf::Texture* source_gworld;
				graf_pos_ref(source_gworld, source_rect) = spec_scen_g.find_graphic(picture_wanted);
				rect_draw_some_item(*source_gworld, source_rect, terrain_buttons_gworld, to_rect, sf::BlendAlpha);
				picture_wanted++;
				to_rect.offset(to_rect.width(), 0);
				graf_pos_ref(source_gworld, source_rect) = spec_scen_g.find_graphic(picture_wanted);
				rect_draw_some_item(*source_gworld, source_rect, terrain_buttons_gworld, to_rect, sf::BlendAlpha);
				picture_wanted++;
				to_rect.offset(-to_rect.width(), to_rect.height());
				graf_pos_ref(source_gworld, source_rect) = spec_scen_g.find_graphic(picture_wanted);
				rect_draw_some_item(*source_gworld, source_rect, terrain_buttons_gworld, to_rect, sf::BlendAlpha);
				picture_wanted++;
				to_rect.offset(to_rect.width(), 0);
				graf_pos_ref(source_gworld, source_rect) = spec_scen_g.find_graphic(picture_wanted);
				rect_draw_some_item(*source_gworld, source_rect, terrain_buttons_gworld, to_rect, sf::BlendAlpha);
			} else if(picture_wanted >= 3000) {
				picture_wanted %= 1000;
				to_rect.width() = to_rect.width() / 2;
				to_rect.height() = to_rect.height() / 2;
				to_rect.offset(to_rect.width() / 2, 0);
				sf::Texture* source_gworld;
				graf_pos_ref(source_gworld, source_rect) = spec_scen_g.find_graphic(picture_wanted);
				rect_draw_some_item(*source_gworld, source_rect, terrain_buttons_gworld, to_rect, sf::BlendAlpha);
				picture_wanted++;
				to_rect.offset(0, to_rect.height());
				graf_pos_ref(source_gworld, source_rect) = spec_scen_g.find_graphic(picture_wanted);
				rect_draw_some_item(*source_gworld, source_rect, terrain_buttons_gworld, to_rect, sf::BlendAlpha);
			} else if(picture_wanted >= 2000) {
				picture_wanted %= 1000;
				to_rect.width() = to_rect.width() / 2;
				to_rect.height() = to_rect.height() / 2;
				to_rect.offset(0, to_rect.height() / 2);
				sf::Texture* source_gworld;
				graf_pos_ref(source_gworld, source_rect) = spec_scen_g.find_graphic(picture_wanted);
				rect_draw_some_item(*source_gworld, source_rect, terrain_buttons_gworld, to_rect, sf::BlendAlpha);
				picture_wanted++;
				to_rect.offset(to_rect.width(), 0);
				graf_pos_ref(source_gworld, source_rect) = spec_scen_g.find_graphic(picture_wanted);
				rect_draw_some_item(*source_gworld, source_rect, terrain_buttons_gworld, to_rect, sf::BlendAlpha);
			} else if(picture_wanted >= 1000) {
				picture_wanted %= 1000;
				sf::Texture* source_gworld;
				graf_pos_ref(source_gworld, source_rect) = spec_scen_g.find_graphic(picture_wanted);
				rect_draw_some_item(*source_gworld, source_rect, terrain_buttons_gworld, to_rect, sf::BlendAlpha);
			} else {
				auto pic_info = m_pic_index[picture_wanted];
				picture_wanted = pic_info.i;
				if(pic_info.x == 2 && pic_info.y == 2) {
					to_rect.width() = to_rect.width() / 2;
					to_rect.height() = to_rect.height() / 2;
					source_rect = calc_rect(2 * ((picture_wanted % 20) / 10), (picture_wanted % 20) % 10);
					rect_draw_some_item(monst_gworld[picture_wanted / 20], source_rect, terrain_buttons_gworld, to_rect, sf::BlendAlpha);
					picture_wanted++;
					to_rect.offset(to_rect.width(), 0);
					source_rect = calc_rect(2 * ((picture_wanted % 20) / 10), (picture_wanted % 20) % 10);
					rect_draw_some_item(monst_gworld[picture_wanted / 20], source_rect, terrain_buttons_gworld, to_rect, sf::BlendAlpha);
					picture_wanted++;
					to_rect.offset(-to_rect.width(), to_rect.height());
					source_rect = calc_rect(2 * ((picture_wanted % 20) / 10), (picture_wanted % 20) % 10);
					rect_draw_some_item(monst_gworld[picture_wanted / 20], source_rect, terrain_buttons_gworld, to_rect, sf::BlendAlpha);
					picture_wanted++;
					to_rect.offset(to_rect.width(), 0);
					source_rect = calc_rect(2 * ((picture_wanted % 20) / 10), (picture_wanted % 20) % 10);
					rect_draw_some_item(monst_gworld[picture_wanted / 20], source_rect, terrain_buttons_gworld, to_rect, sf::BlendAlpha);
				} else if(pic_info.y == 2) {
					to_rect.width() = to_rect.width() / 2;
					to_rect.height() = to_rect.height() / 2;
					to_rect.offset(to_rect.width() / 2, 0);
					source_rect = calc_rect(2 * ((picture_wanted % 20) / 10), (picture_wanted % 20) % 10);
					rect_draw_some_item(monst_gworld[picture_wanted / 20], source_rect, terrain_buttons_gworld, to_rect, sf::BlendAlpha);
					picture_wanted++;
					to_rect.offset(0, to_rect.height());
					source_rect = calc_rect(2 * ((picture_wanted % 20) / 10), (picture_wanted % 20) % 10);
					rect_draw_some_item(monst_gworld[picture_wanted / 20], source_rect, terrain_buttons_gworld, to_rect, sf::BlendAlpha);
				} else if(pic_info.x == 2) {
					to_rect.width() = to_rect.width() / 2;
					to_rect.height() = to_rect.height() / 2;
					to_rect.offset(0, to_rect.height() / 2);
					source_rect = calc_rect(2 * ((picture_wanted % 20) / 10), (picture_wanted % 20) % 10);
					rect_draw_some_item(monst_gworld[picture_wanted / 20], source_rect, terrain_buttons_gworld, to_rect, sf::BlendAlpha);
					picture_wanted++;
					to_rect.offset(to_rect.width(), 0);
					source_rect = calc_rect(2 * ((picture_wanted % 20) / 10), (picture_wanted % 20) % 10);
					rect_draw_some_item(monst_gworld[picture_wanted / 20], source_rect, terrain_buttons_gworld, to_rect, sf::BlendAlpha);
				} else {
					source_rect = calc_rect(2 * ((picture_wanted % 20) / 10), (picture_wanted % 20) % 10);
					rect_draw_some_item(monst_gworld[picture_wanted / 20], source_rect, terrain_buttons_gworld, to_rect, sf::BlendAlpha);
				}
			}
		} else if(overall_mode == MODE_PLACE_ITEM || overall_mode == MODE_PLACE_SAME_ITEM) {
			extern short mode_count;
			picture_wanted = scenario.scen_items[mode_count].graphic_num;
			if(picture_wanted >= 1000) {
				sf::Texture* source_gworld;
				graf_pos_ref(source_gworld, source_rect) = spec_scen_g.find_graphic(picture_wanted % 1000);
				rect_draw_some_item(*source_gworld,source_rect,terrain_buttons_gworld,draw_rect,sf::BlendAlpha);
			} else if(picture_wanted < 50) {
				source_rect = calc_rect(picture_wanted % 5,picture_wanted / 5);
				rect_draw_some_item(items_gworld,source_rect,terrain_buttons_gworld,draw_rect,sf::BlendAlpha);
			} else {
				draw_rect.inset(5, 9);
				rectangle tiny_from = {0,0,18,18};
				tiny_from.offset((picture_wanted % 10) * 18,(picture_wanted / 10) * 18);
				rect_draw_some_item(tiny_obj_gworld,tiny_from,terrain_buttons_gworld,draw_rect,sf::BlendAlpha);
			}
		}
		draw_rect.offset(0,40);
		place_selected_terrain(current_ground, draw_rect);
	}
	
	terrain_buttons_gworld.display();
	draw_rect = terrain_buttons_rect;
	draw_rect.offset(RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
	rect_draw_some_item(terrain_buttons_gworld.getTexture(),terrain_buttons_rect,mainPtr,draw_rect);
}

void set_string(std::string string,std::string string2) {
	current_string[0] = string;
	current_string[1] = string2;
}

/*
void draw_cur_string() {
	rectangle from_rect,draw_rect;
	
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
	location check(i,j);
	
	if(editing_town)
		for(k = 0; k < town->special_locs.size(); k++)
			if(town->special_locs[k] == check && town->special_locs[k].spec >= 0)
				return true;
	if(!editing_town)
		for(k = 0; k < current_terrain->special_locs.size(); k++)
			if(current_terrain->special_locs[k] == check && current_terrain->special_locs[k].spec >= 0)
				return true;
	
	return false;
}

void sort_specials() {
}

bool is_spot(short i,short j){
	if(editing_town)
		return is_field_type(i,j,SPECIAL_SPOT);
	else if(i >= 0 && i < 48 && j >= 0 && j < 48)
		return current_terrain->special_spot[i][j];
	return false;
}

bool is_field_type(short i,short j,eFieldType field_type) {
	unsigned short k;
	
	for(k = 0; k < town->preset_fields.size(); k++)
		if((town->preset_fields[k].type == field_type) &&
			(town->preset_fields[k].loc.x == i) &&
			(town->preset_fields[k].loc.y == j))
			return true;
	return false;
}

void make_field_type(short i,short j,eFieldType field_type) {
	unsigned short k;
	
	if(is_field_type(i,j,field_type))
		return;
	for(k = 0; k < town->preset_fields.size(); k++)
		if(town->preset_fields[k].type == 0) {
			town->preset_fields[k].loc.x = i;
			town->preset_fields[k].loc.y = j;
			town->preset_fields[k].type = field_type;
			return;
		}
	cTown::cField the_field;
	the_field.loc.x = i;
	the_field.loc.y = j;
	the_field.type = field_type;
	town->preset_fields.push_back(the_field);
}


void take_field_type(short i,short j,eFieldType field_type) {
	unsigned short k;
	
	for(k = 0; k < town->preset_fields.size(); k++)
		if((town->preset_fields[k].type == field_type) &&
			(town->preset_fields[k].loc.x == i) &&
			(town->preset_fields[k].loc.y == j)) {
			town->preset_fields[k].type = FIELD_DISPEL;
			return;
		}
}

bool container_there(location l) {
	if(!editing_town)
		return false;
	if(scenario.ter_types[town->terrain(l.x,l.y)].special == eTerSpec::IS_A_CONTAINER)
		return true;
	if(is_field_type(l.x,l.y, OBJECT_BARREL))
		return true;
	if(is_field_type(l.x,l.y, OBJECT_CRATE))
		return true;
	return 0;
}

void record_display_strings(){}
