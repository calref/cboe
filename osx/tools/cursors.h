//
//  cursors.h
//  BoE
//
//  Created by Celtic Minstrel on 03/06/09.
//  Wrappers for NSCursor, based on an Apple example
//

#ifndef BOE_CURSORS_H
#define BOE_CURSORS_H

class Cursor {
	void* ptr;
public:
	Cursor(const char* imgPath, float hotSpotX, float hotSpotY);
	~Cursor();
	void apply();
};

void setCursorWatch();
// This hides the cursor until it moves
void obscureCursor();

#endif
