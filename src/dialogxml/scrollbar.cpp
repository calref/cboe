//
//  scrollbar.cpp
//  BoE
//
//  Created by Celtic Minstrel on 14-03-28.
//
//

#include "scrollbar.hpp"
#include "restypes.hpp"
#include "graphtool.hpp"
#include "mathutil.hpp"

sf::Texture cScrollbar::scroll_gw;

cScrollbar::cScrollbar(cDialog& parent) : cControl(CTRL_SCROLL, parent) {}

cScrollbar::cScrollbar(sf::RenderWindow& parent) : cControl(CTRL_SCROLL, parent), pos(0), max(0), pgsz(10) {}

void cScrollbar::init() {
	scroll_gw.loadFromImage(*ResMgr::get<ImageRsrc>("dlogscroll"));
}

bool cScrollbar::isClickable(){
	return true;
}

void cScrollbar::setPosition(long newPos) {
	pos = minmax(0,max,newPos);
}

void cScrollbar::setMaximum(long newMax) {
	max = ::max(0,newMax);
	setPosition(pos);
}

void cScrollbar::setPageSize(long newSize) {
	pgsz = newSize;
}

long cScrollbar::getPosition() {
	return pos;
}

long cScrollbar::getMaximum() {
	return max;
}

long cScrollbar::getPageSize() {
	return pgsz;
}

void cScrollbar::attachClickHandler(click_callback_t f) throw(xHandlerNotSupported) {
	onClick = f;
}

void cScrollbar::attachFocusHandler(focus_callback_t) throw(xHandlerNotSupported) {
	throw xHandlerNotSupported(true);
}

bool cScrollbar::triggerClickHandler(cDialog& me, std::string id, eKeyMod mods) {
	// TODO: Implement detection of scrolling stuff, maybe even dragging the thumb
	if(onClick != NULL) return onClick(me,id,mods);
	return false;
}

bool cScrollbar::handleClick(location where) {
	if(max == 0) return false;
	sf::Event e;
	bool done = false, clicked = false;
	inWindow->setActive();
	depressed = true;
	int bar_height = frame.height() - 32;
	rectangle thumbRect = frame;
	thumbRect.top += 16 + pos * (bar_height - 16) / max;
	thumbRect.height() = 16;
	if(where.y < frame.top + 16)
		pressedPart = PART_UP;
	else if(where.y < thumbRect.top)
		pressedPart = PART_PGUP;
	else if(where.y < thumbRect.bottom)
		pressedPart = PART_THUMB;
	else if(where.y < frame.bottom - 16)
		pressedPart = PART_PGDN;
	else pressedPart = PART_DOWN;
	int dy = where.y - thumbRect.top;
	while(!done){
		redraw();
		if(!inWindow->pollEvent(e)) continue;
		if(e.type == sf::Event::MouseButtonReleased){
			done = true;
			clicked = frame.contains(e.mouseButton.x, e.mouseButton.y);
			depressed = false;
			switch(pressedPart) {
				case PART_UP: pos--; break;
				case PART_PGUP: pos -= pgsz; break;
				case PART_PGDN: pos += pgsz; break;
				case PART_DOWN: pos++; break;
				case PART_THUMB: break;
			}
		} else if(e.type == sf::Event::MouseMoved){
			restore_cursor();
			switch(pressedPart) {
				case PART_UP:
					depressed = e.mouseMove.y < frame.top + 16;
					break;
				case PART_PGUP:
					depressed = e.mouseMove.y >= frame.top + 16 && e.mouseMove.y < thumbRect.top;
					break;
				case PART_THUMB:
					depressed = true;
					// We want the pos that will make thumbRect.top = mousePos.y - dy
					// In draw(), thumbRect.top is calculated as frame.top + 16 + pos * (bar_height - 16) / max
					// So solving for pos gives (mousePos.y - dy - frame.top - 16) * max / (bar_height - 16)
					pos = (sf::Mouse::getPosition(*inWindow).y - dy - frame.top - 16) * max / (bar_height - 16);
					break;
				case PART_PGDN:
					depressed = e.mouseMove.y >= thumbRect.bottom && e.mouseMove.y < frame.bottom - 16;
					break;
				case PART_DOWN:
					depressed = e.mouseMove.y >= frame.bottom - 16;
					break;
			}
			if(pressedPart != PART_THUMB && !frame.contains(e.mouseMove.x, e.mouseMove.y)) depressed = false;
		}
		pos = minmax(0,max,pos);
		thumbRect.top = frame.top;
		thumbRect.top += 16 + pos * (bar_height - 16) / max;
		thumbRect.top = minmax(sf::Mouse::getPosition(*inWindow).y,frame.bottom - 32,thumbRect.top);
		thumbRect.height() = 16;
	}
	redraw();
	return clicked;
}

void cScrollbar::setFormat(eFormat prop, short) throw(xUnsupportedProp) {
	throw xUnsupportedProp(prop);
}

short cScrollbar::getFormat(eFormat prop) throw(xUnsupportedProp) {
	throw xUnsupportedProp(prop);
}

void cScrollbar::setColour(sf::Color) throw(xUnsupportedProp) {
	// TODO: Colour is unsupported
}

sf::Color cScrollbar::getColour() throw(xUnsupportedProp) {
	// TODO: Colour is unsupported
	return sf::Color();
}

void cScrollbar::draw() {
	if(!isVisible()) return;
	static const rectangle up_rect = {0,0,16,16}, down_rect = {0,16,16,32}, thumb_rect = {0,32,16,48}, bar_rect = {0,48,16,64};
	int bar_height = frame.height() - 32;
	inWindow->setActive();
	rectangle draw_rect = frame, from_rect = up_rect;
	draw_rect.height() = 16;
	if(depressed && pressedPart == PART_UP)
		from_rect.offset(0,16);
	rect_draw_some_item(scroll_gw, from_rect, *inWindow, draw_rect);
	if(pos > 0) {
		from_rect = bar_rect;
		int top = draw_rect.bottom, height = pos * (bar_height - 16) / max;
		if(depressed && pressedPart == PART_PGUP)
			from_rect.offset(0,16);
		draw_rect.top = top;
		while(draw_rect.top - top < height) {
			draw_rect.bottom = draw_rect.top + 16;
			rect_draw_some_item(scroll_gw, from_rect, *inWindow, draw_rect);
			draw_rect.top = draw_rect.bottom;
		}
		draw_rect.bottom = top + height;
	}
	if(max > 0) {
		draw_rect.top = draw_rect.bottom;
		draw_rect.height() = 16;
		from_rect = thumb_rect;
		if(depressed && pressedPart == PART_THUMB)
			from_rect.offset(0,16);
		rect_draw_some_item(scroll_gw, from_rect, *inWindow, draw_rect);
	}
	if(pos < max || max == 0) {
		from_rect = bar_rect;
		int top = draw_rect.bottom, bottom = frame.bottom - 16;
		if(depressed && pressedPart == PART_PGDN)
			from_rect.offset(0,16);
		draw_rect.top = top;
		while(draw_rect.top < bottom) {
			draw_rect.bottom = draw_rect.top + 16;
			rect_draw_some_item(scroll_gw, from_rect, *inWindow, draw_rect);
			draw_rect.top = draw_rect.bottom;
		}
		draw_rect.bottom = bottom;
	}
	draw_rect = frame;
	draw_rect.top = draw_rect.bottom - 16;
	from_rect = down_rect;
	if(depressed && pressedPart == PART_DOWN)
		from_rect.offset(0,16);
	rect_draw_some_item(scroll_gw, from_rect, *inWindow, draw_rect);
}

cControl::storage_t cScrollbar::store() {
	storage_t storage = cControl::store();
	storage["scroll-pos"] = pos;
	return storage;
}

void cScrollbar::restore(storage_t to) {
	cControl::restore(to);
	if(to.find("scroll-pos") != to.end())
		pos = boost::any_cast<int>(to["scroll-pos"]);
	else pos = 0;
}
