
#include <cstring>
#include <cstdio>
#include <list>
#include <unordered_map>
#include <memory>

#include "boe.global.hpp"

#include "universe/universe.hpp"
#include "boe.graphics.hpp"
#include "boe.newgraph.hpp"
#include "boe.graphutil.hpp"
#include "boe.monster.hpp"
#include "boe.locutils.hpp"
#include "boe.text.hpp"
#include "gfx/render_image.hpp"
#include "gfx/render_shapes.hpp"
#include "gfx/render_text.hpp"
#include "gfx/tiling.hpp"
#include "sounds.hpp"
#include "mathutil.hpp"
#include "dialogxml/widgets/button.hpp"
#include "tools/enum_map.hpp"
#include "tools/drawable_manager.hpp"

#include "boe.party.hpp"
#include "boe.town.hpp"
#include "boe.items.hpp"
#include "boe.dlgutil.hpp"
#include "boe.infodlg.hpp"
#include "boe.ui.hpp"

#include "dialogxml/widgets/scrollbar.hpp"

#include "fileio/resmgr/res_image.hpp"
#include "boe.menus.hpp"
#include "tools/winutil.hpp"
#include "tools/prefs.hpp"
#include "replay.hpp"
#include <boost/lexical_cast.hpp>


#ifndef MSBUILD_GITREV
#include "tools/gitrev.hpp"
#endif

extern sf::RenderWindow mainPtr;
extern eItemWinMode stat_window;
extern eGameMode overall_mode;
extern short current_spell_range;
extern bool party_in_memory;
extern bool flushingInput;
extern bool cartoon_happening, fog_lifted;
extern short anim_step;
extern effect_pat_type current_pat;
extern location center;
extern short which_combat_type;
extern bool monsters_going,boom_anim_active;
extern sf::Image spell_pict;
extern short current_ground;
extern short num_targets_left;
extern location spell_targets[8];
extern std::shared_ptr<cScrollbar> text_sbar,item_sbar,shop_sbar;
extern std::shared_ptr<cButton> done_btn, help_btn;
extern sf::Texture bg_gworld;
extern const rectangle sbar_rect,item_sbar_rect,shop_sbar_rect;
extern rectangle startup_top;
extern rectangle talk_area_rect, word_place_rect;
extern location store_anim_ul;
extern long register_flag;
extern long ed_flag,ed_key;
extern short fast_bang;
extern tessel_ref_t bg[];
extern cUniverse univ;
extern cCustomGraphics spec_scen_g;
extern sf::RenderWindow mini_map;
bool map_visible = false;
extern std::string save_talk_str1, save_talk_str2;
extern cDrawableManager drawable_mgr;
extern void close_map(bool record = false);

rectangle		menuBarRect;
Region originalGrayRgn, newGrayRgn, underBarRgn;
sf::View mainView;

long anim_ticks = 0;

extern enum_map(eGuiArea, rectangle) win_to_rects;

// 0 - title  1 - button  2 - credits  3 - base button
rectangle startup_from[4] = {{0,0,274,602},{274,0,322,301},{0,301,67,579},{274,301,314,341}};
extern enum_map(eStartButton, rectangle) startup_button;

rectangle	top_left_rec = {0,0,36,28};
short which_graphic_index[6] = {50,50,50,50,50,50};

char combat_graphics[5] = {28,29,36,79,2};
short debug_nums[6] = {0,0,0,0,0,0};

char light_area[13][13];
char unexplored_area[13][13];

// Declare the graphics
sf::RenderTexture pc_stats_gworld;
sf::RenderTexture item_stats_gworld;
sf::RenderTexture text_area_gworld;
sf::RenderTexture terrain_screen_gworld;
sf::RenderTexture text_bar_gworld;
sf::RenderTexture map_gworld;

bool has_run_anim = false,currently_loading_graphics = false;

rectangle main_win_rect = {0,0,410,250};
rectangle main_win2_source_rect = {0,0,410,265};
rectangle main_win2_rect = {0,250,410,515};

rectangle tiny_world_1_source_rect = {0,0,190,145},
tiny_world_1_rect = {195,242,385,475};

rectangle share_mess_source_rect = {0,0,59,120},
share_mess_rect = {120,384,179,504};
rectangle start_buttons_source_rect = {0,0,186,190},
start_buttons_rect = {214,30,400,220};

// Array to store which spots have been seen. Time-saver for drawing fields
char spot_seen[9][9];

char anim_str[60];
location anim_str_loc;

extern tessel_ref_t bw_pats[6];

extern short combat_posing_monster , current_working_monster ; // 0-5 PC 100 + x - monster x
bool supressing_some_spaces = false;
location ok_space[4] = {loc(),loc(),loc(),loc()};
sf::Image hold_pict;

void adjust_window_mode() {
	sf::ContextSettings winSettings;
	winSettings.stencilBits = 1;
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	hideMenuBar();
	double ui_scale = get_ui_scale();
	if(ui_scale < 0.1) ui_scale = 1.0;
	float width = boe_width * ui_scale, height = boe_height * ui_scale;
	
	// TODO: Make display_mode an enum
	// 0 - center 1- ul 2 - ur 3 - dl 4 - dr 5 - small win
	int mode = get_int_pref("DisplayMode");
	if(mode == 5) {
		// Increase window height to make room for the menubar on Linux
		int winHeight = height;
		winHeight += os_specific_y_offset();

		mainPtr.create(sf::VideoMode(width, winHeight, 32), "Blades of Exile", sf::Style::Titlebar | sf::Style::Close, winSettings);

		// Center the small window on the desktop
		mainPtr.setPosition({static_cast<int>((desktop.width - width) / 2), static_cast<int>((desktop.height - height) / 2)});
	} else {
		mainPtr.create(desktop, "Blades of Exile", sf::Style::None, winSettings);
		mainPtr.setPosition({0,0});
	}

	// Initialize the view
	mainView.setSize(width, height);
	mainView.setCenter(width / 2, height / 2);

	sf::FloatRect mainPort = compute_viewport(mainPtr, mode, ui_scale, width, height);
	mainView.setViewport(mainPort);
	
#ifndef __APPLE__ // This overrides Dock icon on OSX, which isn't what we want at all
	const ImageRsrc& icon = ResMgr::graphics.get("icon", true);
	mainPtr.setIcon(icon->getSize().x, icon->getSize().y, icon->copyToImage().getPixelsPtr());
#endif

#ifdef SFML_SYSTEM_WINDOWS
	// On windows, the file dialogs are constructed with mainPtr as a parent,
	// so they need to be reconstructed after mainPtr is.
	init_fileio();
#endif
	init_menubar();
	adjust_window_for_menubar(mode, width, height);
	showMenuBar();
}

// Viewport is applied to the View and designates where in the OS window the source of the View is drawn.
sf::FloatRect compute_viewport(const sf::RenderWindow& mainPtr, int mode, float ui_scale, float width, float height) {

	sf::FloatRect viewport;

	// Dimensions of the OS window.
	rectangle windRect { mainPtr };

	// Width and height: how large the viewport is. They seem to be calculated
	// in terms of *source* dimensions, with values above 1 resulting in an upscale.
	viewport.width  = ui_scale * width / windRect.width();
	viewport.height = ui_scale * height / windRect.height();

	// Buffer in pixels between ui edge and window edge. There seem to be
	// implicit (builtin) buffers the top and bottom of the UI so this is just for the sides.
	const int extra_horizontal_buffer { 7 };

	// Left and top: where the viewport is.
	// Left and top seem to be in terms *target* dimensions,
	// so top = 0.5 with window height 450 means 225 px offset from the top.
	if(mode == 0) {
		// Fullscreen centered
		viewport.left = float((windRect.width() - width) / 2) / windRect.width();
		viewport.top  = float((windRect.height() - height - os_specific_y_offset()) / 2)/ windRect.height();
	} else if(mode == 1) {
		// Fullscreen top left
		viewport.left = float(extra_horizontal_buffer) / windRect.width();
		viewport.top  = float(os_specific_y_offset()) / windRect.height();
	} else if(mode == 2) {
		// Fullscreen top right
		viewport.left = float(windRect.right - width - extra_horizontal_buffer) / windRect.width();
		viewport.top  = float(os_specific_y_offset()) / windRect.height();
	} else if(mode == 3) {
		// Fullscreen bottom left
		viewport.left = float(extra_horizontal_buffer) / windRect.width();
		// DIRTY HACK: windRect in fullscreen modes gives us the entire display, but at the same time
		// there could be a windows taskbar / mac os dock / xfce taskbar / etc that consumes a part
		// of that display, and that we do not know size of. So we need to account for that somehow,
		// so we add 28 more pixels (this was the amount in the previous version of this code).
		viewport.top  = float(windRect.bottom - height - os_specific_y_offset() - 28) / windRect.height();
	} else if(mode == 4) {
		// Fullscreen bottom right
		viewport.left = float(windRect.right - width - extra_horizontal_buffer) / windRect.width();
		// DIRTY HACK: same as for mode 3
		viewport.top  = float(windRect.bottom - height - os_specific_y_offset() - 28) / windRect.height();
	} else if(mode == 5) {
		// Small windowed
		viewport.left = 0;
		viewport.top  = float(os_specific_y_offset()) / windRect.height();
	}

	return viewport;
}

void init_startup() {
	// Preload the main startup images
	ResMgr::graphics.get("startup", true);
	ResMgr::graphics.get("startbut", true);
	ResMgr::graphics.get("startanim", true);
}

void draw_startup(short but_type) {
	sf::Texture& startup_gworld = *ResMgr::graphics.get("startup", true);
	rect_draw_some_item(startup_gworld,startup_from[0],mainPtr,startup_top);
	
	for(auto btn : startup_button.keys()) {
		rect_draw_some_item(startup_gworld,startup_from[1],mainPtr,startup_button[btn]);
		draw_start_button(btn,but_type);
	}
	draw_startup_anim(false);
	
	draw_startup_stats();
}

void draw_startup_anim(bool advance) {
	static short startup_anim_pos = 43; // was a global variable, but since it's only used in this function I moved it here
	rectangle anim_to = {4,1,44,276},anim_from;
	rectangle anim_size = {0,0,48,301};
	
	anim_from = anim_to;
	anim_from.offset(-1,-4 + startup_anim_pos);
	auto scroll_sprite = *ResMgr::graphics.get("startanim",true);
	if(advance) startup_anim_pos = (startup_anim_pos + 1) % scroll_sprite.getSize().y;
	rect_draw_some_item(*ResMgr::graphics.get("startbut",true),anim_size,mainPtr,startup_button[STARTBTN_SCROLL]);
	anim_to.offset(startup_button[STARTBTN_SCROLL].left, startup_button[STARTBTN_SCROLL].top);
	rect_draw_some_item(scroll_sprite,anim_from,mainPtr,anim_to,sf::BlendAlpha);
}

void draw_startup_stats() {
	rectangle from_rect,to_rect,party_to = {0,0,36,28},pc_rect,frame_rect;
	
	TextStyle style;
	style.font = FONT_DUNGEON;
	style.pointSize = 24;
	
	to_rect = startup_top;
	to_rect.offset(20 - 18, 35);
	style.colour = sf::Color::White;
	style.lineHeight = 18;
	
	if(!party_in_memory) {
		style.pointSize = 20;
		to_rect.offset(193,40);
		win_draw_string(mainPtr,to_rect,"No Party in Memory",eTextMode::WRAP,style);
	} else {
		frame_rect = startup_top;
		frame_rect.inset(50,50);
		frame_rect.top += 30;
		frame_rect.left += 18;
		frame_rect.offset(-9,10);
		// TODO: Maybe I should rename that variable
		::frame_rect(mainPtr, frame_rect, sf::Color::White);

		to_rect.offset(221,37);
		win_draw_string(mainPtr,to_rect,"Your party:",eTextMode::WRAP,style);
		style.pointSize = 12;
		style.font = FONT_BOLD;
		for(short i = 0; i < 6; i++) {
			pc_rect = startup_top;
			pc_rect.left += 18;
			pc_rect.right = pc_rect.left + 300;
			pc_rect.bottom = pc_rect.top + 79;
			pc_rect.offset(60 + 232 * (i / 3) - 9,95 + 45 * (i % 3));

			if(univ.party[i].main_status != eMainStatus::ABSENT) {
				to_rect = party_to;
				to_rect.offset(pc_rect.left,pc_rect.top);
				pic_num_t pic = univ.party[i].which_graphic;
				if(pic >= 1000) {
					std::shared_ptr<const sf::Texture> gw;
					graf_pos_ref(gw, from_rect) = spec_scen_g.find_graphic(pic % 1000, pic >= 10000);
					rect_draw_some_item(*gw,from_rect,mainPtr,to_rect,sf::BlendAlpha);
				} else if(pic >= 100) {
					pic -= 100;
					// Note that we assume it's a 1x1 graphic.
					// PCs can't be larger than that, but we leave it to the scenario designer to avoid assigning larger graphics.
					from_rect = get_monster_template_rect(pic, 0, 0);
					int which_sheet = m_pic_index[pic].i / 20;
					sf::Texture& monst_gworld = *ResMgr::graphics.get("monst" + std::to_string(1 + which_sheet));
					rect_draw_some_item(monst_gworld,from_rect,mainPtr,to_rect,sf::BlendAlpha);
				} else {
					from_rect = calc_rect(2 * (pic / 8), pic % 8);
					sf::Texture& pc_gworld = *ResMgr::graphics.get("pcs");
					rect_draw_some_item(pc_gworld,from_rect,mainPtr,to_rect,sf::BlendAlpha);
				}
				
				style.pointSize = 14;
				pc_rect.offset(35,0);
				win_draw_string(mainPtr,pc_rect,univ.party[i].name,eTextMode::WRAP,style);
				to_rect.offset(pc_rect.left + 8,pc_rect.top + 8);
				
			}
			style.pointSize = 12;
			pc_rect.offset(12,16);
			std::string status = "Level " + std::to_string(univ.party[i].level);
			switch(univ.party[i].main_status) {
				case eMainStatus::ALIVE:
					switch(univ.party[i].race) {
						case eRace::HUMAN: status += " Human"; break;
						case eRace::NEPHIL: status += " Nephilim"; break;
						case eRace::SLITH: status += " Slithzerikai"; break;
						case eRace::VAHNATAI: status += " Vahnatai"; break;
						case eRace::REPTILE: status += " Reptile"; break;
						case eRace::BEAST: status += " Beast"; break;
						case eRace::IMPORTANT: status += " V.I.P."; break;
						case eRace::MAGE: status += " Human Mage"; break;
						case eRace::PRIEST: status += " Human Priest"; break;
						case eRace::HUMANOID: status += " Humanoid"; break;
						case eRace::DEMON: status += " Demon"; break;
						case eRace::UNDEAD: status += " Undead"; break;
						case eRace::GIANT: status += " Giant"; break;
						case eRace::SLIME: status += " Slime"; break;
						case eRace::STONE: status += " Golem"; break;
						case eRace::BUG: status += " Bug"; break;
						case eRace::DRAGON: status += " Dragon"; break;
						case eRace::MAGICAL: status += " Magical Creature"; break;
						case eRace::PLANT: status += " Plant"; break;
						case eRace::BIRD: status += " Bird"; break;
						default: status += " *ERROR INVALID RACE*"; break;
					}
					win_draw_string(mainPtr,pc_rect,status,eTextMode::WRAP,style);
					pc_rect.offset(0,13);
					status = "Health " + std::to_string(univ.party[i].max_health);
					status += ", Spell pts. " + std::to_string(univ.party[i].max_sp);
					break;
				case eMainStatus::DEAD:
					status = "Dead";
					break;
				case eMainStatus::DUST:
					status = "Dust";
					break;
				case eMainStatus::STONE:
					status = "Stone";
					break;
				case eMainStatus::FLED:
					status = "Fled";
					break;
				default: //absent, and all variations thereof; do nothing
					status.clear();
					break;
			}
			if(!status.empty())
				win_draw_string(mainPtr,pc_rect,status,eTextMode::WRAP,style);
		}
	}
	
	std::ostringstream sout;
	sout << "Created 1997, Free Open Source, v" << oboeVersionString();
#if defined(GIT_REVISION) && defined(GIT_TAG_REVISION)
	if(strcmp(GIT_REVISION, GIT_TAG_REVISION) != 0) {
		sout << " [" << GIT_REVISION << "]";
	}
#endif
	std::string copyright = sout.str();
	style.font = FONT_BOLD;
	style.pointSize = 10;
	pc_rect = startup_top;
	pc_rect.offset(5,5);
	pc_rect.top = pc_rect.bottom - 30;
	pc_rect.left = pc_rect.right - string_length(copyright, style) - 32;
	// TODO: Should replace this with a more appropriate copyright string
	// Windows replaced it with "That is not dead which can eternally lie..." - I don't think that's quite appropriate though.
	win_draw_string(mainPtr, pc_rect, copyright, eTextMode::WRAP, style);
}



void draw_start_button(eStartButton which_position,short which_button) {
	rectangle from_rect,to_rect;
	const char *button_labels[MAX_eStartButton];
	for(int i = 0; i < MAX_eStartButton; ++i){
		button_labels[i] = startup_button_names[i].c_str();
	}

	// The 0..65535 version of the blue component was 14472; the commented version was 43144431
	sf::Color base_color = {0,0,57};
	
	from_rect = startup_from[3];
	from_rect.offset((which_button > 0) ? 40 : 0,0);
	to_rect = startup_button[which_position];
	to_rect.left += 4; to_rect.top += 4;
	to_rect.right = to_rect.left + 40;
	to_rect.bottom = to_rect.top + 40;
	rect_draw_some_item(*ResMgr::graphics.get("startup",true),from_rect,mainPtr,to_rect);
	
	TextStyle style;
	style.font = FONT_DUNGEON;
	style.pointSize = 24;
	to_rect = startup_button[which_position];
	//to_rect.left += 80;
	to_rect.offset(10, 5);
	if(which_button == 5)
		which_button = 4;
	// In the 0..65535 range, this was 14472 + (12288 * which_button)
	base_color.b += (48 * which_button);
	style.colour = base_color;
	style.lineHeight = 18;
	win_draw_string(mainPtr,to_rect,button_labels[which_position],eTextMode::CENTRE,style);
}

void arrow_button_click(rectangle button_rect) {
	if(recording){
		// This action is purely cosmetic, for playing the animation and sound accompanying a click on a button whose real action
		// is recorded afterward
		record_action("arrow_button_click", boost::lexical_cast<std::string>(button_rect));
	}
	mainPtr.setActive();
	clip_rect(mainPtr, button_rect);
	// TODO: Mini-event loop so that the click doesn't happen until releasing the mouse button
	
	refresh_stat_areas(1);
	mainPtr.display();
	play_sound(37, time_in_ticks(5));
	undo_clip(mainPtr);
	refresh_stat_areas(0);
}


void reload_startup() {
	close_map();
	init_startup();
	
	text_sbar->hide();
	item_sbar->hide();
	shop_sbar->hide();
	done_btn->hide();
	help_btn->hide();
}

void end_startup() {
	load_main_screen();
	
	text_sbar->show();
	item_sbar->show();
}

static void loadImageToRenderTexture(sf::RenderTexture& tex, std::string imgName) {
	// Clear scale-aware text stored for the previous texture instance:
	clear_scale_aware_text(tex);

	sf::Texture& temp_gworld = *ResMgr::graphics.get(imgName);
	rectangle texrect(temp_gworld);
	tex.create(texrect.width(), texrect.height());
	rect_draw_some_item(temp_gworld, texrect, tex, texrect, sf::BlendNone);
}

void load_main_screen() {
	// Preload the main game interface images
	ResMgr::graphics.get("invenbtns");
	loadImageToRenderTexture(terrain_screen_gworld, "terscreen");
	loadImageToRenderTexture(pc_stats_gworld, "statarea");
	loadImageToRenderTexture(item_stats_gworld, "inventory");
	loadImageToRenderTexture(text_area_gworld, "transcript");
	loadImageToRenderTexture(text_bar_gworld, "textbar");
	ResMgr::graphics.get("buttons");
}

void redraw_screen(int refresh) {
	// We may need to update some of the offscreen textures
	if(overall_mode != MODE_STARTUP) {
		if(refresh & REFRESH_TERRAIN) draw_terrain(1);
		if(refresh & REFRESH_STATS) put_pc_screen();
		if(refresh & REFRESH_INVEN) put_item_screen(stat_window);
		if(refresh & REFRESH_TRANS) print_buf();
	}

	// Temporarily switch to the original view to fill in the background
	mainPtr.setView(mainPtr.getDefaultView());
	put_background();
	mainPtr.setView(mainView);
	switch(overall_mode) {
		case MODE_STARTUP:
			draw_startup(0);
			break;
		case MODE_TALKING:
			if(refresh & REFRESH_DLOG)
				place_talk_str(save_talk_str1, save_talk_str2, 0, rectangle());
			refresh_talking();
			break;
		case MODE_SHOPPING:
			if(refresh & REFRESH_DLOG) draw_shop_graphics(false,false,{0,0,0,0});
			refresh_shopping();
			break;
		default:
			redraw_terrain();
			if(refresh & REFRESH_BAR)
				draw_text_bar();
			refresh_text_bar();
			UI::toolbar.draw(mainPtr);
			break;
	}
	if(overall_mode == MODE_COMBAT)
		frame_active_pc(univ.current_pc().combat_pos);
	if(overall_mode == MODE_FANCY_TARGET)
		draw_targets(center);
	if(overall_mode != MODE_STARTUP) {
		if(!is_out())
			draw_targeting_line(sf::Mouse::getPosition(mainPtr));
		refresh_stat_areas(0);
	}
	done_btn->draw();
	help_btn->draw();

	drawable_mgr.draw_all();

	mainPtr.display();
}

void put_background() {
	tessel_ref_t bg_pict;
	
	if(overall_mode == MODE_STARTUP)
		bg_pict = bg[4];
	else if(overall_mode == MODE_RESTING)
		bg_pict = bg[4];
	else if(is_out()) {
		if(univ.out->bg_out >= 0)
			bg_pict = bg[univ.out->bg_out];
		else bg_pict = bg[univ.scenario.bg_out];
	} else if(is_combat()) {
		if(which_combat_type == 1 && univ.town->bg_fight >= 0) // TODO: Verify this means town combat
			bg_pict = bg[univ.town->bg_fight];
		else if(univ.out->bg_fight >= 0)
			bg_pict = bg[univ.out->bg_fight];
		else bg_pict = bg[univ.scenario.bg_fight];
	} else if(univ.town->lighting_type != LIGHT_NORMAL) {
		if(univ.town->bg_town >= 0)
			bg_pict = bg[univ.town->bg_town];
		else if(univ.out->bg_dungeon >= 0)
			bg_pict = bg[univ.out->bg_dungeon];
		else bg_pict = bg[univ.scenario.bg_dungeon];
	} else {
		if(univ.town->bg_town >= 0)
			bg_pict = bg[univ.town->bg_town];
		else if(univ.out->bg_town >= 0)
			bg_pict = bg[univ.out->bg_town];
		else bg_pict = bg[univ.scenario.bg_town];
	}
	tileImage(mainPtr, rectangle(mainPtr), bg_pict);
}

void draw_text_bar() {
	location loc;
	
	loc = (is_out()) ? global_to_local(univ.party.out_loc) : univ.party.town_loc;
	
	bool in_area = false;
	if(is_out()) {
		for(short i = 0; i < univ.out->area_desc.size(); i++)
			if(loc.in(univ.out->area_desc[i])) {
				put_text_bar(univ.out->area_desc[i].descr);
				in_area = true;
			}
		if(!in_area) {
			put_text_bar(univ.out->name);
		}
	}
	if(is_town()) {
		for(short i = 0; i < univ.town->area_desc.size(); i++)
			if(loc.in(univ.town->area_desc[i])) {
				put_text_bar(univ.town->area_desc[i].descr);
				in_area = true;
			}
		if(!in_area) {
			put_text_bar(univ.town->name);
		}
		
	}
	if((is_combat()) && (univ.cur_pc < 6) && !monsters_going) {
		std::ostringstream sout;

		cPlayer& current_pc = univ.current_pc();
		sout << current_pc.name << " (ap: " << current_pc.ap << ')';

		// Spellcasters print a hint for recasting.
		// There's not enough space to print 2 hints for dual-casters,
		// so just handle the last type cast.
		eSkill type = current_pc.last_cast_type;
		std::string hint_prefix = "";
		std::ostringstream hint_out;
		switch(type){
			case eSkill::MAGE_SPELLS:
				hint_prefix = "M";
				break;
			case eSkill::PRIEST_SPELLS:
				hint_prefix = "P";
				break;
			// The only other expected value is eSkill::INVALID
			default:
				break;
		}
		if(!hint_prefix.empty()){
			hint_out << hint_prefix << ": ";
			if(current_pc.last_cast[type] != eSpell::NONE){
				const cSpell& spell = (*current_pc.last_cast[type]);
				if(pc_can_cast_spell(current_pc,type) && spell.cost <= current_pc.get_magic()) {
					hint_out << "Recast " << spell.name();
				}else{
					hint_out << "Cannot recast";
				}
			}else{
				hint_out << "No spell to recast";
			}
		}

		put_text_bar(sout.str(), hint_out.str());
	}
	if((is_combat()) && (monsters_going))
		// Print bar for 1st monster with >0 ap - that is monster that is going
		for(short i = 0; i < univ.town.monst.size(); i++)
			if((univ.town.monst[i].is_alive()) && (univ.town.monst[i].ap > 0)) {
				put_text_bar(print_monster_going(univ.town.monst[i].number,univ.town.monst[i].ap));
				i = 400;
			}
}

void put_text_bar(std::string str, std::string right_str) {
	text_bar_gworld.setActive(false);
	auto& bar_gw = *ResMgr::graphics.get("textbar");
	rect_draw_some_item(bar_gw, rectangle(bar_gw), text_bar_gworld, rectangle(bar_gw));
	clear_scale_aware_text(text_bar_gworld);
	TextStyle style;
	style.colour = sf::Color::White;
	style.font = FONT_BOLD;
	style.pointSize = 12;
	style.lineHeight = 12;
	rectangle to_rect = rectangle(text_bar_gworld);
	to_rect.top += 7;
	to_rect.left += 5;
	to_rect.right -= 5;
	win_draw_string(text_bar_gworld, to_rect, str, eTextMode::LEFT_TOP, style);
	// the recast hint will replace status icons:
	if(!right_str.empty()){
		// Style has to be wrap to get right-alignment
		win_draw_string(text_bar_gworld, to_rect, right_str, eTextMode::WRAP, style, true);
	}else if(!monsters_going) {
		sf::Texture& status_gworld = *ResMgr::graphics.get("staticons");
		to_rect.top -= 2;
		to_rect.left = to_rect.right - 15;
		to_rect.width() = 12;
		to_rect.height() = 12;
		for(auto next : univ.party.status) {
			const auto& statInfo = *next.first;
			if(next.second > 0) {
				rect_draw_some_item(status_gworld, get_stat_effect_rect(statInfo.icon), text_bar_gworld, to_rect, sf::BlendAlpha);
				to_rect.offset(-15, 0);
			}
		}
	}
	
	text_bar_gworld.setActive();
	text_bar_gworld.display();
}

void refresh_text_bar() {
	mainPtr.setActive(false);
	rect_draw_some_item(text_bar_gworld, rectangle(text_bar_gworld), mainPtr, win_to_rects[WINRECT_STATUS]);
	mainPtr.setActive();
}

// this is used for determinign whether to round off walkway corners
// right now, trying a restrictive rule (just cave floor and grass, mainly)
bool is_nature(short x, short y, unsigned short ground_t) {
	ter_num_t ter_type;
	
	ter_type = coord_to_ter((short) x,(short) y);
	return ground_t == univ.scenario.ter_types[ter_type].ground_type;
}

std::vector<location> forcecage_locs;
extern std::list<text_label_t> posted_labels;

//mode ... if 1, don't place on screen after redoing
// if 2, only redraw over active monst
void draw_terrain(short mode) {
	location where_draw;
	location sector_p_in,view_loc;
	char can_draw;
	ter_num_t spec_terrain;
	bool draw_frills = true;
	
	if(overall_mode == MODE_TALKING || overall_mode == MODE_SHOPPING || overall_mode == MODE_STARTUP)
		return;
	
	if(mode == 2) {
		if(current_working_monster < 0) return;
		supressing_some_spaces = true;
		for(short i = 0; i < 4; i++) ok_space[i].x = -1;
		if(current_working_monster >= 100) {
			for(short i = 0; i < univ.town.monst[current_working_monster - 100].x_width; i++)
				for(short j = 0; j < univ.town.monst[current_working_monster - 100].y_width; j++) {
					ok_space[i + 2 * j].x = univ.town.monst[current_working_monster - 100].cur_loc.x + i;
					ok_space[i + 2 * j].y = univ.town.monst[current_working_monster - 100].cur_loc.y + j;
					ok_space[i + 2 * j].x = ok_space[i + 2 * j].x - center.x + 4;
					ok_space[i + 2 * j].y = ok_space[i + 2 * j].y - center.y + 4;
				}
		}
		if(current_working_monster < 6) {
			ok_space[0] = univ.party[current_working_monster].combat_pos;
			ok_space[0].x = ok_space[0].x - center.x + 4;
			ok_space[0].y = ok_space[0].y - center.y + 4;
		}
		mode = 0;
	}
	
	mainPtr.setActive();
	
	int max_dim_x, max_dim_y;
	if(is_out()){
		max_dim_x = min(96, 48 * univ.scenario.outdoors.width());
		max_dim_y = min(96, 48 * univ.scenario.outdoors.height());
	}else {
		max_dim_x = max_dim_y = univ.town->max_dim;
	}

	for(short i = 0; i < 13; i++)
		for(short j = 0; j < 13; j++) {
			light_area[i][j] = 0;
			unexplored_area[i][j] = 0;
		}
	
	
	sector_p_in.x = univ.party.outdoor_corner.x + univ.party.i_w_c.x;
	sector_p_in.y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;
	
	if(is_town())
		view_loc = univ.party.town_loc;
	if(is_combat())
		view_loc = univ.party[(univ.cur_pc < 6) ? univ.cur_pc : first_active_pc()].combat_pos;
	
	for(short i = 0; i < 13; i++)
		for(short j = 0; j < 13; j++) {
			where_draw =  (is_out()) ? univ.party.out_loc : center;
			where_draw.x += i - 6;
			where_draw.y += j - 6;
			if (where_draw.x < 0 || where_draw.y < 0 || where_draw.x >= max_dim_x || where_draw.y >= max_dim_y)
				continue;
			if(!(is_out()))
				light_area[i][j] = (is_town()) ? pt_in_light(view_loc,where_draw) : combat_pt_in_light(where_draw);
			if(!is_out() && !univ.town.is_on_map(where_draw.x, where_draw.y))
				unexplored_area[i][j] = 0;
			else unexplored_area[i][j] = 1 - is_explored(where_draw.x,where_draw.y);
		}
	
	forcecage_locs.clear();
	for(short q = 0; q < 9; q++) {
		for(short r = 0; r < 9; r++) {
			where_draw = (is_out()) ? univ.party.out_loc : center;
			where_draw.x += q - 4;
			where_draw.y += r - 4;
			if (where_draw.x < 0 || where_draw.y < 0 || where_draw.x >= max_dim_x || where_draw.y >= max_dim_y){
				// Out of bounds. Draw darkness
				draw_one_terrain_spot(q,r,-1);
				continue;
			}
			draw_frills = true;
			if(!is_out() && !univ.town.is_on_map(where_draw.x, where_draw.y)) {
				draw_frills = false;
				// Warning - this section changes where_draw
				if(where_draw.x < 0)
					where_draw.x = -1;
				if(where_draw.x > univ.town->max_dim - 1)
					where_draw.x = univ.town->max_dim;
				if(where_draw.y < 0)
					where_draw.y = -1;
				if(where_draw.y > univ.town->max_dim - 1)
					where_draw.y = univ.town->max_dim;
				if(can_see_light(view_loc,where_draw,sight_obscurity) < 5)
					can_draw = 1;
				else can_draw = 0;
				spec_terrain = 0;
			}
			else if(is_out()) {
				if(!univ.out.is_on_map(where_draw.x, where_draw.y))
					can_draw = 0;
				else {
					spec_terrain = univ.out[where_draw.x][where_draw.y];
					can_draw = univ.out.out_e[where_draw.x][where_draw.y];
				}
			}
			else if(is_combat()) {
				spec_terrain = univ.town->terrain(where_draw.x,where_draw.y);
				can_draw = (((is_explored(where_draw.x,where_draw.y)) ||
							 (which_combat_type == 0) || (monsters_going) || (overall_mode != MODE_COMBAT))
							&& (party_can_see(where_draw) < 6)) ? 1 : 0;
			}
			else {
				spec_terrain = univ.town->terrain(where_draw.x,where_draw.y);
				can_draw = is_explored(where_draw.x,where_draw.y);
				
				if(can_draw > 0) {
					if(!pt_in_light(univ.party.town_loc,where_draw))
						can_draw = 0;
				}
				if((overall_mode == MODE_LOOK_TOWN) && (can_draw == 0))
					can_draw = (party_can_see(where_draw) < 6) ? 1 : 0;
			}
			spot_seen[q][r] = can_draw;
			
			if(fog_lifted) can_draw = true;
			
			if((can_draw != 0) && (overall_mode != MODE_RESTING)) { // if can see, not a pit, and not resting
				if(is_combat()) anim_ticks = 0;
				
				eTrimType trim = univ.scenario.ter_types[spec_terrain].trim_type;
				
				// Finally, draw this terrain spot
				if(trim == eTrimType::WALKWAY){
					int trim = -1;
					unsigned short ground_t = univ.scenario.ter_types[spec_terrain].trim_ter;
					ter_num_t ground_ter = univ.scenario.get_ter_from_ground(ground_t);
					if(!loc_off_act_area(where_draw)) {
						if(is_nature(where_draw.x - 1,where_draw.y,ground_t)){ // check left
							if(is_nature(where_draw.x,where_draw.y - 1,ground_t)){ // check up
								if(is_nature(where_draw.x + 1,where_draw.y,ground_t)){ // check right
									if(is_nature(where_draw.x,where_draw.y + 1,ground_t)) // check down
										trim = 8;
									else trim = 4;
								}else if(is_nature(where_draw.x,where_draw.y + 1,ground_t)) // check down
									trim = 7;
								else trim = 1;
							}else if(is_nature(where_draw.x,where_draw.y + 1,ground_t)){ // check down
								if(is_nature(where_draw.x + 1,where_draw.y,ground_t)) // check right
									trim = 6;
								else trim = 0;
							}
						}else if(is_nature(where_draw.x,where_draw.y - 1,ground_t)){ // check up
							if(is_nature(where_draw.x + 1,where_draw.y,ground_t)){ // check right
								if(is_nature(where_draw.x,where_draw.y + 1,ground_t)) // check down
									trim = 5;
								else trim = 2;
							}
						}else if(is_nature(where_draw.x + 1,where_draw.y,ground_t)){ // check right
							if(is_nature(where_draw.x,where_draw.y + 1,ground_t)) // check down
								trim = 3;
						}
					}
					draw_one_terrain_spot(q,r,trim < 0 ? spec_terrain : ground_ter);
					if(trim >= 0)
						draw_trim(q,r,trim + 50,spec_terrain);
				}else if(spec_terrain == 65535) {
					draw_one_terrain_spot(q,r,-1);
				}else{
					current_ground = univ.scenario.get_ground_from_ter(spec_terrain);
					draw_one_terrain_spot(q,r,spec_terrain);
				}
			}
			else {  // Can't see. Place darkness.
				draw_one_terrain_spot(q,r,-1);
			}
			
			if((can_draw != 0) && (overall_mode != MODE_RESTING) && draw_frills)
				place_trim((short) q,(short) r,where_draw,spec_terrain);
//			if((is_town() && univ.town.is_spot(where_draw.x,where_draw.y)) ||
//			   (is_out() && univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].special_spot[where_draw.x][where_draw.y]))
//				Draw_Some_Item(roads_gworld, calc_rect(6, 0), terrain_screen_gworld, loc(q,r), 1, 0);
			// TODO: Move draw_sfx, draw_items, draw_fields, draw_spec_items, etc to here
			
			if(is_town() || is_combat())
				draw_items(where_draw);
			if(is_out() && univ.out.out_e[where_draw.x][where_draw.y] && univ.out.is_road(where_draw.x,where_draw.y))
				place_road(q,r,where_draw,true);
			else if(is_town() && univ.town.is_explored(where_draw.x,where_draw.y) && univ.town.is_road(where_draw.x, where_draw.y))
				place_road(q,r,where_draw,true);
			else place_road(q,r,where_draw,false);
			draw_fields(where_draw);
			//draw_monsters(where_draw);
			//draw_vehicles(where_draw);
			//if(is_combat) draw_pcs(where_draw); else draw_party(where_draw);
		}
	}
	
//	if((overall_mode != MODE_RESTING) && (!is_out()))
//		draw_sfx();
//
//	// Now place items
//	if((overall_mode > MODE_OUTDOORS) && (overall_mode != MODE_LOOK_OUTDOORS) && (overall_mode != MODE_RESTING))
//		draw_items();
//
//	// Now place fields
//	if((overall_mode != MODE_RESTING) && (!is_out())) {
//		draw_fields();
//		draw_spec_items();
//		}
//
	// Not camping. Place misc. stuff
	if(overall_mode != MODE_RESTING) {
		if(is_out())
			draw_outd_boats(univ.party.out_loc);
		else if((is_town()) || (which_combat_type == 1))
			draw_town_boat(center);
		draw_monsters();
	}
	
	if(is_out() || (is_town() && point_onscreen(univ.party.town_loc,center))
		|| (overall_mode == MODE_RESTING))
		draw_party_symbol(center);
	else if(overall_mode != MODE_LOOK_TOWN)
		draw_pcs(center);
	// Draw top half of forcecages (this list is populated by draw_fields)
	// TODO: Move into the above loop to eliminate global variable
	for(location fc_loc : forcecage_locs)
		Draw_Some_Item(*ResMgr::graphics.get("fields"),calc_rect(2,0),terrain_screen_gworld,fc_loc,1,0);
	// Draw any posted labels, then clear them out
	clip_rect(terrain_screen_gworld, {13, 13, 337, 265});
	for(text_label_t lbl : posted_labels)
		draw_text_label(lbl);
	undo_clip(terrain_screen_gworld);
	posted_labels.clear();
	
	// Now do the light mask thing
	apply_light_mask(false);
	apply_unseen_mask();
	
	terrain_screen_gworld.display();
	
	if(mode == 0) {
		redraw_terrain();
		draw_text_bar();
		if(is_combat())
			frame_active_pc(center);
		if(overall_mode == MODE_FANCY_TARGET)
			draw_targets(center);
	}
	supressing_some_spaces = false;
}


static ter_num_t get_ground_for_shore(ter_num_t ter){
	if(univ.scenario.ter_types[ter].block_horse) return current_ground;
	else if(univ.scenario.ter_types[ter].blocksMove()) return current_ground;
	else return ter;
}

void place_trim(short q,short r,location where,ter_num_t ter_type) {
	bool at_top = false,at_bot = false,at_left = false,at_right = false;
	location targ;
	
	// FIrst quick check ... if a pit or barrier in outdoor combat, no trim
	if((is_combat()) && (which_combat_type == 0) && (ter_type == 90))
		return;
	
	targ.x = q;
	targ.y = r;
	if(supressing_some_spaces && (targ != ok_space[0]) && (targ != ok_space[1]) &&
		(targ != ok_space[2]) && (targ != ok_space[3]))
		return;
	
	
	if(where.x == 0)
		at_left = true;
	if(where.y == 0)
		at_top = true;
	if((overall_mode == MODE_OUTDOORS) || (overall_mode == MODE_LOOK_OUTDOORS)) {
		if(where.x == 95)
			at_right = true;
		if(where.y == 95)
			at_bot = true;
	}
	else {
		// TODO: Shouldn't we subtract one here?
		// The outdoors case (above) does subtract 1, so one of them must be wrong...
		if(where.x == univ.town->max_dim)
			at_right = true;
		if(where.y == univ.town->max_dim)
			at_bot = true;
	}
	
	// First, trim for fluids
	if(is_fluid(ter_type)){
		short trim = get_fluid_trim(where, ter_type);
		if(trim != 0) {
			ter_num_t shore;
			if(trim & 2){ // upper right
				shore = coord_to_ter(where.x + 1, where.y - 1);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,5,shore);
			}if(trim & 8){ // lower right
				shore = coord_to_ter(where.x + 1, where.y + 1);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,7,shore);
			}if(trim & 32){ // lower left
				shore = coord_to_ter(where.x - 1, where.y + 1);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,6,shore);
			}if(trim & 128){ // upper left
				shore = coord_to_ter(where.x - 1, where.y - 1);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,4,shore);
			}if(trim & 1){ // top
				shore = coord_to_ter(where.x, where.y - 1);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,2,shore);
			}if(trim & 4){ // right
				shore = coord_to_ter(where.x + 1, where.y);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,1,shore);
			}if(trim & 16){ // bottom
				shore = coord_to_ter(where.x, where.y + 1);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,3,shore);
			}if(trim & 64){ // left
				shore = coord_to_ter(where.x - 1, where.y);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,0,shore);
			}
		}
	}
	
	if(is_wall(ter_type) && !at_top && !at_bot && !at_left && !at_right){
		//if(((ter_type >= 100) && (ter_type <= 137)) && !at_top &&
		//	!at_bot && !at_left && !at_right) {
		ter_num_t to_left = coord_to_ter(where.x - 1,where.y);
		ter_num_t above = coord_to_ter(where.x,where.y - 1);
		ter_num_t to_right = coord_to_ter(where.x + 1,where.y);
		ter_num_t below = coord_to_ter(where.x,where.y + 1);
		if(is_wall(to_left) && is_wall(above) && is_ground(to_right) && is_ground(below))
			draw_trim(q,r,11,to_right);
		
		if(is_wall(to_left) && is_wall(below) && is_ground(to_right) && is_ground(above))
			draw_trim(q,r,9,to_right);
		
		if(is_wall(to_right) && is_wall(above) && is_ground(to_left) && is_ground(below))
			draw_trim(q,r,10,to_left);
		
		if(is_wall(to_right) && is_wall(below) && is_ground(to_left) && is_ground(above))
			draw_trim(q,r,8,to_left);
		
		if(is_ground(to_left) && is_ground(above) && is_ground(to_right) && is_wall(below)) {
			draw_trim(q,r,8,to_right);
			draw_trim(q,r,9,to_right);
		}
		
		if(is_wall(to_left) && is_ground(below) && is_ground(to_right) && is_ground(above)) {
			draw_trim(q,r,9,to_right);
			draw_trim(q,r,11,to_right);
		}
		
		if(is_ground(to_right) && is_wall(above) && is_ground(to_left) && is_ground(below)) {
			draw_trim(q,r,10,to_left);
			draw_trim(q,r,11,to_left);
		}
		
		if(is_wall(to_right) && is_ground(below) && is_ground(to_left) && is_ground(above)) {
			draw_trim(q,r,8,to_left);
			draw_trim(q,r,10,to_left);
		}
		
		if(is_ground(to_right) && is_ground(below) && is_ground(to_left) && is_ground(above)) {
			draw_trim(q,r,8,to_left);
			draw_trim(q,r,9,to_right);
			draw_trim(q,r,10,to_left);
			draw_trim(q,r,11,to_right);
		}
		//	}
	}
}

static void init_trim_mask(std::unique_ptr<sf::Texture>& mask, rectangle src_rect) {
	sf::RenderTexture render;
	rectangle dest_rect;
	dest_rect.top = src_rect.top % 36;
	dest_rect.bottom = (src_rect.bottom - 1) % 36 + 1;
	dest_rect.left = src_rect.left % 28;
	dest_rect.right = (src_rect.right - 1) % 28 + 1;
	std::tie(dest_rect.top, dest_rect.bottom) = std::make_tuple(36 - dest_rect.top, 36 - dest_rect.bottom);
	render.create(28, 36);
	render.clear(sf::Color::White);
	rect_draw_some_item(*ResMgr::graphics.get("trim"), src_rect, render, dest_rect);
	render.display();
	mask.reset(new sf::Texture);
	mask->create(28, 36);
	mask->update(render.getTexture().copyToImage());
}

//which_trim is 3 -> drawing wall trim -> might shift down if ground is grass
//short which_mode;  // 0 top 1 bottom 2 left 3 right 4 up left 5 up right 6 down right 7 down left
void draw_trim(short q,short r,short which_trim,ter_num_t ground_ter) {
	// which_trim
	// 0 - left, 1 - right, 2 - top, 3 - bottom, 4 - tl, 5 - tr, 6 - bl, 7 - br
	// 8 - wall tl, 9 - wall tr, 10 - wall bl, 11 - wall br
	// 50 - walkway bl, 51 - walkway tl, 52 - walkway tr, 53 - walkway br
	// 54 - walkway top, 55 - walkway right, 56 - walkway bottom, 57 - walkway left
	// 58 - lone walkway
	static rectangle trim_rects[12] = {
		{0,0,36,14}, {0,0,36,14},
		{0,0,18,28}, {0,0,18,28},
		{0,0,18,14}, {0,0,18,14}, {0,0,18,14}, {0,0,18,14},
		{0,0,18,14}, {0,0,18,14}, {0,0,18,14}, {0,0,18,14},
	};
	static rectangle walkway_rects[9] = {
		{0,0,36,28}, {0,0,36,28}, {0,0,36,28}, {0,0,36,28},
		{0,0,36,28}, {0,0,36,28}, {0,0,36,28}, {0,0,36,28},
		{0,0,36,28},
	};
	static std::unique_ptr<sf::Texture> trim_masks[12], walkway_masks[9];
	rectangle from_rect = {0,0,36,28},to_rect;
	std::shared_ptr<const sf::Texture> from_gworld;
	sf::Texture* mask;
	static bool inited = false;
	if(!inited){
		inited = true;
		trim_rects[1].offset(14,0);
		trim_rects[2].offset(28,0);
		trim_rects[3].offset(28,18);
		trim_rects[4].offset(56,0);
		trim_rects[5].offset(70,0);
		trim_rects[6].offset(56,18);
		trim_rects[7].offset(70,18);
		trim_rects[8].offset(84,0);
		trim_rects[9].offset(98,0);
		trim_rects[10].offset(84,18);
		trim_rects[11].offset(98,18);
		for(short i = 0; i < 12; i++) trim_rects[i].offset(112,36);
		for(short i = 0; i < 8 ; i++) walkway_rects[i].offset((i%4)*28,(i/4)*36);
		walkway_rects[8].offset(196,0);
	}
	sf::Color test_color = {0,0,0}, store_color;
	
	unsigned short pic = univ.scenario.ter_types[ground_ter].picture;
	if(pic < 960){
		int which_sheet = pic / 50;
		from_gworld = &ResMgr::graphics.get("ter" + std::to_string(1 + which_sheet));
		pic %= 50;
		from_rect.offset(28 * (pic % 10), 36 * (pic / 10));
	}else if(pic < 1000){
		from_gworld = &ResMgr::graphics.get("teranim");
		pic -= 960;
		from_rect.offset(112 * (pic / 5),36 * (pic % 5));
	}else{
		pic %= 1000;
		graf_pos_ref(from_gworld, from_rect) = spec_scen_g.find_graphic(pic);
	}
	if(which_trim < 50) {
		if(!trim_masks[which_trim])
			init_trim_mask(trim_masks[which_trim], trim_rects[which_trim]);
		mask = trim_masks[which_trim].get();
	} else {
		int which = which_trim - 50;
		if(!walkway_masks[which])
			init_trim_mask(walkway_masks[which], walkway_rects[which]);
		mask = walkway_masks[which].get();
	}
	to_rect = coord_to_rect(q,r);
	
	rect_draw_some_item(*from_gworld, from_rect, *mask, terrain_screen_gworld, to_rect);
}


static bool extend_road_terrain(int x, int y) {
	if(is_out() && univ.out.is_road(x,y))
	   return true;
	if(is_town() && univ.town.is_road(x,y))
		return true;
	ter_num_t ter = coord_to_ter(x,y);
	eTrimType trim = univ.scenario.ter_types[ter].trim_type;
	eTerSpec spec = univ.scenario.ter_types[ter].special;
	ter_num_t flag = univ.scenario.ter_types[ter].flag1;
	if(trim == eTrimType::CITY || trim == eTrimType::WALKWAY)
		return true;
	if(spec == eTerSpec::BRIDGE)
		return true;
	if(spec == eTerSpec::TOWN_ENTRANCE && trim != eTrimType::NONE)
		return true; // cave entrance, most likely
	if(spec == eTerSpec::UNLOCKABLE || spec == eTerSpec::CHANGE_WHEN_STEP_ON)
		return true; // closed door, possibly locked; or closed portcullis
	if(spec == eTerSpec::CHANGE_WHEN_USED && univ.scenario.ter_types[flag].special == eTerSpec::CHANGE_WHEN_STEP_ON && univ.scenario.ter_types[flag].flag1 == ter)
		return true; // open door (I think) TODO: Verify this works
	if(spec == eTerSpec::LOCKABLE)
		return true; // open portcullis (most likely)
	return false;
}

static bool can_build_roads_on(ter_num_t ter) {
	if(impassable(ter)) return false;
	if(univ.scenario.ter_types[ter].special == eTerSpec::BRIDGE) return false;
	return true;
}

static bool connect_roads(ter_num_t ter){
	if(ter >= univ.scenario.ter_types.size()) return false;
	eTrimType trim = univ.scenario.ter_types[ter].trim_type;
	eTerSpec spec = univ.scenario.ter_types[ter].special;
	if(trim == eTrimType::CITY)
		return true;
	if(spec == eTerSpec::TOWN_ENTRANCE && trim != eTrimType::NONE)
		return true; // cave entrance, most likely
	return false;
}

void place_road(short q,short r,location where,bool here) {
	rectangle to_rect;
	static const rectangle road_rects[5] = {
		{76,28,80,41},	// horizontal partial
		{72,60,90,64},	// vertical partial
		{72,28,75,56},	// horizontal full
		{72,56,108,60},	// vertical full
		{80,28,84,32},	// central spot
	};
	static const rectangle road_dest_rects[7] = {
		{0,12,18,16},	// top
		{16,15,20,28},	// right
		{18,12,36,16},	// bottom
		{16,0,20,13},	// left
		{0,12,36,16},	// top + bottom
		{16,0,20,28},	// right + left
		{16,12,20,16},	// central spot
	};
	
	sf::Texture& roads_gworld = *ResMgr::graphics.get("fields");
	
	if(here){
		to_rect = road_dest_rects[6];
		to_rect.offset(13 + q * 28,13 + r * 36);
		rect_draw_some_item(roads_gworld, road_rects[4], terrain_screen_gworld, to_rect);
		
		if((where.y == 0) || extend_road_terrain(where.x, where.y - 1)) {
			to_rect = road_dest_rects[0];
			to_rect.offset(13 + q * 28,13 + r * 36);
			rect_draw_some_item (roads_gworld, road_rects[1], terrain_screen_gworld, to_rect);
		}
		
		if(((is_out()) && (where.x == 96)) || (!(is_out()) && (where.x == univ.town->max_dim - 1))
			|| extend_road_terrain(where.x + 1, where.y)) {
			to_rect = road_dest_rects[1];
			to_rect.offset(13 + q * 28,13 + r * 36);
			rect_draw_some_item (roads_gworld, road_rects[0], terrain_screen_gworld, to_rect);
		}
		
		if(((is_out()) && (where.y == 96)) || (!(is_out()) && (where.y == univ.town->max_dim - 1))
			|| extend_road_terrain(where.x, where.y + 1)) {
			to_rect = road_dest_rects[2];
			to_rect.offset(13 + q * 28,13 + r * 36);
			rect_draw_some_item (roads_gworld, road_rects[1], terrain_screen_gworld, to_rect);
		}
		
		if((where.x == 0) || extend_road_terrain(where.x - 1, where.y)) {
			to_rect = road_dest_rects[3];
			to_rect.offset(13 + q * 28,13 + r * 36);
			rect_draw_some_item (roads_gworld, road_rects[0], terrain_screen_gworld, to_rect);
		}
	}else{
		// TODO: I suspect this branch is now irrelevant.
		ter_num_t ter = coord_to_ter(where.x, where.y);
		ter_num_t ref = coord_to_ter(where.x,where.y);
		bool horz = false, vert = false;
		eTrimType trim = eTrimType::NONE, vertTrim = eTrimType::NONE;
		if(ref < univ.scenario.ter_types.size()) {
			trim = univ.scenario.ter_types[ref].trim_type;
		}
		if(ter < univ.scenario.ter_types.size()) {
			vertTrim = univ.scenario.ter_types[ter].trim_type;
		}
		if(where.y > 0)
			ter = coord_to_ter(where.x,where.y - 1);
		if((where.y == 0) || connect_roads(ter))
			vert = can_build_roads_on(ref);
		else if((vertTrim == eTrimType::S && trim == eTrimType::N) || (vertTrim == eTrimType::N && trim == eTrimType::S))
			vert = can_build_roads_on(ref);
		
		if(((is_out()) && (where.x < 96)) || (!(is_out()) && (where.x < univ.town->max_dim - 1)))
			ter = coord_to_ter(where.x + 1,where.y);
		eTrimType horzTrim = univ.scenario.ter_types[ter].trim_type;
		if(((is_out()) && (where.x == 96)) || (!(is_out()) && (where.x == univ.town->max_dim - 1))
			|| connect_roads(ter))
			horz = can_build_roads_on(ref);
		else if((horzTrim == eTrimType::W && trim == eTrimType::E) || (horzTrim == eTrimType::E && trim == eTrimType::W))
			horz = can_build_roads_on(ref);
		
		if(vert){
			if(((is_out()) && (where.y < 96)) || (!(is_out()) && (where.y < univ.town->max_dim - 1)))
				ter = coord_to_ter(where.x,where.y + 1);
			eTrimType vertTrim = univ.scenario.ter_types[ter].trim_type;
			if(((is_out()) && (where.y == 96)) || (!(is_out()) && (where.y == univ.town->max_dim - 1))
				|| connect_roads(ter))
				vert = can_build_roads_on(ref);
			else if((vertTrim == eTrimType::S && trim == eTrimType::N) || (vertTrim == eTrimType::N && trim == eTrimType::S))
				vert = can_build_roads_on(ref);
			else vert = false;
		}
		
		if(horz){
			if(where.x > 0)
				ter = coord_to_ter(where.x - 1,where.y);
			eTrimType horzTrim = univ.scenario.ter_types[ter].trim_type;
			if((where.x == 0) || connect_roads(ter))
				horz = can_build_roads_on(ref);
			else if((horzTrim == eTrimType::W && trim == eTrimType::E) || (horzTrim == eTrimType::E && trim == eTrimType::W))
				horz = can_build_roads_on(ref);
			else horz = false;
		}
		
		if(horz){
			to_rect = road_dest_rects[5];
			to_rect.offset(13 + q * 28,13 + r * 36);
			rect_draw_some_item (roads_gworld, road_rects[2], terrain_screen_gworld, to_rect);
		}
		if(vert){
			to_rect = road_dest_rects[4];
			to_rect.offset(13 + q * 28,13 + r * 36);
			rect_draw_some_item (roads_gworld, road_rects[3], terrain_screen_gworld, to_rect);
		}
	}
}

void draw_rest_screen() {
	eGameMode store_mode;
	
	store_mode = overall_mode;
	overall_mode = MODE_RESTING;
	
	for(int q = 0; q < 9; q++) {
		for(int r = 0; r < 9; r++) {
			draw_one_terrain_spot(q,r,-1);
		}
	}
	draw_party_symbol(center);
	terrain_screen_gworld.display();
	
	overall_mode = store_mode ;
}

// if mode is 100, force
//short type; // 0 - flame 1 - magic 2 - poison 3 - blood 4 - cold
// 10s digit indicates sound  0 - normal ouch  1 - small sword  2 - loud sword
// 3 - pole  4 - club  5 - fireball hit  6 - squish  7 - cold
// 8 - acid  9 - claw  10 - bite  11 - slime  12 - zap  13 - missile hit
void boom_space(location where,short mode,short type,short damage,short sound) {
	location where_draw(4,4);
	rectangle source_rect = {0,0,36,28},text_rect,dest_rect = {13,13,49,41},big_to = {13,13,337,265},store_rect;
	short del_len;
	short x_adj = 0,y_adj = 0;
	short sound_lookup[20] = {
		97,69,70,71,72, 73,55,75,42,86,
		87,88,89,98,0, 0,0,0,0,0};
	short sound_to_play = sound < 0 ? -sound : sound_lookup[sound];
	
	//sound_key = type / 10;
	//type = type % 10;
	
//	if((cartoon_happening) && (anim_step < 140))
//		return;
	if((mode != 100) && (party_can_see(where) == 6))
		return;
	if(type < 0 || type > 5)
		return;
	if((boom_anim_active) && (type != 3))
		return;
	if(is_out())
		return;
	
	// Redraw terrain in proper position
	if(((!point_onscreen(center,where) && is_combat()) || (overall_mode == MODE_OUTDOORS))
		) {
		play_sound(sound_to_play);
		
		return;
	}
	else if(is_combat()) {
		if(which_combat_type == 1)
			draw_terrain(0);
		else draw_terrain(0);
	}
	else if(fast_bang < 2) {
		draw_terrain(0);
		if(fast_bang == 1)
			fast_bang = 2;
	}
	refresh_stat_areas(0);
	
	where_draw.x = where.x - center.x + 4;
	where_draw.y = where.y - center.y + 4;
//	source_rect.left += 28 * type;
//	source_rect.right += 28 * type;
	
	// adjust for possible big monster
	if(iLiving* who = univ.target_there(where, TARG_MONST)) {
		cCreature* monst = dynamic_cast<cCreature*>(who);
		x_adj += 14 * (monst->x_width - 1);
		y_adj += 18 * (monst->y_width - 1);
	}
	dest_rect.offset(where_draw.x * 28,where_draw.y * 36);
	source_rect = store_rect = dest_rect;
	dest_rect.offset(x_adj,y_adj);
	dest_rect &= big_to;
	
	dest_rect.offset(win_to_rects[WINRECT_TERVIEW].topLeft());
	
	source_rect.offset(-store_rect.left + 28 * type,-store_rect.top);
	rect_draw_some_item(*ResMgr::graphics.get("booms"),source_rect,mainPtr,dest_rect,sf::BlendAlpha);
	
	if(damage > 0 && dest_rect.right - dest_rect.left >= 28 && dest_rect.bottom - dest_rect.top >= 36) {
		TextStyle style;
		style.lineHeight = 10;
		text_rect = dest_rect;
		text_rect.top += 13;
		text_rect.height() = 10;
		std::string dam_str = std::to_string(damage);
		style.colour = sf::Color::White;
		text_rect.offset(-1,-1);
		win_draw_string(mainPtr,text_rect,dam_str,eTextMode::CENTRE,style);
		text_rect.offset(2,2);
		win_draw_string(mainPtr,text_rect,dam_str,eTextMode::CENTRE,style);
		style.colour = sf::Color::Black;
		text_rect.offset(-1,-1);
		win_draw_string(mainPtr,text_rect,dam_str,eTextMode::CENTRE,style);
	}
	mainPtr.display();
	bool skip_boom_delay = get_bool_pref("SkipBoomDelay");
	play_sound((skip_boom_delay?-1:1)*sound_to_play);
	if((sound == 6) && (fast_bang == 0) && (!skip_boom_delay))
		sf::sleep(time_in_ticks(12));
	
	
	if(fast_bang == 0 && !skip_boom_delay) {
		del_len = get_int_pref("GameSpeed") * 3 + 4;
		if(!get_bool_pref("PlaySounds", true))
			sf::sleep(time_in_ticks(del_len));
	}
	redraw_terrain();
	if(is_combat())
		frame_active_pc(center);
}

// dir = 0 - down, 1 - left, 2 - right, 3 - up, 4 - down/left, 5 - up/left, 6 - up/right, 7 - down/right
// pos = row or column to centre the arrow in, range 0..8, ignored for dir >= 4
static void draw_one_pointing_arrow(int dir, int pos) {
	rectangle from_rect = {62, 61, 70, 69};
	from_rect.offset(9 * (dir % 4), 9 * (dir / 4));
	
	rectangle ter_view_rect = win_to_rects[WINRECT_TERVIEW];
	rectangle to_rect = ter_view_rect;
	
	// Left-pointing arrows
	if(dir == 1 || dir == 4 || dir == 5)
		to_rect.left += 2;
	// Right-pointing arrows
	if(dir == 2 || dir == 6 || dir == 7)
		to_rect.left = to_rect.right - 10;
	// Up-pointing arrows
	if(dir == 3 || dir == 5 || dir == 6)
		to_rect.top += 2;
	// Down-pointing arrows
	if(dir == 0 || dir == 4 || dir == 7)
		to_rect.top = to_rect.bottom - 10;
	// Horizontal arrows
	if(dir == 0 || dir == 3)
		to_rect.left += 23 + pos * 28;
	// Vertical arrows
	if(dir == 1 || dir == 2)
		to_rect.top += 23 + pos * 36;
	
	to_rect.width() = to_rect.height() = 8;
	
	rect_draw_some_item(*ResMgr::graphics.get("invenbtns"), from_rect, mainPtr, to_rect, sf::BlendAlpha);
}

void draw_pointing_arrows() {
	if(monsters_going || !scrollableModes.count(overall_mode))
		return;
	
	draw_one_pointing_arrow(0, 3);
	draw_one_pointing_arrow(0, 5);
	draw_one_pointing_arrow(1, 3);
	draw_one_pointing_arrow(1, 5);
	draw_one_pointing_arrow(2, 3);
	draw_one_pointing_arrow(2, 5);
	draw_one_pointing_arrow(3, 3);
	draw_one_pointing_arrow(3, 5);
	draw_one_pointing_arrow(4, 0);
	draw_one_pointing_arrow(5, 0);
	draw_one_pointing_arrow(6, 0);
	draw_one_pointing_arrow(7, 0);
}

void redraw_terrain() {
	rectangle to_rect = win_to_rects[WINRECT_TERVIEW], from_rect(terrain_screen_gworld);
	rect_draw_some_item(terrain_screen_gworld.getTexture(), from_rect, mainPtr, to_rect);
	apply_light_mask(true);
	
	
	// Now place arrows
	draw_pointing_arrows();
	
	
}


void draw_targets(location center) {
	if(!univ.party.is_alive())
		return;
	const rectangle src_rect{0,46,12,58};
	
	sf::Texture& src_gworld = *ResMgr::graphics.get("invenbtns");
	for(short i = 0; i < 8; i++)
		if((spell_targets[i].x != -1) && (point_onscreen(center,spell_targets[i]))) {
			rectangle dest_rect = coord_to_rect(spell_targets[i].x - center.x + 4,spell_targets[i].y - center.y + 4);
			dest_rect.inset(8,12);
			dest_rect.offset(win_to_rects[WINRECT_TERVIEW].topLeft());
			rect_draw_some_item(src_gworld,src_rect,mainPtr,dest_rect,sf::BlendAlpha);
		}
}

//mode;  // 0 - red   1 - green
void frame_space(location where,short mode,short width,short height) {
	location where_put;
	rectangle to_frame;
	
	if(!point_onscreen(center,where))
		return;
	
	where_put.x = 4 + where.x - center.x;
	where_put.y = 4 + where.y - center.y;
	
	to_frame.top = 13 + where_put.y * 36;
	to_frame.left = 13 + where_put.x * 28;
	to_frame.bottom = 49 + where_put.y * 36 + 36 * (height - 1);
	to_frame.right = 41 + where_put.x * 28 + 28 * (width - 1);
	to_frame.offset(win_to_rects[WINRECT_TERVIEW].topLeft());
	
	frame_roundrect(mainPtr, to_frame, 8, (mode == 0) ? Colours::RED : Colours::GREEN);
}


void erase_spot(short i,short j) {
	rectangle to_erase;
	
	to_erase = coord_to_rect(i,j);
	fill_rect(mainPtr, to_erase, sf::Color::Black);
	
}

void draw_targeting_line(location where_curs) {
	location which_space,store_loc;
	rectangle redraw_rect,redraw_rect2,target_rect;
	location from_loc;
	rectangle on_screen_terrain_area = win_to_rects[WINRECT_TERVIEW];
	on_screen_terrain_area.inset(13, 13);
	
	where_curs = mainPtr.mapPixelToCoords(where_curs, mainView);
	
	if(overall_mode >= MODE_COMBAT)
		from_loc = univ.current_pc().combat_pos;
	else from_loc = univ.party.town_loc;
	if((overall_mode == MODE_SPELL_TARGET) || (overall_mode == MODE_FIRING) || (overall_mode == MODE_THROWING) || (overall_mode == MODE_FANCY_TARGET)
		|| ((overall_mode == MODE_TOWN_TARGET) && (current_pat.pattern[4][4] != 0))) {
		
		if(where_curs.in(on_screen_terrain_area)) {
			// && (point_onscreen(center,univ.party[current_pc].combat_pos))){
			which_space.x = center.x + (where_curs.x - 37) / 28 - 4;
			which_space.y = center.y + (where_curs.y - 25) / 36 - 4;
			
			int xBound = (short) (from_loc.x - center.x + 4);
			int yBound = (short) (from_loc.y - center.y + 4);
			xBound = (xBound * 28) + 46;
			yBound = (yBound * 36) + 38;
			
			if((can_see_light(from_loc,which_space,sight_obscurity) < 5)
				&& (dist(from_loc,which_space) <= current_spell_range)) {
				mainPtr.setActive(false);
				clip_rect(mainPtr, on_screen_terrain_area);
				draw_line(mainPtr, where_curs, location(xBound, yBound), 2, {128,128,128}, sf::BlendAdd);
				redraw_rect.left = min(where_curs.x,xBound) - 4;
				redraw_rect.right = max(where_curs.x,xBound) + 4;
				redraw_rect.top = min(where_curs.y,yBound) - 4;
				redraw_rect.bottom = max(where_curs.y,yBound) + 4;
				redraw_rect2 = redraw_rect & on_screen_terrain_area;
				
				// Now place targeting pattern
				for(short i = 0; i < 9; i++)
					for(short j = 0; j < 9; j++) {
						store_loc.x = center.x + i - 4;
						store_loc.y = center.y + j - 4;
						if((abs(store_loc.x - which_space.x) <= 4) &&
							(abs(store_loc.y - which_space.y) <= 4) &&
							(current_pat.pattern[store_loc.x - which_space.x + 4][store_loc.y - which_space.y + 4] != 0)) {
							target_rect.left = 13 + 28 * i + 19;
							target_rect.right = target_rect.left + 28;
							target_rect.top = 13 + 36 * j + 7;
							target_rect.bottom = target_rect.top + 36;
							frame_rect(mainPtr, target_rect, sf::Color::White);
							target_rect.inset(-5,-5);
							redraw_rect2 = rectunion(target_rect,redraw_rect2);
							
							// Now place number of shots left, if drawing center of target
							if((overall_mode == MODE_FANCY_TARGET) && (store_loc.x - which_space.x + 4 == 4)
								&& (store_loc.y - which_space.y + 4 == 4)) {
								TextStyle style;
								style.colour = sf::Color::White;
								style.lineHeight = 12;
								const char chr[2] = {static_cast<char>(num_targets_left + '0')};
								int x = ((target_rect.left + target_rect.right) / 2) - 3;
								int y = (target_rect.top + target_rect.bottom) / 2;
								win_draw_string(mainPtr, rectangle(y, x, y + 12, x + 12), chr, eTextMode::CENTRE, style);
							}
							
						}
					}
				
				redraw_rect2.inset(-5,-5);
				mainPtr.setActive();
				undo_clip(mainPtr);
			}
		}
	}
}

void redraw_partial_terrain(rectangle redraw_rect) {
	rectangle from_rect;
	
	from_rect = redraw_rect;
	
	// as rect_draw_some_item will shift redraw_rect before drawing, we need to compensate
	redraw_rect.offset(5,5);
	
	rect_draw_some_item(terrain_screen_gworld.getTexture(),from_rect,mainPtr,redraw_rect);
	
}

/*
void HideShowMenuBar( ) {
	GDHandle	mainScreen;
	
	// store current gray region that displays
	// menu bar and the current height of the menu bar
	originalGrayRgn 	= LMGetGrayRgn();
	menuBarHeight	= LMGetMBarHeight();
	
	// calculate the rect of the menu bar to test
	// if mouse down is in if desired
	mainScreen 			= GetMainDevice();
	menuBarRect 			= (**mainScreen).gdRect;
	menuBarRect.bottom 	= menuBarHeight;
	
	HideMenuBar();
	
	while( !Button() );
	
	ShowMenuBar();
	
	// restore the original gray region to
	// make the menu bar visible
	LMSetGrayRgn( originalGrayRgn );
}

*/
