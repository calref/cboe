#include <Windows.h>
#include <commdlg.h>
#include "string.h"
#include "Global.h"
#include "stdio.h"
#include "tfileio.h"
#include "keydlgs.h"
#include "graphics.h"
#include "scenario.h"
#include "edsound.h"
#include "graphutl.h"

#define	DONE_BUTTON_ITEM	1
#define	NIL		0L

extern char szBladBase[128];	// blscened.cpp

extern scenario_data_type scenario;
extern HDC main_dc;

ave_tr_type ave_t;
tiny_tr_type tiny_t;
	scenario_data_type *temp_scenario;

HWND	the_dialog;
extern HWND mainPtr;
extern town_record_type town;
extern big_tr_type t_d;
extern short town_type, max_dim[3];  // 0 - big 1 - ave 2 - small
extern short cur_town,overall_mode;
extern location cur_out;
extern piles_of_stuff_dumping_type *data_store;
extern talking_record_type talking;
extern outdoor_record_type current_terrain;
extern unsigned char borders[4][50];
extern Boolean change_made;
extern scen_item_data_type scen_item_list;
extern char scen_strs[160][256];
extern char scen_strs2[110][256];
extern char talk_strs[170][256];
extern char town_strs[180][256];
extern HBITMAP spec_scen_g;
extern HBITMAP spec_scen_zoom_g;
extern char current_string[256] ;
short specials_res_id,data_dump_file_id;
char start_name[256];
short start_volume,data_volume,jl = 0;
long start_dir,data_dir;
// outdoor_record_type current_terrain;
Boolean cur_scen_is_win = TRUE;
talking_record_type *dummy_talk_ptr;			
town_record_type *dummy_town_ptr;
extern char file_path_name[256];

// Big waste!
char last_load_file[63] = "newscen.exs";
char szFileName [128] = "newscen.exs";
char szFileName2 [128] = "tempscen.exs";
char szTitleName [128] = "newscen.exs";
OPENFILENAME ofn;
SCROLLINFO lpsi;
OFSTRUCT store;
Boolean suppress_load_file_name = FALSE;

void print_write_position ();

void Get_Path(char* path){

char file_path[256]="";                   // initialization
GetModuleFileName(NULL,file_path,256); // get path to the executable

int i=255;                       // initialize the first while loop
while(file_path[i] != '\\')
   {
    i--;                                   // find the last '\' in the path to the executable
   }                                       // in order to get rid of 'blades of exile.exe'

   
int j=0;                                // initialize the second loop

for(j=0;j<i+1;j++)
   {
    path[j]=file_path[j];           // transfert the path to argument string
   }
path[i+1]='\0';                  // close the argument string after the last '\'
}

void file_initialize()
{

static char *szFilter[] = {"Blades of Exile Scenarios (*.EXS)","*.exs",
		"Text Files (*.TXT)","*.txt","All Files (*.*)","*.*",""};


		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = mainPtr;
		ofn.hInstance = NULL;
		ofn.lpstrFilter = szFilter[0];
		ofn.lpstrCustomFilter = NULL;
		ofn.nMaxCustFilter = 0;
		ofn.nFilterIndex = 0;
		ofn.lpstrFile = NULL;
		ofn.nMaxFile = 128;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 128;
		ofn.lpstrInitialDir = "scenarios\\";
		ofn.lpstrTitle = NULL;
		ofn.Flags = 0;
		ofn.nFileOffset = 0;
		ofn.nFileExtension = 0;
		ofn.lpstrDefExt = "exs";
		ofn.lCustData = 0L;
		ofn.lpfnHook = NULL;
		ofn.lpTemplateName = NULL;
		
		lpsi.cbSize = sizeof(SCROLLINFO);
		lpsi.fMask = SIF_RANGE;
		lpsi.nMin = 0;
		lpsi.nMax = 0;
    	lpsi.nPos = 0;	
		
}

// Here we go. this is going to hurt.
// Note no save as is available for scenarios.
//At this point, szFileName MUST contain the name for the currently edited scen.
// Strategy ... assemble a big Dummy file containing the whole scenario 
//chunk by chunk, copy the dummy over the original, and delete the dummy
// the whole scenario is too big be be shifted around at once
void save_scenario()
{
	char *buffer = NULL;
	DWORD buf_len = 100005;
	short error;
	short out_num;
	long scen_ptr_move = 0,save_town_size = 0,save_out_size = 0;
	outdoor_record_type *dummy_out_ptr;
	HGLOBAL temp_buffer;
	long total_file_size = 0;

	short i,j,k,num_outdoors;
	HANDLE file_id, dummy_f;
	DWORD dwByteRead, len;


	//OK. First find out what file name we're working with, and make the dummy file
	// which we'll build the new scenario in
	dummy_f = CreateFile(szFileName2, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (dummy_f == INVALID_HANDLE_VALUE){
			oops_error(11);
			return;
			}
	
	file_id = CreateFile(szFileName, GENERIC_READ , FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file_id == INVALID_HANDLE_VALUE){
			oops_error(12);
			return;
			}

	// Now we need to set up a buffer for moving the data over to the dummy
	temp_buffer = GlobalAlloc(GMEM_FIXED,buf_len);
	if (temp_buffer == NULL) {
			CloseHandle(file_id); 	CloseHandle(dummy_f); oops_error(14);
		return;
		}
	buffer = (char*) GlobalLock(temp_buffer);
	if (buffer == NULL) {
		CloseHandle(file_id); CloseHandle(dummy_f); oops_error(14);
		return;
		}
	scenario.prog_make_ver[0] = 1;
	scenario.prog_make_ver[1] = 0;
	scenario.prog_make_ver[2] = 0;

	// Now, the pointer in scen_f needs to move along, so that the correct towns are sucked in.
	// To do so, we'll remember the size of the saved town and out now.
	out_num = cur_out.y * scenario.out_width + cur_out.x;
	save_out_size = (long) (scenario.out_data_size[out_num][0]) + (long) (scenario.out_data_size[out_num][1]);
	save_town_size = (long) (scenario.town_data_size[cur_town][0]) + (long) (scenario.town_data_size[cur_town][1])
					+ (long) (scenario.town_data_size[cur_town][2]) + (long) (scenario.town_data_size[cur_town][3])
					+ (long) (scenario.town_data_size[cur_town][4]);
	scen_ptr_move = sizeof(scenario_data_type);
	scen_ptr_move += sizeof(scen_item_data_type);
	for (i = 0; i < 270; i++)  // scenario strings
		scen_ptr_move += scenario.scen_str_len[i];



	// We're finally set up. Let's first set up the new scenario field
	// We need the new info for the current town and outdoors, which may have been changed
	scenario.town_data_size[cur_town][0] = sizeof(town_record_type);
	if (scenario.town_size[cur_town] == 0) 
		scenario.town_data_size[cur_town][0] += sizeof(big_tr_type);
		else if (scenario.town_size[cur_town] == 1) 
			scenario.town_data_size[cur_town][0] += sizeof(ave_tr_type);
			else scenario.town_data_size[cur_town][0] += sizeof(tiny_tr_type);
	scenario.town_data_size[cur_town][1] = 0;
	for (i = 0; i < 60; i++)
		scenario.town_data_size[cur_town][1] += strlen(town_strs[i]);
	scenario.town_data_size[cur_town][2] = 0;
	for (i = 60; i < 140; i++)
		scenario.town_data_size[cur_town][2] += strlen(town_strs[i]);
	scenario.town_data_size[cur_town][3] = sizeof(talking_record_type);
	for (i = 0; i < 80; i++)
		scenario.town_data_size[cur_town][3] += strlen(talk_strs[i]);
	scenario.town_data_size[cur_town][4] = 0;
	for (i = 80; i < 170; i++)
		scenario.town_data_size[cur_town][4] += strlen(talk_strs[i]);

	scenario.out_data_size[out_num][0] = sizeof(outdoor_record_type);
	scenario.out_data_size[out_num][1] = 0;
	for (i = 0; i < 120; i++)
		scenario.out_data_size[out_num][1] += strlen(data_store->out_strs[i]);
		
	for (i = 0; i < 300; i++) 
		scenario.scen_str_len[i] = 0;
	for (i = 0; i < 160; i++)
		scenario.scen_str_len[i] = strlen(scen_strs[i]);
	for (i = 160; i < 270; i++)
		scenario.scen_str_len[i] = strlen(scen_strs2[i - 160]);
	scenario.last_town_edited = cur_town;
	scenario.last_out_edited = cur_out;
	
	// now write scenario data
	scenario.flag1 = 20;
	scenario.flag2 = 40;
	scenario.flag3 = 60;
	scenario.flag4 = 80; /// these mean made on PC

    // scenario data
	if(WriteFile(dummy_f, &scenario, sizeof(scenario_data_type), &dwByteRead, NULL) == FALSE){
        CloseHandle(file_id); CloseHandle(dummy_f);oops_error(15);
		return;
        }
		
    // item data
    if(WriteFile(dummy_f, &scen_item_list, sizeof(scen_item_data_type), &dwByteRead, NULL) == FALSE){
        CloseHandle(file_id); CloseHandle(dummy_f);oops_error(16);
		return;}
	for (i = 0; i < 270; i++) { // scenario strings
		if ( i < 160) {
            if(WriteFile(dummy_f, &(scen_strs[i]), scenario.scen_str_len[i], &dwByteRead, NULL) == FALSE){
                CloseHandle(file_id); CloseHandle(dummy_f);oops_error(17);
		        return;
                    }
			}
			else {
                if(WriteFile(dummy_f, &(scen_strs2[i - 160]), scenario.scen_str_len[i], &dwByteRead, NULL) == FALSE){
                    CloseHandle(file_id); CloseHandle(dummy_f);oops_error(17);
		            return;
                    }
				}	
		}
	
	SetFilePointer(file_id,scen_ptr_move,NULL,0);
	
	// OK ... scenario written. Now outdoors.
	num_outdoors = scenario.out_width * scenario.out_height;
	for (i = 0; i < num_outdoors; i++)
		if (i == out_num) {
			// write outdoors
			for (j = 0; j < 180; j++)
				current_terrain.strlens[j] = 0;
			for (j = 0; j < 120; j++)
				current_terrain.strlens[j] = strlen(data_store->out_strs[j]);
			if(WriteFile(dummy_f, &current_terrain, sizeof(outdoor_record_type), &dwByteRead, NULL) == FALSE){
                    CloseHandle(file_id); CloseHandle(dummy_f);oops_error(18);
		            return;
                    }
			for (j = 0; j < 120; j++) {
				WriteFile(dummy_f, &(data_store->out_strs[j]), current_terrain.strlens[j], &dwByteRead, NULL);
				}

        	SetFilePointer(file_id,save_out_size,NULL,1);
			}
			else {
				len = (long) (scenario.out_data_size[i][0]) + (long) (scenario.out_data_size[i][1]);
				error = ReadFile(file_id, buffer, len, &dwByteRead, NULL);
				dummy_out_ptr = (outdoor_record_type *) buffer;
				port_out(dummy_out_ptr);
				if (error == FALSE) {CloseHandle(file_id); CloseHandle(dummy_f);oops_error(19);}
				if (WriteFile(dummy_f, buffer, len, &dwByteRead, NULL) == FALSE) {
					SysBeep(2); CloseHandle(file_id); CloseHandle(dummy_f);oops_error(20);
					return;
					}			
				}
				
	// now, finally, write towns.
	for (k = 0; k < scenario.num_towns; k++)
		if (k == cur_town) {
			for (i = 0; i < 180; i++)
				town.strlens[i] = 0;
			for (i = 0; i < 140; i++)
				town.strlens[i] = strlen(town_strs[i]);
			
			// write towns
			if(WriteFile(dummy_f, &town, sizeof(town_record_type), &dwByteRead, NULL) == FALSE){
            CloseHandle(file_id); CloseHandle(dummy_f);oops_error(21);}

			switch (scenario.town_size[cur_town]) {
				case 0:
					WriteFile(dummy_f, &t_d, sizeof(big_tr_type), &dwByteRead, NULL);
					break;
			
				case 1:
					for (i = 0; i < 48; i++)
						for (j = 0; j < 48; j++) {
							ave_t.terrain[i][j] = t_d.terrain[i][j];
							ave_t.lighting[i / 8][j] = t_d.lighting[i / 8][j];					
							}					
					for (i = 0; i < 16; i++) {
						ave_t.room_rect[i] = t_d.room_rect[i];
						}
					for (i = 0; i < 40; i++) {
						ave_t.creatures[i] = t_d.creatures[i];
						}
                    WriteFile(dummy_f, &ave_t, sizeof(ave_tr_type), &dwByteRead, NULL);
				break;
			
			
				case 2:
					for (i = 0; i < 32; i++)
						for (j = 0; j < 32; j++) {
							tiny_t.terrain[i][j] = t_d.terrain[i][j];
							tiny_t.lighting[i / 8][j] = t_d.lighting[i / 8][j];					
							}
					for (i = 0; i < 16; i++) {
						tiny_t.room_rect[i] = t_d.room_rect[i];
						}
					for (i = 0; i < 30; i++) {
						tiny_t.creatures[i] = t_d.creatures[i];
						}
                    WriteFile(dummy_f, &tiny_t, sizeof(tiny_tr_type), &dwByteRead, NULL)	;				
					break;
				}
			for (j = 0; j < 140; j++) {
                WriteFile(dummy_f, &(town_strs[j]), town.strlens[j], &dwByteRead, NULL);
				}

			for (i = 0; i < 200; i++)
				talking.strlens[i] = 0;
			for (i = 0; i < 170; i++)
				talking.strlens[i] = strlen(talk_strs[i]);
            if(WriteFile(dummy_f, &talking, sizeof(talking_record_type), &dwByteRead, NULL) == FALSE){
                CloseHandle(file_id); CloseHandle(dummy_f);oops_error(22);}
			for (j = 0; j < 170; j++) {
                WriteFile(dummy_f, &(talk_strs[j]), talking.strlens[j], &dwByteRead, NULL);			
				}
        	SetFilePointer(file_id,save_town_size,NULL,1);
			}
			else { /// load unedited town into buffer and save, doing translataions when necessary

                if(ReadFile(file_id, buffer, sizeof(town_record_type), &dwByteRead, NULL)== FALSE){
                    CloseHandle(file_id); CloseHandle(dummy_f);oops_error(24);}
				dummy_town_ptr = (town_record_type *) buffer;
				port_dummy_town();
				if (WriteFile(dummy_f,buffer,sizeof(town_record_type),&dwByteRead, NULL) == FALSE){  
                        CloseHandle(file_id); CloseHandle(dummy_f);oops_error(23);return;}						
			if (scenario.town_size[k] == 0) 
					len = (long) ( sizeof(big_tr_type));
					else if (scenario.town_size[k] == 1) 
						len = (long) ( sizeof(ave_tr_type));
						else len = (long) ( sizeof(tiny_tr_type));
	            if(ReadFile(file_id,buffer,len,&dwByteRead,NULL)== FALSE){			
                CloseHandle(file_id); CloseHandle(dummy_f);oops_error(24);}
				port_dummy_t_d(scenario.town_size[k],buffer);
				if (WriteFile(dummy_f,buffer,len,&dwByteRead, NULL)== FALSE){
                CloseHandle(file_id); CloseHandle(dummy_f);oops_error(23);return;}						
				
				len = (long) (scenario.town_data_size[k][1])
					+ (long) (scenario.town_data_size[k][2]);
                if(ReadFile(file_id,buffer,len,&dwByteRead,NULL) == FALSE){
                CloseHandle(file_id); CloseHandle(dummy_f);oops_error(24);}
                if(WriteFile(dummy_f, buffer, len, &dwByteRead, NULL) == FALSE){
                CloseHandle(file_id); CloseHandle(dummy_f);oops_error(23);return;}						
				len = (long) (scenario.town_data_size[k][3]);
                if(ReadFile(file_id, buffer, len, &dwByteRead, NULL) == FALSE){
                CloseHandle(file_id); CloseHandle(dummy_f);oops_error(24);}
				dummy_talk_ptr = (talking_record_type *) buffer;
				port_dummy_talk_nodes();
                if(WriteFile(dummy_f, buffer, len, &dwByteRead, NULL) == FALSE){
                CloseHandle(file_id); CloseHandle(dummy_f);oops_error(23);return;}						
				len = (long) (scenario.town_data_size[k][4]);
                if(ReadFile(file_id, buffer, len, &dwByteRead, NULL) == FALSE){
                CloseHandle(file_id); CloseHandle(dummy_f);oops_error(24);}
                if(WriteFile(dummy_f, buffer, len, &dwByteRead, NULL) == FALSE){
                    CloseHandle(file_id); CloseHandle(dummy_f);oops_error(23);return;}
				}
	
	change_made = FALSE;


	// now, everything is moved over. Delete the original, and move everything from the
	// dummy into a new original
	if(CloseHandle(file_id) == 0){
    CloseHandle(file_id); CloseHandle(dummy_f);oops_error(25);}
	cur_scen_is_win = TRUE;
	
    file_id = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(file_id == INVALID_HANDLE_VALUE ){
		oops_error(100);
		return;
		}
	total_file_size = (long) SetFilePointer (dummy_f,0,NULL,2);

    SetFilePointer(dummy_f,0,NULL,0);

	while (total_file_size > 0) {
		if (total_file_size >= 60000)
			len = 60000;
			else len = total_file_size;
        if(ReadFile(dummy_f, buffer, len, &dwByteRead, NULL) == FALSE){
        CloseHandle(dummy_f);oops_error(102);
        }
        if(WriteFile(file_id, buffer, len, &dwByteRead, NULL) == FALSE){
        CloseHandle(file_id); CloseHandle(dummy_f);oops_error(103);return;}
		total_file_size -= len;
		}
	error = CloseHandle(dummy_f);
	error = CloseHandle(file_id);
	if (error == 0) {CloseHandle(file_id); CloseHandle(dummy_f);oops_error(26);}
	DeleteFile(szFileName2);
	GlobalUnlock(temp_buffer);
	GlobalFree(temp_buffer);
	
}

void load_scenario()
{
	short i,j,k,l;
	Boolean file_ok = FALSE;
	HANDLE file_id;
	short error;
	DWORD dwByteRead, len;

	ofn.hwndOwner = mainPtr;
	ofn.lpstrFile = szFileName;
	ofn.lpstrFileTitle = szTitleName;
	ofn.Flags = 0;

	if (GetOpenFileName(&ofn) == 0)
		return;
		
	SetCurrentDirectory(file_path_name);
		
	file_id = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file_id == INVALID_HANDLE_VALUE)
	{
		oops_error(28);
		beep();
		return;
	}
	
	if (ReadFile(file_id, &scenario, sizeof(scenario_data_type), &dwByteRead, NULL) == FALSE)
	{
		CloseHandle(file_id);
		oops_error(30);
		return;
	}
		
	if ((scenario.flag1 == 10) && (scenario.flag2 == 20)
	 && (scenario.flag3 == 30)
	  && (scenario.flag4 == 40)) {
		cur_scen_is_win = FALSE;
		file_ok = TRUE;
		port_scenario();
		}
	if ((scenario.flag1 == 20) && (scenario.flag2 == 40)
	 && (scenario.flag3 == 60)
	  && (scenario.flag4 == 80)) {

		cur_scen_is_win = TRUE;
		file_ok = TRUE;
		}
	 if (file_ok == FALSE) {
		CloseHandle(file_id);
		give_error("This is not a legitimate Blades of Exile scenario.","",0);
		return;
		}
		
	len = sizeof(scen_item_data_type); // item data
	
	if (ReadFile(file_id, &scen_item_list, sizeof(scen_item_data_type), &dwByteRead, NULL) == FALSE)
	{
		CloseHandle(file_id);
		oops_error(30);
		return;
	}
		
	port_item_list();
	for (i = 0; i < 270; i++) {
		len = scenario.scen_str_len[i];
		if (i < 160) {
			ReadFile(file_id, &(scen_strs[i]), len, &dwByteRead, NULL);
			scen_strs[i][len] = 0;
			}
			else {
				ReadFile(file_id, &(scen_strs2[i - 160]), len, &dwByteRead, NULL);
				scen_strs2[i - 160][len] = 0;
				}
		}
	
	CloseHandle(file_id);
	
	//store_file_reply = file_to_load;
	overall_mode = 60;
	change_made = FALSE;
	load_town(scenario.last_town_edited);
	//load_town(0);
	load_outdoors(scenario.last_out_edited,0);
	load_spec_graphics();
	augment_terrain(scenario.last_out_edited);
}

//extern GWorldPtr spec_scen_g;
void load_spec_graphics()
{
	short i;
	char file_name[256];

	if (spec_scen_g != NULL) {
		DeleteObject(spec_scen_g);
		spec_scen_g = NULL;
		}

	//build_scen_file_name(file_name);
	sprintf(file_name,"%s",szFileName);
    i = strlen(file_name);
	file_name[i - 3] = 'b';
	file_name[i - 2] = 'm';
	file_name[i - 1] = 'p';
	suppress_load_file_name = TRUE;
	spec_scen_g = ReadBMP(file_name);
	suppress_load_file_name = FALSE;


/*	if (spec_scen_zoom_g != NULL) {
		DeleteObject(spec_scen_zoom_g);
		spec_scen_zoom_g = NULL;
		}

	//build_scen_file_name(file_name);
	sprintf(file_name,"%szoom",szFileName);
	for (i = 0; i < 256; i++) {
		if (file_name[i] == '.') {
  			file_name[i + 1] = 'b';
			file_name[i + 2] = 'm';
			file_name[i + 3] = 'p';
			i = 256;
			}
		}
	suppress_load_file_name = TRUE;
	spec_scen_zoom_g = ReadBMP(file_name);
	suppress_load_file_name = FALSE;
	*/
}


void augment_terrain(location to_create)
{
	location to_load;
	short i,j;
	
	for (i = 0 ; i < 4; i++)
		for (j = 0 ; j < 50; j++)
			borders[i][j] = 90;
			
	to_load = to_create;
	if (to_create.y > 0) {
		to_load.y--;
		load_outdoors(to_load, 1);
		}
	to_load = to_create;
	if (to_create.y < scenario.out_height - 1) {
		to_load.y++;
		load_outdoors(to_load, 3);
		}

	to_load = to_create;
	if (to_create.x < scenario.out_width - 1) {
		to_load.x++;
		load_outdoors(to_load, 2);
		}
	to_load = to_create;
	if (to_create.x > 0) {
		to_load.x--;
		load_outdoors(to_load, 4);
		}
}

//mode -> 0 - primary load  1 - add to top  2 - right  3 - bottom  4 - left
void load_outdoors(location which_out,short mode)
{
	short i,j;
    HANDLE file_id;
    DWORD dwByteRead;
	long len_to_jump,store;
	short out_sec_num;
	outdoor_record_type store_out;
	short error;

	if (overall_mode == 61)
		return;

	file_id = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file_id == INVALID_HANDLE_VALUE){
			oops_error(31);
			return;
			}
	
	out_sec_num = scenario.out_width * which_out.y + which_out.x;
	
	len_to_jump = sizeof(scenario_data_type);
	len_to_jump += sizeof(scen_item_data_type);
	for (i = 0; i < 300; i++)
		len_to_jump += (long) scenario.scen_str_len[i];
	store = 0;
	for (i = 0; i < out_sec_num; i++)
		for (j = 0; j < 2; j++)
			store += (long) (scenario.out_data_size[i][j]);
	len_to_jump += store;
	
    if(SetFilePointer(file_id,len_to_jump,NULL,0) == INVALID_SET_FILE_POINTER){CloseHandle(file_id);oops_error(32);}

    if(ReadFile(file_id, &store_out, sizeof(outdoor_record_type),&dwByteRead, NULL) == FALSE){
    CloseHandle(file_id);oops_error(33);}

	if (mode == 0) {
		current_terrain = store_out;
   		port_out(&current_terrain);
		for (i = 0; i < 120; i++) {
            ReadFile(file_id, &(data_store->out_strs[i]), current_terrain.strlens[i],&dwByteRead, NULL);
			data_store->out_strs[i][current_terrain.strlens[i]] = 0;
			}
		cur_out = which_out;
		}

	if (mode == 1) 
		for (j = 0; j < 48; j++) {				
				borders[0][j] = store_out.terrain[j][47];
				}
	if (mode == 2) 
		for (j = 0; j < 48; j++) {				
				borders[1][j] = store_out.terrain[0][j];
				}
	if (mode == 3) 
		for (j = 0; j < 48; j++) {				
				borders[2][j] = store_out.terrain[j][0];
				}
	if (mode == 4) 
		for (j = 0; j < 48; j++) {				
				borders[3][j] = store_out.terrain[47][j];
				}
				

	error = CloseHandle(file_id);
	if (error == 0) {CloseHandle(file_id);oops_error(33);}

}

void load_town(short which_town)
{
	short i,j;
    HANDLE file_id;
    DWORD dwByteRead;
	long len_to_jump = 0,store;
	short error;
	
	if (overall_mode == 61)
		return;
		
	file_id = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file_id == INVALID_HANDLE_VALUE){
			oops_error(34);
			return;
			}
	
	len_to_jump = sizeof(scenario_data_type);
	len_to_jump += sizeof(scen_item_data_type);
	for (i = 0; i < 300; i++)
		len_to_jump += (long) scenario.scen_str_len[i];
	store = 0;
	for (i = 0; i < 100; i++)
		for (j = 0; j < 2; j++)
			store += (long) (scenario.out_data_size[i][j]);
	for (i = 0; i < which_town; i++)
		for (j = 0; j < 5; j++)
			store += (long) (scenario.town_data_size[i][j]);
	len_to_jump += store;
	
	if(SetFilePointer(file_id,len_to_jump,NULL,0) == INVALID_SET_FILE_POINTER){CloseHandle(file_id);oops_error(35);}
	
	error = ReadFile(file_id, &town, sizeof(town_record_type),&dwByteRead,NULL);
	port_town();
	if (error == FALSE) {CloseHandle(file_id);oops_error(36);}

	switch (scenario.town_size[which_town]) {
		case 0:
            ReadFile(file_id, &t_d, sizeof(big_tr_type),&dwByteRead,NULL);
			port_t_d();
			break;

		case 1:
                ReadFile(file_id, &ave_t, sizeof(ave_tr_type),&dwByteRead,NULL);
				for (i = 0; i < 48; i++)
					for (j = 0; j < 48; j++) {
						t_d.terrain[i][j] = ave_t.terrain[i][j];
						t_d.lighting[i / 8][j] = ave_t.lighting[i / 8][j];
						}
				for (i = 0; i < 16; i++) {
					t_d.room_rect[i] = ave_t.room_rect[i];
					}
				for (i = 0; i < 40; i++) {
					t_d.creatures[i] = ave_t.creatures[i];
					}
				port_t_d();
				for (i = 40; i < 60; i++) {
					t_d.creatures[i].number = 0;
					}
			break;

		case 2:
            ReadFile(file_id, &tiny_t, sizeof(tiny_tr_type),&dwByteRead,NULL);
			for (i = 0; i < 32; i++)
				for (j = 0; j < 32; j++) {
					t_d.terrain[i][j] = tiny_t.terrain[i][j];
					t_d.lighting[i / 8][j] = tiny_t.lighting[i / 8][j];					
					}
			for (i = 0; i < 16; i++) {
				t_d.room_rect[i] = tiny_t.room_rect[i];
				}
			for (i = 0; i < 30; i++) {
				t_d.creatures[i] = tiny_t.creatures[i];
				}
			port_t_d();
			for (i = 30; i < 60; i++) {
				t_d.creatures[i].number = 0;
				}
			break;
		}


	for (i = 0; i < 140; i++) {
        ReadFile(file_id,&(town_strs[i]),town.strlens[i],&dwByteRead, NULL);
		town_strs[i][town.strlens[i]] = 0;
		}

    if(ReadFile(file_id, &talking, sizeof(talking_record_type), &dwByteRead, NULL) == FALSE){CloseHandle(file_id);oops_error(37);}
	port_talk_nodes();
	
	for (i = 0; i < 170; i++) {
		ReadFile(file_id,&(talk_strs[i]),talking.strlens[i],&dwByteRead, NULL);
		talk_strs[i][talking.strlens[i]] = 0;
		}
			
	town_type = scenario.town_size[which_town];
	cur_town = which_town;
	error = CloseHandle(file_id);
	if (error == 0) {CloseHandle(file_id);oops_error(38);}
}

// if which_town is -1, load town from base
void import_town(short which_town)
{
	short i,j;
	HANDLE file_id;
	Boolean file_ok = FALSE;
	short error;
	long len_to_jump = 0,store;
	DWORD buf_len = 100000;
	DWORD dwByteRead;
	char *buffer = NULL;
	char szFileName3 [128] = "scen.exs";
	HGLOBAL temp_buffer;
	
	if (which_town >= 0) {
		ofn.hwndOwner = mainPtr;
		ofn.lpstrFile = szFileName3;
		ofn.lpstrFileTitle = szTitleName;
		ofn.Flags = 0;

		if (GetOpenFileName(&ofn) == 0)
			return;
		}
		else {
//			sprintf((char *) szFileName3,".\\scenarios\\BLADBASE.EXS");
			strncpy(szFileName3, szBladBase, 128);
			which_town = 0;
			}

	SetCurrentDirectory(file_path_name);
	
	file_id = CreateFile(szFileName3, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file_id == INVALID_HANDLE_VALUE){
			oops_error(42);
			return;
			}
	
	temp_buffer = GlobalAlloc(GMEM_FIXED,buf_len);
	if (temp_buffer == NULL) {
		CloseHandle(file_id); oops_error(41);
		return;
		}
	buffer = (char *) GlobalLock(temp_buffer);
	if (buffer == NULL) {
		CloseHandle(file_id); oops_error(41);
		return;
		}

    if(ReadFile(file_id, buffer, sizeof(scenario_data_type),&dwByteRead,NULL) == FALSE){
		CloseHandle(file_id); oops_error(43); return;
		}
	temp_scenario = (scenario_data_type *) buffer;
	
	if (temp_scenario->town_size[which_town] != scenario.town_size[cur_town]) {
		give_error("Oops ... the town in the selected scenario and the current town are different sizes. Import failed.","",0);
		GlobalUnlock(temp_buffer); GlobalFree(temp_buffer); CloseHandle(file_id);
		return;
		}
	if (which_town >= temp_scenario->num_towns) {
		give_error("Oops ... the selected scenario doesn't have enough towns. The town you selected doesn't exist inside this scenario.","",0);
		GlobalUnlock(temp_buffer); GlobalFree(temp_buffer); CloseHandle(file_id); 
		return;
		}
	
	if ((temp_scenario->flag1 == 20) && (temp_scenario->flag2 == 40)
	 && (temp_scenario->flag3 == 60)
	  && (temp_scenario->flag4 == 80)) {
	  	file_ok = TRUE;
	  	}
	 if (file_ok == FALSE) {
		GlobalUnlock(temp_buffer); GlobalFree(temp_buffer); CloseHandle(file_id); give_error("This is not a legitimate Blades of Exile scenario. If it is a scenario, note that it needs to have been saved by the Windows scenario editor.","",0); return;	 
	 	}
		
	len_to_jump = sizeof(scenario_data_type);
	len_to_jump += sizeof(scen_item_data_type);
	for (i = 0; i < 300; i++)
		len_to_jump += (long) temp_scenario->scen_str_len[i];
	store = 0;
	for (i = 0; i < 100; i++)
		for (j = 0; j < 2; j++)
			store += (long) (temp_scenario->out_data_size[i][j]);
	for (i = 0; i < which_town; i++)
		for (j = 0; j < 5; j++)
			store += (long) (temp_scenario->town_data_size[i][j]);
	len_to_jump += store;

	if(SetFilePointer(file_id,len_to_jump,NULL,0) == INVALID_SET_FILE_POINTER){CloseHandle(file_id);oops_error(44);}
	
	if(ReadFile(file_id,&town,sizeof(town_record_type),&dwByteRead,NULL) == FALSE){
        CloseHandle(file_id);oops_error(45);}

	switch (temp_scenario->town_size[which_town]) {
		case 0:
            ReadFile(file_id,&t_d,sizeof(big_tr_type),&dwByteRead,NULL);
			break;
			
		case 1:
                ReadFile(file_id,&ave_t,sizeof(ave_tr_type),&dwByteRead,NULL);
				for (i = 0; i < 48; i++)
					for (j = 0; j < 48; j++) {
						t_d.terrain[i][j] = ave_t.terrain[i][j];
						t_d.lighting[i / 8][j] = ave_t.lighting[i / 8][j];					
						}
				for (i = 0; i < 16; i++) {
					t_d.room_rect[i] = ave_t.room_rect[i];
					}
				for (i = 0; i < 40; i++) {
					t_d.creatures[i] = ave_t.creatures[i];
					}
				for (i = 40; i < 60; i++) {
					t_d.creatures[i].number = 0;
					}
			break;
			
		case 2:
            ReadFile(file_id,&tiny_t,sizeof(tiny_tr_type),&dwByteRead,NULL);
			for (i = 0; i < 32; i++)
				for (j = 0; j < 32; j++) {
					t_d.terrain[i][j] = tiny_t.terrain[i][j];
					t_d.lighting[i / 8][j] = tiny_t.lighting[i / 8][j];					
					}
			for (i = 0; i < 16; i++) {
				t_d.room_rect[i] = tiny_t.room_rect[i];
				}
			for (i = 0; i < 30; i++) {
				t_d.creatures[i] = tiny_t.creatures[i];
				}
			for (i = 30; i < 60; i++) {
				t_d.creatures[i].number = 0;
				}
			break;
		}

	for (i = 0; i < 140; i++) {
        ReadFile(file_id,&(town_strs[i]),town.strlens[i],&dwByteRead,NULL);
		town_strs[i][town.strlens[i]] = 0;
		}

    if(ReadFile(file_id, &talking, sizeof(talking_record_type), &dwByteRead, NULL) == FALSE){
    CloseHandle(file_id);oops_error(46);}
	
	for (i = 0; i < 170; i++) {
        ReadFile(file_id, &(talk_strs[i]), talking.strlens[i], &dwByteRead, NULL);
		talk_strs[i][talking.strlens[i]] = 0;
		}

	error = CloseHandle(file_id);
	if (error == 0) {CloseHandle(file_id);oops_error(47);}
	GlobalUnlock(temp_buffer);
	GlobalFree(temp_buffer);	
}

// When this is called, the current town is the town to make town 0.
void make_new_scenario(char *file_name,short out_width,short out_height,short making_warriors_grove,
	short use_grass,char *title)
{
	short i,j,k,num_outdoors;
	HANDLE dummy_f,file_id;
	DWORD buf_len = 100000, dwByteRead;
	short error;
	short out_num;
	long len,scen_ptr_move = 0,save_town_size = 0,save_out_size = 0;
	location loc;
	short x,y;
	HGLOBAL temp_buffer;
	char *buffer;

	// Step 1 - load scenario file from scenario base. It contains all the monsters and
	// items done up properly!
	
		file_id = CreateFile(szBladBase, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file_id == INVALID_HANDLE_VALUE){
			give_error("The Blades Editor is having trouble finding the file 'bladbase.exs'.",
				"Please check approporiate settings in 'blades.ini'.",0);
			file_name[0] = 0;
			return;
			}

	sprintf(szFileName, "scenarios\\%s", file_name);

	temp_buffer = GlobalAlloc(GMEM_FIXED,buf_len);
	if (temp_buffer == NULL) {
		CloseHandle(file_id); oops_error(114);
		return;
		}
	buffer = (char *) GlobalLock(temp_buffer);
	if (buffer == NULL) {
		CloseHandle(file_id); oops_error(114);
		return;
		}
	
    if (ReadFile(file_id, &scenario, sizeof(scenario_data_type), &dwByteRead, NULL) == FALSE){
		CloseHandle(file_id); oops_error(82); return;
		}

    if (ReadFile(file_id, &scen_item_list, sizeof(scen_item_data_type), &dwByteRead, NULL) == FALSE){
		CloseHandle(file_id); oops_error(83); return;
		}
	for (i = 0; i < 270; i++) {
		if (i < 160) {
            ReadFile(file_id, &(scen_strs[i]), scenario.scen_str_len[i], &dwByteRead, NULL);
			scen_strs[i][scenario.scen_str_len[i]] = 0;
			}
			else {
                ReadFile(file_id, &(scen_strs2[i - 160]), scenario.scen_str_len[i], &dwByteRead, NULL);
				scen_strs2[i - 160][scenario.scen_str_len[i]] = 0;
				}
		}
	strcpy((char *) scen_strs[0],(char *) title);
	scenario.scen_str_len[0] = strlen(title);
	scen_strs[0][scenario.scen_str_len[0]] = 0;
	CloseHandle(file_id);

	//OK. FIrst find out what file name we're working with, and make the dummy file
	// which we'll build the new scenario in   Blades of Exile Base
		dummy_f = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		
		if (file_id == INVALID_HANDLE_VALUE){
		oops_error(111);
		return;
		}

	scenario.prog_make_ver[0] = 1;
	scenario.prog_make_ver[1] = 0;
	scenario.prog_make_ver[2] = 0;
	cur_town = 0;
	town_type = 1;
	scenario.num_towns = 1;
	scenario.town_size[0] = 1;
	scenario.out_width = out_width;
	scenario.out_height = out_height;	
	cur_out.x = 0;
	cur_out.y = 0;
	scenario.last_out_edited = cur_out;

	// We're finally set up. Let's first set up the new scenario field
	// We need the new info for the current town and outdoors, which may have been changed
	scenario.town_data_size[cur_town][0] = sizeof(town_record_type);
	if (scenario.town_size[cur_town] == 0) 
		scenario.town_data_size[cur_town][0] += sizeof(big_tr_type);
		else if (scenario.town_size[cur_town] == 1) 
			scenario.town_data_size[cur_town][0] += sizeof(ave_tr_type);
			else scenario.town_data_size[cur_town][0] += sizeof(tiny_tr_type);
	scenario.town_data_size[cur_town][1] = 0;
	for (i = 0; i < 60; i++)
		scenario.town_data_size[cur_town][1] += strlen(town_strs[i]);
	scenario.town_data_size[cur_town][2] = 0;
	for (i = 60; i < 140; i++)
		scenario.town_data_size[cur_town][2] += strlen(town_strs[i]);
	scenario.town_data_size[cur_town][3] = sizeof(talking_record_type);
	for (i = 0; i < 80; i++)
		scenario.town_data_size[cur_town][3] += strlen(talk_strs[i]);
	scenario.town_data_size[cur_town][4] = 0;
	for (i = 80; i < 170; i++)
		scenario.town_data_size[cur_town][4] += strlen(talk_strs[i]);

	num_outdoors = scenario.out_width * scenario.out_height;
	for (i = 0; i < num_outdoors; i++) {
		scenario.out_data_size[i][0] = sizeof(outdoor_record_type);
		scenario.out_data_size[i][1] = 0;
		for (j = 0; j < 120; j++)
			scenario.out_data_size[i][1] += strlen(data_store->out_strs[j]);
		}
		
	for (i = 0; i < 300; i++) 
		scenario.scen_str_len[i] = 0;
	for (i = 0; i < 160; i++)
		scenario.scen_str_len[i] = strlen(scen_strs[i]);
	for (i = 160; i < 270; i++)
		scenario.scen_str_len[i] = strlen(scen_strs2[i - 160]);
	scenario.last_town_edited = cur_town;
	scenario.last_out_edited = cur_out;
	
	// now write scenario data
	scenario.flag1 = 20;
	scenario.flag2 = 40;
	scenario.flag3 = 60;
	scenario.flag4 = 80; /// these mean made on mac
	
    //scenario data
	scen_ptr_move += sizeof(scenario_data_type);
    if (WriteFile(dummy_f, &scenario, sizeof(scenario_data_type), &dwByteRead, NULL) == FALSE){
		CloseHandle(dummy_f);
		oops_error(3);
		return;
		}	
    // item data
	scen_ptr_move += sizeof(scen_item_data_type);
    if (WriteFile(dummy_f, &scen_item_list, sizeof(scen_item_data_type), &dwByteRead, NULL) == FALSE){
 		CloseHandle(dummy_f);
		oops_error(4);
		return;
		}	
	for (i = 0; i < 270; i++) { // scenario strings
		scen_ptr_move += scenario.scen_str_len[i];
		if (i < 160) {
            if (WriteFile(dummy_f, &(scen_strs[i]), scenario.scen_str_len[i], &dwByteRead, NULL) == FALSE){
				CloseHandle(dummy_f);oops_error(5);
				return;
				}		
			}	
			else {
                if (WriteFile(dummy_f, &(scen_strs2[i - 160]), scenario.scen_str_len[i], &dwByteRead, NULL) == FALSE){
					CloseHandle(dummy_f);oops_error(5);
					return;
					}		
				}
		}
	
	
	// OK ... scenario written. Now outdoors.
	num_outdoors = scenario.out_width * scenario.out_height;
	for (i = 0; i < num_outdoors; i++)
		{
			loc.x = i % scenario.out_width;
			loc.y = i / scenario.out_width;
			
			for (x = 0; x < 48; x++)
				for (y = 0; y < 48; y++) {
					current_terrain.terrain[x][y] = (use_grass > 0) ? 2 : 0;
					
					if ((x < 4) && (loc.x == 0))
						current_terrain.terrain[x][y] = (use_grass > 0) ? 22 : 5;
					if ((y < 4) && (loc.y == 0))
						current_terrain.terrain[x][y] = (use_grass > 0) ? 22 : 5;
					if ((x > 43) && (loc.x == scenario.out_width - 1))
						current_terrain.terrain[x][y] = (use_grass > 0) ? 22 : 5;
					if ((y > 43) && (loc.y == scenario.out_height - 1))
						current_terrain.terrain[x][y] = (use_grass > 0) ? 22 : 5;
					if ((i == 0) && (making_warriors_grove > 0)) {
						current_terrain.terrain[24][24] = (use_grass > 0) ? 234 : 232;
						current_terrain.exit_locs[0].x = 24;
						current_terrain.exit_locs[0].y = 24;
						current_terrain.exit_dests[0] = 0;
						}
						else current_terrain.exit_locs[0].x = 100;
					}
					
			// write outdoors
			for (j = 0; j < 180; j++)
				current_terrain.strlens[j] = 0;
			for (j = 0; j < 120; j++)
				current_terrain.strlens[j] = strlen(data_store->out_strs[j]);
            if (WriteFile(dummy_f, &current_terrain, sizeof(outdoor_record_type), &dwByteRead, NULL) == FALSE){
                CloseHandle(dummy_f);oops_error(6);}

			for (j = 0; j < 120; j++) {
                if (WriteFile(dummy_f, &(data_store->out_strs[j]), current_terrain.strlens[j], &dwByteRead, NULL) == FALSE){
                CloseHandle(dummy_f);oops_error(7);}
				}
			
			}


	// now, finally, write towns.
	for (k = 0; k < scenario.num_towns; k++)
		{
			for (i = 0; i < 180; i++)
				town.strlens[i] = 0;
			for (i = 0; i < 140; i++)
				town.strlens[i] = strlen(town_strs[i]);
			
			// write towns
            if(WriteFile(dummy_f, &town, sizeof(town_record_type), &dwByteRead, NULL) == FALSE){
            CloseHandle(dummy_f);oops_error(8);}

			switch (scenario.town_size[cur_town]) {
				case 0:
                    WriteFile(dummy_f, &t_d, sizeof(big_tr_type), &dwByteRead, NULL);
					break;
			
				case 1:
					for (i = 0; i < 48; i++)
						for (j = 0; j < 48; j++) {
							ave_t.terrain[i][j] = t_d.terrain[i][j];
							ave_t.lighting[i / 8][j] = t_d.lighting[i / 8][j];					
							}					
					for (i = 0; i < 16; i++) {
						ave_t.room_rect[i] = t_d.room_rect[i];
						}
					for (i = 0; i < 40; i++) {
						ave_t.creatures[i] = t_d.creatures[i];
						}
                    WriteFile(dummy_f, &ave_t, sizeof(ave_tr_type), &dwByteRead, NULL);
				break;
			
			
				case 2:
					for (i = 0; i < 32; i++)
						for (j = 0; j < 32; j++) {
							tiny_t.terrain[i][j] = t_d.terrain[i][j];
							tiny_t.lighting[i / 8][j] = t_d.lighting[i / 8][j];					
							}
					for (i = 0; i < 16; i++) {
						tiny_t.room_rect[i] = t_d.room_rect[i];
						}
					for (i = 0; i < 30; i++) {
						tiny_t.creatures[i] = t_d.creatures[i];
						}
                    WriteFile(dummy_f, &tiny_t, sizeof(tiny_tr_type), &dwByteRead, NULL);
					break;
				}
			for (j = 0; j < 140; j++) {
                WriteFile(dummy_f, &(town_strs[j]), town.strlens[j], &dwByteRead, NULL);
				}

			for (i = 0; i < 200; i++)
				talking.strlens[i] = 0;
			for (i = 0; i < 170; i++)
				talking.strlens[i] = strlen(talk_strs[i]);
            if (WriteFile(dummy_f, &talking, sizeof(talking_record_type), &dwByteRead, NULL) == FALSE){
                CloseHandle(dummy_f);oops_error(9);}
			for (j = 0; j < 170; j++) {
                WriteFile(dummy_f, &(talk_strs[j]), talking.strlens[j], &dwByteRead, NULL);
				}
			
			}

	
	change_made = TRUE;
	// now, everything is moved over. Delete the original, and rename the dummy
	error = CloseHandle(dummy_f);		
	if (error == 0) {CloseHandle(dummy_f);oops_error(10);}

	GlobalUnlock(temp_buffer);
	GlobalFree(temp_buffer);
}

void oops_error(short error)
{
	char error_str[256];
	
		SysBeep(50);
		SysBeep(50);
		SysBeep(50);
	sprintf((char *) error_str,"You may need more memory ... run the editor with no other programs running. Be sure to back your scenario up often. Error number: %d.",error);
	give_error("The program encountered an error while loading/saving/creating the scenario. To prevent future problems, the program will now terminate. Trying again may solve the problem.",(char *) error_str,0);
	PostQuitMessage(0);
}

void start_data_dump()
{
	short i,j,last_town = cur_town;
	char get_text[280];
	HANDLE data_dump_file_id;
    DWORD dwByteRead;
	location out_sec,last_out = cur_out;
	
    data_dump_file_id = CreateFile("Scendata.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (data_dump_file_id == INVALID_HANDLE_VALUE)
	{
		oops_error(28);
		beep();
		return;
	}

	sprintf(get_text,"Scenario data for %s:\n",scen_strs[0]);
	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
	sprintf(get_text,"\nTerrain types for %s:\n",scen_strs[0]);
	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
	for (i = 0; i < 256; i++) {
		sprintf(get_text,"  Terrain type %d: %s\n",i,scen_item_list.ter_names[i]);
    	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
		}
	sprintf(get_text,"\nMonster types for %s:\n",scen_strs[0]);
	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
	for (i = 0; i < 256; i++) {
		sprintf(get_text,"  Monster type %d: %s\n",i,scen_item_list.monst_names[i]);
    	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
		}
	sprintf(get_text,"\nItem types for %s:\n",scen_strs[0]);
	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
	for (i = 0; i < 400; i++) {
		sprintf(get_text,"  Item type %d: %s\n",i,scen_item_list.scen_items[i].full_name);
    	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
    	if(scen_item_list.scen_items[i].special_class > 0){
    		sprintf(get_text,"    Item special class: %d\n",scen_item_list.scen_items[i].special_class);
        	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
            }
    	}
	sprintf(get_text,"\n\nNames of the Outdoor Sections in %s:\n",scen_strs[0]);
	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
		for (out_sec.y = 0; out_sec.y < scenario.out_height ; out_sec.y++)
			for (out_sec.x = 0; out_sec.x < scenario.out_width ; out_sec.x++) {
			load_outdoors(out_sec,0);
			sprintf(get_text,"  Section X = %d, Y = %d:  %s \n",(short) out_sec.x,(short) out_sec.y,data_store->out_strs[0]);
        	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
			}		
	sprintf(get_text,"\nNames of the towns in %s:\n",scen_strs[0]);
	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
			for (j = 0; j < scenario.num_towns; j++) {
		load_town(j);
		sprintf(get_text,"  Town %d: %s\n",j,town_strs[0]);
    	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
		}
		sprintf(get_text,"\n  Outdoor Start Zone: X = %d, Y = %d\n",scenario.out_sec_start.x,scenario.out_sec_start.y);
        WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);		
		sprintf(get_text,"  Outdoor Start Location: X = %d, Y = %d\n",scenario.out_start.x,scenario.out_start.y);
    	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
		sprintf(get_text,"  Starting Town: = %d\n",scenario.which_town_start);
    	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
		sprintf(get_text,"  Starting Town Location: X = %d, Y = %d\n",scenario.where_start.x,scenario.where_start.y);
    	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
		sprintf(get_text,"  Last Outdoor Zone Edited: X = %d, Y = %d\n",scenario.last_out_edited.x,scenario.last_out_edited.y);
    	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
		sprintf(get_text,"  Last Town Edited: = %d\n\n\n",scenario.last_town_edited);
    	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
		sprintf(get_text,"Names of the Special Items in %s:\n",scen_strs[0]);
    	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);

		for (short i = 0; i < 50; i++) {
		sprintf(get_text,"Special Item \"%d \"%s \"%s\n",i,scen_strs[60 + i * 2],scen_strs[61 + i * 2]);
    	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
		}

		sprintf(get_text,"\n\nLocations of Town Entrances in %s:\n",scen_strs[0]);
    	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
		for (out_sec.y = 0; out_sec.y < scenario.out_height ; out_sec.y++)
			for (out_sec.x = 0; out_sec.x < scenario.out_width ; out_sec.x++) {
				load_outdoors(out_sec,0);
			sprintf(get_text,"\n\n  Section X = %d, Y = %d: %s\n",(short) out_sec.x,(short) out_sec.y,data_store->out_strs[0]);
        	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
			for (short i = 0; i < 8; i++) {
					if ((current_terrain.exit_locs[i].x != 100) && (current_terrain.exit_locs[i].y != 0)) {
					sprintf(get_text,"Town entrance %d, town %d, x = %d, y = %d\n",i,current_terrain.exit_dests[i],current_terrain.exit_locs[i].x,current_terrain.exit_locs[i].y);
        	    	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
			}
		}
	}

   	sprintf(get_text,"\n -------- End of file.");
   	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);

	load_outdoors(last_out,0);
	load_town(last_town);

	CloseHandle(data_dump_file_id);

}

void start_monst_data_dump(){

    short i,j,last_town = cur_town;
	char get_text[280];
	HANDLE data_dump_file_id;
    DWORD dwByteRead;
   	location out_sec,last_out = cur_out;

    data_dump_file_id = CreateFile("Monstdata.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (data_dump_file_id == INVALID_HANDLE_VALUE)
	{
		oops_error(28);
		beep();
		return;
	}

	sprintf(get_text,"Monsters data dump for %s :\n\n",scen_strs[0]);
    WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
	
	sprintf(get_text,"Outdoor Monsters :\n");
    WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);

	for (out_sec.x = 0; out_sec.x < scenario.out_width ; (out_sec.x)++)
		for (out_sec.y = 0; out_sec.y < scenario.out_height ; (out_sec.y)++) {
			load_outdoors(out_sec,0);
			sprintf(get_text,"\n Section X = %d, Y = %d:  %s \n",(short) out_sec.x,(short) out_sec.y,data_store->out_strs[0]);
        	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
           	sprintf(get_text,"\n  Outdoor Special Encounters :\n");
            WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
            for(i=0;i<4;i++){
                sprintf(get_text,"\n  Encounter %d :\n",i);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);
                sprintf(get_text,"\n   Hostiles :\n\n    15-30 %s (number %d)\n",scen_item_list.monst_names[current_terrain.special_enc[i].monst[0]],current_terrain.special_enc[i].monst[0]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);                
                sprintf(get_text,"\n    7-10 %s (number %d)\n",scen_item_list.monst_names[current_terrain.special_enc[i].monst[1]],current_terrain.special_enc[i].monst[1]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);      
                sprintf(get_text,"\n    4-6 %s (number %d)\n",scen_item_list.monst_names[current_terrain.special_enc[i].monst[2]],current_terrain.special_enc[i].monst[2]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);                            
                sprintf(get_text,"\n    3-5 %s (number %d)\n",scen_item_list.monst_names[current_terrain.special_enc[i].monst[3]],current_terrain.special_enc[i].monst[3]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);  
                sprintf(get_text,"\n    2-3 %s (number %d)\n",scen_item_list.monst_names[current_terrain.special_enc[i].monst[4]],current_terrain.special_enc[i].monst[4]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);  
                sprintf(get_text,"\n    1-2 %s (number %d)\n",scen_item_list.monst_names[current_terrain.special_enc[i].monst[5]],current_terrain.special_enc[i].monst[5]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);  
                sprintf(get_text,"\n    1 %s (number %d)\n",scen_item_list.monst_names[current_terrain.special_enc[i].monst[6]],current_terrain.special_enc[i].monst[6]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);
                sprintf(get_text,"\n   Friendly :\n\n    7-10 %s (number %d)\n",scen_item_list.monst_names[current_terrain.special_enc[i].friendly[0]],current_terrain.special_enc[i].friendly[0]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);                
                sprintf(get_text,"\n    2-4 %s (number %d)\n",scen_item_list.monst_names[current_terrain.special_enc[i].friendly[1]],current_terrain.special_enc[i].friendly[1]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);      
                sprintf(get_text,"\n    1 %s (number %d)\n",scen_item_list.monst_names[current_terrain.special_enc[i].friendly[2]],current_terrain.special_enc[i].friendly[2]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);                            
            }
            for(i=0;i<4;i++){
                sprintf(get_text,"\n  Wandering %d :\n",i);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);
                sprintf(get_text,"\n   Hostiles :\n\n    15-30 %s (number %d)\n",scen_item_list.monst_names[current_terrain.wandering[i].monst[0]],current_terrain.wandering[i].monst[0]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);                
                sprintf(get_text,"\n    7-10 %s (number %d)\n",scen_item_list.monst_names[current_terrain.wandering[i].monst[1]],current_terrain.wandering[i].monst[1]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);      
                sprintf(get_text,"\n    4-6 %s (number %d)\n",scen_item_list.monst_names[current_terrain.wandering[i].monst[2]],current_terrain.wandering[i].monst[2]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);                            
                sprintf(get_text,"\n    3-5 %s (number %d)\n",scen_item_list.monst_names[current_terrain.wandering[i].monst[3]],current_terrain.wandering[i].monst[3]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);  
                sprintf(get_text,"\n    2-3 %s (number %d)\n",scen_item_list.monst_names[current_terrain.wandering[i].monst[4]],current_terrain.wandering[i].monst[4]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);  
                sprintf(get_text,"\n    1-2 %s (number %d)\n",scen_item_list.monst_names[current_terrain.wandering[i].monst[5]],current_terrain.wandering[i].monst[5]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);  
                sprintf(get_text,"\n    1 %s (number %d)\n",scen_item_list.monst_names[current_terrain.wandering[i].monst[6]],current_terrain.wandering[i].monst[6]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);
                sprintf(get_text,"\n   Friendly :\n\n    7-10 %s (number %d)\n",scen_item_list.monst_names[current_terrain.wandering[i].friendly[0]],current_terrain.wandering[i].friendly[0]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);                
                sprintf(get_text,"\n    2-4 %s (number %d)\n",scen_item_list.monst_names[current_terrain.wandering[i].friendly[1]],current_terrain.wandering[i].friendly[1]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);      
                sprintf(get_text,"\n    1 %s (number %d)\n",scen_item_list.monst_names[current_terrain.wandering[i].friendly[2]],current_terrain.wandering[i].friendly[2]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);                            
            }
        sprintf(get_text,"\n---------------------------------------------------\n");
        WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
    }

    sprintf(get_text,"\n*****************************************************\n");
    WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
	sprintf(get_text,"\nTown Monsters :\n");
    WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
	
	for (j = 0; j < scenario.num_towns; j++) {
		load_town(j);
    	sprintf((char *)get_text,"\n Town %d : %s\n",j,town_strs[0]);
    	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
        
        for(i=0;i<4;i++){
                sprintf(get_text,"\n  Wandering %d :\n",i);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);
                sprintf(get_text,"\n    1 %s (number %d)\n",scen_item_list.monst_names[town.wandering[i].monst[0]],town.wandering[i].monst[0]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);                
                sprintf(get_text,"\n    1 %s (number %d)\n",scen_item_list.monst_names[town.wandering[i].monst[1]],town.wandering[i].monst[1]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);                            
                sprintf(get_text,"\n    1 %s (number %d)\n",scen_item_list.monst_names[town.wandering[i].monst[2]],town.wandering[i].monst[2]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);  
                sprintf(get_text,"\n    1-2 %s (number %d)\n",scen_item_list.monst_names[town.wandering[i].monst[3]],town.wandering[i].monst[3]);
                WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);  
            }
            
        for(i=0;i<60;i++){
            sprintf(get_text,"\n  Monster %d :\n\n",i);
            WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);
            sprintf(get_text,"   Number : %d, Type : %s\n", t_d.creatures[i].number,scen_item_list.monst_names[t_d.creatures[i].number]);
            WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);
            switch(t_d.creatures[i].start_attitude){
                case 0:
                sprintf(get_text,"   Start Attitude : Friendly, Docile\n");
                break;
                case 1:
                sprintf(get_text,"   Start Attitude : Hostile Type A\n");
                break;
                case 2:
                sprintf(get_text,"   Start Attitude : Friendly, Will Fight\n");
                break;
                case 3:
                sprintf(get_text,"   Start Attitude : Hostile Type B\n");
                break;                                
                }
            WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);   
            if(t_d.creatures[i].mobile == TRUE)
            sprintf(get_text,"   Monster can move : Yes\n");
            else
            sprintf(get_text,"   Monster can move : No\n");
            WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);   
            sprintf(get_text,"   Personality : %d\n",t_d.creatures[i].personality);
            WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);
            sprintf(get_text,"   Facial Graphic : %d\n",t_d.creatures[i].facial_pic);
            WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);              
            switch(t_d.creatures[i].time_flag){
                case 0:
                sprintf(get_text,"   Timing : Always Here\n");
                break;
                case 1:
                sprintf(get_text,"   Timing : Appear on day %d\n",t_d.creatures[i].monster_time);
                break;
                case 2:
                sprintf(get_text,"   Timing : Disappear on day %d\n",t_d.creatures[i].monster_time);
                break;
                //case 3 doesn't exist
                case 4:
                sprintf(get_text,"   Timing : Sometimes here A\n");
                break;                               
                case 5:
                sprintf(get_text,"   Timing : Sometimes here B\n");
                break;     
                case 6:
                sprintf(get_text,"   Timing : Sometimes here C\n");
                break;
                case 7:
                sprintf(get_text,"   Timing : Appear when event %d\n",t_d.creatures[i].time_code);
                break;                                                          
                case 8:
                sprintf(get_text,"   Timing : Disappear when event %d\n",t_d.creatures[i].time_code);
                break;                                                                          
            }
            WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);
            sprintf(get_text,"   Event Code : %d\n", t_d.creatures[i].time_code);
            WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);
            sprintf(get_text,"   Part of Special Encounter : %d\n",t_d.creatures[i].spec_enc_code);
            WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);
            sprintf(get_text,"   Special to call on death : %d\n", t_d.creatures[i].special_on_kill);
            WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);            
            sprintf(get_text,"   Life Flag 1 : %d\n",t_d.creatures[i].spec1);
            WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);
            sprintf(get_text,"   Life Flag 2 : %d\n",t_d.creatures[i].spec2);
            WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);
            sprintf(get_text,"   Start Location : x=%d, y=%d\n",t_d.creatures[i].start_loc.x,t_d.creatures[i].start_loc.y);
            WriteFile(data_dump_file_id, get_text, strlen(get_text),&dwByteRead,NULL);            
            }
        sprintf(get_text,"\n---------------------------------------------------\n");
        WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
        }

    	sprintf(get_text,"\n -------- End of file.");
   	    WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);

        load_town(last_town);
       	load_outdoors(last_out,0);
        CloseHandle(data_dump_file_id);
}

void start_spec_data_dump(){

    short i,j,count=0,last_town = cur_town;
	char get_text[280];
	HANDLE data_dump_file_id;
    DWORD dwByteRead;
   	location out_sec,last_out = cur_out;

    data_dump_file_id = CreateFile("Specdata.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (data_dump_file_id == INVALID_HANDLE_VALUE)
	{
		oops_error(28);
		beep();
		return;
	}

	sprintf(get_text,"Specials data dump for %s :\n\n",scen_strs[0]);
    WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);

	sprintf(get_text,"Scenario Specials :\n\n");
    WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
    
    for(i=0;i < 256 ; i++){
        if(scenario.scen_specials[i].type != 0){
    	   sprintf(get_text," Node Number : %d\n\n",i);
           WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
           sprintf(get_text,"  Node Type : ");
           WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
           get_str(get_text,22,scenario.scen_specials[i].type + 1);
           WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
           sprintf(get_text,"\n  Stuff Done Flag Part A : %d,",scenario.scen_specials[i].sd1);
           WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
           sprintf(get_text," Part B : %d\n",scenario.scen_specials[i].sd2);           
           WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
           sprintf(get_text,"  Message Part 1 : %d\n",scenario.scen_specials[i].m1);           
           WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
           sprintf(get_text,"  Message Part 2 : %d\n",scenario.scen_specials[i].m2);           
           WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
           sprintf(get_text,"  Picture : %d\n",scenario.scen_specials[i].pic);           
           WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
           sprintf(get_text,"  Extra 1a : %d\n",scenario.scen_specials[i].ex1a);           
           WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);           
           sprintf(get_text,"  Extra 1b : %d\n",scenario.scen_specials[i].ex1b);           
           WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);           
           sprintf(get_text,"  Extra 2a : %d\n",scenario.scen_specials[i].ex2a);           
           WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);           
           sprintf(get_text,"  Extra 2b : %d\n",scenario.scen_specials[i].ex2b);           
           WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
           sprintf(get_text,"  Special to Jump to : %d\n\n",scenario.scen_specials[i].jumpto);           
           WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);           
            }
    }

    sprintf(get_text,"\n---------------------------------------------------\n");
    WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
	sprintf(get_text,"Scenario Timers :\n");
    WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);

    for(i= 0;i < 20 ;i++){
    if(scenario.scenario_timer_times[i] > 0){
        count++;
        sprintf(get_text," \nTimer Number : %d\n\n",i);
        WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
        sprintf(get_text," Moves between calls : %d\n",scenario.scenario_timer_times[i]);
        WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);        
        sprintf(get_text," Scenario Special to call : %d\n",scenario.scenario_timer_specs[i]);
        WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
        }
    }
    
    if(count == 0){
    sprintf(get_text,"\nNo Scenario Timers.\n");
    WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
    }

    sprintf(get_text,"\n*****************************************************\n");
    WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
	sprintf(get_text,"Outdoor Specials :\n");
    WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);

	for (out_sec.x = 0; out_sec.x < scenario.out_width ; (out_sec.x)++)
		for (out_sec.y = 0; out_sec.y < scenario.out_height ; (out_sec.y)++) {
			load_outdoors(out_sec,0);
			sprintf(get_text,"\n Section X = %d, Y = %d:  %s \n",(short) out_sec.x,(short) out_sec.y,data_store->out_strs[0]);
        	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
        	
        for(i=0;i < 60 ; i++){
            if(current_terrain.specials[i].type != 0){
        	   sprintf(get_text," \n Node Number : %d\n\n",i);
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
               sprintf(get_text,"  Node Type : ");
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
               get_str(get_text,22,current_terrain.specials[i].type + 1);
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
               sprintf(get_text,"\n  Stuff Done Flag Part A : %d,",current_terrain.specials[i].sd1);
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
               sprintf(get_text," Part B : %d\n",current_terrain.specials[i].sd2);           
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
               sprintf(get_text,"  Message Part 1 : %d\n",current_terrain.specials[i].m1);           
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
               sprintf(get_text,"  Message Part 2 : %d\n",current_terrain.specials[i].m2);           
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
               sprintf(get_text,"  Picture : %d\n",current_terrain.specials[i].pic);           
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
               sprintf(get_text,"  Extra 1a : %d\n",current_terrain.specials[i].ex1a);           
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);           
               sprintf(get_text,"  Extra 1b : %d\n",current_terrain.specials[i].ex1b);           
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);           
               sprintf(get_text,"  Extra 2a : %d\n",current_terrain.specials[i].ex2a);           
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);           
               sprintf(get_text,"  Extra 2b : %d\n",current_terrain.specials[i].ex2b);           
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
               sprintf(get_text,"  Special to Jump to : %d\n",current_terrain.specials[i].jumpto);           
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);           
            }
        }
 
        sprintf(get_text,"\n---------------------------------------------------\n");
        WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
    }

    sprintf(get_text,"\n*****************************************************\n");
    WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
	sprintf(get_text,"\nTown Specials :\n");
    WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
	
	for (j = 0; j < scenario.num_towns; j++) {
		load_town(j);
    	sprintf((char *)get_text,"\n Town %d : %s\n",j,town_strs[0]);
    	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
        
        for(i=0;i < 100 ; i++){
            if(town.specials[i].type != 0){
        	   sprintf(get_text," \n Node Number : %d\n\n",i);
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
               sprintf(get_text,"  Node Type : ");
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
               get_str(get_text,22,town.specials[i].type + 1);
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
               sprintf(get_text,"\n  Stuff Done Flag Part A : %d,",town.specials[i].sd1);
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
               sprintf(get_text," Part B : %d\n",town.specials[i].sd2);           
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
               sprintf(get_text,"  Message Part 1 : %d\n",town.specials[i].m1);           
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
               sprintf(get_text,"  Message Part 2 : %d\n",town.specials[i].m2);           
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
               sprintf(get_text,"  Picture : %d\n",town.specials[i].pic);           
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
               sprintf(get_text,"  Extra 1a : %d\n",town.specials[i].ex1a);           
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);           
               sprintf(get_text,"  Extra 1b : %d\n",town.specials[i].ex1b);           
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);           
               sprintf(get_text,"  Extra 2a : %d\n",town.specials[i].ex2a);           
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);           
               sprintf(get_text,"  Extra 2b : %d\n",town.specials[i].ex2b);           
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
               sprintf(get_text,"  Special to Jump to : %d\n",town.specials[i].jumpto);           
               WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);           
            }
        }

        sprintf(get_text,"\n---------------------------------------------------\n");
        WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);        
        sprintf(get_text,"Town Timers :\n");
        WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);

        count = 0;
    
        for(i= 0;i < 8 ;i++){
        if(town.timer_spec_times[i] > 0){
            count++;
            sprintf(get_text," \nTimer Number : %d\n\n",i);
            WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
            sprintf(get_text," Moves between calls : %d\n",town.timer_spec_times[i]);
            WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);        
            sprintf(get_text," Town Special to call : %d\n",town.timer_specs[i]);
            WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
            }
        }

        if(count == 0){
        sprintf(get_text,"\nNo Town Timers.\n");
        WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
        }
        
        sprintf(get_text,"\n---------------------------------------------------\n");
        WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
        }

    	sprintf(get_text,"\n -------- End of file.");
   	    WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);

        load_town(last_town);
       	load_outdoors(last_out,0);
        CloseHandle(data_dump_file_id);
}

void start_shopping_data_dump()
{
	short i,j,last_town = cur_town,count;
	char res[256];
	char get_text[280];
	HANDLE data_dump_file_id;
	DWORD dwByteRead;
	location out_sec,last_out = cur_out;
	
    data_dump_file_id = CreateFile("Scenario Shopping.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (data_dump_file_id == INVALID_HANDLE_VALUE)
	{
		oops_error(28);
		beep();
		return;
	}

	sprintf(get_text,"Scenario shopping data printout for %s:\n",scen_strs[0]);
	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
	sprintf(get_text,"\nList of outdoor stores:\n");
	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
		for (out_sec.x = 0; out_sec.x < scenario.out_width ; (out_sec.x)++)
		for (out_sec.y = 0; out_sec.y < scenario.out_height ; (out_sec.y)++) {
			count = 0;
			load_outdoors(out_sec,0);
			sprintf(get_text,"\n Section X = %d, Y = %d:  %s \n",(short) out_sec.x,(short) out_sec.y,data_store->out_strs[0]);
        	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
			for (i = 0; i < 60; i++) {
				if (current_terrain.specials[i].type == 229) {
                count++;
   				sprintf(get_text,"\n  Number of node : %d\n",i);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);	
				sprintf(get_text,"  Name of Store : %s\n",data_store->out_strs[current_terrain.specials[i].m1+10]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);

				switch(current_terrain.specials[i].ex1b){
                case 0:
				sprintf(get_text,"  Store type  : 0 - Items\n");
                break;

				case 1:
  				sprintf(get_text,"  Store type  : 1 - Mage spells\n");
				break;
				
				case 2:
  				sprintf(get_text,"  Store type  : 2 - Priest spells\n");
				break;

				case 3:
  				sprintf(get_text,"  Store type  : 3 - Alchemy\n");
				break;				

				case 4:
  				sprintf(get_text,"  Store type  : 4 - Healing\n");
				break;
				
                }
                
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				sprintf(get_text,"  Number of first item in store : %d, full name : %s\n",current_terrain.specials[i].ex1a, scen_item_list.scen_items[current_terrain.specials[i].ex1a].full_name);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				sprintf(get_text,"  Number of items in store : %d\n",current_terrain.specials[i].ex2a);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				sprintf(get_text,"  Cost adjust (0 .. 6, lower = cheaper) : %d\n",current_terrain.specials[i].ex2b);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
    			}
			}
            if(count == 0)
			sprintf(get_text,"\n -- No shop found.\n");
			else if (count == 1)
			sprintf(get_text,"\n -- Found 1 shop.\n");
            else
			sprintf(get_text,"\n -- Found %d shops.\n",count);
        	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
	}

	sprintf(get_text,"\nList of town stores:\n");
	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
	
	for (j = 0; j < scenario.num_towns; j++) {
		count = 0;
		load_town(j);
		sprintf((char *)get_text,"\n Town %d: %s\n",j,town_strs[0]);
	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
        for (i = 0; i < 60; i++) {
				if (talking.talk_nodes[i].type == 3 || (talking.talk_nodes[i].type >= 7 && talking.talk_nodes[i].type <= 17) || (talking.talk_nodes[i].type >= 20 && talking.talk_nodes[i].type <= 23)) {
                count++;
   				sprintf(get_text,"\n  Number of talking node : %d\n",i);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
                GetIndString(res,40,1+talking.talk_nodes[i].type *7);
				sprintf(get_text,"  Store type : %s\n",res);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);

                switch(talking.talk_nodes[i].type){

                case 3:
				sprintf(get_text,"  Cost of Inn : %d\n",talking.talk_nodes[i].extras[0]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
                sprintf(get_text,"  Quality of Inn (0 .. 3) : %d\n",talking.talk_nodes[i].extras[1]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				sprintf(get_text,"  Move party to X : %d\n",talking.talk_nodes[i].extras[2]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				sprintf(get_text,"  Move party to Y : %d\n",talking.talk_nodes[i].extras[3]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
                break;

                case 7:
   				sprintf(get_text,"  Name of Store : %s\n",talk_strs[2*i+40]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
                sprintf(get_text,"  Number of first item in store : %d, full name : %s\n",talking.talk_nodes[i].extras[1], scen_item_list.scen_items[talking.talk_nodes[i].extras[1]].full_name);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				sprintf(get_text,"  Number of items in shop : %d\n",talking.talk_nodes[i].extras[2]);
               	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				sprintf(get_text,"  Cost adjust (0 .. 6, lower = cheaper) : %d\n",talking.talk_nodes[i].extras[0]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				break;

                case 9:
                sprintf(get_text,"  Name of Store : %s\n",talk_strs[2*i+40]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				GetIndString(res,38,1+talking.talk_nodes[i].extras[1]);
                sprintf(get_text,"  Number of first spell in shop : %d, name : %s\n",talking.talk_nodes[i].extras[1], res);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				sprintf(get_text,"  Number of spells in shop : %d\n",talking.talk_nodes[i].extras[2]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				sprintf(get_text,"  Cost adjust (0 .. 6, lower = cheaper) : %d\n",talking.talk_nodes[i].extras[0]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				break;
				
				case 10:
                sprintf(get_text,"  Name of Store : %s\n",talk_strs[2*i+40]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				GetIndString(res,38,50+talking.talk_nodes[i].extras[1]);
                sprintf(get_text,"  Number of first spell in shop : %d, name : %s\n",talking.talk_nodes[i].extras[1], res);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				sprintf(get_text,"  Number of spells in shop : %d\n",talking.talk_nodes[i].extras[2]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				sprintf(get_text,"  Cost adjust (0 .. 6, lower = cheaper) : %d\n",talking.talk_nodes[i].extras[0]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				break;
				
				case 11:
                sprintf(get_text,"  Name of Store : %s\n",talk_strs[2*i+40]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				GetIndString(res,38,100+talking.talk_nodes[i].extras[1]);
				sprintf(get_text,"  Cost adjust (0 .. 6, lower = cheaper) : %d\n",talking.talk_nodes[i].extras[0]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
                sprintf(get_text,"  Number of first recipe in shop : %d, name : %s\n",talking.talk_nodes[i].extras[1], res);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				sprintf(get_text,"  Number of recipes in shop : %d\n",talking.talk_nodes[i].extras[2]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				break;
				
				case 12:
                sprintf(get_text,"  Name of healer : %s\n",talk_strs[2*i+40]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
                sprintf(get_text,"  Cost adjust (0 .. 6, lower = cheaper) : %d\n",talking.talk_nodes[i].extras[0]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
                break;
                
                case 16:
                sprintf(get_text,"  Cost to identify : %d\n",talking.talk_nodes[i].extras[0]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
                break;
                
                case 17:
                sprintf(get_text,"  Type of enchantment : %d\n",talking.talk_nodes[i].extras[0]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
                break;
                
                case 20:
				sprintf(get_text,"  Cost of Boat : %d\n",talking.talk_nodes[i].extras[0]);
               	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
                sprintf(get_text,"  Number of first boat sold : %d\n",talking.talk_nodes[i].extras[1]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				sprintf(get_text,"  Total number of boats sold : %d\n",talking.talk_nodes[i].extras[2]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
                break;
            
                case 21:
				sprintf(get_text,"  Cost of Horse : %d\n",talking.talk_nodes[i].extras[0]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
                sprintf(get_text,"  Number of first horse sold : %d\n",talking.talk_nodes[i].extras[1]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				sprintf(get_text,"  Total number of horses sold : %d\n",talking.talk_nodes[i].extras[2]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
                break;

                case 22:
				sprintf(get_text,"  Number of item being sold : %d\n",talking.talk_nodes[i].extras[0]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
                sprintf(get_text,"  Cost of item : %d\n",talking.talk_nodes[i].extras[1]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
                break;                


                case 23:
                sprintf(get_text,"  Name of Store : %s\n",talk_strs[2*i+40]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
				sprintf(get_text,"  Cost adjust (0 .. 6, lower = cheaper) : %d\n",talking.talk_nodes[i].extras[0]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
                sprintf(get_text,"  Which shop (0 .. 4) : %d\n",talking.talk_nodes[i].extras[1]);
            	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
                break;                                            
                        }
                    }
    			}
    			
            if(count == 0)
			sprintf(get_text,"\n -- No shop found.\n");
			else if (count == 1)
			sprintf(get_text,"\n -- Found 1 shop.\n");
            else
			sprintf(get_text,"\n -- Found %d shops.\n",count);
        	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
    	}

			sprintf(get_text,"\n -------- End of file.");
           	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);

		load_outdoors(last_out,0);
		load_town(last_town);

	CloseHandle(data_dump_file_id);

}

void scen_text_dump()
{
	short i,j,last_town = cur_town;
	HANDLE data_dump_file_id;
	DWORD dwByteRead;
	char get_text[300];
	long len;
	location out_sec,last_out = cur_out;
	
	data_dump_file_id = CreateFile("Scentext.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (data_dump_file_id == INVALID_HANDLE_VALUE)
	{
		oops_error(28);
		beep();
		return;
	}

	sprintf((char *)get_text,"\nScenario Text Write Up for %s:\n",scen_strs[0]);
   	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
	sprintf((char *)get_text,"\nScenario Text for %s:\n",scen_strs[0]);
   	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
	for (i = 0; i < 260; i++)
		if (((i < 160) ? scen_strs[i][0] : scen_strs2[i - 160][0]) != '*') {
			if (i < 160)
				sprintf((char *)get_text,"  Message %d: %s\n",i,scen_strs[i]);
				else sprintf((char *)get_text,"  Message %d: %s\n",i,scen_strs2[i - 160]);
               	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
			}

	sprintf((char *)get_text,"\n\nOutdoor Sections Text for %s:",scen_strs[0]);
   	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
		for (out_sec.y = 0; out_sec.y < scenario.out_height ; out_sec.y++)
			for (out_sec.x = 0; out_sec.x < scenario.out_width ; out_sec.x++) {
			sprintf((char *)get_text,"\n\n  Section X = %d, Y = %d: \n",(short) out_sec.x,(short) out_sec.y);
           	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
			load_outdoors(out_sec,0);
			for (i = 0; i < 108; i++)
				if (data_store->out_strs[i][0] != '*') {
					sprintf((char *)get_text,"  Message %d: %s\n",i,data_store->out_strs[i]);
                   	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
					}
		}

	sprintf((char *)get_text,"\n\nTown Text for %s:",scen_strs[0]);
   	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
	for (j = 0; j < scenario.num_towns; j++) {
		load_town(j);
		sprintf((char *)get_text,"\n\n  Town %d: %s\n",j,town_strs[0]);
       	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
		sprintf((char *)get_text,"\n  Town Messages:\n");
       	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
			for (i = 0; i < 135; i++)
				if (town_strs[i][0] != '*') {
					sprintf((char *)get_text,"  Message %d: %s\n",i,town_strs[i]);
            	   	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
					}
		sprintf((char *)get_text,"\n   Dialog for Town %d: %s\n",j,town_strs[0]);
       	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
		for (i = 0; i < 10; i++) {
			sprintf((char *)get_text,"  Personality %d name: %s\n",j * 10 + i,talk_strs[i]);
    	   	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
			sprintf((char *)get_text,"  Personality %d look: %s\n",j * 10 + i,talk_strs[i + 10]);
    	   	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
			sprintf((char *)get_text,"  Personality %d ask name: %s\n",j * 10 + i,talk_strs[i + 20]);
    	   	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
			sprintf((char *)get_text,"  Personality %d ask job: %s\n",j * 10 + i,talk_strs[i + 30]);
    	   	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
			sprintf((char *)get_text,"  Personality %d confused: %s\n",j * 10 + i,talk_strs[i + 160]);
    	   	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
			}
			for (i = 40; i < 160; i++)
				if (strlen((char *) (talk_strs[i])) > 0) {
					sprintf((char *)get_text,"  Node %d: %s\n",(i - 40) / 2,talk_strs[i]);
        	   	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
					}
			sprintf((char *)get_text,"\n");
		   	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);
		}
	//		out_sec.y = scenario.last_out_edited.y;

    sprintf(get_text,"\n -------- End of file.");
   	WriteFile(data_dump_file_id, get_text,strlen(get_text),&dwByteRead,NULL);

	load_outdoors(last_out,0);
	load_town(last_town);

	CloseHandle(data_dump_file_id);

}
void port_talk_nodes()
{
	short i,j,k,l;

	if (cur_scen_is_win == TRUE)
		return;
	for (i = 0; i < 60; i++) {
		flip_short(&talking.talk_nodes[i].personality);
		flip_short(&talking.talk_nodes[i].type);
		flip_short(&talking.talk_nodes[i].extras[0]);
		flip_short(&talking.talk_nodes[i].extras[1]);
		flip_short(&talking.talk_nodes[i].extras[2]);
		flip_short(&talking.talk_nodes[i].extras[3]);
		}
}

void port_town()
{
	short i,j,k,l;

	if (cur_scen_is_win == TRUE)
		return;
	flip_short(&town.town_chop_time);
	flip_short(&town.town_chop_key);
	flip_short(&town.lighting);
	for (i =0 ; i < 4; i++)
		flip_short(&town.exit_specs[i]);
	flip_rect(&town.in_town_rect);
	for (i =0 ; i < 64; i++) {
		flip_short(&town.preset_items[i].item_code);
		flip_short(&town.preset_items[i].ability);
		}
	for (i =0 ; i < 50; i++) {
		flip_short(&town.preset_fields[i].field_type);
		}
	flip_short(&town.max_num_monst);
	flip_short(&town.spec_on_entry);
	flip_short(&town.spec_on_entry_if_dead);
	for (i =0 ; i < 8; i++)
		flip_short(&town.timer_spec_times[i]);
	for (i =0 ; i < 8; i++)
		flip_short(&town.timer_specs[i]);
	flip_short(&town.difficulty);
	for (i =0 ; i < 100; i++)
		flip_spec_node(&town.specials[i]);

}

void port_dummy_town()
{
	short i,j,k,l;

	if (cur_scen_is_win == TRUE)
		return;
	flip_short(&dummy_town_ptr->town_chop_time);
	flip_short(&dummy_town_ptr->town_chop_key);
	flip_short(&dummy_town_ptr->lighting);
	for (i =0 ; i < 4; i++)
		flip_short(&dummy_town_ptr->exit_specs[i]);
	flip_rect(&dummy_town_ptr->in_town_rect);
	for (i =0 ; i < 64; i++) {
		flip_short(&dummy_town_ptr->preset_items[i].item_code);
		flip_short(&dummy_town_ptr->preset_items[i].ability);
		}
	for (i =0 ; i < 50; i++) {
		flip_short(&dummy_town_ptr->preset_fields[i].field_type);
		}
	flip_short(&dummy_town_ptr->max_num_monst);
	flip_short(&dummy_town_ptr->spec_on_entry);
	flip_short(&dummy_town_ptr->spec_on_entry_if_dead);
	for (i =0 ; i < 8; i++)
		flip_short(&dummy_town_ptr->timer_spec_times[i]);
	for (i =0 ; i < 8; i++)
		flip_short(&dummy_town_ptr->timer_specs[i]);
	flip_short(&dummy_town_ptr->difficulty);
	for (i =0 ; i < 100; i++)
		flip_spec_node(&dummy_town_ptr->specials[i]);

}

void port_dummy_t_d(short size,char *buffer)
{
	short i,j,k,l;
	big_tr_type *d1;
	ave_tr_type *d2;
	tiny_tr_type *d3;

	if (cur_scen_is_win == TRUE)
		return;

	switch (size) {
		case 0:
			d1 = (big_tr_type *) buffer;
			for (i =0 ; i < 16; i++)
				flip_rect(&d1->room_rect[i]);
			for (i =0 ; i < 60; i++) {
				flip_short(&d1->creatures[i].spec1);
				flip_short(&d1->creatures[i].spec2);
				flip_short(&d1->creatures[i].monster_time);
				flip_short(&d1->creatures[i].personality);
				flip_short(&d1->creatures[i].special_on_kill);
				flip_short(&d1->creatures[i].facial_pic);
				}
			break;
		case 1:
			d2 = (ave_tr_type *) buffer;
			for (i =0 ; i < 16; i++)
				flip_rect(&d2->room_rect[i]);
			for (i =0 ; i < 40; i++) {
				flip_short(&d2->creatures[i].spec1);
				flip_short(&d2->creatures[i].spec2);
				flip_short(&d2->creatures[i].monster_time);
				flip_short(&d2->creatures[i].personality);
				flip_short(&d2->creatures[i].special_on_kill);
				flip_short(&d2->creatures[i].facial_pic);
				}
			break;
		case 2:
			d3 = (tiny_tr_type *) buffer;
			for (i =0 ; i < 16; i++)
				flip_rect(&d3->room_rect[i]);
			for (i =0 ; i < 30; i++) {
				flip_short(&d3->creatures[i].spec1);
				flip_short(&d3->creatures[i].spec2);
				flip_short(&d3->creatures[i].monster_time);
				flip_short(&d3->creatures[i].personality);
				flip_short(&d3->creatures[i].special_on_kill);
				flip_short(&d3->creatures[i].facial_pic);
				}
			break;
		}
}

void port_dummy_talk_nodes()
{
	short i,j,k,l;

	if (cur_scen_is_win == TRUE)
		return;
	for (i = 0; i < 60; i++) {
		flip_short(&dummy_talk_ptr->talk_nodes[i].personality);
		flip_short(&dummy_talk_ptr->talk_nodes[i].type);
		flip_short(&dummy_talk_ptr->talk_nodes[i].extras[0]);
		flip_short(&dummy_talk_ptr->talk_nodes[i].extras[1]);
		flip_short(&dummy_talk_ptr->talk_nodes[i].extras[2]);
		flip_short(&dummy_talk_ptr->talk_nodes[i].extras[3]);
		}
}

void port_t_d()
{
	short i,j,k,l;
	if (cur_scen_is_win == TRUE)
		return;

	for (i =0 ; i < 16; i++)
		flip_rect(&t_d.room_rect[i]);
	for (i =0 ; i < 60; i++) {
		flip_short(&t_d.creatures[i].spec1);
		flip_short(&t_d.creatures[i].spec2);
		flip_short(&t_d.creatures[i].monster_time);
		flip_short(&t_d.creatures[i].personality);
		flip_short(&t_d.creatures[i].special_on_kill);
		flip_short(&t_d.creatures[i].facial_pic);

		}
}

void port_scenario()
{
	short i,j,k,l;

	if (cur_scen_is_win == TRUE)
		return;

	flip_short(&scenario.flag_a);
	flip_short(&scenario.flag_b);
	flip_short(&scenario.flag_c);
	flip_short(&scenario.flag_d);
	flip_short(&scenario.flag_e);
	flip_short(&scenario.flag_f);
	flip_short(&scenario.flag_g);
	flip_short(&scenario.flag_h);
	flip_short(&scenario.adjust_diff);
	flip_short(&scenario.intro_mess_pic);
	flip_short(&scenario.intro_mess_len);
	flip_short(&scenario.which_town_start);
	for (i = 0; i < 200; i++)
		for (j = 0; j < 5; j++)
			flip_short(&scenario.town_data_size[i][j]);
	for (i = 0; i < 10; i++)
		flip_short(&scenario.town_to_add_to[i]);
	for (i = 0; i < 10; i++)
		for (j = 0; j < 2; j++)
			flip_short(&scenario.flag_to_add_to_town[i][j]);
	for (i = 0; i < 100; i++)
		for (j = 0; j < 2; j++)
			flip_short(&scenario.out_data_size[i][j]);
	for (i = 0; i < 3; i++)
		flip_rect(&scenario.store_item_rects[i]);
	for (i = 0; i < 3; i++)
		flip_short(&scenario.store_item_towns[i]);
	for (i = 0; i < 50; i++)
		flip_short(&scenario.special_items[i]);
	for (i = 0; i < 50; i++)
		flip_short(&scenario.special_item_special[i]);
	flip_short(&scenario.rating);
	flip_short(&scenario.uses_custom_graphics);
	for (i = 0; i < 256; i++) {
		flip_short(&scenario.scen_monsters[i].health);
		flip_short(&scenario.scen_monsters[i].m_health);
		flip_short(&scenario.scen_monsters[i].max_mp);
		flip_short(&scenario.scen_monsters[i].mp);
		flip_short(&scenario.scen_monsters[i].a[1]);
		flip_short(&scenario.scen_monsters[i].a[0]);
		flip_short(&scenario.scen_monsters[i].a[2]);
		flip_short(&scenario.scen_monsters[i].morale);
		flip_short(&scenario.scen_monsters[i].m_morale);
		flip_short(&scenario.scen_monsters[i].corpse_item);
		flip_short(&scenario.scen_monsters[i].corpse_item_chance);
		flip_short(&scenario.scen_monsters[i].picture_num);
		}

	for (i = 0; i < 256; i++) {
		flip_short(&scenario.ter_types[i].picture);
		}
	for (i = 0; i < 30; i++) {
		flip_short(&scenario.scen_boats[i].which_town);
		}
	for (i = 0; i < 30; i++) {
		flip_short(&scenario.scen_horses[i].which_town);
		}
	for (i = 0; i < 20; i++)
		flip_short(&scenario.scenario_timer_times[i]);
	for (i = 0; i < 20; i++)
		flip_short(&scenario.scenario_timer_specs[i]);
	for (i = 0; i < 256; i++) {
		flip_spec_node(&scenario.scen_specials[i]);
		}
		for (i = 0; i < 10; i++)  {
		flip_short(&scenario.storage_shortcuts[i].ter_type);
		flip_short(&scenario.storage_shortcuts[i].property);
		for (j = 0; j < 10; j++)  {
			flip_short(&scenario.storage_shortcuts[i].item_num[j]);
			flip_short(&scenario.storage_shortcuts[i].item_odds[j]);
			}
		}
	flip_short(&scenario.last_town_edited);
}


void port_item_list()
{
	short i,j,k,l;

	if (cur_scen_is_win == TRUE)
		return;

	for (i = 0; i < 400; i++) {
		flip_short(&(scen_item_list.scen_items[i].variety));
		flip_short(&(scen_item_list.scen_items[i].item_level));
		flip_short(&(scen_item_list.scen_items[i].value));
		}
}

void port_out(outdoor_record_type *out)
{
	short i,j,k,l;

	if (cur_scen_is_win == TRUE)
		return;

	for (i = 0; i < 4; i++) {
		flip_short(&(out->wandering[i].spec_on_meet));
		flip_short(&(out->wandering[i].spec_on_win));
		flip_short(&(out->wandering[i].spec_on_flee));
		flip_short(&(out->wandering[i].cant_flee));
		flip_short(&(out->wandering[i].end_spec1));
		flip_short(&(out->wandering[i].end_spec2));
		flip_short(&(out->special_enc[i].spec_on_meet));
		flip_short(&(out->special_enc[i].spec_on_win));
		flip_short(&(out->special_enc[i].spec_on_flee));
		flip_short(&(out->special_enc[i].cant_flee));
		flip_short(&(out->special_enc[i].end_spec1));
		flip_short(&(out->special_enc[i].end_spec2));
		}
	for (i = 0; i < 8; i++)
		flip_rect(&(out->info_rect[i]));
	for (i = 0; i < 60; i++)
		flip_spec_node(&(out->specials[i]));
}

void flip_spec_node(special_node_type *spec)
{
	flip_short(&(spec->type));
	flip_short(&(spec->sd1));
	flip_short(&(spec->sd2));
	flip_short(&(spec->pic));
	flip_short(&(spec->m1));
	flip_short(&(spec->m2));
	flip_short(&(spec->ex1a));
	flip_short(&(spec->ex1b));
	flip_short(&(spec->ex2a));
	flip_short(&(spec->ex2b));
	flip_short(&(spec->jumpto));
}

void flip_short(short *s)
{
	char store,*s1, *s2;

	s1 = (char *) s;
	s2 = s1 + 1;
	store = *s1;
	*s1 = *s2;
	*s2 = store;

}

void alter_rect(RECT *r)
{
	short a;

	a = r->top;
	r->top = r->left;
	r->left = a;
	a = r->bottom;
	r->bottom = r->right;
	r->right = a;
}

void flip_rect(RECT *s)
{
	flip_short((short *) &(s->top));
	flip_short((short *) &(s->bottom));
	flip_short((short *) &(s->left));
	flip_short((short *) &(s->right));
	alter_rect(s);
	}

short FSRead(HFILE file,long *len,char *buffer)
{
	long error = 0;

	if ((error = _lread(file, (char *) buffer, (UINT) (*len)))  == HFILE_ERROR)
		return -1;
	return 0;
}

short SetFPos(HFILE file, short mode, long len)
{
	long error = 0; 
	
	switch (mode) {
		case 1: error = _llseek(file,len,0); break; 
		case 2: error = _llseek(file,len,2); break; 
		case 3: error = _llseek(file,len,1); break; 
		}

	if (error == HFILE_ERROR)
		return -1;
	return 0;
}

void flip_rect(RECT16 *s)
{
	flip_short((short *) &(s->top));
	flip_short((short *) &(s->bottom));
	flip_short((short *) &(s->left));
	flip_short((short *) &(s->right));
	alter_rect(s);
}

void alter_rect(RECT16 *r) 
{
	short a;

	a = r->top;
	r->top = r->left;
	r->left = a;
	a = r->bottom;
	r->bottom = r->right;
	r->right = a;
}

/*
void import_outdoors(location which_out)
{
	short i,j,k,l;
	HFILE file_id;
	Boolean file_ok = FALSE;
	short error;
	long len,len_to_jump = 0,store;
	DWORD buf_len = 100000;
	char *buffer = NULL;
	char szFileName3 [128] = "scen.exs";
	HGLOBAL temp_buffer;
	short out_sec_num;
	outdoor_record_type store_out;
	
	if (overall_mode == 61)
		return;

	file_id = _lopen(szFileName,OF_READ | OF_SHARE_DENY_WRITE);
	if (file_id == HFILE_ERROR) {
		oops_error(31);
		beep();
		return;
		}	
	
	out_sec_num = scenario.out_width * which_out.y + which_out.x;
	
	
	if (out_sec_num >= 0) {
		ofn.hwndOwner = mainPtr;
		ofn.lpstrFile = szFileName3;
		ofn.lpstrFileTitle = szTitleName;
		ofn.Flags = 0;

		if (GetOpenFileName(&ofn) == 0)
			return;
		}
		else {
//			sprintf((char *) szFileName3,".\\scenarios\\BLADBASE.EXS");
			strncpy(szFileName3, szBladBase, 128);
			which_town = 0;
			}
			
	SetCurrentDirectory(file_path_name);

	file_id = _lopen(szFileName3,OF_READ | OF_SHARE_DENY_WRITE);
	if (file_id == HFILE_ERROR) {
		oops_error(42); beep();
		return;
		}
	
	temp_buffer = GlobalAlloc(GMEM_FIXED,buf_len);
	if (temp_buffer == NULL) {
		_lclose(file_id); oops_error(41);
		return;
		}
	buffer = (char *) GlobalLock(temp_buffer);
	if (buffer == NULL) {
		_lclose(file_id); oops_error(41);
		return;
		}

	len = (long) sizeof(scenario_data_type);
	if ((error = FSRead(file_id, &len, buffer)) != 0){
		_lclose(file_id); oops_error(43); return;
		}
	temp_scenario = (scenario_data_type *) buffer;
	
	if ((which.out.x >= temp_scenario->out_width) || (which.out.y >= temp_scenario->out_height)) {
		give_error("Oops ... the selected scenario doesn't have those dimensions. The zone you selected doesn't exist inside this scenario.","",0);
		GlobalUnlock(temp_buffer); GlobalFree(temp_buffer); _lclose(file_id); 
		return;
		}
	
	if ((temp_scenario->flag1 == 20) && (temp_scenario->flag2 == 40)
	 && (temp_scenario->flag3 == 60)
	  && (temp_scenario->flag4 == 80)) {
	  	file_ok = TRUE;
	  	}
	 if (file_ok == FALSE) {
		GlobalUnlock(temp_buffer); GlobalFree(temp_buffer); _lclose(file_id); give_error("This is not a legitimate Blades of Exile scenario. If it is a scenario, note that it needs to have been saved by the Windows scenario editor.","",0); 
		return;	 
	 	}
	len_to_jump = sizeof(scenario_data_type);
	len_to_jump += sizeof(scen_item_data_type);
	for (i = 0; i < 300; i++)
		len_to_jump += (long) scenario.scen_str_len[i];
	store = 0;
	for (i = 0; i < out_sec_num; i++)
		for (j = 0; j < 2; j++)
			store += (long) (scenario.out_data_size[i][j]);
	len_to_jump += store;
	
	error = SetFPos (file_id, 1, len_to_jump);	
	if (error != 0) {_lclose(file_id);oops_error(32);}

	len = sizeof(outdoor_record_type);
	error = FSRead(file_id, &len, (char *) &store_out);
	if (error != 0) {_lclose(file_id);oops_error(33);}

	if (mode == 0) {
		current_terrain = store_out;
		port_out(&current_terrain);
		for (i = 0; i < 120; i++) {
			len = (long) (current_terrain.strlens[i]);
			FSRead(file_id, &len, (char *) &(data_store->out_strs[i]));
			data_store->out_strs[i][len] = 0;
			}

		}
	
	if (mode == 0) 	{
		cur_out = which_out;
		}
		
	if (mode == 1) 
		for (j = 0; j < 48; j++) {				
				borders[0][j] = store_out.terrain[j][47];
				}
	if (mode == 2) 
		for (j = 0; j < 48; j++) {				
				borders[1][j] = store_out.terrain[0][j];
				}
	if (mode == 3) 
		for (j = 0; j < 48; j++) {				
				borders[2][j] = store_out.terrain[j][0];
				}
	if (mode == 4) 
		for (j = 0; j < 48; j++) {				
				borders[3][j] = store_out.terrain[47][j];
				}
				

	error = _lclose(file_id);
	if (error != 0) {_lclose(file_id);oops_error(33);}

}
*/
