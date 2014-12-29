//
//  cursors.m
//  BoE
//
//  Created by Celtic Minstrel on 03/06/09.
//  Wrappers for NSCursor, based on an Apple example
//

#include "cursors.h"
#include <Cocoa/Cocoa.h>
#include <string>
#include "restypes.hpp"

cursor_type current_cursor = sword_curs;
cursor_type arrow_curs[3][3] = {
	{NW_curs, N_curs, NE_curs},
	{W_curs,wait_curs,E_curs},
	{SW_curs, S_curs, SE_curs},
};

const std::string cursors[25] = {
	"wand", "eyedropper", "brush", "spraycan",
	"eraser", "topleft", "bottomright", "hand",
	"NW", "N", "NE",
	"W", "wait", "E",
	"SW", "S", "SE",
	"sword", "boot", "drop", "target",
	"talk", "key", "look", "watch",
};

static NSImage* imageFromURL(CFURLRef url){
	CGImageSourceRef imageSource = CGImageSourceCreateWithURL(url, NULL);
	CGImageRef theImage = nil;
	
	if(imageSource == nil) return nil;
	
	theImage = CGImageSourceCreateImageAtIndex(imageSource, 0, NULL);
	if(theImage == nil) return nil;
	
	CFRelease( imageSource );
	
	NSRect imageRect = NSMakeRect(0.0, 0.0, 0.0, 0.0);
	
	// Get the image dimensions.
	imageRect.size.height = CGImageGetHeight(theImage);
	imageRect.size.width = CGImageGetWidth(theImage);
	
	// Create a new image to receive the Quartz image data.
	NSImage	*newImage = [[NSImage alloc] initWithSize:imageRect.size]; 
	[newImage lockFocus];
	
	// Get the Quartz context and draw.
	CGContextRef imageContext = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
	CGContextDrawImage(imageContext, *(CGRect*)&imageRect, theImage);
	[newImage unlockFocus];
	
	return newImage;
}

Cursor::Cursor(std::string path, float hotSpotX, float hotSpotY){
	FSRef ref;
	FSPathMakeRef((UInt8*)path.c_str(), &ref, NULL);
	CFURLRef imgPath = CFURLCreateFromFSRef(NULL, &ref);
	
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
