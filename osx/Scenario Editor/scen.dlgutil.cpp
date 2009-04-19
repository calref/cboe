
#include "scen.global.h"
#include "scen.dlgutil.h"

DialogPtr make_dialog(int resource_num)
{
	DialogPtr the_dialog;

	the_dialog = GetNewDialog (resource_num, 0, IN_FRONT);
	if (the_dialog == NULL) 
		SysBeep(50);
	ShowWindow (GetDialogWindow(the_dialog));

	return the_dialog;
}

void put_num_in_text(short num_to_put,DialogPtr the_dialog, short item_num)
{
	Str255		text_entry;
	short	the_type;
	Handle	the_handle = NULL;
	Rect	the_rect;
	
	NumToString ((long) num_to_put , text_entry);
	GetDialogItem( the_dialog, item_num, &the_type, &the_handle, &the_rect );
	SetDialogItemText ( the_handle, text_entry);	
}

long get_text_item_num(short item_num,DialogPtr the_dialog)
{
	long storage;
	long		number_given = 0;

	short	the_type;
	Handle	the_handle = NULL;
	Rect	the_rect;
	Str255 	the_string;	
	
	GetDialogItem( the_dialog, item_num, &the_type, &the_handle, &the_rect );
	
	GetDialogItemText ( the_handle, the_string);
					
	StringToNum( the_string, &number_given);	
	storage = number_given;

	return storage;
}

