//
//  qdpict.cpp
//  BoE
//
//  Created by Celtic Minstrel on 14-04-11.
//
//

#ifndef __APPLE__
#error qdpict.cpp uses Apple APIs at the moment; try omitting it from the build
#endif

// TODO: Don't use the resource manager
#include <CoreServices/CoreServices.h>

#include <cstdint>
#include <memory>

#include "dlogutil.h"
#include "porting.h"
#include "location.h"

static int16_t extract_word(char* ptr) {
	int16_t s = *(int16_t*) ptr;
	flip_short(&s);
	return s;
}

template<typename T>
static typename std::make_unsigned<T>::type to_unsigned(T val) {
	return *reinterpret_cast<typename std::make_unsigned<T>::type*>(&val);
}

template<typename T> class ptr_guard {
	T* the_ptr;
	T*const guard;
	void boundcheck() {
		if(the_ptr > guard) oopsError(127, the_ptr - guard);
	}
public:
	ptr_guard(T* begin, T* end) : the_ptr(begin), guard(end) {}
	ptr_guard<T>& operator+=(ptrdiff_t amt) {
		the_ptr += amt;
		boundcheck();
		return *this;
	}
	ptr_guard<T> operator++(int) {
		ptr_guard<T> tmp = *this;
		the_ptr++;
		boundcheck();
		return tmp;
	}
	void align(size_t alignment, size_t sz) {
		size_t spaceLeft = guard - the_ptr;
		void* ptr = the_ptr;
		the_ptr = (char*)std::align(alignment, sz, ptr, spaceLeft);
		boundcheck();
	}
	T& operator[](size_t i) {
		if(the_ptr + i > guard) oopsError(128, i - (guard - the_ptr));
		return the_ptr[i];
	}
	template<typename T2> explicit operator T2*() {return (T2*)the_ptr;}
	operator T*() {return the_ptr;}
	T& operator*() {return *the_ptr;}
	T* operator->() {return the_ptr;}
};

static void loadColourTable(ptr_guard<char>& picData, sf::Color(& clut)[256], int numColours) {
	while(numColours >= 0) {
		int i = extract_word(picData);
		if(i > 256) oopsError(5);
		picData += 2;
		clut[i].r = to_unsigned(extract_word(picData)) / 256;
		picData += 2;
		clut[i].g = to_unsigned(extract_word(picData)) / 256;
		picData += 2;
		clut[i].b = to_unsigned(extract_word(picData)) / 256;
		picData += 2;
		clut[i].a = 0xff;
		numColours--;
	}
}

static void loadUnpackedPixels(ptr_guard<char>& picData,ptr_guard<unsigned char>& pixels,sf::Color(& clut)[256],legacy::Rect& bounds,int rowBytes) {
	for(int i = 0; i < bounds.bottom - bounds.top; i++) {
		for(int j = 0; j < bounds.right - bounds.left; j++) {
			size_t pixel = i * rowBytes + j;
			size_t iClr = to_unsigned(*picData++);
			pixels[4 * pixel + 0] = clut[iClr].r;
			pixels[4 * pixel + 1] = clut[iClr].g;
			pixels[4 * pixel + 2] = clut[iClr].b;
			pixels[4 * pixel + 3] = clut[iClr].a;
		}
	}
}

static void loadPackedPixels(ptr_guard<char>& picData,ptr_guard<unsigned char>& pixels,sf::Color(& clut)[256],legacy::Rect& bounds,int rowBytes) {
	// Packed data
	int countSize = rowBytes > 250 ? 2 : 1;
	for(int i = 0; i < bounds.bottom - bounds.top; i++) {
		int byteCount = countSize == 2 ? extract_word(picData) : *picData;
		picData += countSize;
		char* start = picData;
		int j = 0;
		while(picData - start + 1 < byteCount) {
			int repCount = *picData++;
			if(repCount == -128) continue;
			int k = j;
			if(repCount >= 0) {
				repCount++;
				while(j - k < repCount) {
					size_t pixel = i * rowBytes + j;
					size_t iClr = to_unsigned(*picData++);
					pixels[4 * pixel + 0] = clut[iClr].r;
					pixels[4 * pixel + 1] = clut[iClr].g;
					pixels[4 * pixel + 2] = clut[iClr].b;
					pixels[4 * pixel + 3] = clut[iClr].a;
					j++;
				}
			} else {
				repCount = 1 - repCount;
				size_t val = to_unsigned(*picData++);
				while(j - k < repCount) {
					size_t pixel = i * rowBytes + j;
					pixels[4 * pixel + 0] = clut[val].r;
					pixels[4 * pixel + 1] = clut[val].g;
					pixels[4 * pixel + 2] = clut[val].b;
					pixels[4 * pixel + 3] = clut[val].a;
					j++;
				}
			}
		}
		
	}
}

static legacy::Rect loadPixMapData(ptr_guard<char>& picData, ptr_guard<unsigned char>& pixels, int pixMapType) {
	int rowBytes = extract_word(picData) & 0x7fff;
	picData += 2; // Skip rowBytes; assume we have a v2 (colour) bitmap
	legacy::Rect bounds = *(legacy::Rect*)picData;
	flip_rect(&bounds);
	picData += sizeof(legacy::Rect);
	picData += 2 + 2 + 4; // Skip version, packType, and packSize
	picData += 4 + 4; // Skip hRes and vRes
	picData += 2 + 2; // Skip pixelType and pixelSize
	if(picData[-1] != 8) oopsError(3);
	picData += 2 + 2; // Skip cmpCount and cmpSize
	picData += 4 + 4 + 4; // Skip planeBytes, pbTable, pmReserved
	picData += 4 + 2; // Skip ctSeed and ctFlags
	int numColours = extract_word(picData);
	if(numColours >= 256) oopsError(4);
	picData += 2;
	sf::Color clut[256];
	loadColourTable(picData, clut, numColours);
	picData += 2 * sizeof(legacy::Rect); // Skip source and dest rect
	picData += 2; // Skip mode
	if(pixMapType == 0x91 || pixMapType == 0x99) {
		// Same code as for Clip, above
		// TODO: I'm skipping a byte here, but that might be part of the size
		picData++;
		// This size field includes the size field itself
		picData += *picData - 1;
	}
	if(rowBytes < 8)
		loadUnpackedPixels(picData, pixels, clut, bounds, rowBytes);
	else loadPackedPixels(picData, pixels, clut, bounds, rowBytes);
	return bounds;
}

static rectangle loadFromPictResource(Handle resHandle, unsigned char*& pixelStore) {
	HLock(resHandle);
	// TODO: Use picSize to ensure I don't go out of bounds
	size_t picSize = GetHandleSize(resHandle);
	ptr_guard<char> picData(*resHandle, *resHandle + picSize);
	picData += 2; // Skip picture size field
	picData += sizeof(legacy::Rect); // Skip picture frame field (actual frame is stored later)
	if(strncmp(picData, "\0\x11\x02\xff", 4) != 0) // QuickDraw version code (version 2)
		oopsError(1);
	picData += 4; // Skip version field
	if(strncmp(picData, "\x0c\0", 2) != 0) // Header opcode
		oopsError(2);
	picData += 2 + 4; // Skip header opcode and picture size (which is -1)
	rectangle picFrame;
	// Next is the bounding rect with 4-byte components
	// However, they are fixed-point, so the second 2 bytes are fractional
	// We'll ignore that part.
	picFrame.top = extract_word(picData); picData += 4;
	picFrame.left = extract_word(picData); picData += 4;
	picFrame.bottom = extract_word(picData); picData += 4;
	picFrame.right = extract_word(picData); picData += 4;
	// We're using this info solely to initialize the target data array.
	// I'm not sure I have the components in the right order, but for this, it doesn't actually matter
	size_t picDataSize = picFrame.height() * picFrame.width() * 4; // Four bytes per pixel
	pixelStore = new unsigned char[picDataSize];
	ptr_guard<unsigned char> pixels(pixelStore, pixelStore + picDataSize);
	// Then 4 reserved bytes
	picData += 4;
	legacy::Rect bounds;
	// Now we need to skip any superfluous opcodes until we get to pixel data - opcode 90, 91, 98, or 99
	// We're assuming it's stored as pixel data, so any other opcodes will just be ignored
	bool done = false;
	while(!done) {
		// All opcodes have a first byte of 0 (other than the header opcode)
		picData++;
		switch(*(unsigned char*)picData++) {
			case 0x00: // Nop
			case 0x1C: // HiliteMode
			case 0x1E: // DefHilite
			case 0x38: // FrameSameRect
			case 0x39: // PaintSameRect
			case 0x3A: // EraseSameRect
			case 0x3B: // InvertSameRect
			case 0x3C: // FillSameRect
			case 0x48: // FrameSameRRect
			case 0x49: // PaintSameRRect
			case 0x4A: // EraseSameRRect
			case 0x4B: // InvertSameRRect
			case 0x4C: // FillSameRRect
			case 0x58: // FrameSameOval
			case 0x59: // PaintSameOval
			case 0x5A: // EraseSameOval
			case 0x5B: // InvertSameOval
			case 0x5C: // FillSameOval
			case 0x78: // FrameSameRect
			case 0x79: // PaintSameRect
			case 0x7A: // EraseSameRect
			case 0x7B: // InvertSameRect
			case 0x7C: // FillSameRect
			case 0x88: // FrameRgn
			case 0x89: // PaintRgn
			case 0x8A: // EraseRgn
			case 0x8B: // InvertRgn
			case 0x8C: // FillRgn
			case 0x3D: case 0x3E: case 0x3F: // Reserved
			case 0x4D: case 0x4E: case 0x4F: // Reserved
			case 0x5D: case 0x5E: case 0x5F: // Reserved
			case 0x7D: case 0x7E: case 0x7F: // Reserved
			case 0x8D: case 0x8E: case 0x8F: // Reserved
				break;
			case 0x04: // TxFace
			case 0x11: // Version
				picData++;
				break;
			case 0x03: // TxFont
			case 0x05: // TxMode
			case 0x08: // PnMode
			case 0x0D: // TxSize
			case 0x15: // PbLocHFrac
			case 0x16: // ChExtra
			case 0x23: // ShortLineFrom
			case 0xA0: // ShortComment
				picData += 2;
				break;
			case 0x06: // SpExtra
			case 0x07: // PnSize
			case 0x0B: // OvSize
			case 0x0C: // Origin
			case 0x0E: // FgColor
			case 0x0F: // BkColor
			case 0x21: // LineFrom
			case 0x68: // FrameSameArc
			case 0x69: // PaintSameArc
			case 0x6A: // EraseSameArc
			case 0x6B: // InvertSameArc
			case 0x6C: // FillSameArc
			case 0x6D: case 0x6E: case 0x6F: // Reserved
				picData += 4;
				break;
			case 0x22: // ShortLine
				picData += 6;
				break;
			case 0x02: // BkPat
			case 0x09: // PnPat
			case 0x0A: // FillPat
			case 0x10: // TxRatio
			case 0x20: // Line
			case 0x30: // FrameRect
			case 0x31: // PaintRect
			case 0x32: // EraseRect
			case 0x33: // InvertRect
			case 0x34: // FillRect
			case 0x40: // FrameRRect
			case 0x41: // PaintRRect
			case 0x42: // EraseRRect
			case 0x43: // InvertRRect
			case 0x44: // FillRRect
			case 0x50: // FrameOval
			case 0x51: // PaintOval
			case 0x52: // EraseOval
			case 0x53: // InvertOval
			case 0x54: // FillOval
			case 0x35: case 0x36: case 0x37: // Reserved
			case 0x45: case 0x46: case 0x47: // Reserved
			case 0x55: case 0x56: case 0x57: // Reserved
				picData += 8;
				break;
			case 0x60: // FrameArc
			case 0x61: // PaintArc
			case 0x62: // EraseArc
			case 0x63: // InvertArc
			case 0x64: // FillArc
			case 0x65: case 0x66: case 0x67: // Reserved
				picData += 12;
				break;
			case 0x01: // Clip
			case 0x80: // FrameRgn
			case 0x81: // PaintRgn
			case 0x82: // EraseRgn
			case 0x83: // InvertRgn
			case 0x84: // FillRgn
			case 0x85: case 0x86: case 0x87:
				// TODO: I'm skipping a byte here, but that might be part of the size
				picData++;
				// This size field includes the size field itself
				picData += *picData - 1;
				break;
			case 0x28: // LongText
				picData += 4;
				picData += *picData + 1;
				break;
			case 0x29: // DHText
			case 0x2A: // DVText
				picData++;
				picData += *picData + 1;
				break;
			case 0x2B: // DHDVText
			case 0xA1: // LongComment
				picData += 2;
				picData += *picData + 1;
				break;
				// MARK: Main part of load function - the pixel data
			case 0x90: // BitsRect
				bounds = loadPixMapData(picData, pixels, 0x90);
				break;
			case 0x91: // BitsRgn
				bounds = loadPixMapData(picData, pixels, 0x91);
				break;
			case 0x98: // PackBitsRect
				bounds = loadPixMapData(picData, pixels, 0x98);
				break;
			case 0x99: // PackBitsRgn
				bounds = loadPixMapData(picData, pixels, 0x99);
				break;
			case 0xFF:
				done = true;
				break;
			case 0x12: // BkPixPat
			case 0x13: // PnPixPat
			case 0x14: // FillPixPat
				// argument is a pixpat
			case 0x1A: // RGBFgCol
			case 0x1B: // RGBBkCol
			case 0x1D: // HiliteColor
			case 0x1F: // OpColor
				// argument is an RGB colour
			case 0x70: // FramePoly
			case 0x71: // PaintPoly
			case 0x72: // ErasePoly
			case 0x73: // InvertPoly
			case 0x74: // FillPoly
			case 0x75: case 0x76: case 0x77: // Reserved
				// argument is a polygon
			default:
				// These are mostly reserved opcodes whose first field is a two-byte length
				// I don't want to deal with them, and it's highly unlikely they'll come up.
				// Thus, just give an error if they do.
				oopsError(9);
				break;
		}
		// There may be a byte of padding to align it to word boundaries.
		picData.align(sizeof(int16_t), sizeof(int16_t));
	}
	HUnlock(resHandle);
	return rectangle(bounds.top, bounds.left, bounds.bottom, bounds.right);
}

bool tryLoadPictFromResourceFile(fs::path& gpath, sf::Image& graphics_store); // Suppress "no prototype" warning
bool tryLoadPictFromResourceFile(fs::path& gpath, sf::Image& graphics_store) {
	static const char*const noGraphics = "The game will still work without the custom graphics, but some things will not look right.";
	// TODO: There's no way around it; I'll have to read resource files for this section.
	FSRef file;
	ResFileRefNum custRef;
	OSErr err = FSPathMakeRef((const UInt8*)gpath.c_str(), &file, NULL);
	err = FSOpenResourceFile(&file, 0, NULL, fsRdPerm, &custRef);
	if(err != noErr) { // TODO: Is the error that would be returned if the resources were stored in the resource fork?
		HFSUniStr255 rsrc;
		err = FSGetResourceForkName(&rsrc);
		err = FSOpenResourceFile(&file, rsrc.length, rsrc.unicode, fsRdPerm, &custRef);
		if(err != noErr) {
			giveError("An old-style .meg graphics file was found, but neither data nor resource fork could be read.",noGraphics);
			return false;
		}
	}
	int nPicts = Count1Resources('PICT');
	if(nPicts < 1) {
		CloseResFile(custRef);
		giveError("An old-style .meg graphics file was found, but it did not contain any PICT resources.",noGraphics);
		return false;
	}
	Handle resHandle = Get1Resource('PICT', 1);
	if(ResError() == resNotFound) {
		CloseResFile(custRef);
		giveError("An old-style .meg graphics file was found, but it did not contain a PICT resource of ID 1.",noGraphics);
		return false;
	}
	if(resHandle == NULL) {
		CloseResFile(custRef);
		giveError("An old-style .meg graphics file was found, but an error occurred while loading it.",noGraphics);
		return false;
	}
	unsigned char* data = NULL;
	rectangle picFrame = loadFromPictResource(resHandle, data);
	CloseResFile(custRef);
	if(picFrame.width() <= 0 || picFrame.height() <= 0) {
		if(data != NULL) delete[] data;
		giveError("An old-style .meg graphics file was found, but an error occurred while reading it.",noGraphics);
		return false;
	}
	graphics_store.create(picFrame.width(), picFrame.height(), data);
	if(data != NULL) delete[] data;
	return true;
}
