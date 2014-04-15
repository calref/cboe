//
//  cursors.m
//  BoE
//
//  Created by Celtic Minstrel on 03/06/09.
//  Wrappers for NSCursor, based on an Apple example
//

#import <Cocoa/Cocoa.h>
#import "cursors.h"
NSAutoreleasePool *pool;

NSImage* ImageFromURL(CFURLRef url){
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

CursorRef CreateCursorFromFile(const char* path, float hotSpotX, float hotSpotY){
	FSRef ref;
	FSPathMakeRef((UInt8*)path, &ref, NULL);
	CFURLRef imgPath = CFURLCreateFromFSRef(NULL, &ref);
#if 0
	static BOOL inited = NO;
	if(!inited){
		// TODO: Why is this here? Should I move it somewhere else?
		NSApplicationLoad();
		pool = [[NSAutoreleasePool alloc] init];
		[[[NSWindow alloc] init] release];
		inited = YES;
	}
#endif
	
	NSImage *img = ImageFromURL(imgPath);
	NSCursor *cursor = [[NSCursor alloc] initWithImage:img hotSpot:NSMakePoint(hotSpotX, hotSpotY)];
	[img release];
	
	CursorRef theCursor = malloc(sizeof(CocoaCursor));
	theCursor->ptr = cursor;
	return theCursor;
}

void DisposeNSCursor(CursorRef cursor){
	[(NSCursor*)cursor->ptr release];
	free(cursor);
}

void SetNSCursor(CursorRef cursor){
	[(NSCursor*)cursor->ptr set];
}

void SetNSCursorWatch() {
}

void HideNSCursor(){
	[NSCursor hide];
}

void ShowNSCursor(){
	[NSCursor unhide];
}

void CleanUp(){
//	[pool release];
}
