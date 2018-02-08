/*
 *  graphtool.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#define GRAPHTOOL_CPP
#include "graphtool.hpp"

#ifdef __APPLE__
#include <OpenGl/GL.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#endif

#include <iostream>
#include <typeinfo>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include <boost/math/constants/constants.hpp>

#include "restypes.hpp"
#include "mathutil.hpp"
#include "fileio.hpp"

using boost::math::constants::pi;

rectangle bg_rects[21];
tessel_ref_t bg[21];
tessel_ref_t bw_pats[6];
bool use_win_graphics = false;
sf::Shader maskShader;
extern fs::path progDir;
// TODO: Shouldn't need this
extern sf::RenderWindow mainPtr;

static void register_main_patterns();

void init_graph_tool(){
	fs::path shaderPath = progDir/"data"/"shaders";
	fs::path fragPath = shaderPath/"mask.frag", vertPath = shaderPath/"mask.vert";
	
	do {
		std::ifstream fin;
		fin.open(fragPath.string().c_str());
		if(!fin.good()) {
			std::cerr << std_fmterr << ": Error loading fragment shader" << std::endl;
			break;
		}
		fin.seekg(0, std::ios::end);
		int size = fin.tellg();
		fin.seekg(0);
		char* fbuf = new char[size + 1];
		fbuf[size] = 0;
		fin.read(fbuf, size);
		fbuf[fin.gcount()] = 0;
		fin.close();

		fin.open(vertPath.string().c_str());
		if(!fin.good()) {
			std::cerr << std_fmterr << ": Error loading vertex shader" << std::endl;
			delete[] fbuf;
			break;
		}
		fin.seekg(0, std::ios::end);
		size = fin.tellg();
		fin.seekg(0);
		char* vbuf = new char[size + 1];
		vbuf[size] = 0;
		fin.read(vbuf, size);
		vbuf[fin.gcount()] = 0;
	
		if(!maskShader.loadFromMemory(vbuf, fbuf)) {
			std::cerr << "Error: Failed to load shaders from " << shaderPath << "\nVertex:\n" << vbuf << "\nFragment:\n" << fbuf << std::endl;
		}
		delete[] fbuf;
		delete[] vbuf;
	} while(false);
	static const location pat_offs[17] = {
		{0,3}, {1,1}, {2,1}, {2,0},
		{3,0}, {3,1}, {1,3}, {0,0},
		{0,2}, {1,2}, {0,1}, {2,2},
		{2,3}, {3,2}, {1,0}, {4,0}, {3,3}
	};
	static const int pat_i[17] = {
		2, 3, 4, 5, 6, 8, 9, 10,
		11,12,13,14,15,16,17,19,20
	};
	for(short i = 0; i < 17; i++){
		bg_rects[pat_i[i]] = {0,0,64,64};
		bg_rects[pat_i[i]].offset(64 * pat_offs[i].x,64 * pat_offs[i].y);
	}
	rectangle tmp_rect = bg_rects[19];
	tmp_rect.offset(0, 64);
	bg_rects[0] = bg_rects[1] = bg_rects[18] = bg_rects[7] = tmp_rect;
	bg_rects[0].right -= 32;
	bg_rects[0].bottom -= 32;
	bg_rects[1].left += 32;
	bg_rects[1].bottom -= 32;
	bg_rects[18].right -= 32;
	bg_rects[18].top += 32;
	bg_rects[7].left += 32;
	bg_rects[7].top += 32;
	register_main_patterns();
}

void draw_splash(const sf::Texture& splash, sf::RenderWindow& targ, rectangle dest_rect) {
	rectangle from_rect = rectangle(splash);
	targ.clear(sf::Color::Black);
	rect_draw_some_item(splash, from_rect, targ, dest_rect);
	targ.display();
}

static void rect_draw_some_item(const sf::Texture& src_gworld,rectangle src_rect,sf::RenderTarget& targ_gworld,rectangle targ_rect,sf::RenderStates mode);

void rect_draw_some_item(sf::RenderTarget& targ_gworld,rectangle targ_rect) {
	fill_rect(targ_gworld, targ_rect, sf::Color::Black);
}

void rect_draw_some_item(const sf::Texture& src_gworld,rectangle src_rect,sf::RenderTarget& targ_gworld,rectangle targ_rect,sf::BlendMode mode){
	rect_draw_some_item(src_gworld, src_rect, targ_gworld, targ_rect, sf::RenderStates(mode));
}

void rect_draw_some_item(const sf::Texture& src_gworld,rectangle src_rect,sf::RenderTarget& targ_gworld,rectangle targ_rect,sf::RenderStates mode) {
	setActiveRenderTarget(targ_gworld);
	sf::Sprite tile(src_gworld, src_rect);
	tile.setPosition(targ_rect.left, targ_rect.top);
	double xScale = targ_rect.width(), yScale = targ_rect.height();
	xScale /= src_rect.width();
	yScale /= src_rect.height();
	tile.setScale(xScale, yScale);
	targ_gworld.draw(tile, mode);
}

void rect_draw_some_item(const sf::Texture& src_gworld,rectangle src_rect,rectangle targ_rect,location offset, sf::BlendMode mode) {
	targ_rect.offset(offset);
	rect_draw_some_item(src_gworld,src_rect,mainPtr,targ_rect,mode);
}

void rect_draw_some_item(const sf::Texture& src_gworld,rectangle src_rect,const sf::Texture& mask_gworld,sf::RenderTarget& targ_gworld,rectangle targ_rect) {
	static sf::RenderTexture src;
	static bool inited = false;
	if(!inited || src_rect.width() != src.getSize().x || src_rect.height() != src.getSize().y) {
		src.create(src_rect.width(), src_rect.height());
		inited =  true;
	}
	rectangle dest_rect = src_rect;
	dest_rect.offset(-dest_rect.left,-dest_rect.top);
	rect_draw_some_item(src_gworld, src_rect, src, dest_rect);
	src.display();
	
	maskShader.setParameter("texture", sf::Shader::CurrentTexture);
	maskShader.setParameter("mask", mask_gworld);
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
	bool showBreaks = false;
	location offset = {0,0};
	// Hilite ranges are, like the STL, of the form [first, last).
	std::vector<hilite_t> hilite_ranges;
	sf::Color hilite_fg, hilite_bg = sf::Color::Transparent;
	enum {RECTS, SNIPPETS} returnType;
	std::vector<rectangle> returnRects;
	std::vector<snippet_t> snippets;
};

void win_draw_string(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,text_params_t& options);

void win_draw_string(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,eTextMode mode,TextStyle style, location offset) {
	text_params_t params;
	params.mode = mode;
	params.style = style;
	params.offset = offset;
	win_draw_string(dest_window, dest_rect, str, params);
}

std::vector<rectangle> draw_string_hilite(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,TextStyle style,std::vector<hilite_t> hilites,sf::Color hiliteClr) {
	text_params_t params;
	params.mode = eTextMode::WRAP;
	params.hilite_ranges = hilites;
	params.style = style;
	params.hilite_fg = hiliteClr;
	params.returnType = text_params_t::RECTS;
	win_draw_string(dest_window, dest_rect, str, params);
	return params.returnRects;
}

std::vector<snippet_t> draw_string_sel(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,TextStyle style,std::vector<hilite_t> hilites,sf::Color hiliteClr) {
	text_params_t params;
	params.mode = eTextMode::WRAP;
	params.showBreaks = true;
	params.hilite_ranges = hilites;
	params.style = style;
	params.hilite_fg = style.colour;
	params.hilite_bg = hiliteClr;
	params.returnType = text_params_t::RECTS;
	win_draw_string(dest_window, dest_rect, str, params);
	return params.snippets;
}

static void push_snippets(size_t start, size_t end, text_params_t& options, size_t& iHilite, const std::string& str, location loc) {
	std::vector<hilite_t>& hilites = options.hilite_ranges;
	std::vector<snippet_t>& snippets = options.snippets;
	// Check if we have any hilites on this line.
	// We assume the list of hilites is sorted, so we just need to
	// check the current entry.
	size_t upper_bound = end;
	do {
		bool hilited = false;
		// Skip any hilite rules that have start = end
		while(iHilite < hilites.size() && hilites[iHilite].first == hilites[iHilite].second)
			iHilite++;
		if(iHilite < hilites.size()) {
			// Possibilities: (vertical bars indicate line boundaries, parentheses for hilite boundaries, dots are data)
			// 1.  |...|...(...) --> no hilite on this line
			// 2a. |...(...|...) --> hilite starts on this line and continues past it
			// 2b. |...(...)...| --> hilite starts and ends on this line
			// 3a. (...|...)...| --> hilite starts (or continues) at the start of the line and ends on this line
			// 3b. (...|......)| --> entire line hilited
			// Case 1 needs no special handling; check case 3, then case 2.
			if(hilites[iHilite].first <= start) {
				hilited = true;
				if(hilites[iHilite].second < end) // 3a
					end = hilites[iHilite].second;
			} else if(hilites[iHilite].first < end)
				end = hilites[iHilite].first;
			// 2b will reduce to 3a after shifting start over
		}
		size_t amount = end - start;
		snippets.push_back({str.substr(start,amount), loc, hilited});
		loc.x += string_length(snippets[snippets.size()-1].text, options.style);
		start = end;
		end = upper_bound;
		if(iHilite < hilites.size() && start >= hilites[iHilite].second)
			iHilite++;
	} while(start < upper_bound);
}

void win_draw_string(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,text_params_t& options) {
	if(str.empty()) return; // Nothing to do!
	short line_height = options.style.lineHeight;
	sf::Text str_to_draw;
	options.style.applyTo(str_to_draw);
	short str_len;
	unsigned short last_line_break = 0,last_word_break = 0;
	short total_width = 0;
	short adjust_x = 0,adjust_y = 0;
	
	adjust_x = options.offset.x;
	adjust_y = options.offset.y;
	str_to_draw.setString("fj"); // Something that has both an ascender and a descender
	adjust_y -= str_to_draw.getLocalBounds().height;
	
	str_to_draw.setString(str);
	str_len = str.length();
	if(str_len == 0) {
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
	
	// Special stuff
	size_t iHilite = 0;
	
	location moveTo;
	line_height -= 2; // TODO: ...why are we arbitrarily reducing the line height from the requested value?
	
	if(mode == eTextMode::WRAP) {
		moveTo = location(dest_rect.left + 1 + adjust_x, dest_rect.top + 1 + adjust_y + 9);
		short i;
		for(i = 0; text_len(i) != text_len(i + 1) && i < str_len; i++) {
			if(((text_len(i) - text_len(last_line_break) > (dest_rect.width() - 6))
				 && (last_word_break >= last_line_break)) || (str[i] == '|')) {
				if(str[i] == '|') {
					if(!options.showBreaks) str[i] = ' ';
					last_word_break = i + 1;
				} else if(last_line_break == last_word_break)
					last_word_break = i;
				push_snippets(last_line_break, last_word_break, options, iHilite, str, moveTo);
				moveTo.y += line_height;
				last_line_break = last_word_break;
			}
			if(str[i] == ' ')
				last_word_break = i + 1;
		}
		
		if(i - last_line_break > 0) {
			std::string snippet = str.substr(last_line_break);
			if(!snippet.empty())
				push_snippets(last_line_break, str.length() + 1, options, iHilite, str, moveTo);
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
		push_snippets(0, str.length() + 1, options, iHilite, str, moveTo);
	}
	
	for(auto& snippet : options.snippets) {
		str_to_draw.setString(snippet.text);
		str_to_draw.setPosition(snippet.at);
		if(snippet.hilited) {
			rectangle bounds = str_to_draw.getGlobalBounds();
			// Adjust so that drawing the same text to
			// the same rect is positioned exactly right
			bounds.left = snippet.at.x - 1;
			bounds.top = snippet.at.y + 5;
			if(options.returnType == text_params_t::RECTS)
				options.returnRects.push_back(bounds);
			str_to_draw.setColor(options.hilite_fg);
			bounds.inset(0,-4);
			fill_rect(dest_window, bounds, options.hilite_bg);
		} else str_to_draw.setColor(options.style.colour);
		dest_window.draw(str_to_draw);
		if(options.style.font == FONT_BOLD) {
			str_to_draw.move(1, 0);
			dest_window.draw(str_to_draw);
		}
	}
}

size_t string_length(std::string str, TextStyle style, short* height){
	size_t total_width = 0;
	
	sf::Text text;
	style.applyTo(text);
	text.setString(str);
	total_width = text.getLocalBounds().width;
	if(height) *height = text.getLocalBounds().height;
	return total_width;
}

rectangle calc_rect(short i, short j){
	rectangle base_rect = {0,0,36,28};
	
	base_rect.offset(i * 28, j * 36);
	return base_rect;
}

graf_pos cCustomGraphics::find_graphic(pic_num_t which_rect, bool party) {
	bool valid = true;
	if(party && !party_sheet) valid = false;
	else if(!party && !is_old && (which_rect / 100) >= numSheets)
		valid = false;
	else if(numSheets == 0) valid = false;
	if(!valid) {
	INVALID:
		sf::Texture* blank = ResMgr::get<ImageRsrc>("blank").get();
		return {blank, {0,0,36,28}};
	}
	short sheet = which_rect / 100;
	if(is_old || party) sheet = 0;
	else which_rect %= 100;
	rectangle store_rect = {0,0,36,28};
	
	store_rect.offset(28 * (which_rect % 10),36 * (which_rect / 10));
	sf::Texture* the_sheet = party ? party_sheet.get() : &sheets[sheet];
	rectangle test(*the_sheet);
	if((store_rect & test) != store_rect) goto INVALID; // FIXME: HACK
	return std::make_pair(the_sheet,store_rect);
}

size_t cCustomGraphics::count(bool party) {
	if(!party && sheets == nullptr) return 0;
	else if(party && party_sheet == nullptr) return 0;
	else if(is_old || party) {
		rectangle bounds(party ? *party_sheet : sheets[0]);
		if(bounds.width() < 280) return bounds.width() / 28;
		return 10 * bounds.height() / 36;
	} else {
		size_t count = 100 * (numSheets - 1);
		rectangle bounds(sheets[numSheets - 1]);
		if(bounds.width() < 280) count += bounds.width() / 28;
		else count += 10 * bounds.height() / 36;
		return count;
	}
}

void cCustomGraphics::copy_graphic(pic_num_t dest, pic_num_t src, size_t numSlots) {
	if(numSlots < 1) return;
	if(!party_sheet) {
		sf::Image empty;
		empty.create(280, 180, sf::Color::Transparent);
		party_sheet.reset(new sf::Texture);
		party_sheet->create(280, 180);
		party_sheet->update(empty);
		numSheets = 1;
	}
	size_t havePics = count();
	if(havePics < dest + numSlots) {
		int addRows = 1;
		while(havePics + 10 * addRows < dest + numSlots)
			addRows++;
		sf::RenderTexture temp;
		temp.create(280, party_sheet->getSize().y + 36 * addRows);
		temp.clear(sf::Color::Transparent);
		rect_draw_some_item(*party_sheet, rectangle(*party_sheet), temp, rectangle(*party_sheet));
		*party_sheet = temp.getTexture();
	}
	sf::Texture* from_sheet;
	sf::Texture* to_sheet;
	sf::Texture* last_src = nullptr;
	sf::RenderTexture temp;
	rectangle from_rect, to_rect;
	for(size_t i = 0; i < numSlots; i++) {
		graf_pos_ref(from_sheet, from_rect) = find_graphic(src + i);
		graf_pos_ref(to_sheet, to_rect) = find_graphic(dest + i, true);
		if(to_sheet != last_src) {
			if(last_src) *last_src = temp.getTexture();
			last_src = to_sheet;
			temp.create(to_sheet->getSize().x, to_sheet->getSize().y);
			rect_draw_some_item(*to_sheet, rectangle(*to_sheet), temp, rectangle(*to_sheet));
		}
		rect_draw_some_item(*from_sheet, from_rect, temp, to_rect);
	}
	*last_src = temp.getTexture();
}

extern std::string scenario_temp_dir_name;
void cCustomGraphics::convert_sheets() {
	if(!is_old) return;
	int num_graphics = count();
	is_old = false;
	sf::Image old_graph = sheets[0].copyToImage();
	delete[] sheets;
	numSheets = num_graphics / 100;
	if(num_graphics % 100) numSheets++;
	sheets = new sf::Texture[numSheets];
	extern fs::path tempDir;
	fs::path pic_dir = tempDir/scenario_temp_dir_name/"graphics";
	for(size_t i = 0; i < numSheets; i++) {
		sf::IntRect subrect;
		subrect.top = i * 280;
		subrect.width = 280;
		subrect.height = 360;
		
		sf::Image sheet;
		sheet.create(280, 360);
		sheet.copy(old_graph, 0, 0, subrect);
		
		sheets[i].create(280, 360);
		sheets[i].update(sheet);

		fs::path sheetPath = pic_dir/("sheet" + std::to_string(i) + ".png");
		sheets[i].copyToImage().saveToFile(sheetPath.string().c_str());
	}
	ResMgr::pushPath<ImageRsrc>(pic_dir);
}

void cCustomGraphics::replace_sheet(size_t num, sf::Image& newSheet) {
	if(num >= numSheets) return; // TODO: Fail silently? Is that a good idea?
	sheets[num].loadFromImage(newSheet);
	// Then we need to do some extra stuff to ensure the dialog engine also sees the change
	extern fs::path tempDir;
	std::string sheetname = "sheet" + std::to_string(num);
	fs::path tmpPath = tempDir/scenario_temp_dir_name/"graphics"/(sheetname + ".png");
	newSheet.saveToFile(tmpPath.string().c_str());
	ResMgr::free<ImageRsrc>(sheetname);
}

void cCustomGraphics::init_sheet(size_t num) {
	sheets[num].create(280,360);
	sf::Image fill1, fill2;
	fill1.create(28,36,{0xff,0xff,0xc0});
	fill2.create(28,36,{0xc0,0xff,0xc0});
	for(int y = 0; y < 10; y++) {
		for(int x = 0; x < 10; x++) {
			if(x % 2 == y % 2) {
				sheets[num].update(fill1.getPixelsPtr(), 28, 36, x * 28, y * 36);
			} else {
				sheets[num].update(fill2.getPixelsPtr(), 28, 36, x * 28, y * 36);
			}
		}
	}
}

// TODO: This doesn't belong in this file
std::string get_str(std::string list, short j){
	if(j == 0) return list;
	StringRsrc& strings = *ResMgr::get<StringRsrc>(list);
	return strings[j - 1];
}

extern const std::vector<m_pic_index_t> m_pic_index = {
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
};

// TODO: Put these classes in a header?
class EllipseShape : public sf::Shape {
	float divSz;
	int points;
	float a, b;
public:
	explicit EllipseShape(sf::Vector2f size, std::size_t points = 30) : points(points) {
		a = size.x / 2.0f;
		b = size.y / 2.0f;
		divSz = 2 * pi<float>() / points;
		update();
	}
	
	std::size_t getPointCount() const override {
		return points;
	}
	
	sf::Vector2f getPoint(std::size_t i) const override {
		float t = i * divSz;
		return sf::Vector2f(a + a*sin(t), b + b*cos(t));
	}
	
	// TODO: Additional functions?
};

class RoundRectShape : public sf::Shape {
	float divSz;
	int points;
	float w,h,r;
public:
	RoundRectShape(sf::Vector2f size, float cornerRadius, std::size_t points = 32) : points(points / 4) {
		w = size.x;
		h = size.y;
		r = cornerRadius;
		divSz = 2 * pi<float>() / points;
		update();
	}
	
	std::size_t getPointCount() const override {
		return points * 4;
	}
	
	sf::Vector2f getPoint(std::size_t i) const override {
		const float pi = ::pi<float>();
		const float half_pi = 0.5 * pi;
		float t = i * divSz;
		switch(i / points) {
			case 0: // top left corner
				return {r + r*sinf(t + pi), r + r*cosf(t + pi)};
			case 1: // bottom left corner
				return {r + r*cosf(t + half_pi), h - r + r*sinf(t - half_pi)};
			case 2: // bottom right corner
				return {w - r + r*cosf(t + half_pi), h - r - r*sinf(t + half_pi)};
			case 3: // top right corner
				return {w - r - r*cosf(t - half_pi), r + r*sinf(t - half_pi)};
		}
		// Unreachable
		std::cerr << "Whoops, rounded rectangle had bad point!" << std::endl;
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
	shape.setOutlineThickness(-1.0);
	target.draw(shape);
}

void fill_rect(sf::RenderTarget& target, rectangle rect, sf::Color colour) {
	sf::RectangleShape fill(sf::Vector2f(rect.width(), rect.height()));
	fill_shape(target, fill, rect.left, rect.top, colour);
}

void frame_rect(sf::RenderTarget& target, rectangle rect, sf::Color colour) {
	sf::RectangleShape frame(sf::Vector2f(rect.width(), rect.height()));
	frame_shape(target, frame, rect.left, rect.top, colour);
}

void fill_roundrect(sf::RenderTarget& target, rectangle rect, int rad, sf::Color colour) {
	RoundRectShape fill(sf::Vector2f(rect.width(), rect.height()), rad);
	fill_shape(target, fill, rect.left, rect.top, colour);
}

void frame_roundrect(sf::RenderTarget& target, rectangle rect, int rad, sf::Color colour) {
	RoundRectShape frame(sf::Vector2f(rect.width(), rect.height()), rad);
	frame_shape(target, frame, rect.left, rect.top, colour);
}

void fill_circle(sf::RenderTarget& target, rectangle rect, sf::Color colour) {
	EllipseShape fill(sf::Vector2f(rect.width(), rect.height()));
	fill_shape(target, fill, rect.left, rect.top, colour);
}

void frame_circle(sf::RenderTarget& target, rectangle rect, sf::Color colour) {
	EllipseShape frame(sf::Vector2f(rect.width(), rect.height()));
	frame_shape(target, frame, rect.left, rect.top, colour);
}

void fill_region(sf::RenderWindow& target, Region& region, sf::Color colour) {
	clip_region(target, region);
	fill_rect(target, rectangle(target), colour);
	undo_clip(target);
}

void Region::addEllipse(rectangle frame) {
	EllipseShape* ellipse = new EllipseShape(sf::Vector2f(frame.width(), frame.height()));
	ellipse->setFillColor(sf::Color::Black);
	shapes.push_back(std::shared_ptr<sf::Shape>(ellipse));
}

void Region::addRect(rectangle rect){
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

rectangle Region::getEnclosingRect() {
	if(shapes.empty()) return rectangle();
	rectangle bounds = shapes[0]->getGlobalBounds();
	for(size_t i = 0; i < shapes.size(); i++) {
		rectangle shapeRect = shapes[i]->getGlobalBounds();
		if(shapeRect.top < bounds.top) bounds.top = shapeRect.top;
		if(shapeRect.left < bounds.left) bounds.top = shapeRect.top;
		if(shapeRect.bottom > bounds.bottom) bounds.top = shapeRect.top;
		if(shapeRect.right > bounds.right) bounds.top = shapeRect.top;
	}
	return bounds;
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

void clip_rect(sf::RenderTarget& where, rectangle rect) {
	rect &= rectangle(where); // Make sure we don't draw out of bounds
	// TODO: Make sure this works for the scissor test...
	setActiveRenderTarget(where);
	glEnable(GL_SCISSOR_TEST);
	glScissor(rect.left, rectangle(where).height() - rect.bottom, rect.width(), rect.height());
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

struct tessel_t {
	sf::RenderTexture* tessel;
	sf::Texture* img;
	rectangle srcRect;
};

bool operator==(const tessel_ref_t& a, const tessel_ref_t& b) {
	return a.key == b.key;
}

template<> struct std::hash<tessel_ref_t> {
	size_t operator()(tessel_ref_t key) const {
		return key.key;
	}
};

std::unordered_map<tessel_ref_t, tessel_t> tiling_reservoir;
static int tessel_index = 0;

tessel_ref_t prepareForTiling(sf::Texture& srcImg, rectangle srcRect) {
	tessel_ref_t ref = {tessel_index++};
	tiling_reservoir[ref].img = &srcImg;
	tiling_reservoir[ref].srcRect = srcRect;
	tiling_reservoir[ref].tessel = new sf::RenderTexture;
	tiling_reservoir[ref].tessel->create(srcRect.width(), srcRect.height());
	rectangle tesselRect(*tiling_reservoir[ref].tessel);
	rect_draw_some_item(srcImg, srcRect, *tiling_reservoir[ref].tessel, tesselRect);
	tiling_reservoir[ref].tessel->display();
	tiling_reservoir[ref].tessel->setRepeated(true);
	return ref;
}


static void register_main_patterns() {
	rectangle bw_rect = {0,0,8,8};
	sf::Texture& bg_gworld = *ResMgr::get<ImageRsrc>("pixpats");
	sf::Texture& bw_gworld = *ResMgr::get<ImageRsrc>("bwpats");
	for(int i = 0; i < 21; i++) {
		if(i < 6) {
			bw_pats[i] = prepareForTiling(bw_gworld, bw_rect);
			bw_rect.offset(8,0);
		}
		bg[i] = prepareForTiling(bg_gworld, bg_rects[i]);
	}
}

void tileImage(sf::RenderTarget& target, rectangle area, tessel_ref_t tessel, sf::BlendMode mode) {
	// First, set up a dictionary of all textures ever tiled.
	// The key type is a pair<Texture*,rectangle>.
	// The value type is a Texture.
	tessel_t& tesselInfo = tiling_reservoir[tessel];
	rectangle clipArea = area;
	area.left -= area.left % tesselInfo.srcRect.width();
	area.top -= area.top % tesselInfo.srcRect.height();
	area &= rectangle(target); // Make sure we don't draw out of bounds
	
	sf::RectangleShape tesselShape(sf::Vector2f(area.width(),area.height()));
	tesselShape.setTexture(&tesselInfo.tessel->getTexture());
	tesselShape.setTextureRect(area);
	tesselShape.setPosition(area.left, area.top);
	sf::RenderStates renderMode(mode);
	setActiveRenderTarget(target);
	clip_rect(target, clipArea);
	target.draw(tesselShape, renderMode);
	undo_clip(target);
}

short can_see(location p1,location p2,std::function<short(short,short)> get_obscurity) {
	short storage = 0;
	
	if(p1.y == p2.y) {
		if(p1.x > p2.x) {
			for(short count = p2.x + 1; count < p1.x; count++)
				storage += get_obscurity(count, p1.y);
		} else {
			for(short count = p1.x + 1; count < p2.x; count++)
				storage += get_obscurity(count, p1.y);
		}
	} else if(p1.x == p2.x) {
		if(p1.y > p2.y) {
			for(short count = p1.y - 1; count > p2.y; count--)
				storage += get_obscurity(p1.x, count);
		} else {
			for(short count = p1.y + 1; count < p2.y; count++) 
				storage += get_obscurity(p1.x, count);
		}
	} else {
		short dx = p2.x - p1.x;
		short dy = p2.y - p1.y;
		
		if(abs(dy) > abs(dx)) {
			if(p2.y > p1.y) {
				for(short count = 1; count < dy; count++)
					storage += get_obscurity(p1.x + (count * dx) / dy, p1.y + count);
			} else {
				for(short count = -1; count > dy; count--)
					storage += get_obscurity(p1.x + (count * dx) / dy, p1.y + count);
			}
		}
		if(abs(dy) <= abs(dx)) {
			if(p2.x > p1.x) {
				for(short count = 1; count < dx; count++)
					storage += get_obscurity(p1.x + count, p1.y + (count * dy) / dx);
			} else {
				for(short count = -1; count > dx; count--)
					storage += get_obscurity(p1.x + count, p1.y + (count * dy) / dx);
			}
		}
	}
	return storage;
}
