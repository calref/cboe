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
#include "dlogutil.h"
#include "restypes.hpp"
#include <CoreFoundation/CoreFoundation.h>
#include "fileio.h"

#define	DONE_BUTTON_ITEM	1

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

extern bool play_sounds;
extern short current_active_pc;
extern long stored_key;
extern sf::RenderWindow mainPtr;

extern cItemRec item_list[400];
extern cUniverse univ;

extern bool file_in_mem,party_in_scen,scen_items_loaded;

bool ae_loading = false;

typedef struct {
	char expl[96][96];
}	out_info_type;

char *party_encryptor;	

std::string last_load_file = "Blades of Exile Save";

extern void update_item_menu();

extern short store_flags[3];
fs::path store_file_reply;

short give_intro_hint,display_mode;
short jl;
fs::path file_to_load;

void load_base_item_defs();
bool load_scen_item_defs(char scen_name[256]);

fs::path progDir;

void init_directories()
{
	char cPath[768];
	CFBundleRef mainBundle=CFBundleGetMainBundle();
	
	CFStringRef progURL = CFURLCopyFileSystemPath(CFBundleCopyBundleURL(mainBundle), kCFURLPOSIXPathStyle);
	const char* tmp = CFStringGetCStringPtr(progURL, kCFStringEncodingASCII);//kCFStringEncodingUTF8);
	if(tmp == NULL){
		bool success = CFStringGetCString(progURL, cPath, sizeof(cPath), kCFStringEncodingUTF8);
		if(success) {
			progDir = cPath;
			std::cout << cPath << "\n\n" << progDir << "\n\n";
		} else {
			std::cout << "Couldn't retrieve application path.\n";
			exit(1);
		}
	}else progDir = tmp;
	progDir = progDir.parent_path();
	std::cout<<progDir<<'\n';
	// Initialize the resource manager paths
	ResMgr::pushPath<ImageRsrc>(progDir/"Scenario Editor"/"graphics.exd"/"mac");
	ResMgr::pushPath<CursorRsrc>(progDir/"Scenario Editor"/"graphics.exd"/"mac"/"cursors");
	ResMgr::pushPath<FontRsrc>(progDir/"data"/"fonts");
	ResMgr::pushPath<StringRsrc>(progDir/"data"/"strings");
	ResMgr::pushPath<SoundRsrc>(progDir/"Scenario Editor"/"sounds.exa");
	
}

//bool select_save_location(FSSpec* to_save_ptr){
//	if(to_save_ptr==NULL)
//		return(false);
//	OSErr error;
//	char message[256] = "Select saved game:                                     ";
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
	fs::path basePath = progDir/"Scenario Editor"/"BoE Bases"/"bladbase.exs";
	load_scenario(basePath, true);
}

bool load_scen_item_defs(std::string scen_name){
	fs::path scenPath = progDir/"Blades of Exile Scenarios"/(scen_name + ".exs");
	return load_scenario(scenPath, true);
}
