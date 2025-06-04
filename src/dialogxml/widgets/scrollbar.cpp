//
//  scrollbar.cpp
//  BoE
//
//  Created by Celtic Minstrel on 14-03-28.
//
//

#include "scrollbar.hpp"
#include "dialogxml/dialogs/dialog.hpp"
#include "dialogxml/widgets/container.hpp"
#include "fileio/resmgr/res_image.hpp"
#include "gfx/render_image.hpp"
#include "gfx/render_shapes.hpp"
#include "mathutil.hpp"
#include "tools/cursors.hpp"
#include "replay.hpp"
#include <boost/lexical_cast.hpp>
#include "winutil.hpp"

std::string cScrollbar::scroll_textures[NUM_STYLES] = {
	"dlogscrollwh",
	"dlogscrollled",
};

cScrollbar::cScrollbar(iComponent& parent)
	: cControl(CTRL_SCROLL, parent)
	, pos(0)
	, max(0)
	, pgsz(10)
{}

void cScrollbar::init() {
}

bool cScrollbar::isClickable() const {
	return true;
}

bool cScrollbar::isFocusable() const {
	return false;
}

bool cScrollbar::isScrollable() const {
	return true;
}

void cScrollbar::setPosition(long newPos, bool record) {
	if(record && recording){
		std::map<std::string,std::string> info;
		info["name"] = name;
		if(info["name"].empty()) {
			if(auto pane = dynamic_cast<cContainer*>(&getParent())){
				info["name"] = pane->getName();
			}
		}
		// Might as well record newPos before it gets clamped, so replays will verify that clamping
		// still works.
		info["newPos"] = boost::lexical_cast<std::string>(newPos);
		record_action("scrollbar_setPosition", info);
	}
	pos = minmax(0,max,newPos);
}

void cScrollbar::setMaximum(long newMax) {
	max = ::max(0,newMax);
	setPosition(pos);
}

void cScrollbar::setPageSize(long newSize) {
	pgsz = newSize;
}

void cScrollbar::setVertical(bool is) {
	vert = is;
}

void cScrollbar::setLink(std::string l) {
	link = l;
}

void cScrollbar::setStyle(eScrollStyle newStyle) {
	style = newStyle;
}

void cScrollbar::set_wheel_event_rect(rectangle rect) {
	this->wheel_event_rect = rect;
}

long cScrollbar::getPosition() const {
	return pos;
}

long cScrollbar::getMaximum() const {
	return max;
}

long cScrollbar::getPageSize() const {
	return pgsz;
}

bool cScrollbar::isVertical() const {
	return vert;
}

std::string cScrollbar::getLink() const {
	return link;
}

eScrollStyle cScrollbar::getStyle() const {
	return style;
}

bool cScrollbar::handle_event(const sf::Event& event) {
	// Not visible -> not interested
	if(!this->isVisible())
		return false;
	
	// Visible but no maximum -> not interested
	if(this->getMaximum() == 0) {
		return false;
	}
		
	switch(event.type) {
		case sf::Event::MouseButtonPressed:
			return this->handle_mouse_pressed(event);
		case sf::Event::MouseMoved:
			return this->handle_mouse_moved(event);
		case sf::Event::MouseButtonReleased:
			return this->handle_mouse_released(event);
		case sf::Event::MouseWheelMoved: // TODO: Deprecated in latest SFML
			return this->handle_mouse_wheel_scrolled(event);
		default: break;
	}

	return false;
}

bool cScrollbar::handle_mouse_wheel_scrolled(const sf::Event& event) {
	location event_location = this->translated_location({
		event.mouseWheel.x,
		event.mouseWheel.y
	});
	
	// Scrolling outside of catching area or own frame -> not interested.
	if(!(event_location.in(this->wheel_event_rect) || event_location.in(this->getBounds())))
		return false;
	
	this->setPosition(this->getPosition() - event.mouseWheel.delta, true);
	
	return true;
}
 
// Given a (translated) location, determine which part of the scrollbar it corresponds to
auto cScrollbar::location_to_part(const location& location) const -> eScrollbarPart {
	
	eScrollbarPart part;
	
	// Yes, this is a mess, but at least it's relatively small.
	int clickPos       = this->vert ? location.y           : location.x;
	int bar_start      = this->vert ? this->frame.top      : this->frame.left;
	int bar_end        = this->vert ? this->frame.bottom   : this->frame.right;
	int total_bar_size = this->vert ? this->frame.height() : this->frame.width();

	int btn_size  = this->vert
		? this->up_rect[this->style][VERT].height()
		: this->up_rect[this->style][HORZ].width();

	int bar_size = total_bar_size - btn_size * 2;
	int thumbPos = bar_start + btn_size + this->pos * (bar_size - btn_size) / this->max;

	if(clickPos < bar_start + btn_size) {
		part = PART_UP;
	} else if(clickPos < thumbPos) {
		part = PART_PGUP;
	} else if(clickPos < thumbPos + btn_size) {
		part = PART_THUMB;
	} else if(clickPos < bar_end - btn_size) {
		part = PART_PGDN;
	} else {
		part = PART_DOWN;
	}
	
	return part;
}

bool cScrollbar::handle_mouse_pressed(const sf::Event& event) {
	location event_location = this->translated_location({
		event.mouseButton.x,
		event.mouseButton.y
	});
	
	// Mouse pressed somewhere outside -> not interested
	if(!event_location.in(this->getBounds())) return false;
	
	// NOTE: depressed actually means pressed
	this->depressed   = true;
	this->pressedPart = this->location_to_part(event_location);
	
	// If the thumb is being dragged, record the initial click location
	if(this->pressedPart == PART_THUMB) {
		this->mouse_pressed_at = event_location;
		this->drag_start_position = this->pos;
	}
	
	return true;
}

bool cScrollbar::handle_mouse_moved(const sf::Event& event) {
	// Mouse movements while not pressed -> not interested
	// NOTE: depressed actually means pressed
	if(!this->depressed) return false;

	// is there a need for restore_cursor() anywhere around here, and why?
	
	location event_location = this->translated_location({
		event.mouseMove.x,
		event.mouseMove.y
	});
	
	if(this->pressedPart == PART_THUMB) {
		// Thumb being dragged.
		this->handle_thumb_drag(event_location);
		return true;
	} else {
		// Dragging something ... but not thumb
		if(!event_location.in(this->getBounds())) {
			// Mouse was moved out of the scrollbar and not dragging thumb
			
			// NOTE: depressed actually means pressed
			this->depressed = false;
			
			// The event is outside the scrollbar so someone else might want to consume this.
			return false;
		}
	}
	
	// Dragging something, but not thumb, but still within scrollbar bounds. Okay.jpg.
	return true;
}

void cScrollbar::handle_thumb_drag(const location& event_location) {
	int bar_start = this->vert ? this->frame.top    : this->frame.left;
	int bar_end   = this->vert ? this->frame.bottom : this->frame.right;
	int btn_size  = this->vert
		? this->up_rect[this->style][VERT].height()
		: this->up_rect[this->style][HORZ].width();

	// XXX A lot of this looks like it can be precalculated once, but
	// be careful with the integer rounding.
	
	int thumb_min_position = bar_start + btn_size;
	int thumb_max_position = bar_end - 2 * btn_size; // <- two button sizes: one for the top/left arrow and one for the thumb

	int start   = this->vert ? this->mouse_pressed_at.y : this->mouse_pressed_at.x;
	int current = this->vert ? event_location.y         : event_location.x;
	
	// This is done in this particular way to minimize integer rounding
	// that would otherwise heavily impact the result (when max is
	// significantly large compared to the pixel size of the scrollbar).
	int diff_in_steps = (current - start) * this->max / (thumb_max_position - thumb_min_position);

	this->setPosition(this->drag_start_position + diff_in_steps, true);
}

void cScrollbar::handlePressedPart(eScrollbarPart pressedPart) {
	long newPos = -1;
	switch(pressedPart) {
		case PART_UP:   newPos = pos - 1; break;
		case PART_DOWN: newPos = pos + 1; break;
		
		case PART_PGUP: newPos = pos - this->pgsz; break;
		case PART_PGDN: newPos = pos + this->pgsz; break;
		
		case PART_THUMB: break;
		
		default: break;
	}
	if(newPos != -1){
		setPosition(newPos, true);
	}

}

bool cScrollbar::handle_mouse_released(const sf::Event&) {
	// Mouse released while not pressed -> not interested
	// NOTE: depressed actually means pressed
	if(!this->depressed) return false;

	handlePressedPart(this->pressedPart);

	// NOTE: depressed actually means pressed
	this->depressed = false;

	return true;
}

bool cScrollbar::handleClick(location where, cFramerateLimiter& fps_limiter) {
	if(max == 0) return false;
	sf::Event e;
	bool done = false, clicked = false;
	depressed = true;
	int btn_size = vert ? up_rect[style][VERT].height() : up_rect[style][HORZ].width();
	int total_bar_size = vert ? frame.height() : frame.width();
	int bar_start = vert ? frame.top : frame.left;
	int bar_end = vert ? frame.bottom : frame.right;
	int bar_size = total_bar_size - btn_size * 2;
	int thumbPos = bar_start + btn_size + pos * (bar_size - btn_size) / max;
	int clickPos = vert ? where.y : where.x;
	if(clickPos < bar_start + btn_size)
		pressedPart = PART_UP;
	else if(clickPos < thumbPos)
		pressedPart = PART_PGUP;
	else if(clickPos < thumbPos + btn_size)
		pressedPart = PART_THUMB;
	else if(clickPos < bar_end - btn_size)
		pressedPart = PART_PGDN;
	else pressedPart = PART_DOWN;
	int diff = clickPos - thumbPos;
	while(!done){
		redraw();
		while(pollEvent(getWindow(), e)){
			location mouseLoc = sf::Mouse::getPosition(getWindow());
			mouseLoc = getWindow().mapPixelToCoords(mouseLoc);
			int mousePos = vert ? mouseLoc.y : mouseLoc.x;
			if(e.type == sf::Event::MouseButtonReleased){
				done = true;
				location clickLoc(e.mouseButton.x, e.mouseButton.y);
				clickLoc = getWindow().mapPixelToCoords(clickLoc);
				clicked = frame.contains(clickLoc);
				depressed = false;
				handlePressedPart(pressedPart);
			} else if(e.type == sf::Event::MouseMoved){
				restore_cursor();
				switch(pressedPart) {
					case PART_UP:
						depressed = mousePos < bar_start + btn_size;
						break;
					case PART_PGUP:
						depressed = mousePos >= bar_start + btn_size && mousePos < thumbPos;
						break;
					case PART_THUMB:
						depressed = true;
						// We want the pos that will make thumbPos = mousePos - diff
						// In draw(), thumbPos is calculated as bar_start + bar_thickness + pos * (bar_size - bar_thickness) / max
						// So solving for pos gives (mousePos - diff - bar_start - bar_thickness) * max / (bar_size - bar_thickness)
						setPosition((mousePos - diff - bar_start - btn_size) * max / (bar_size - btn_size), true);
						break;
					case PART_PGDN:
						depressed = mousePos >= thumbPos + btn_size && mousePos < bar_end - btn_size;
						break;
					case PART_DOWN:
						depressed = mousePos >= bar_end - btn_size;
						break;
				}
				location toLoc(e.mouseMove.x, e.mouseMove.y);
				toLoc = getWindow().mapPixelToCoords(toLoc);
				if(pressedPart != PART_THUMB && !frame.contains(toLoc)) depressed = false;
			}
			if(getDialog() && !link.empty())
				getDialog()->getControl(link).setTextToNum(pos);
			thumbPos = bar_start;
			thumbPos += btn_size + pos * (bar_size - btn_size) / max;
			thumbPos = minmax(mousePos,bar_end - btn_size * 2,thumbPos);
		}
		fps_limiter.frame_finished();
	}
	redraw();
	return clicked;
}

const rectangle cScrollbar::up_rect[NUM_STYLES][4] = {
	{{0,0,16,16}, {16,0,32,16}, {32,0,48,16}, {48,0,64,16}},
	{{0,0,10,14}, {10,0,20,14}, {20,0,34,10}, {34,0,48,10}},
};
const rectangle cScrollbar::down_rect[NUM_STYLES][4] = {
	{{0,16,16,32}, {16,16,32,32}, {32,16,48,32}, {48,16,64,32}},
	{{0,14,10,28}, {10,14,20,28}, {20,10,34,20}, {34,10,48,20}},
};
const rectangle cScrollbar::thumb_rect[NUM_STYLES][4] = {
	{{0,32,16,48}, {16,32,32,48}, {32,32,48,48}, {48,32,64,48}},
	{{0,28,10,42}, {10,28,20,42}, {20,20,34,30}, {34,20,48,30}},
};
const rectangle cScrollbar::bar_rect[NUM_STYLES][4] = {
	{{0,48,16,64}, {16,48,32,64}, {32,48,48,64}, {48,48,64,64}},
	{{0,42,10,56}, {10,42,20,56}, {20,30,34,40}, {34,30,48,40}},
};

void cScrollbar::draw_horizontal() {
	int btn_size = up_rect[style][HORZ].width();
	int bar_width = frame.width() - btn_size * 2;
	getWindow().setActive();
	rectangle draw_rect = frame, from_rect = up_rect[style][HORZ];
	draw_rect.width() = btn_size;
	if(depressed && pressedPart == PART_UP)
		from_rect = up_rect[style][HORZ_PRESSED];
	sf::Texture scroll_gw = *ResMgr::graphics.get(scroll_textures[style]);
	rect_draw_some_item(scroll_gw, from_rect, getWindow(), draw_rect);
	if(pos > 0) {
		from_rect = bar_rect[style][HORZ];
		int left = draw_rect.right, width = pos * (bar_width - btn_size) / max;
		if(depressed && pressedPart == PART_PGUP)
			from_rect = bar_rect[style][HORZ_PRESSED];
		draw_rect.left = left;
		while(draw_rect.left - left < width) {
			draw_rect.right = draw_rect.left + btn_size;
			rect_draw_some_item(scroll_gw, from_rect, getWindow(), draw_rect);
			draw_rect.left = draw_rect.right;
		}
		draw_rect.right = left + width;
	}
	if(max > 0) {
		draw_rect.left = draw_rect.right;
		draw_rect.width() = btn_size;
		from_rect = thumb_rect[style][HORZ];
		if(depressed && pressedPart == PART_THUMB)
			from_rect = thumb_rect[style][HORZ_PRESSED];
		rect_draw_some_item(scroll_gw, from_rect, getWindow(), draw_rect);
	}
	if(pos < max || max == 0) {
		from_rect = bar_rect[style][HORZ];
		int left = draw_rect.right, right = frame.right - btn_size;
		if(depressed && pressedPart == PART_PGDN)
			from_rect = bar_rect[style][HORZ_PRESSED];
		// Make sure the pattern lines up the same on both sides of the thumb
		int diff = left % btn_size - frame.left % btn_size;
		if(diff < 0) diff += btn_size;
		draw_rect.left = left - diff;
		draw_rect.right = draw_rect.left + btn_size;
		rectangle clip_rec = frame;
		clip_rec.left = left;
		clip_rec.right = draw_rect.right;
		clip_rect(getWindow(), clip_rec);
		rect_draw_some_item(scroll_gw, from_rect, getWindow(), draw_rect);
		undo_clip(getWindow());
		draw_rect.left += btn_size;
		while(draw_rect.left < right) {
			draw_rect.right = draw_rect.left + btn_size;
			rect_draw_some_item(scroll_gw, from_rect, getWindow(), draw_rect);
			draw_rect.left = draw_rect.right;
		}
		draw_rect.right = right;
	}
	draw_rect = frame;
	draw_rect.left = draw_rect.right - btn_size;
	from_rect = down_rect[style][HORZ];
	if(depressed && pressedPart == PART_DOWN)
		from_rect = down_rect[style][HORZ_PRESSED];
	rect_draw_some_item(scroll_gw, from_rect, getWindow(), draw_rect);
}

void cScrollbar::draw_vertical() {
	int btn_size = up_rect[style][VERT].height();
	int bar_height = frame.height() - btn_size * 2;
	getWindow().setActive();
	rectangle draw_rect = frame, from_rect = up_rect[style][VERT];
	draw_rect.height() = btn_size;
	if(depressed && pressedPart == PART_UP)
		from_rect = up_rect[style][VERT_PRESSED];
	sf::Texture scroll_gw = *ResMgr::graphics.get(scroll_textures[style]);
	rect_draw_some_item(scroll_gw, from_rect, getWindow(), draw_rect);
	if(pos > 0) {
		from_rect = bar_rect[style][VERT];
		int top = draw_rect.bottom, height = pos * (bar_height - btn_size) / max;
		if(depressed && pressedPart == PART_PGUP)
			from_rect = bar_rect[style][VERT_PRESSED];
		draw_rect.top = top;
		while(draw_rect.top - top < height) {
			draw_rect.bottom = draw_rect.top + btn_size;
			rect_draw_some_item(scroll_gw, from_rect, getWindow(), draw_rect);
			draw_rect.top = draw_rect.bottom;
		}
		draw_rect.bottom = top + height;
	}
	if(max > 0) {
		draw_rect.top = draw_rect.bottom;
		draw_rect.height() = btn_size;
		from_rect = thumb_rect[style][VERT];
		if(depressed && pressedPart == PART_THUMB)
			from_rect = thumb_rect[style][VERT_PRESSED];
		rect_draw_some_item(scroll_gw, from_rect, getWindow(), draw_rect);
	}
	if(pos < max || max == 0) {
		from_rect = bar_rect[style][VERT];
		int top = draw_rect.bottom, bottom = frame.bottom - btn_size;
		if(depressed && pressedPart == PART_PGDN)
			from_rect = bar_rect[style][VERT_PRESSED];
		// Make sure the pattern lines up the same on both sides of the thumb
		int diff = top % btn_size - frame.top % btn_size;
		if(diff < 0) diff += btn_size;
		draw_rect.top = top - diff;
		draw_rect.bottom = draw_rect.top + btn_size;
		rectangle clip_rec = frame;
		clip_rec.top = top;
		clip_rec.bottom = draw_rect.bottom;
		clip_rect(getWindow(), clip_rec);
		rect_draw_some_item(scroll_gw, from_rect, getWindow(), draw_rect);
		undo_clip(getWindow());
		draw_rect.top += btn_size;
		while(draw_rect.top < bottom) {
			draw_rect.bottom = draw_rect.top + btn_size;
			rect_draw_some_item(scroll_gw, from_rect, getWindow(), draw_rect);
			draw_rect.top = draw_rect.bottom;
		}
		draw_rect.bottom = bottom;
	}
	draw_rect = frame;
	draw_rect.top = draw_rect.bottom - btn_size;
	from_rect = down_rect[style][VERT];
	if(depressed && pressedPart == PART_DOWN)
		from_rect = down_rect[style][VERT_PRESSED];
	rect_draw_some_item(scroll_gw, from_rect, getWindow(), draw_rect);
}

void cScrollbar::draw() {
	if(!isVisible()) return;
	if(vert) draw_vertical();
	else draw_horizontal();
}

bool cScrollbar::parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) {
	std::string name = attr.Name();
	if(name == "vertical"){
		std::string val;
		attr.GetValue(&val);
		if(val == "true") vert = true;
		else vert = false;
	}else if(name == "style"){
		std::string val;
		attr.GetValue(&val);
		if(val == "white") style = SCROLL_WHITE;
		else if(val == "led") style = SCROLL_LED;
		else throw xBadVal("slider", name, val, attr.Row(), attr.Column(), fname);
	}else if(name == "link"){
		attr.GetValue(&link);
	}else if(name == "initial"){
		attr.GetValue(&pos);
	}else if(name == "max"){
		attr.GetValue(&max);
	}else if(name == "page-size"){
		attr.GetValue(&pgsz);
	}
	return cControl::parseAttribute(attr, tagName, fname);
}


void cScrollbar::validatePostParse(ticpp::Element& who, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& nodes) {
	cControl::validatePostParse(who, fname, attrs, nodes);
	if(!attrs.count("max")) throw xMissingAttr(who.Value(), "max", who.Row(), who.Column(), fname);
	if(pos > max) throw xBadVal(who.Value(), "initial", std::to_string(pos), who.Row(), who.Column(), fname);
	if(!attrs.count("vertical")) {
		// Try to guess whether it's horizontal or vertical based on its size
		int width, height;
		who.GetAttributeOrDefault("width", &width, 0);
		who.GetAttributeOrDefault("height", &height, 0);
		if(height > width) vert = true;
		else if(width > height) vert = false;
		else throw xMissingAttr(who.Value(), "vertical", who.Row(), who.Column(), fname);
	}
	/*
	int thickness = vert ? up_rect[style][VERT].width() : up_rect[style][HORZ].height();
	if(width > 0) {
		frame.right = frame.left + width;
		if(height == 0 && !foundVertical)
			vert = false;
	}else{
		if(vert) frame.width() = thickness;
		else frame.width() = 25;
	}
	if(height > 0) {
		frame.bottom = frame.top + height;
		if(width == 0 && !foundVertical)
			vert = true;
	}else{
		frame.bottom = frame.top + 10;
		if(vert) frame.height() = 25;
		else frame.height() = thickness;
	}
	*/
	if(getDialog()->hasControl(link))
		getDialog()->getControl(link).setTextToNum(pos);
}

location cScrollbar::getPreferredSize() const {
	if(vert) return {up_rect[style][VERT].width(), 0};
	else return {0, up_rect[style][HORZ].height()};
}

cControl::storage_t cScrollbar::store() const {
	storage_t storage = cControl::store();
	storage["scroll-pos"] = pos;
	storage["scroll-max"] = max;
	return storage;
}

void cScrollbar::restore(storage_t to) {
	cControl::restore(to);
	if(to.find("scroll-pos") != to.end())
		pos = boost::any_cast<int>(to["scroll-pos"]);
	if(to.find("scroll-max") != to.end())
		pos = boost::any_cast<int>(to["scroll-max"]);
	else pos = 0;
}
