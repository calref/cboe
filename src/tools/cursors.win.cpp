
#include "cursors.h"
#include <Windows.h>
#include <exception>
#include <SFML/Graphics/RenderWindow.hpp>
#include "restypes.hpp"

extern cursor_type current_cursor;
extern sf::RenderWindow mainPtr;

static const COLORREF clrMagenta = RGB(255, 0, 255);

// This loads an image from a file and replaces transparency with magenta.
// This is probably undesirable in the general case, but since we only expect transparent GIFs, it should work out.
HBITMAP LoadPicture(std::string filename) {
	sf::Image gif;
	gif.loadFromFile(filename);
	HDC dc = CreateCompatibleDC(NULL);
	HBITMAP bmp = CreateCompatibleBitmap(dc, gif.getSize().x, gif.getSize().y);
	SelectObject(dc, &bmp);
	// Not exactly efficient, but it gets the job done.
	for(int x = 0; x < gif.getSize().x; x++) {
		for(int y = 0; y < gif.getSize().y; y++) {
			sf::Color clr = gif.getPixel(x, y);
			if(clr == sf::Color::Transparent)
				SetPixel(dc, x, y, clrMagenta);
			else SetPixel(dc, x, y, RGB(clr.r, clr.g, clr.b));
		}
	}
	DeleteDC(dc);
	return bmp;
}

// This function taken from <http://www.codeproject.com/Articles/5220/Creating-a-color-cursor-from-a-bitmap>
void GetMaskBitmaps(HBITMAP hSourceBitmap, COLORREF clrTransparent, HBITMAP& hAndMaskBitmap, HBITMAP& hXorMaskBitmap) {
	HDC hDC = GetDC(NULL);
	HDC hMainDC = CreateCompatibleDC(hDC);
	HDC hAndMaskDC = CreateCompatibleDC(hDC);
	HDC hXorMaskDC = CreateCompatibleDC(hDC);

	//Get the dimensions of the source bitmap
	BITMAP bm;
	GetObject(hSourceBitmap, sizeof(BITMAP), &bm);

	hAndMaskBitmap = CreateCompatibleBitmap(hDC, bm.bmWidth, bm.bmHeight);
	hXorMaskBitmap = CreateCompatibleBitmap(hDC, bm.bmWidth, bm.bmHeight);

	//Select the bitmaps to DC
	HBITMAP hOldMainBitmap = (HBITMAP)SelectObject(hMainDC, hSourceBitmap);
	HBITMAP hOldAndMaskBitmap = (HBITMAP)SelectObject(hAndMaskDC, hAndMaskBitmap);
	HBITMAP hOldXorMaskBitmap = (HBITMAP)SelectObject(hXorMaskDC, hXorMaskBitmap);

	//Scan each pixel of the souce bitmap and create the masks
	COLORREF MainBitPixel;
	for(int x = 0; x<bm.bmWidth; ++x) {
		for(int y = 0; y<bm.bmHeight; ++y) {
			MainBitPixel = GetPixel(hMainDC, x, y);
			if(MainBitPixel == clrTransparent) {
				SetPixel(hAndMaskDC, x, y, RGB(255, 255, 255));
				SetPixel(hXorMaskDC, x, y, RGB(0, 0, 0));
			} else {
				SetPixel(hAndMaskDC, x, y, RGB(0, 0, 0));
				SetPixel(hXorMaskDC, x, y, MainBitPixel);
			}
		}
	}

	SelectObject(hMainDC, hOldMainBitmap);
	SelectObject(hAndMaskDC, hOldAndMaskBitmap);
	SelectObject(hXorMaskDC, hOldXorMaskBitmap);

	DeleteDC(hXorMaskDC);
	DeleteDC(hAndMaskDC);
	DeleteDC(hMainDC);

	ReleaseDC(NULL, hDC);
}

Cursor::Cursor(fs::path imgPath, float hotSpotX, float hotSpotY) {
	HBITMAP cursorImage = LoadPicture(imgPath.string()), cursorAnd, cursorXor;
	if(cursorImage == NULL) {
		std::string error = "Error loading cursor from " + imgPath.string();
		throw std::exception(error.c_str());
	}
	GetMaskBitmaps(cursorImage, clrMagenta, cursorAnd, cursorXor);

	ICONINFO iconinfo = {0};
	iconinfo.fIcon = FALSE;
	iconinfo.xHotspot = hotSpotX;
	iconinfo.yHotspot = hotSpotY;
	iconinfo.hbmMask = cursorAnd;
	iconinfo.hbmColor = cursorXor;

	HCURSOR hCursor = CreateIconIndirect(&iconinfo);
	ptr = &hCursor;
	DeleteObject(cursorImage);
	DeleteObject(cursorAnd);
	DeleteObject(cursorXor);
}

Cursor::~Cursor() {
	HCURSOR* curs = (HCURSOR*)ptr;
	DestroyIcon(*curs);
}

void Cursor::apply() {
	HCURSOR* curs = (HCURSOR*)ptr;
	SetCursor(*curs);
	// TODO: This ensures the cursor stays set when the mouse moves. Is it necessary, though?
	SetClassLong(mainPtr.getSystemHandle(), GCL_HCURSOR, (LONG)(*curs));
}

void obscureCursor() {
	SetCursor(NULL);
}

void set_cursor(cursor_type which_c) {
	if(which_c != watch_curs)
		current_cursor = which_c;
	if(which_c == text_curs) {
		SetCursor(LoadCursor(NULL, IDC_IBEAM));
	} else {
		Cursor& curs = *ResMgr::get<CursorRsrc>(cursors[which_c]);
		curs.apply();
	}
}

void restore_cursor() {
	set_cursor(current_cursor);
}
