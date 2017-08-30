
#include "cursors.hpp"
#include <windows.h>
#include <exception>
#include <SFML/Graphics/RenderWindow.hpp>
#include "res_cursor.hpp"

extern sf::RenderWindow mainPtr;

// This function adapted from <http://www.codeproject.com/Articles/5220/Creating-a-color-cursor-from-a-bitmap>
static void GetMaskBitmaps(const sf::Image& srcImage, HBITMAP& hAndMaskBitmap, HBITMAP& hXorMaskBitmap) {
	HDC hDC = GetDC(NULL);
	HDC hMainDC = CreateCompatibleDC(hDC);
	HDC hAndMaskDC = CreateCompatibleDC(hDC);
	HDC hXorMaskDC = CreateCompatibleDC(hDC);

	hAndMaskBitmap = CreateCompatibleBitmap(hDC, srcImage.getSize().x, srcImage.getSize().y);
	hXorMaskBitmap = CreateCompatibleBitmap(hDC, srcImage.getSize().x, srcImage.getSize().y);

	// Select the bitmaps to DC
	HBITMAP hOldAndMaskBitmap = (HBITMAP)SelectObject(hAndMaskDC, hAndMaskBitmap);
	HBITMAP hOldXorMaskBitmap = (HBITMAP)SelectObject(hXorMaskDC, hXorMaskBitmap);

	// Scan each pixel of the source bitmap and create the masks
	sf::Color mainBitPixel;
	for(size_t x = 0; x < srcImage.getSize().x; ++x) {
		for(size_t y = 0; y < srcImage.getSize().y; ++y) {
			mainBitPixel = srcImage.getPixel(x, y);
			if(mainBitPixel.a == 0) {
				SetPixel(hAndMaskDC, x, y, RGB(255, 255, 255));
				SetPixel(hXorMaskDC, x, y, RGB(0, 0, 0));
			} else {
				SetPixel(hAndMaskDC, x, y, RGB(0, 0, 0));
				SetPixel(hXorMaskDC, x, y, RGB(mainBitPixel.r, mainBitPixel.g, mainBitPixel.b));
			}
		}
	}

	SelectObject(hAndMaskDC, hOldAndMaskBitmap);
	SelectObject(hXorMaskDC, hOldXorMaskBitmap);

	DeleteDC(hXorMaskDC);
	DeleteDC(hAndMaskDC);
	DeleteDC(hMainDC);

	ReleaseDC(NULL, hDC);
}

Cursor::Cursor(fs::path imgPath, float hotSpotX, float hotSpotY) {
	sf::Image gif;
	if(!gif.loadFromFile(imgPath.string())) {
		std::string error = "Error loading cursor from " + imgPath.string();
		throw error;
	}
	// Calculate the AND and XOR masks
	HBITMAP cursorAnd = CreateCompatibleBitmap(GetDC(NULL), gif.getSize().x, gif.getSize().y);
	HBITMAP cursorXor = CreateCompatibleBitmap(GetDC(NULL), gif.getSize().x, gif.getSize().y);
	GetMaskBitmaps(gif, cursorAnd, cursorXor);

	ICONINFO iconinfo = {0};
	iconinfo.fIcon = FALSE;
	iconinfo.xHotspot = hotSpotX;
	iconinfo.yHotspot = hotSpotY;
	iconinfo.hbmMask = cursorAnd;
	iconinfo.hbmColor = cursorXor;

	HCURSOR hCursor = CreateIconIndirect(&iconinfo);
	if(hCursor == NULL) {
		std::string error = "Error creating cursor from " + imgPath.string();
		error += " (error code " + std::to_string(GetLastError()) + ")";
		throw error;
	}
	ptr = hCursor;
	DeleteObject(cursorAnd);
	DeleteObject(cursorXor);
}

Cursor::~Cursor() {
	HCURSOR curs = (HCURSOR)ptr;
	DestroyIcon(curs);
}

void Cursor::apply() {
	HCURSOR curs = (HCURSOR)ptr;
	SetCursor(curs);
}

void obscureCursor() {
	SetCursor(NULL);
}

void set_cursor(cursor_type which_c) {
	if(which_c != watch_curs)
		Cursor::current = which_c;
	if(which_c == text_curs) {
		SetCursor(LoadCursor(NULL, IDC_IBEAM));
	} else {
		Cursor& curs = *ResMgr::get<CursorRsrc>(cursors[which_c]);
		curs.apply();
	}
}

void restore_cursor() {
	set_cursor(Cursor::current);
}
