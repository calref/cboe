//#inculde <cMemory>
#include <cstring>
#include <cstdio>

#include "pc.global.h"
#include "classes.h"
#include "pc.fileio.h"
#include "pc.graphics.h"
#include "graphtool.h"
#include "soundtool.h"
#include "pc.editors.h"
#include "mathutil.h"
#include "dlgutil.h"

#define	DONE_BUTTON_ITEM	1
#define IN_FRONT	(WindowPtr)-1L

DialogPtr	the_dialog;

/* Adventure globals */
//extern party_record_type party;
//extern outdoor_record_type outdoors[2][2];
//extern current_town_type c_town;
//extern big_tr_type t_d;
//extern town_item_list	t_i;
//extern unsigned char misc_i[64][64],sfx[64][64];
//extern unsigned char out[96][96],out_e[96][96];
//extern setup_save_type setup_save;
//extern stored_items_list_type stored_items[3];

//extern stored_town_maps_type town_maps;
//extern stored_outdoor_maps_type o_maps;

extern bool play_sounds,save_blocked;
extern short current_active_pc;
extern long stored_key;
extern WindowPtr mainPtr;

extern cItemRec item_list[400];
extern cUniverse univ;

extern bool file_in_mem,party_in_scen,scen_items_loaded;

bool ae_loading = false;

typedef struct {
	char expl[96][96];
}	out_info_type;

char *party_encryptor;	

Str63 last_load_file = "\pBlades of Exile Save";

extern void update_item_menu();

extern short store_flags[3];
FSSpec store_file_reply;

short give_intro_hint,display_mode;
short jl;
FSSpec file_to_load;

void load_base_item_defs();
bool load_scen_item_defs(char scen_name[256]);

//bool select_save_location(FSSpec* to_save_ptr){
//	if(to_save_ptr==NULL)
//		return(false);
//	OSErr error;
//	Str255 message = "\pSelect saved game:                                     ";
//	NavTypeListHandle type_list;
//	NavDialogCreationOptions dialogOptions;
//	NavDialogRef theDialog;
//	NavUserAction theAction;
//	NavReplyRecord dialogReply;
//	AEKeyword dummyKeyword;
//	long descCount;
//	long descNum;
//	/* XXX FIXME general lack of error checking in these nav services calls */
//	NavGetDefaultDialogCreationOptions(&dialogOptions);
//	NavCreatePutFileDialog(&dialogOptions, 'beSV', 'blx!',NULL, NULL, &theDialog);
//	NavDialogRun(theDialog);
//	theAction = NavDialogGetUserAction(theDialog);
//	if (theAction == kNavUserActionCancel || theAction == kNavUserActionNone) {
//		NavDialogDispose(theDialog);
//		return(false);
//	}
//	NavDialogGetReply(theDialog, &dialogReply);
//	if(dialogReply.validRecord){
//		//  Deal with multiple file selection
//		long    count;
//		error = AECountItems(&(dialogReply.selection), &descCount);
//		// Set up index for file list
//		if (error == noErr){
//			long index;
//			for (index = 1; index <= 1; index++){
//				AEKeyword   theKeyword;
//				DescType    actualType;
//				Size        actualSize;
//				FSSpec      documentFSSpec;
//				// Get a pointer to selected file
//				error = AEGetNthPtr(&(dialogReply.selection), index,
//									typeFSS, &theKeyword,
//									&actualType,&documentFSSpec,
//									sizeof(documentFSSpec),
//									&actualSize);
//				if (error == noErr){
//					FSRef tempRef;
//					FSpMakeFSRef(&documentFSSpec,&tempRef);
//					UniChar uniname[256];
//					CFRange range = {0,255};
//					CFStringGetCharacters (dialogReply.saveFileName,range,uniname);
//					uniname[(UniCharCount)CFStringGetLength (dialogReply.saveFileName)]=0;
//					error=FSCreateFileUnicode(&tempRef, (UniCharCount)CFStringGetLength (dialogReply.saveFileName), (UniChar *) uniname, kFSCatInfoNone, NULL, NULL, to_save_ptr);
//					if(error==noErr){//file didn't exist and so we just created it
//						printf("created file\n");
//						//kludge to correctly set creator and file types so that we'll recognize the file when we go to open it later
//						FInfo fileInfo;
//						FSpGetFInfo(to_save_ptr, &fileInfo);
//						fileInfo.fdCreator='blx!';
//						fileInfo.fdType='beSV';
//						FSpSetFInfo(to_save_ptr, &fileInfo);
//					}
//					else{
//						if(error==dupFNErr){ //file already exists
//							UInt8 tempPath[512];
//							FSpMakeFSRef(to_save_ptr, &tempRef);
//							FSRefMakePath (&tempRef,tempPath,512);
//							CFStringRef path = CFStringCreateWithFormat(NULL, NULL, CFSTR("%s/%S"),tempPath,uniname);
//							CFStringGetCString(path, (char*)tempPath, 512, kCFStringEncodingUTF8);
//							FSPathMakeRef(tempPath, &tempRef, NULL);
//							error=FSGetCatalogInfo(&tempRef, kFSCatInfoNone, NULL,NULL, to_save_ptr, NULL);
//						}
//						else{ //something bad happened
//							printf("creation error was: %i\n",error);
//							return(false);
//						}
//					}
//				}
//				else
//					return(false);
//			}
//		}
//		//  Dispose of NavReplyRecord
//		error = NavDisposeReply(&dialogReply);
//	}
//	
//	store_file_reply = *to_save_ptr;
//	
//	NavDialogDispose(theDialog);
//	return(true);
//}

void leave_town()
{
	store_flags[0] = 5790;
}

void remove_party_from_scen()
{
	store_flags[1] = 200;
	party_in_scen = false;
	load_base_item_defs();
}

/* 
 * XXX This was referenced but not defined, so I copied the implementation
 * from blxfileio.c. Need to check that it's OK.
 */
short init_data(short flag)
{
	long k = 0;

	k = (long) flag;
	k = k * k;
	jl = jl * jl + 84 + k;
	k = k + 51;
	jl = jl * 2 + 1234 + k;
	k = k % 3000;
	jl = jl * 54;
	jl = jl * 2 + 1234 + k;
	k = k * 82;
	k = k % 10000;
	k = k + 10000;

	return (short) k;
}

void load_base_item_defs(){
	OSErr err;
	char cPath[512];
	CFBundleRef mainBundle=CFBundleGetMainBundle();
	CFURLRef baseScenURL = CFBundleCopyResourceURL(mainBundle,CFSTR("bladbase"),CFSTR("exs"),NULL);
	CFStringRef baseScenPath = CFURLCopyFileSystemPath(baseScenURL, kCFURLPOSIXPathStyle);
	CFStringGetCString(baseScenPath, cPath, 512, kCFStringEncodingUTF8);
	FSRef bSRef;
	FSPathMakeRef((UInt8*)cPath, &bSRef, false);
	short forkRef;
	HFSUniStr255 forkName;
	FSGetDataForkName(&forkName);
	err=FSOpenFork(&bSRef, forkName.length, forkName.unicode, fsRdPerm, &forkRef);
	if(err!=noErr){
		printf("Unable to open base scenario data\n");
		ExitToShell();
	}
	err=FSReadFork(forkRef, fsFromStart, 41942, 26400, &(item_list[0]), NULL);
	if(err!=noErr){
		printf("Unable to read base scenario data\n");
		ExitToShell();
	}
	err=FSCloseFork(forkRef);
	if(err!=noErr){
		printf("Unable to close base scenario data\n");
		ExitToShell();
	}
}

bool load_scen_item_defs(char scen_name[256]){
	OSErr err;
	char cPath[768];
	CFBundleRef mainBundle=CFBundleGetMainBundle();
	CFURLRef progURL = CFBundleCopyBundleURL(mainBundle);
	CFStringRef progDir = CFURLCopyFileSystemPath(progURL, kCFURLPOSIXPathStyle);
	CFRange findRes;
	if(!CFStringFindWithOptions(progDir, CFSTR("/"), CFRangeMake(0, CFStringGetLength(progDir)), kCFCompareBackwards, &findRes)){
		printf("Error: Unable to find scenario directory\n");
		return(false);
	}
	CFStringRef scenPath = CFStringCreateWithFormat(NULL,NULL,CFSTR("%@/Blades of Exile Scenarios/%s"),CFStringCreateWithSubstring(NULL, progDir, CFRangeMake(0,findRes.location)),scen_name);
	CFStringGetCString(scenPath, cPath, 768, kCFStringEncodingUTF8);
	FSRef scenRef;
	FSPathMakeRef((UInt8*)cPath, &scenRef, false);
	short forkRef;
	HFSUniStr255 forkName;
	FSGetDataForkName(&forkName);
	err=FSOpenFork(&scenRef, forkName.length, forkName.unicode, fsRdPerm, &forkRef);
	if(err!=noErr){
		printf("Unable to open scenario data\n");
		return(false);
	}
	err=FSReadFork(forkRef, fsFromStart, 41942, 26400, &(item_list[0]), NULL);
	if(err!=noErr){
		printf("Unable to read scenario data\n");
		return(false);
	}
	err=FSCloseFork(forkRef);
	if(err!=noErr){
		printf("Unable to close scenario data\n");
	}
	return(true);
}