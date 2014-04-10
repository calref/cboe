//
//  cursors.h
//  BoE
//
//  Created by Celtic Minstrel on 03/06/09.
//  Wrappers for NSCursor, based on an Apple example
//

#ifdef __cplusplus
#define C_DECL extern "C"
#else
#define C_DECL
#endif

struct CocoaCursor {
	void* ptr;
};
typedef struct CocoaCursor CocoaCursor, *CursorRef; // including the struct keyword for Objective-C's sake

C_DECL CursorRef CreateCursorFromFile(const char* imgPath, float hotSpotX, float hotSpotY);
C_DECL void DisposeNSCursor(CursorRef cursor);
C_DECL void SetNSCursor(CursorRef cursor);
C_DECL void SetNSCursorWatch();
C_DECL void HideNSCursor();
C_DECL void ShowNSCursor();
C_DECL void CleanUp();

#undef C_DECL