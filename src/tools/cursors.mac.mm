//
//  cursors.m
//  BoE
//
//  Created by Celtic Minstrel on 03/06/09.
//  Wrappers for NSCursor, based on an Apple example
//

#include "cursors.hpp"
#include <Cocoa/Cocoa.h>
#include <string>
#include "restypes.hpp"

extern cursor_type current_cursor;

static NSImage* imageFromURL(CFURLRef url) NS_RETURNS_RETAINED;
static NSImage* imageFromURL(CFURLRef url){
	CGImageSourceRef imageSource = CGImageSourceCreateWithURL(url, nullptr);
	CGImageRef theImage = nil;
	
	if(imageSource == nil) return nil;
	
	theImage = CGImageSourceCreateImageAtIndex(imageSource, 0, nullptr);
	if(theImage == nil) {
		CFRelease( imageSource );
		return nil;
	}
	
	CFRelease( imageSource );
	
	NSRect imageRect = NSMakeRect(0.0, 0.0, 0.0, 0.0);
	
	// Get the image dimensions.
	imageRect.size.height = CGImageGetHeight(theImage);
	imageRect.size.width = CGImageGetWidth(theImage);
	
	// Create a new image to receive the Quartz image data.
	NSImage* newImage = [[NSImage alloc] initWithSize:imageRect.size];
	[newImage lockFocus];
	
	// Get the Quartz context and draw.
	CGContextRef imageContext = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
	CGContextDrawImage(imageContext, *(CGRect*)&imageRect, theImage);
	[newImage unlockFocus];
	CGImageRelease(theImage);
	
	return newImage;
}

Cursor::Cursor(fs::path path, float hotSpotX, float hotSpotY){
	NSString *ref = [[NSFileManager defaultManager] stringWithFileSystemRepresentation:path.c_str() length:strlen(path.c_str())];
	CFURLRef imgPath = (CFURLRef)[NSURL fileURLWithPath:ref];
	
	NSImage *img = imageFromURL(imgPath);
	NSCursor *cursor = [[NSCursor alloc] initWithImage:img hotSpot:NSMakePoint(hotSpotX, hotSpotY)];
	[img release];
	
	ptr = cursor;
}

Cursor::~Cursor(){
	[(NSCursor*)ptr release];
}

void Cursor::apply(){
	[(NSCursor*)ptr set];
}

void obscureCursor() {
	[NSCursor setHiddenUntilMouseMoves: YES];
}

void set_cursor(cursor_type which_c) {
	if(which_c != watch_curs)
		current_cursor = which_c;
	if(which_c == text_curs) {
		[[NSCursor IBeamCursor] set];
	} else {
		Cursor& curs = *ResMgr::get<CursorRsrc>(cursors[which_c]);
		curs.apply();
	}
}

void restore_cursor(){
	set_cursor(current_cursor);
}
