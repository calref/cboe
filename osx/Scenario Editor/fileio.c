#include <Carbon/Carbon.h>
#include "Global.h"
#include "stdio.h"
#include "fileio.h"


#define	DONE_BUTTON_ITEM	1
#define IN_FRONT	(WindowPtr)-1L
#define	NIL		0L

extern unsigned char borders[4][48];
extern Point to_create;

DialogPtr	the_dialog;
extern outdoor_record_type	current_terrain;

void print_write_position ();

Point Get_Square_To_Edit()
{

short		the_item;
char		dialog_done = FALSE;
Ptr			dlog_storage = NULL;
Point		store_target;


	store_target = to_create;
	
	the_dialog = GetNewDialog (LOAD_DIALOG_ID, 0, IN_FRONT);
	if (the_dialog == NULL)
		SysBeep(50);
	ShowWindow (the_dialog);

	print_write_position (3, store_target);
	print_write_position (6, store_target);
	
	while (dialog_done == FALSE)
	{
			ModalDialog(NIL, &the_item);
			switch(the_item)
			{
				case DONE_BUTTON_ITEM:
					dialog_done = TRUE;
					break;

				case 4:
					if (store_target.h > 0)
						store_target.h--;
					break;
				case 5:
					if (store_target.h < 8)
						store_target.h++;
					break;
					
				case 7:
					if (store_target.v > 0)
						store_target.v--;
					break;
				case 8:
					if (store_target.v < 9)
						store_target.v++;
					break;
			}

			print_write_position (3, store_target);
			print_write_position (6, store_target);						

		}

	DisposeDialog ( the_dialog );

	return store_target;
}

void print_write_position (which_coord, cur_pos)
int		which_coord;
Point	cur_pos;
{
short		the_type;
Handle		the_handle;
Rect		the_rect;
Str255		message_string;

			
		GetDialogItem( the_dialog, which_coord, &the_type, &the_handle, &the_rect );
		if (which_coord == 6)
			sprintf ((char *)message_string, "  Ypos = %d        ", cur_pos.v);
			else sprintf ((char *)message_string, "  Xpos = %d        ", cur_pos.h);
		SetDialogItemText (the_handle, message_string);
}

void augment_terrain(to_create)
Point to_create;
{
	Point to_load;

	to_load = to_create;
	if (to_create.v > 0) {
		to_load.v--;
		load_terrain(to_load, 1);
		}
	to_load = to_create;
	if (to_create.v < 9) {
		to_load.v++;
		load_terrain(to_load, 3);
		}

	to_load = to_create;
	if (to_create.h < 8) {
		to_load.h++;
		load_terrain(to_load, 2);
		}
	to_load = to_create;
	if (to_create.h > 0) {
		to_load.h--;
		load_terrain(to_load, 4);
		}
}

void load_terrain(to_create, mode)
Point	to_create; 
short 	mode;  // 0 - primary load  1 - add to top  2 - right  3 - bottom  4 - left
{
	short file_id;
	short i,j,k;
	long num_records_to_offset;
	outdoor_record_type store;
	unsigned char to_put;
	Str255 start_name;
	short start_volume;
	long start_dir,len;
	OSErr error;
	
	HGetVol((StringPtr) start_name,&start_volume,&start_dir);
	error = HOpen(start_volume,start_dir,"\pExile III Outdoors",3,&file_id);
	
	if (error != 0) {
		printf("Can't find outdoor data.  \n");
		ExitToShell();
		}
	
	num_records_to_offset = sizeof(outdoor_record_type) * (long) (to_create.v * 9 + to_create.h);

	SetFPos (file_id, 1, num_records_to_offset);
	

	len = sizeof(outdoor_record_type);
	FSRead(file_id, &len, (char *) &store);
	if (mode == 0)
		current_terrain = store;

	if (mode == 1) 
		for (j = 0; j < 48; j++) {				
				borders[0][j] = store.terrain[j][47];
				}
	if (mode == 2) 
		for (j = 0; j < 48; j++) {				
				borders[1][j] = store.terrain[0][j];
				}
	if (mode == 3) 
		for (j = 0; j < 48; j++) {				
				borders[2][j] = store.terrain[j][0];
				}
	if (mode == 4) 
		for (j = 0; j < 48; j++) {				
				borders[3][j] = store.terrain[47][j];
				}
				

	FSClose(file_id);

}

void save_terrain(to_save)
Point to_save;
{
	short file_id;
	long size;
	long num_records_to_offset;
	short i;
	outdoor_record_type *store2;
	Str255 start_name;
	short start_volume;
	long start_dir;
	OSErr error;

    size = sizeof(outdoor_record_type);

	HGetVol((StringPtr) start_name,&start_volume,&start_dir);
	error = HOpen(start_volume,start_dir,"\pExile III Outdoors",3,&file_id);

	num_records_to_offset = sizeof(outdoor_record_type) * (to_save.v * 9 + to_save.h);
//	if ((i = fseek(f, num_records_to_offset, SEEK_CUR)) != 0)
//		SysBeep(50);
	SetFPos (file_id, 1, num_records_to_offset);
			
//	i = fwrite((char *) &current_terrain, size,(size_t) 1, f);
//	fclose (f); 

	FSWrite(file_id, &size, (char *) &current_terrain); 
	FSClose(file_id);
}

void create_outdoor_file()
{
	Str255 start_name;
	short start_volume,file_id;
	long start_dir,size;
	OSErr error;
	short i;
	
    size = sizeof(outdoor_record_type);

	HGetVol((StringPtr) start_name,&start_volume,&start_dir);
	error = HCreate(start_volume,start_dir,"\pExile III Outdoors",'EX3!','OUT3');
	error = HOpen(start_volume,start_dir,"\pExile III Outdoors",3,&file_id);
	for (i = 0; i < 90; i++)
		FSWrite(file_id, &size, (char *) &current_terrain); 
	FSClose(file_id);
}