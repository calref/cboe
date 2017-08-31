//
//  gfxsheets.cpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-14.
//
//

#include "gfxsheets.hpp"

#include "location.hpp"
#include "res_image.hpp"
#include "render_image.hpp"

bool use_win_graphics = false;

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

extern const std::vector<m_pic_index_t> m_pic_index = {
	{1, 1, 1},   {2, 1, 1},   {3, 1, 1},   {4, 1, 1},   {5, 1, 1},
	{6, 1, 1},   {7, 1, 1},   {8, 1, 1},   {9, 1, 1},   {10, 1, 1},
	{11, 1, 1},  {12, 1, 1},  {13, 1, 1},  {14, 1, 1},  {15, 1, 1},
	{16, 1, 1},  {17, 1, 1},  {18, 1, 1},  {19, 1, 1},  {20, 1, 1},
	{21, 1, 1},  {22, 1, 1},  {23, 1, 1},  {24, 1, 1},  {25, 1, 1},
	{26, 1, 1},  {27, 1, 1},  {28, 1, 1},  {29, 1, 1},  {30, 1, 1},
	{31, 1, 1},  {32, 1, 1},  {33, 1, 1},  {34, 1, 1},  {35, 1, 1},
	{36, 1, 1},  {37, 1, 1},  {38, 1, 1},  {39, 1, 1},  {40, 1, 1},
	{41, 1, 1},  {42, 1, 1},  {43, 1, 1},  {44, 1, 1},  {46, 1, 1},
	{47, 1, 1},  {48, 1, 1},  {49, 1, 1},  {50, 1, 1},  {51, 1, 2},
	{53, 1, 2},  {55, 1, 2},  {57, 1, 2},  {59, 1, 1},  {60, 1, 1},
	{61, 1, 1},  {62, 1, 1},  {63, 1, 1},  {64, 1, 1},  {65, 1, 1},
	{66, 1, 1},  {67, 1, 1},  {68, 1, 1},  {69, 1, 1},  {70, 1, 1},
	{71, 1, 1},  {72, 1, 1},  {73, 1, 1},  {74, 1, 1},  {75, 1, 1},
	{76, 1, 1},  {77, 1, 1},  {78, 1, 1},  {79, 2, 1},  {81, 1, 1},
	{82, 1, 1},  {83, 1, 2},  {85, 1, 1},  {86, 1, 1},  {87, 1, 1},
	{88, 1, 1},  {89, 1, 1},  {90, 1, 1},  {91, 1, 1},  {92, 1, 1},
	{93, 1, 1},  {94, 1, 1},  {95, 1, 1},  {96, 1, 1},  {97, 1, 1},
	{98, 1, 1},  {99, 1, 1},  {100, 1, 1}, {101, 1, 1}, {102, 1, 1},
	{103, 1, 1}, {104, 1, 1}, {105, 1, 1}, {106, 1, 1}, {107, 1, 1},
	{108, 1, 1}, {109, 2, 1}, {111, 1, 1}, {112, 1, 1}, {113, 1, 1},
	{114, 2, 1}, {116, 1, 1}, {117, 1, 1}, {118, 1, 1}, {119, 1, 1},
	{120, 2, 1}, {122, 1, 1}, {123, 1, 2}, {125, 1, 2}, {127, 1, 1},
	{128, 1, 1}, {129, 1, 1}, {130, 1, 1}, {131, 2, 2}, {135, 1, 1},
	{136, 1, 1}, {137, 2, 1}, {139, 1, 1}, {140, 1, 1}, {141, 1, 1},
	{142, 1, 1}, {143, 1, 1}, {144, 1, 1}, {145, 1, 1}, {146, 1, 1},
	{147, 1, 1}, {148, 1, 1}, {149, 1, 1}, {150, 1, 1}, {151, 1, 1},
	{152, 1, 1}, {153, 1, 1}, {154, 1, 1}, {155, 2, 2}, {159, 1, 1},
	{160, 2, 2}, {164, 2, 1}, {166, 2, 1}, {168, 1, 2}, {170, 1, 1},
	{171, 1, 1}, {172, 1, 1}, {173, 1, 1}, {174, 1, 1}, {175, 1, 1},
	{176, 1, 1}, {177, 1, 1}, {178, 1, 1}, {179, 1, 1}, {180, 1, 1},
	{181, 1, 1}, {182, 1, 1}, {183, 1, 1}, {184, 1, 1}, {185, 1, 1},
	{186, 1, 1}, {187, 1, 1}, {188, 1, 1}, {189, 1, 1}, {190, 1, 1},
	{191, 1, 1}, {192, 1, 1}, {193, 1, 1}, {194, 1, 1}, {195, 1, 1},
	{196, 1, 1}, {197, 1, 1}, {198, 1, 1},
};
