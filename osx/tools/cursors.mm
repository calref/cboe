//
//  cursors.m
//  BoE
//
//  Created by Celtic Minstrel on 03/06/09.
//  Wrappers for NSCursor, based on an Apple example
//

#include "cursors.h"
#include <Cocoa/Cocoa.h>

static NSImage* ImageFromURL(CFURLRef url){
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

Cursor::Cursor(const char* path, float hotSpotX, float hotSpotY){
	FSRef ref;
	FSPathMakeRef((UInt8*)path, &ref, NULL);
	CFURLRef imgPath = CFURLCreateFromFSRef(NULL, &ref);
	
	NSImage *img = ImageFromURL(imgPath);
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

void setCursorWatch() {
}

void obscureCursor() {
	[NSCursor setHiddenUntilMouseMoves: YES];
}
