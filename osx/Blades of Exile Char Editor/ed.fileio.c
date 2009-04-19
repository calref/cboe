#include <Memory.h>
//#include <Menus.h>
//#include <Windows.h>
//#include <TextEdit.h>
//#include <Dialogs.h>
//#include <ToolUtils.h>
//#include <Files.h>
//#include <StandardFile.h>
//#include <Resources.h>
//#include <QDOffscreen.h>
#include <Carbon/Carbon.h>
#include <string.h>
#include "stdio.h"

#include "ed.global.h"
#include "ed.fileio.h"
#include "ed.graphics.h"
#include "graphtool.h"
#include "soundtool.h"
#include "ed.editors.h"
#include "mathutil.h"

#define	DONE_BUTTON_ITEM	1
#define IN_FRONT	(WindowPtr)-1L
#define	NIL		0L

DialogPtr	the_dialog;

/* Adventure globals */
extern party_record_type party;
extern pc_record_type adven[6];
extern outdoor_record_type outdoors[2][2];
extern current_town_type c_town;
extern big_tr_type t_d;
extern town_item_list	t_i;
extern unsigned char misc_i[64][64],sfx[64][64];
extern unsigned char out[96][96],out_e[96][96];
extern setup_save_type setup_save;
extern stored_items_list_type stored_items[3];

extern stored_town_maps_type town_maps;
extern stored_outdoor_maps_type o_maps;

extern Boolean registered,play_sounds,sys_7_avail,save_blocked,ed_reg;
extern short current_active_pc;
extern long register_flag,stored_key;
extern WindowPtr mainPtr;
extern long ed_flag,ed_key;

extern item_record_type item_list[400];

extern Boolean file_in_mem,party_in_scen,scen_items_loaded;

Boolean ae_loading = FALSE;

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

void load_file()
{
	//SFReply reply;
	//StandardFileReply s_reply;
	Point where = {40,40};
	Str255 message = "Select saved game:";
	long file_size;
	OSErr error;
	NavTypeListHandle type_list;
	NavDialogCreationOptions dialogOptions;
	NavDialogRef theDialog;
	NavUserAction theAction;
	NavReplyRecord dialogReply;
	AEKeyword dummyKeyword;
	long descCount;
	long descNum;
	FSRef fileRef;
	short file_id,i,j,k;
	Boolean town_restore = FALSE;
	Boolean maps_there = FALSE;
	Boolean map_doh = FALSE;
	Boolean in_scen = FALSE;
	
	flag_type fred;
	flag_type *store;
	char flag_data[8];
	
	town_item_list *item_ptr;	
	long len,store_len,count;
	out_info_type *explored_ptr;	
	char *party_ptr;
	char *pc_ptr;
	flag_type flag;
	flag_type *flag_ptr;
	stored_items_list_type *items_ptr;
	
	short flags[3][2] = {{5790,1342}, // slot 0 ... 5790 - out  1342 - town
						{100,200}, // slot 1 100  in scenario, 200 not in
						{3422,5567}}; // slot 2 ... 3422 - no maps  5567 - maps
	
	OSType typeList[2] = {'beSV','TEXT'};
	type_list = (NavTypeListHandle)NewHandle(sizeof(NavTypeList) + 2 * sizeof(OSType));
	
	if (type_list != NULL){
		(*type_list)->componentSignature = 'blx!';
		(*type_list)->osTypeCount = 2;
		BlockMoveData(&typeList, (*type_list)->osType, 2 * sizeof(OSType));
	}
	
	if (sys_7_avail == FALSE) {
		/* not reached when using Carbon */
		
		/* SFPGetFile(where,message, NULL, 1, type_list, NULL, &reply,-2000,NULL);
		 
		 if (reply.good == 0) {	
		 return;
		 }
		 
		 
		 if ((error = FSOpen(reply.fName,reply.vRefNum,&file_id)) > 0){
		 FCD(1064,0);
		 SysBeep(2);
		 return;
		 } */
	}
	else {
		if (ae_loading == FALSE) {
			//StandardGetFile(NULL,1,type_list,&s_reply);
			// StandardGetFile is not available in Carbon -jmr
			/* XXX FIXME general lack of error checking in these nav services calls */
			NavGetDefaultDialogCreationOptions(&dialogOptions);
			NavCreateChooseFileDialog(&dialogOptions, type_list, NULL,
									  NULL, NULL, NULL, &theDialog);
			NavDialogRun(theDialog);
			theAction = NavDialogGetUserAction(theDialog);
			if (theAction == kNavUserActionCancel || theAction == kNavUserActionNone) {
				NavDialogDispose(theDialog);
				return;
			}
			NavDialogGetReply(theDialog, &dialogReply);
			
			AECountItems(&(dialogReply.selection), &descCount);
			for (descNum = 1 ; descNum <= descCount ; descNum++) {
				DescType returnedType;
				Size actualSize;
				AEGetNthPtr(&(dialogReply.selection), descNum, typeWildCard,
							&dummyKeyword, &returnedType, (Ptr)(&fileRef),
							sizeof(fileRef), &actualSize);
				/* we'd get typeFSS on classic Mac OS, but I have omitted that check due to laziness -jmr */
				if (returnedType == typeFSRef)
					break;
			}
			if (descNum > descCount) {
				/* no valid file reference came back from the dialog - shouldn't happen... */
				NavDisposeReply(&dialogReply);
				NavDialogDispose(theDialog);
				return;
			}
			
			FSGetCatalogInfo(&fileRef, kFSCatInfoNone, NULL,
							 NULL, &file_to_load, NULL);
			
			NavDisposeReply(&dialogReply);
			NavDialogDispose(theDialog);
		}
		
		if ((error = FSpOpenDF(&file_to_load,1,&file_id)) != 0) {
			FCD(1064,0);
			SysBeep(2);
			return;
		}		
		
	}
	
	file_size = sizeof(party_record_type);
	
	len = sizeof(flag_type);
	
	//	sprintf((char *) debug, "  Len %d               ", (short) len);
	//	add_string_to_buf((char *) debug);
	
	for (i = 0; i < 3; i++) {
		if ((error = FSRead(file_id, &len, (char *) flag_data)) != 0) {
			FSClose(file_id);
			FCD(1064,0);
			return;
		}
		flag_ptr = (flag_type *) flag_data;
		flag = *flag_ptr;
		store_flags[i] = (short) flag.i;
		if ((flag.i != flags[i][0]) && (flag.i != flags[i][1])) { // OK BoE save file?
			FSClose(file_id);
			FCD(1063,0);
			return;
		} 
		
		if ((i == 0) && (flag.i == flags[i][1]))
			town_restore = TRUE;
		if ((i == 1) && (flag.i == flags[i][0])) {
			in_scen = TRUE;
		}
		if ((i == 2) && (flag.i == flags[i][1]))
			maps_there = TRUE;
	}
	
	// LOAD PARTY	
	len = (long) sizeof(party_record_type);
	store_len = len;
	party_ptr = (char *) &party;
	if ((error = FSRead(file_id, &len, (char *) party_ptr)) != 0){
		FSClose(file_id);
		SysBeep(2);
		FCD(1064,0);
		return;
	}
	for (count = 0; count < store_len; count++)
		party_ptr[count] ^= 0x5C;	
	
	// LOAD SETUP
	len = (long) sizeof(setup_save_type);
	if ((error = FSRead(file_id, &len, (char *) &setup_save)) != 0){
		FSClose(file_id);
		SysBeep(2);
		FCD(1064,0);
		return;
	}
	
	// LOAD PCS
	store_len = (long) sizeof(pc_record_type);
	for (i = 0; i < 6; i++) {
		len = store_len;
		pc_ptr = (char *) &adven[i];
		if ((error = FSRead(file_id, &len, (char *) pc_ptr))  != 0){
			FSClose(file_id);
			SysBeep(2);
			FCD(1064,0);
			return;
		}
		for (count = 0; count < store_len; count++)
			pc_ptr[count] ^= 0x6B;	
	}
	
	if (in_scen == TRUE) {
		// LOAD OUTDOOR MAP
		len = (long) sizeof(out_info_type);
		if ((error = FSRead(file_id, &len, (char *) out_e)) != 0){
			FSClose(file_id);
			SysBeep(2);
			FCD(1064,0);
			return;
		}
		
		// LOAD TOWN 
		if (town_restore == TRUE) {
			len = (long) sizeof(current_town_type);
			if ((error = FSRead(file_id, &len, (char *) &c_town)) != 0){
				FSClose(file_id);
				SysBeep(2);
				FCD(1064,0);
				return;
			}
			
			len = (long) sizeof(big_tr_type);
			if ((error = FSRead(file_id, &len, (char *) &t_d)) != 0){
				FSClose(file_id);
				SysBeep(2);
				FCD(1064,0);
				return;
			}
			
			len = (long) sizeof(town_item_list);
			if ((error = FSRead(file_id, &len, (char *) &t_i))  != 0){
				FSClose(file_id);
				SysBeep(2);
				FCD(1064,0);
				return;
			}
			
		}
		
		// LOAD STORED ITEMS
		for (i = 0; i < 3; i++) {
			len = (long) sizeof(stored_items_list_type);
			if ((error = FSRead(file_id, &len, (char *) &stored_items[i]))  != 0){
				FSClose(file_id);
				SysBeep(2);
				FCD(1064,0);
				return;
			}		
		}
		
		// LOAD SAVED MAPS
		if (maps_there == TRUE) {
			len = (long) sizeof(stored_town_maps_type);
			if ((error = FSRead(file_id, &len, (char *) &(town_maps)))  != 0){
				FSClose(file_id);
				SysBeep(2);
				FCD(1064,0);
				return;
			}
			
			len = (long) sizeof(stored_outdoor_maps_type);
			if ((error = FSRead(file_id, &len, (char *) &o_maps)) != 0) {
				FSClose(file_id);
				SysBeep(2);
				FCD(1064,0);
				return;
			}	
		}
		
		// LOAD SFX & MISC_I
		len = (long) (64 * 64);
		if ((error = FSRead(file_id, &len, (char *) sfx))  != 0){
			FSClose(file_id);
			SysBeep(2);
			FCD(1064,0);
			return;
		}	
		if ((error = FSRead(file_id, &len, (char *) misc_i))  != 0){
			FSClose(file_id);
			SysBeep(2);
			FCD(1064,0);
			return;
		}	
		
	} // end if_scen
	
	if ((error = FSClose(file_id))  != 0){
		add_string_to_buf("Load: Can't close file.          ");
		SysBeep(2);
		return;
	} 

	for (i = 0; i < 6; i++){
		if (adven[i].main_status > 0) {
			current_active_pc = i;
			i = 6;
		}
	}
	file_in_mem = TRUE;
	save_blocked = FALSE;
	party_in_scen = in_scen;
	
	if ((sys_7_avail == FALSE) && (ae_loading == FALSE)) {
		/* not reached with Carbon */
		/*strcpy ((char *) last_load_file, (char *) reply.fName);*/
	} else {
		strcpy ((char *) last_load_file, (char *) file_to_load.name);
		store_file_reply = file_to_load;
	}
	
	//load item definitions
	if(party_in_scen){
		if(!load_scen_item_defs(party.scen_name)){
			load_base_item_defs();
			scen_items_loaded = false;
		}
		else
			scen_items_loaded = true;
	}
	else{
		load_base_item_defs();
		scen_items_loaded = false;
	}
	update_item_menu();
	
	redraw_screen();
}

bool select_save_location(FSSpec* to_save_ptr){
	if(to_save_ptr==NULL)
		return(false);
	OSErr error;
	Str255 message = "\pSelect saved game:                                     ";
	NavTypeListHandle type_list;
	NavDialogCreationOptions dialogOptions;
	NavDialogRef theDialog;
	NavUserAction theAction;
	NavReplyRecord dialogReply;
	AEKeyword dummyKeyword;
	long descCount;
	long descNum;
	/* XXX FIXME general lack of error checking in these nav services calls */
	NavGetDefaultDialogCreationOptions(&dialogOptions);
	NavCreatePutFileDialog(&dialogOptions, 'beSV', 'blx!',NULL, NULL, &theDialog);
	NavDialogRun(theDialog);
	theAction = NavDialogGetUserAction(theDialog);
	if (theAction == kNavUserActionCancel || theAction == kNavUserActionNone) {
		NavDialogDispose(theDialog);
		return(false);
	}
	NavDialogGetReply(theDialog, &dialogReply);
	if(dialogReply.validRecord){
		//  Deal with multiple file selection
		long    count;
		error = AECountItems(&(dialogReply.selection), &descCount);
		// Set up index for file list
		if (error == noErr){
			long index;
			for (index = 1; index <= 1; index++){
				AEKeyword   theKeyword;
				DescType    actualType;
				Size        actualSize;
				FSSpec      documentFSSpec;
				// Get a pointer to selected file
				error = AEGetNthPtr(&(dialogReply.selection), index,
									typeFSS, &theKeyword,
									&actualType,&documentFSSpec,
									sizeof(documentFSSpec),
									&actualSize);
				if (error == noErr){
					FSRef tempRef;
					FSpMakeFSRef(&documentFSSpec,&tempRef);
					UniChar uniname[256];
					CFRange range = {0,255};
					CFStringGetCharacters (dialogReply.saveFileName,range,uniname);
					uniname[(UniCharCount)CFStringGetLength (dialogReply.saveFileName)]=0;
					error=FSCreateFileUnicode(&tempRef, (UniCharCount)CFStringGetLength (dialogReply.saveFileName), (UniChar *) uniname, kFSCatInfoNone, NULL, NULL, to_save_ptr);
					if(error==noErr){//file didn't exist and so we just created it
						printf("created file\n");
						//kludge to correctly set creator and file types so that we'll recognize the file when we go to open it later
						FInfo fileInfo;
						FSpGetFInfo(to_save_ptr, &fileInfo);
						fileInfo.fdCreator='blx!';
						fileInfo.fdType='beSV';
						FSpSetFInfo(to_save_ptr, &fileInfo);
					}
					else{
						if(error==dupFNErr){ //file already exists
							UInt8 tempPath[512];
							FSpMakeFSRef(to_save_ptr, &tempRef);
							FSRefMakePath (&tempRef,tempPath,512);
							CFStringRef path = CFStringCreateWithFormat(NULL, NULL, CFSTR("%s/%S"),tempPath,uniname);
							CFStringGetCString(path, (char*)tempPath, 512, kCFStringEncodingUTF8);
							FSPathMakeRef(tempPath, &tempRef, NULL);
							error=FSGetCatalogInfo(&tempRef, kFSCatInfoNone, NULL,NULL, to_save_ptr, NULL);
						}
						else{ //something bad happened
							printf("creation error was: %i\n",error);
							return(false);
						}
					}
				}
				else
					return(false);
			}
		}
		//  Dispose of NavReplyRecord
		error = NavDisposeReply(&dialogReply);
	}
	
	store_file_reply = *to_save_ptr;
	
	NavDialogDispose(theDialog);
	return(true);
}

void save_file(FSSpec to_save)
{
	long file_size;
	OSErr error;
	
	short file_id;
	Boolean got_error = FALSE,in_scen = FALSE,town_save = FALSE,save_maps = FALSE;
	FSRef fileRef;
	
	if ((error = FSpOpenDF(&to_save,3,&file_id)) != 0) {
		printf("Save: Couldn't open file, error %i.\n",error);
		add_string_to_buf("Save: Couldn't open file.         ");
		SysBeep(2);
		return;
	}		
	strcpy ((char *) last_load_file, (char *) to_save.name);
	
	short i,j;
	
	long len,store_len,count;
	flag_type flag;
	flag_type *store;
	
	char* party_encryptor;	
	char debug[60];
	
	if (file_in_mem == FALSE)
		return;
	
	if (save_blocked == TRUE) {
		FCD(903,0);
		return;
	}
	
	if (store_flags[0] == 1342)
		town_save = TRUE;
	else
		town_save = FALSE;
	if (store_flags[1] == 100)
		in_scen = TRUE;
	else
		in_scen = FALSE;
	if (store_flags[2] == 5567)
		save_maps = TRUE;
	else
		save_maps = FALSE;
	
	store = &flag;	
	
	len = sizeof(flag_type);
	
	flag.i = store_flags[0];
	if ((error = FSWrite(file_id, &len, (char *) store))  != 0){
		add_string_to_buf("Save: Couldn't write to file.         ");
		FSClose(file_id);
		SysBeep(2);
	}
	flag.i = store_flags[1];
	if ((error = FSWrite(file_id, &len, (char *) store)) != 0) {
		add_string_to_buf("Save: Couldn't write to file.         ");
		FSClose(file_id);
		SysBeep(2);
		return;
	}
	flag.i = store_flags[2];
	if ((error = FSWrite(file_id, &len, (char *) store))  != 0){
		add_string_to_buf("Save: Couldn't write to file.         ");
		FSClose(file_id);
		SysBeep(2); 
		return;
	}
	
	// SAVE PARTY
	party_record_type* party_ptr = &party;	
	len = sizeof(party_record_type);
	
	store_len = len;
	party_encryptor = (char *) party_ptr;
	for (count = 0; count < store_len; count++)
		party_encryptor[count] ^= 0x5C;
	if ((error = FSWrite(file_id, &len, (char *) party_ptr)) != 0) {
		add_string_to_buf("Save: Couldn't write to file.         ");
		FSClose(file_id);
		for (count = 0; count < store_len; count++)
			party_encryptor[count] ^= 0x5C;
		SysBeep(2); 
		return;
	}
	for (count = 0; count < store_len; count++)
		party_encryptor[count] ^= 0x5C;
	
	// SAVE SETUP
	setup_save_type* setup_ptr = &setup_save;	
	len = sizeof(setup_save_type);
	if ((error = FSWrite(file_id, &len, (char *) setup_ptr))  != 0){
		add_string_to_buf("Save: Couldn't write to file.         ");
		FSClose(file_id);
		SysBeep(2); 
		return;
	}
	
	// SAVE PCS	
	store_len = sizeof(pc_record_type);
	for (i = 0; i < 6; i++) {
		pc_record_type* pc_ptr = &adven[i];	
		
		len = store_len;
		party_encryptor = (char *) pc_ptr;
		for (count = 0; count < store_len; count++)
			party_encryptor[count] ^= 0x6B;
		if ((error = FSWrite(file_id, &len, (char *) pc_ptr))  != 0){
			add_string_to_buf("Save: Couldn't write to file.         ");
			FSClose(file_id);
			for (count = 0; count < store_len; count++)
				party_encryptor[count] ^= 0x6B;
			SysBeep(2); 
			return;
		}
		for (count = 0; count < store_len; count++)
			party_encryptor[count] ^= 0x6B;
	}
	
	if (party_in_scen == TRUE) {
		// SAVE OUT DATA
		len = sizeof(out_info_type);
		if ((error = FSWrite(file_id, &len, (char *) out_e)) != 0) {
			add_string_to_buf("Save: Couldn't write to file.         ");
			FSClose(file_id);
			SysBeep(2);
			return;
		}
		if (town_save == TRUE) {
			current_town_type* town_ptr = &c_town;
			len = sizeof(current_town_type);
			if ((error = FSWrite(file_id, &len, (char *) town_ptr)) != 0) {
				add_string_to_buf("Save: Couldn't write to file.         ");
				FSClose(file_id);
				SysBeep(2); 
				return;
			}
			big_tr_type* town_data_ptr = &t_d;	
			len = sizeof(big_tr_type);
			if ((error = FSWrite(file_id, &len, (char *) town_data_ptr)) != 0) {
				add_string_to_buf("Save: Couldn't write to file.         ");
				FSClose(file_id);
				SysBeep(2); 
				return;
			}
			town_item_list* item_ptr = &t_i;	
			len = sizeof(town_item_list);
			if ((error = FSWrite(file_id, &len, (char *) item_ptr)) != 0) {
				add_string_to_buf("Save: Couldn't write to file.         ");
				FSClose(file_id);
				SysBeep(2); 
				return;
			}	
		}
		// Save stored items 
		for (i = 0; i < 3; i++) {
			stored_items_list_type* items_ptr = &stored_items[i];
			len = (long) sizeof(stored_items_list_type);
			if ((error = FSWrite(file_id, &len, (char *) items_ptr))  != 0){
				add_string_to_buf("Save: Couldn't write to file.         ");
				FSClose(file_id);
				SysBeep(2); 
				return;
			}
		}
		// If saving maps, save maps
		if (save_maps == TRUE) {
			stored_town_maps_type* maps_ptr = &(town_maps);
			len = (long) sizeof(stored_town_maps_type);
			if ((error = FSWrite(file_id, &len, (char *) maps_ptr))  != 0){
				add_string_to_buf("Save: Couldn't write to file.         ");
				FSClose(file_id);
				SysBeep(2); 
				return;
			}	
			stored_outdoor_maps_type* o_maps_ptr = &o_maps;
			len = (long) sizeof(stored_outdoor_maps_type);
			if ((error = FSWrite(file_id, &len, (char *) o_maps_ptr)) != 0) {
				add_string_to_buf("Save: Couldn't write to file.         ");
				FSClose(file_id);
				SysBeep(2); 
				return;
			}
		}
		// SAVE SFX and MISC_I
		len = (long) (64 * 64);
		if ((error = FSWrite(file_id, &len, (char *) sfx))  != 0){
			add_string_to_buf("Save: Couldn't write to file.         ");
			FSClose(file_id);
			SysBeep(2); 
			return;
		}
		if ((error = FSWrite(file_id, &len, (char *) misc_i))  != 0){
			add_string_to_buf("Save: Couldn't write to file.         ");
			FSClose(file_id);
			SysBeep(2); 
			return;
		}
	}
	if ((error = FSClose(file_id)) != 0) {
		add_string_to_buf("Save: Couldn't close file.         ");
		SysBeep(2);
		return;
	}
	file_to_load = to_save;
}

void leave_town()
{
	store_flags[0] = 5790;
}

void remove_party_from_scen()
{
	store_flags[1] = 200;
	party_in_scen = FALSE;
	load_base_item_defs();
}

long open_pref_file()
{
	Str255 pref_name;
	short vol_ref = 0;
	long dir_ID = 0,stored_key;
	FSSpec pref;
	short file_ref_num;
	PrefHandle data_handle;
	
	GetIndString(pref_name,5,19);
	if (sys_7_avail == TRUE)
		FindFolder(kOnSystemDisk,kPreferencesFolderType,kDontCreateFolder,&vol_ref,&dir_ID);
	FSMakeFSSpec(vol_ref,dir_ID,pref_name,&pref);
	file_ref_num = FSpOpenResFile(&pref,fsCurPerm);
	if (file_ref_num == -1) {
		save_prefs();
		//make_pref_file(pref);
		return -1;
	}
		
	UseResFile(file_ref_num);
	data_handle = (PrefHandle) Get1Resource('PRFN',128);
	
	if ((**data_handle).l[2] != 0)
		give_intro_hint = TRUE;
	else
		give_intro_hint = FALSE;
	display_mode = (short) ((**data_handle).l[3]);
	play_sounds = (short) ((**data_handle).l[4]);
	register_flag = (long) (800000) - (**data_handle).l[5];
	stored_key = (long) (700000) - (**data_handle).l[7];
	ed_flag = (long) (800000) - (**data_handle).l[6];
	ed_key = (long) (700000) - (**data_handle).l[8];
	
	if ((registered == TRUE) && (stored_key != init_data(register_flag))) {
		display_mode = 0;
		CloseResFile(file_ref_num);
		save_prefs();
		return -100;	
	}
	if ((stored_key != init_data(register_flag)) && ((register_flag < 10000) || (register_flag > 30000)
		|| (display_mode < 0) || (display_mode > 5) || (ed_flag < 0) || (ed_flag > 10000)) ) {
		registered = FALSE;
		if  ((register_flag < 10000) || (register_flag > 30000))
			register_flag = get_ran(1,10000,30000);
		if ((ed_flag < 0) || (ed_flag > 10000))
			ed_flag = 10000;
		
		stored_key = 0;
		display_mode = 0;
		CloseResFile(file_ref_num);
		save_prefs();
		return -100;
	}

	CloseResFile(file_ref_num);
	
	return stored_key;
}

void make_pref_file(FSSpec pref)
{
	short file_ref_num;
	Handle app_handle;
	short res_ID;
	ResType res_type;
	Str255 res_name = "xxx";
	short res_attributes;
	short i,rf;

	
//	rf = CurResFile();
//	UseResFile(rf);
//	app_handle = Get1Resource('PRFN',128);
	
//	for (i = 0; i < 10; i++)
//		(**(PrefHandle)app_handle).l[i] = (long) (get_ran(1,-20000,20000)) * (i + 2);
//	(**(PrefHandle)app_handle).l[2] = (long) (give_intro_hint);
//	(**(PrefHandle)app_handle).l[3] = (long) (display_mode);
//	(**(PrefHandle)app_handle).l[4] = (long) (play_sounds);

//	if (register_flag == 0) {
//		register_flag = (long) get_ran(1,10000,20000);
//		ed_flag = get_ran(1,5000,9999);
//		}

	// Amputating this code, cause it's broken, while save prefs code works OK
	
//	(**(PrefHandle)app_handle).l[5] = (long) (800000) - register_flag;
//	if (registered == TRUE)
//		(**(PrefHandle)app_handle).l[7] = (long) (700000) - init_data(register_flag);

//	(**(PrefHandle)app_handle).l[6] = (long) (800000) - ed_flag;
//	if (ed_reg == TRUE)
//		(**(PrefHandle)app_handle).l[8] = (long) (700000) - init_data(ed_flag);

//	GetResInfo((Handle) app_handle,&res_ID,&res_type,res_name);
//	res_attributes = GetResAttrs(app_handle);
//	DetachResource(app_handle);
	FSpCreateResFile(&pref,'EX3!','E3PR',smSystemScript);
//	save_prefs();
	return;

//	FSpCreateResFile(&pref,'RSED','rsrc',smSystemScript);
	file_ref_num = FSpOpenResFile(&pref,fsCurPerm);

	UseResFile(file_ref_num);
	AddResource(app_handle,res_type,res_ID,res_name);
	SetResAttrs(app_handle,res_attributes);
	ChangedResource(app_handle);		
	WriteResource(app_handle);		
	ReleaseResource(app_handle);		
	CloseResFile(file_ref_num);
}

void save_prefs()
{
	short file_ref_num;
	Handle old_handle,data_handle;
	short vol_ref,i;
	short res_ID;
	long dir_ID;
	ResType res_type = 'PRFN';
	Str255 res_name;
	short res_attributes;
	StringPtr source_str;
	Size byte_count;
	FSSpec pref;
	short app_res_num;
	Str255 pref_name;
	
	app_res_num = CurResFile();
	
	GetIndString(pref_name,5,19);
	if (sys_7_avail == TRUE)
		FindFolder(kOnSystemDisk,kPreferencesFolderType,
			kDontCreateFolder,&vol_ref,&dir_ID);
	FSMakeFSSpec(vol_ref,dir_ID,pref_name,&pref);
	file_ref_num = FSpOpenResFile(&pref,fsCurPerm);
	if (file_ref_num == -1) {
		make_pref_file(pref);
		file_ref_num = FSpOpenResFile(&pref,fsCurPerm);
//		return;
	}
		
	UseResFile(file_ref_num);

	data_handle = NewHandleClear(sizeof(PrefRecord));
	HLock(data_handle);

	for (i = 0; i < 10; i++)
		(**(PrefHandle)data_handle).l[i] = (long) (get_ran(1,-20000,20000)) * (i + 2);
	(**(PrefHandle)data_handle).l[2] = (long) (give_intro_hint);
	(**(PrefHandle)data_handle).l[3] = (long) (display_mode);
	(**(PrefHandle)data_handle).l[4] = (long) (play_sounds);
	

	if (register_flag == 0) {
		register_flag = (long) get_ran(1,10000,20000);
		ed_flag = get_ran(1,5000,9999);
	}
	(**(PrefHandle)data_handle).l[5] = (long) (800000) - register_flag;
	(**(PrefHandle)data_handle).l[6] = (long) (800000) - ed_flag;
	if (registered == TRUE)
		(**(PrefHandle)data_handle).l[7] = (long) (700000) - init_data(register_flag);
	if (ed_reg == TRUE)
		(**(PrefHandle)data_handle).l[8] = (long) (700000) - init_data(ed_flag);
	
	old_handle = Get1Resource('PRFN',128);
	GetResInfo(old_handle,&res_ID,&res_type,res_name);
	res_attributes = GetResAttrs(old_handle);
	RemoveResource(old_handle);
	
	AddResource(data_handle,'PRFN',128,res_name);
	WriteResource(data_handle);
	HUnlock(data_handle);
	ReleaseResource(data_handle);

	CloseResFile(file_ref_num);
	UseResFile(app_res_num);
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
	CFStringRef progPath = CFURLCopyFileSystemPath(progURL, kCFURLPOSIXPathStyle);
	CFRange findRes;
	if(!CFStringFindWithOptions(progPath, CFSTR("/"), CFRangeMake(0, CFStringGetLength(progPath)), kCFCompareBackwards, &findRes)){
		printf("Error: Unable to find scenario directory\n");
		return(false);
	}
	CFStringRef scenPath = CFStringCreateWithFormat(NULL,NULL,CFSTR("%@/Blades of Exile Scenarios/%s"),CFStringCreateWithSubstring(NULL, progPath, CFRangeMake(0,findRes.location)),scen_name);
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