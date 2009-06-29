
#include <Windows.h>
#include <commdlg.h>

#include <string.h>
#include "global.h"
#include "stdio.h"
#include "fileio.h"
#include "text.h"
#include "town.h"
#include "items.h"
#include "graphics.h"
#include "locutils.h"
#include "fields.h"
#include "newgraph.h"
#include "dlgutils.h"
#include "gutils.h"
#include "infodlgs.h"
#include "graphutl.h"
#include "exlsound.h"

#include "globvar.h"

#define	DONE_BUTTON_ITEM	1

extern BOOL fry_startup;

typedef struct {
	char expl[96][96];
	}	out_info_type;

void port_talk_nodes();
void port_town(short mode);
void port_t_d(short mode);
void port_scenario();
void port_party();
void port_pc();
void port_item_list();
void port_stored_items(stored_items_list* list);
void flip_short(short *s);
void flip_long(long *s);
void flip_short(WORD *s);
void flip_rect(RECT *s);
void flip_rect(RECT16* s);
Boolean load_scenario_header(char *filename,short header_entry);
void oops_error(short error);

BOOL check_for_interrupt(){

	if(((GetAsyncKeyState(VK_LCONTROL) & 32768) == 32768) && ((GetAsyncKeyState(0x43) & 32768) == 32768))
        return TRUE;
    
    return FALSE;
    }

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
static char * szFilter[] =
	{	
		"Classic BoE Save Files (*.SAV)\0*.sav\0"
		"Experimental BoE Save Files (*.savx)\0*.savx\0"
		"All Files (*.*)\0*.*\0"
		"\0\0"
	};

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
		ofn.lpstrInitialDir = NULL;
		ofn.lpstrTitle = NULL;
		ofn.Flags = 0;
		ofn.nFileOffset = 0;
		ofn.nFileExtension = 0;
		ofn.lpstrDefExt = "sav";
		ofn.lCustData = 0L;
		ofn.lpfnHook = NULL;
		ofn.lpTemplateName = NULL;
		
		lpsi.cbSize = sizeof(SCROLLINFO);
		lpsi.fMask = SIF_RANGE;
		lpsi.nMin = 0;
		lpsi.nMax = 0;
    	lpsi.nPos = 0;		
}

void load_file()
{
	HANDLE file_id;
	short i,j,k;
    Boolean need_porting = FALSE;
	Boolean town_restore = FALSE;
	Boolean maps_there = FALSE;
	DWORD dwByteRead;
	UINT count;
	char *party_ptr;
	char *pc_ptr;
	short flag;
	Boolean in_scen = FALSE;

	short flags[3][2] = {{5790,1342}, // slot 0 ... 5790 - out  1342 - town
					{100,200}, // slot 1 100  in scenario, 200 not in
					{3422,5567}}; // slot 2 ... 3422 - no maps  5567 - maps
	short mac_flags[3][2] = {{-25066,15877},
	                    {25600,-14336},
	                    {24077,-16619}};

	ofn.hwndOwner = mainPtr;
	ofn.lpstrFile = szFileName;
	ofn.lpstrFileTitle = szTitleName;
	ofn.Flags = 0;

	if (GetOpenFileName(&ofn) == 0)	return;
	
	file_id = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file_id == INVALID_HANDLE_VALUE) return;
	
		SetCurrentDirectory(file_path_name);

	for (i = 0; i < 3; i++)
	{
		if (ReadFile(file_id, &flag, sizeof(short), &dwByteRead, NULL) == FALSE)
		{
			CloseHandle(file_id);
			return;
		}
		
		if( (flag == mac_flags[i][0] ) || (flag == mac_flags[i][1] )){//legacy mac save
            need_porting = TRUE;
            flip_short(&flag);
            }
        if ((flag != flags[i][0]) && (flag != flags[i][1]))//invalid save
		{
			CloseHandle(file_id);
			FCD(1063,0);
			return;
		} 
		


		if ((i == 0) && (flag == flags[i][1])) town_restore = TRUE;
		if ((i == 1) && (flag == flags[i][0])) in_scen = TRUE;
		if ((i == 2) && (flag == flags[i][1])) maps_there = TRUE;
	}

	// LOAD PARTY	
	
	party_ptr = (char *) &party;
	
	if(need_porting){//legacy mac save special loading procedure
    ReadFile(file_id, &party, sizeof(party_record_type)-272, &dwByteRead, NULL);//read until the 2 bytes skipping of the party structure
    SetFilePointer(file_id,-2,NULL,1);//get back of 2 bytes to read the 2 skipped bytes
    ReadFile(file_id, &party.total_m_killed, 4, &dwByteRead, NULL);
    ReadFile(file_id, &party.total_dam_done, 4, &dwByteRead, NULL);    
    ReadFile(file_id, &party.total_xp_gained, 4, &dwByteRead, NULL);        
    ReadFile(file_id, &party.total_dam_taken, 4, &dwByteRead, NULL);        
    ReadFile(file_id, &party.scen_name, 256, &dwByteRead, NULL);        
    }
	else{
	ReadFile(file_id, &party.age, 4, &dwByteRead, NULL);
	ReadFile(file_id, &party.gold, 2, &dwByteRead, NULL);
	ReadFile(file_id, &party.food, 2, &dwByteRead, NULL);
	ReadFile(file_id, party.stuff_done, 310*10, &dwByteRead, NULL);
	ReadFile(file_id, party.item_taken, 200*8, &dwByteRead, NULL);
	ReadFile(file_id, &party.light_level, 2, &dwByteRead, NULL);
	ReadFile(file_id, &party.outdoor_corner.x, 1, &dwByteRead, NULL);
	ReadFile(file_id, &party.outdoor_corner.y, 1, &dwByteRead, NULL);
	ReadFile(file_id, &party.i_w_c.x, 1, &dwByteRead, NULL);
	ReadFile(file_id, &party.i_w_c.y, 1, &dwByteRead, NULL);
	ReadFile(file_id, &party.p_loc.x, 1, &dwByteRead, NULL);
	ReadFile(file_id, &party.p_loc.y, 1, &dwByteRead, NULL);
	ReadFile(file_id, &party.loc_in_sec.x, 1, &dwByteRead, NULL);
	ReadFile(file_id, &party.loc_in_sec.y, 1, &dwByteRead, NULL);
	
	for (i = 0; i < 30; i++)
	{
		ReadFile(file_id, &party.boats[i].boat_loc.x, 1, &dwByteRead, NULL);
		ReadFile(file_id, &party.boats[i].boat_loc.y, 1, &dwByteRead, NULL);
		ReadFile(file_id, &party.boats[i].boat_loc_in_sec.x, 1, &dwByteRead, NULL);
		ReadFile(file_id, &party.boats[i].boat_loc_in_sec.y, 1, &dwByteRead, NULL);
		ReadFile(file_id, &party.boats[i].boat_sector.x, 1, &dwByteRead, NULL);
		ReadFile(file_id, &party.boats[i].boat_sector.y, 1, &dwByteRead, NULL);
		ReadFile(file_id, &party.boats[i].which_town, 2, &dwByteRead, NULL);
		ReadFile(file_id, &party.boats[i].exists, 1, &dwByteRead, NULL);
		ReadFile(file_id, &party.boats[i].property, 1, &dwByteRead, NULL);
	}
	for (i = 0; i < 30; i++)
	{
		ReadFile(file_id, &party.horses[i].horse_loc.x, 1, &dwByteRead, NULL);
		ReadFile(file_id, &party.horses[i].horse_loc.y, 1, &dwByteRead, NULL);
		ReadFile(file_id, &party.horses[i].horse_loc_in_sec.x, 1, &dwByteRead, NULL);
		ReadFile(file_id, &party.horses[i].horse_loc_in_sec.y, 1, &dwByteRead, NULL);
		ReadFile(file_id, &party.horses[i].horse_sector.x, 1, &dwByteRead, NULL);
		ReadFile(file_id, &party.horses[i].horse_sector.y, 1, &dwByteRead, NULL);
		ReadFile(file_id, &party.horses[i].which_town, 2, &dwByteRead, NULL);
		ReadFile(file_id, &party.horses[i].exists, 1, &dwByteRead, NULL);
		ReadFile(file_id, &party.horses[i].property, 1, &dwByteRead, NULL);
	}
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 60; j++)	
		{
			ReadFile(file_id, &party.creature_save[i].dudes[j].active, 2, &dwByteRead, NULL);
			ReadFile(file_id, &party.creature_save[i].dudes[j].attitude, 2, &dwByteRead, NULL);
			ReadFile(file_id, &party.creature_save[i].dudes[j].number, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.creature_save[i].dudes[j].m_loc.x, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.creature_save[i].dudes[j].m_loc.y, 1, &dwByteRead, NULL);
			{
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.m_num, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.level, 1, &dwByteRead, NULL);
				ReadFile(file_id, party.creature_save[i].dudes[j].m_d.m_name, 26, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.health, 2, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.m_health, 2, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.mp, 2, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.max_mp, 2, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.armor, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.skill, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.a, 2*3, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.a1_type, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.a23_type, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.m_type, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.speed, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.ap, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.mu, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.cl, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.breath, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.breath_type, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.treasure, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.spec_skill, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.poison, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.morale, 2, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.m_morale, 2, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.corpse_item, 2, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.corpse_item_chance, 2, &dwByteRead, NULL);
				ReadFile(file_id, party.creature_save[i].dudes[j].m_d.status, 2*15, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.direction, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.immunities, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.x_width, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.y_width, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.radiate_1, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.radiate_2, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.default_attitude, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.summon_type, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.default_facial_pic, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.res1, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.res2, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.res3, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].m_d.picture_num, 2, &dwByteRead, NULL);
			}
			ReadFile(file_id, &party.creature_save[i].dudes[j].mobile, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.creature_save[i].dudes[j].summoned, 2, &dwByteRead, NULL);
			{
				ReadFile(file_id, &party.creature_save[i].dudes[j].monst_start.number, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].monst_start.start_attitude, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].monst_start.start_loc.x, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].monst_start.start_loc.y, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].monst_start.mobile, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].monst_start.time_flag, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].monst_start.extra1, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].monst_start.extra2, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].monst_start.spec1, 2, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].monst_start.spec2, 2, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].monst_start.spec_enc_code, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].monst_start.time_code, 1, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].monst_start.monster_time, 2, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].monst_start.personality, 2, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].monst_start.special_on_kill, 2, &dwByteRead, NULL);
				ReadFile(file_id, &party.creature_save[i].dudes[j].monst_start.facial_pic, 2, &dwByteRead, NULL);
			}
		}
		
		ReadFile(file_id, &party.creature_save[i].which_town, 2, &dwByteRead, NULL);
		ReadFile(file_id, &party.creature_save[i].friendly, 2, &dwByteRead, NULL);
	}
	
	ReadFile(file_id, &party.in_boat, 2, &dwByteRead, NULL);
	ReadFile(file_id, &party.in_horse, 2, &dwByteRead, NULL);
	for (i = 0; i < 10; i++)
	{
		ReadFile(file_id, &party.out_c[i].exists, 1, &dwByteRead, NULL);
		ReadFile(file_id, &party.out_c[i].direction, 2, &dwByteRead, NULL);
		{
			ReadFile(file_id, party.out_c[i].what_monst.monst, 7, &dwByteRead, NULL);
			ReadFile(file_id, party.out_c[i].what_monst.friendly, 3, &dwByteRead, NULL);
			ReadFile(file_id, &party.out_c[i].what_monst.spec_on_meet, 2, &dwByteRead, NULL);
			ReadFile(file_id, &party.out_c[i].what_monst.spec_on_win, 2, &dwByteRead, NULL);
			ReadFile(file_id, &party.out_c[i].what_monst.spec_on_flee, 2, &dwByteRead, NULL);
			ReadFile(file_id, &party.out_c[i].what_monst.cant_flee, 2, &dwByteRead, NULL);
			ReadFile(file_id, &party.out_c[i].what_monst.end_spec1, 2, &dwByteRead, NULL);
			ReadFile(file_id, &party.out_c[i].what_monst.end_spec2, 2, &dwByteRead, NULL);
		}
		ReadFile(file_id, &party.out_c[i].which_sector.x, 1, &dwByteRead, NULL);
		ReadFile(file_id, &party.out_c[i].which_sector.y, 1, &dwByteRead, NULL);
		ReadFile(file_id, &party.out_c[i].m_loc.x, 1, &dwByteRead, NULL);
		ReadFile(file_id, &party.out_c[i].m_loc.y, 1, &dwByteRead, NULL);
	}
	for (i = 0; i < 5; i++)
		for (j = 0; j < 10; j++)
		{
			ReadFile(file_id, &party.magic_store_items[i][j].variety, 2, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].item_level, 2, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].awkward, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].bonus, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].protection, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].charges, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].type, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].magic_use_type, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].graphic_num, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].ability, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].ability_strength, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].type_flag, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].is_special, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].a, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].value, 2, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].weight, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].special_class, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].item_loc.x, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].item_loc.y, 1, &dwByteRead, NULL);
			ReadFile(file_id, party.magic_store_items[i][j].full_name, 25, &dwByteRead, NULL);
			ReadFile(file_id, party.magic_store_items[i][j].name, 15, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].treas_class, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].item_properties, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].reserved1, 1, &dwByteRead, NULL);
			ReadFile(file_id, &party.magic_store_items[i][j].reserved2, 1, &dwByteRead, NULL);
		}
	ReadFile(file_id, &party.imprisoned_monst, 2*4, &dwByteRead, NULL);
	ReadFile(file_id, party.m_seen, 256, &dwByteRead, NULL);
	ReadFile(file_id, party.journal_str, 50, &dwByteRead, NULL);
	ReadFile(file_id, party.journal_day, 2*50, &dwByteRead, NULL);
	ReadFile(file_id, party.special_notes_str, 2*140*2, &dwByteRead, NULL);
	for (i = 0; i < 120; i++)
	{
		ReadFile(file_id, &party.talk_save[i].personality, 2, &dwByteRead, NULL);
		ReadFile(file_id, &party.talk_save[i].town_num, 2, &dwByteRead, NULL);
		ReadFile(file_id, &party.talk_save[i].str1, 2, &dwByteRead, NULL);
		ReadFile(file_id, &party.talk_save[i].str2, 2, &dwByteRead, NULL);
	}
	ReadFile(file_id, &party.direction, 2, &dwByteRead, NULL);
	ReadFile(file_id, &party.at_which_save_slot, 2, &dwByteRead, NULL);
	ReadFile(file_id, party.alchemy, 20, &dwByteRead, NULL);
	ReadFile(file_id, party.can_find_town, 200, &dwByteRead, NULL);
	ReadFile(file_id, party.key_times, 2*100, &dwByteRead, NULL);
	ReadFile(file_id, party.party_event_timers, 2*30, &dwByteRead, NULL);
	ReadFile(file_id, party.global_or_town, 2*30, &dwByteRead, NULL);
	ReadFile(file_id, party.node_to_call, 2*30, &dwByteRead, NULL);
	ReadFile(file_id, party.spec_items, 50, &dwByteRead, NULL);
	ReadFile(file_id, party.help_received, 120, &dwByteRead, NULL);
	ReadFile(file_id, party.m_killed, 2*200, &dwByteRead, NULL);
	ReadFile(file_id, &party.total_m_killed, 4, &dwByteRead, NULL);
	ReadFile(file_id, &party.total_dam_done, 4, &dwByteRead, NULL);
	ReadFile(file_id, &party.total_xp_gained, 4, &dwByteRead, NULL);
	ReadFile(file_id, &party.total_dam_taken, 4, &dwByteRead, NULL);
	ReadFile(file_id, party.scen_name, 256, &dwByteRead, NULL);
}
	for (count = 0; count < sizeof(party_record_type); count++)	party_ptr[count] ^= 0x5C;	

    if(need_porting)//legacy mac save special loading procedure
        port_party();
        
	// LOAD SETUP
	if (ReadFile(file_id, &setup_save, sizeof(setup_save_type), &dwByteRead, NULL) == FALSE)
		{
		CloseHandle(file_id);
		FCD(1064,0);
		return;
		}

	// LOAD PCS
	for (i = 0; i < NUM_OF_PCS; i++)
	{
		pc_ptr = (char *) &adven[i];
		if (ReadFile(file_id, pc_ptr, sizeof(dummy_pc_record_type), &dwByteRead, NULL) == FALSE)
		{
			CloseHandle(file_id);
			FCD(1064,0);
			return;
		}
		
		for (count = 0; count < sizeof(dummy_pc_record_type); count++)
			pc_ptr[count] ^= 0x6B;	
	}
	
    if(need_porting)//legacy mac save special loading procedure
        port_pc();

	if (in_scen == TRUE)
	{
	// LOAD OUTDOOR MAP
	if (ReadFile(file_id, out_e, sizeof(out_info_type), &dwByteRead, NULL) == FALSE)
		{
		CloseHandle(file_id);
		FCD(1064,0);
		return;
		}

	// LOAD TOWN 
	if (town_restore == TRUE)
	{
		if(need_porting){//legacy mac save special loading procedure
        if (ReadFile(file_id, &c_town, sizeof(current_town_type), &dwByteRead, NULL) == FALSE)
		{
			CloseHandle(file_id);
			FCD(1064,0);
			return;
		}
    }
    else{
		
		{
			ReadFile(file_id, &c_town.town_num, 2, &dwByteRead, NULL);
			ReadFile(file_id, &c_town.difficulty, 2, &dwByteRead, NULL);
			
			ReadFile(file_id, &c_town.town, sizeof(town_record_type), &dwByteRead, NULL);
			ReadFile(file_id, c_town.explored, 64 * 64, &dwByteRead, NULL);
			ReadFile(file_id, &c_town.hostile, 1, &dwByteRead, NULL);
			{
				for (j = 0; j < 60; j++)
				{
					ReadFile(file_id, &c_town.monst.dudes[j].active, 2, &dwByteRead, NULL);
					ReadFile(file_id, &c_town.monst.dudes[j].attitude, 2, &dwByteRead, NULL);
					ReadFile(file_id, &c_town.monst.dudes[j].number, 1, &dwByteRead, NULL);
					ReadFile(file_id, &c_town.monst.dudes[j].m_loc.x, 1, &dwByteRead, NULL);
					ReadFile(file_id, &c_town.monst.dudes[j].m_loc.y, 1, &dwByteRead, NULL);
					{
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.m_num, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.level, 1, &dwByteRead, NULL);
						ReadFile(file_id, c_town.monst.dudes[j].m_d.m_name, 26, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.health, 2, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.m_health, 2, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.mp, 2, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.max_mp, 2, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.armor, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.skill, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.a, 2*3, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.a1_type, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.a23_type, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.m_type, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.speed, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.ap, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.mu, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.cl, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.breath, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.breath_type, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.treasure, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.spec_skill, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.poison, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.morale, 2, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.m_morale, 2, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.corpse_item, 2, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.corpse_item_chance, 2, &dwByteRead, NULL);
						ReadFile(file_id, c_town.monst.dudes[j].m_d.status, 2*15, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.direction, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.immunities, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.x_width, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.y_width, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.radiate_1, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.radiate_2, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.default_attitude, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.summon_type, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.default_facial_pic, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.res1, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.res2, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.res3, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].m_d.picture_num, 2, &dwByteRead, NULL);
					}
					ReadFile(file_id, &c_town.monst.dudes[j].mobile, 1, &dwByteRead, NULL);
					ReadFile(file_id, &c_town.monst.dudes[j].summoned, 2, &dwByteRead, NULL);
					{
						ReadFile(file_id, &c_town.monst.dudes[j].monst_start.number, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].monst_start.start_attitude, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].monst_start.start_loc.x, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].monst_start.start_loc.y, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].monst_start.mobile, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].monst_start.time_flag, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].monst_start.extra1, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].monst_start.extra2, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].monst_start.spec1, 2, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].monst_start.spec2, 2, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].monst_start.spec_enc_code, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].monst_start.time_code, 1, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].monst_start.monster_time, 2, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].monst_start.personality, 2, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].monst_start.special_on_kill, 2, &dwByteRead, NULL);
						ReadFile(file_id, &c_town.monst.dudes[j].monst_start.facial_pic, 2, &dwByteRead, NULL);
					}
				}
				ReadFile(file_id, &c_town.monst.which_town, 2, &dwByteRead, NULL);
				ReadFile(file_id, &c_town.monst.friendly, 2, &dwByteRead, NULL);
			}

			ReadFile(file_id, &c_town.in_boat, 1, &dwByteRead, NULL);
			ReadFile(file_id, &c_town.p_loc.x, 1, &dwByteRead, NULL);
			ReadFile(file_id, &c_town.p_loc.y, 1, &dwByteRead, NULL);
		}
    }
			if(need_porting)//legacy mac save special loading procedure
                port_town(1);

		if (ReadFile(file_id, &t_d, sizeof(big_tr_type), &dwByteRead, NULL) == FALSE)
		{
			CloseHandle(file_id);
			FCD(1064,0);
			return;
		}
		    if(need_porting)//legacy mac save special loading procedure
		      port_t_d(1);

		if (ReadFile(file_id, &t_i, sizeof(stored_items_list), &dwByteRead, NULL) == FALSE)
		{
			CloseHandle(file_id);
			FCD(1064,0);
			return;
		}
        if(need_porting)//legacy mac save special loading procedure
            port_stored_items(&t_i);
	}

	// LOAD STORED ITEMS
	for (i = 0; i < 3; i++)
	{
		if (ReadFile(file_id, &stored_items[i], sizeof(stored_items_list), &dwByteRead, NULL) == FALSE)
		{
			CloseHandle(file_id);
			FCD(1064,0);
			return;
		}
    }
        if(need_porting){//legacy mac save special loading procedure
            for(i = 0; i < 3; i++)
                port_stored_items(&stored_items[i]);
	}
	
	// LOAD SAVED MAPS
	if (maps_there == TRUE)
	{
		if (ReadFile(file_id, &(town_maps), sizeof(stored_town_maps_type), &dwByteRead, NULL) == FALSE)
		{
			CloseHandle(file_id);
			FCD(1064,0);
			return;
		}

		if (ReadFile(file_id, &o_maps, sizeof(stored_outdoor_maps_type), &dwByteRead, NULL) == FALSE)
		{
			CloseHandle(file_id);
			FCD(1064,0);
			return;
		}	
	}
	// LOAD SFX & MISC_I
		if (ReadFile(file_id, &sfx, 64 * 64, &dwByteRead, NULL) == FALSE)
		{
			CloseHandle(file_id);
			FCD(1064,0);
			return;
		}	
		
		if (ReadFile(file_id, &misc_i, 64 * 64, &dwByteRead, NULL) == FALSE)
		{
			CloseHandle(file_id);
			FCD(1064,0);
			return;
		}	

	} // end if_scen

	CloseHandle(file_id);

	party_in_memory = TRUE;

	// now if not in scen, this is it.
	if (in_scen == FALSE)
	{
		if (in_startup_mode == FALSE) 
		{
			reload_startup();
			in_startup_mode = TRUE;
			draw_startup(0);
		}
	    in_scen_debug = FALSE;         
		return;
	}
		
	if (load_scenario() == FALSE)  return;
	// if at this point, startup must be over, so make this call to make sure we're ready,
	// graphics wise
	end_startup();

	set_up_ter_pics();
	load_outdoors(party.outdoor_corner.x + 1,party.outdoor_corner.y + 1,1,1,0,0,NULL);
	load_outdoors(party.outdoor_corner.x,party.outdoor_corner.y + 1,0,1,0,0,NULL);
	load_outdoors(party.outdoor_corner.x + 1,party.outdoor_corner.y,1,0,0,0,NULL);
	load_outdoors(party.outdoor_corner.x,party.outdoor_corner.y,0,0,0,0,NULL);

	overall_mode = (town_restore == TRUE) ? 1 : 0;
	stat_screen_mode = 0;
	build_outdoors();
	erase_out_specials();

    belt_present = FALSE;
    
	if (town_restore == FALSE) {
		center = party.p_loc;
		}
		else {	
			load_town(c_town.town_num,2,-1,NULL);
			load_town(c_town.town_num,1,-1,NULL);
			for (i = 0; i < T_M; i++)
				{monster_targs[i].x = 0;  monster_targs[i].y = 0;}

			town_type = scenario.town_size[c_town.town_num];
			// Set up field booleans
			for (j = 0; j < town_size[town_type]; j++)
				for (k = 0; k < town_size[town_type]; k++) {
					// Set up field booleans
					if (is_web(j,k) == TRUE) web = TRUE;
					if (is_crate(j,k) == TRUE) crate = TRUE;
					if (is_barrel(j,k) == TRUE)	barrel = TRUE;
					if (is_fire_barrier(j,k) == TRUE) fire_barrier = TRUE;
					if (is_force_barrier(j,k) == TRUE) force_barrier = TRUE;
					if (is_quickfire(j,k) == TRUE) quickfire = TRUE;
                    if ((scenario.ter_types[t_d.terrain[j][k]].special >= 16) &&
			             (scenario.ter_types[t_d.terrain[j][k]].special <= 19))
				                belt_present = TRUE;
					}		
force_wall = TRUE;fire_wall = TRUE;antimagic = TRUE;scloud = TRUE;ice_wall = TRUE;blade_wall = TRUE;
sleep_field = TRUE;
			center = c_town.p_loc;
			}

	create_clip_region();
	redraw_screen(0);

	current_pc = first_active_pc();
	loaded_yet = TRUE;

	add_string_to_buf("Load: Game loaded.            ");
		
	// Set sounds, map saving, and speed
	/*
	if (((play_sounds == TRUE) && (party.stuff_done[306][1] == 1)) ||
	 ((play_sounds == FALSE) && (party.stuff_done[306][1] == 0))) {
		play_sounds = 1 - play_sounds;
		}
		*/
	give_delays = party.stuff_done[306][2]; 
	if (party.stuff_done[306][0] == 0)
		save_maps = TRUE;
		else save_maps = FALSE;	

	in_startup_mode = FALSE;
	
}

void save_file(short mode) //mode 0 - normal  1 - save as
{
	HANDLE file_id;
	Boolean town_save = FALSE;

	short i, j;

	DWORD count, bytes, dwByteRead;
	short flag;
	short *store;
	party_record_type *party_ptr;
	setup_save_type	*setup_ptr;
	pc_record_type *pc_ptr;
	current_town_type *town_ptr;
		
	char *party_encryptor;	

	if ((in_startup_mode == FALSE) && (is_town()))
		town_save = TRUE;

	ofn.hwndOwner = mainPtr;
	ofn.lpstrFile = szFileName;
	ofn.lpstrFileTitle = szTitleName;
	ofn.Flags = OFN_OVERWRITEPROMPT;
	
	if ((mode == 1) || (in_startup_mode == TRUE)) {
		if (GetSaveFileName(&ofn) == 0)
			return;
		}
		
/*	if (strcmpi(&ofn.lpstrFile[ofn.nFileExtension], "savx") == 0)
	{
		// for experimental formats of save-game files
	}*/

	file_id = CreateFile(szFileName, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file_id == INVALID_HANDLE_VALUE) return;

	SetCurrentDirectory(file_path_name);
		
	store = &flag;	

	flag = (town_save == TRUE) ? 1342 : 5790;
	if (WriteFile(file_id, store, sizeof(short), &bytes, NULL) == FALSE)
	{
		CloseHandle(file_id);
		return;
	}
	flag = (in_startup_mode == FALSE) ? 100 : 200;
	if (WriteFile(file_id, store, sizeof(short), &bytes, NULL) == FALSE)
	{
		CloseHandle(file_id);
		return;
	}
	flag = (save_maps == TRUE) ? 5567 : 3422;
	if (WriteFile(file_id, store, sizeof(short), &bytes, NULL) == FALSE)
	{
		CloseHandle(file_id);
		return;
	}

	// SAVE PARTY
	party_ptr = &party;	

	party_encryptor = (char *) party_ptr;
	for (count = 0; count < sizeof(party_record_type); count++)
		party_encryptor[count] ^= 0x5C;
	
/*	if (WriteFile(file_id, party_ptr, sizeof(party_record_type), &bytes, NULL) == FALSE)
	{
		add_string_to_buf("Save: Couldn't write to file.         ");
		CloseHandle(file_id);
		for (count = 0; count < store_len; count++)
			party_encryptor[count] ^= 0x5C;
			SysBeep(2); 
		return;
		}*/

	WriteFile(file_id, &party.age, 4, &dwByteRead, NULL);
	WriteFile(file_id, &party.gold, 2, &dwByteRead, NULL);
	WriteFile(file_id, &party.food, 2, &dwByteRead, NULL);
	WriteFile(file_id, party.stuff_done, 310*10, &dwByteRead, NULL);
	WriteFile(file_id, party.item_taken, 200*8, &dwByteRead, NULL);
	WriteFile(file_id, &party.light_level, 2, &dwByteRead, NULL);
	WriteFile(file_id, &party.outdoor_corner.x, 1, &dwByteRead, NULL);
	WriteFile(file_id, &party.outdoor_corner.y, 1, &dwByteRead, NULL);
	WriteFile(file_id, &party.i_w_c.x, 1, &dwByteRead, NULL);
	WriteFile(file_id, &party.i_w_c.y, 1, &dwByteRead, NULL);
	WriteFile(file_id, &party.p_loc.x, 1, &dwByteRead, NULL);
	WriteFile(file_id, &party.p_loc.y, 1, &dwByteRead, NULL);
	WriteFile(file_id, &party.loc_in_sec.x, 1, &dwByteRead, NULL);
	WriteFile(file_id, &party.loc_in_sec.y, 1, &dwByteRead, NULL);
	
	for (i = 0; i < 30; i++)
	{
		WriteFile(file_id, &party.boats[i].boat_loc.x, 1, &dwByteRead, NULL);
		WriteFile(file_id, &party.boats[i].boat_loc.y, 1, &dwByteRead, NULL);
		WriteFile(file_id, &party.boats[i].boat_loc_in_sec.x, 1, &dwByteRead, NULL);
		WriteFile(file_id, &party.boats[i].boat_loc_in_sec.y, 1, &dwByteRead, NULL);
		WriteFile(file_id, &party.boats[i].boat_sector.x, 1, &dwByteRead, NULL);
		WriteFile(file_id, &party.boats[i].boat_sector.y, 1, &dwByteRead, NULL);
		WriteFile(file_id, &party.boats[i].which_town, 2, &dwByteRead, NULL);
		WriteFile(file_id, &party.boats[i].exists, 1, &dwByteRead, NULL);
		WriteFile(file_id, &party.boats[i].property, 1, &dwByteRead, NULL);
	}
	for (i = 0; i < 30; i++)
	{
		WriteFile(file_id, &party.horses[i].horse_loc.x, 1, &dwByteRead, NULL);
		WriteFile(file_id, &party.horses[i].horse_loc.y, 1, &dwByteRead, NULL);
		WriteFile(file_id, &party.horses[i].horse_loc_in_sec.x, 1, &dwByteRead, NULL);
		WriteFile(file_id, &party.horses[i].horse_loc_in_sec.y, 1, &dwByteRead, NULL);
		WriteFile(file_id, &party.horses[i].horse_sector.x, 1, &dwByteRead, NULL);
		WriteFile(file_id, &party.horses[i].horse_sector.y, 1, &dwByteRead, NULL);
		WriteFile(file_id, &party.horses[i].which_town, 2, &dwByteRead, NULL);
		WriteFile(file_id, &party.horses[i].exists, 1, &dwByteRead, NULL);
		WriteFile(file_id, &party.horses[i].property, 1, &dwByteRead, NULL);
	}
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 60; j++)	
		{
			WriteFile(file_id, &party.creature_save[i].dudes[j].active, 2, &dwByteRead, NULL);
			WriteFile(file_id, &party.creature_save[i].dudes[j].attitude, 2, &dwByteRead, NULL);
			WriteFile(file_id, &party.creature_save[i].dudes[j].number, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.creature_save[i].dudes[j].m_loc.x, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.creature_save[i].dudes[j].m_loc.y, 1, &dwByteRead, NULL);
			{
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.m_num, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.level, 1, &dwByteRead, NULL);
				WriteFile(file_id, party.creature_save[i].dudes[j].m_d.m_name, 26, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.health, 2, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.m_health, 2, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.mp, 2, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.max_mp, 2, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.armor, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.skill, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.a, 2*3, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.a1_type, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.a23_type, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.m_type, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.speed, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.ap, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.mu, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.cl, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.breath, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.breath_type, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.treasure, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.spec_skill, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.poison, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.morale, 2, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.m_morale, 2, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.corpse_item, 2, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.corpse_item_chance, 2, &dwByteRead, NULL);
				WriteFile(file_id, party.creature_save[i].dudes[j].m_d.status, 2*15, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.direction, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.immunities, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.x_width, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.y_width, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.radiate_1, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.radiate_2, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.default_attitude, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.summon_type, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.default_facial_pic, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.res1, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.res2, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.res3, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].m_d.picture_num, 2, &dwByteRead, NULL);
			}
			WriteFile(file_id, &party.creature_save[i].dudes[j].mobile, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.creature_save[i].dudes[j].summoned, 2, &dwByteRead, NULL);
			{
				WriteFile(file_id, &party.creature_save[i].dudes[j].monst_start.number, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].monst_start.start_attitude, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].monst_start.start_loc.x, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].monst_start.start_loc.y, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].monst_start.mobile, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].monst_start.time_flag, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].monst_start.extra1, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].monst_start.extra2, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].monst_start.spec1, 2, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].monst_start.spec2, 2, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].monst_start.spec_enc_code, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].monst_start.time_code, 1, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].monst_start.monster_time, 2, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].monst_start.personality, 2, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].monst_start.special_on_kill, 2, &dwByteRead, NULL);
				WriteFile(file_id, &party.creature_save[i].dudes[j].monst_start.facial_pic, 2, &dwByteRead, NULL);
			}
		}
		
		WriteFile(file_id, &party.creature_save[i].which_town, 2, &dwByteRead, NULL);
		WriteFile(file_id, &party.creature_save[i].friendly, 2, &dwByteRead, NULL);
	}
	
	WriteFile(file_id, &party.in_boat, 2, &dwByteRead, NULL);
	WriteFile(file_id, &party.in_horse, 2, &dwByteRead, NULL);
	for (i = 0; i < 10; i++)
	{
		WriteFile(file_id, &party.out_c[i].exists, 1, &dwByteRead, NULL);
		WriteFile(file_id, &party.out_c[i].direction, 2, &dwByteRead, NULL);
		{
			WriteFile(file_id, party.out_c[i].what_monst.monst, 7, &dwByteRead, NULL);
			WriteFile(file_id, party.out_c[i].what_monst.friendly, 3, &dwByteRead, NULL);
			WriteFile(file_id, &party.out_c[i].what_monst.spec_on_meet, 2, &dwByteRead, NULL);
			WriteFile(file_id, &party.out_c[i].what_monst.spec_on_win, 2, &dwByteRead, NULL);
			WriteFile(file_id, &party.out_c[i].what_monst.spec_on_flee, 2, &dwByteRead, NULL);
			WriteFile(file_id, &party.out_c[i].what_monst.cant_flee, 2, &dwByteRead, NULL);
			WriteFile(file_id, &party.out_c[i].what_monst.end_spec1, 2, &dwByteRead, NULL);
			WriteFile(file_id, &party.out_c[i].what_monst.end_spec2, 2, &dwByteRead, NULL);
		}
		WriteFile(file_id, &party.out_c[i].which_sector.x, 1, &dwByteRead, NULL);
		WriteFile(file_id, &party.out_c[i].which_sector.y, 1, &dwByteRead, NULL);
		WriteFile(file_id, &party.out_c[i].m_loc.x, 1, &dwByteRead, NULL);
		WriteFile(file_id, &party.out_c[i].m_loc.y, 1, &dwByteRead, NULL);
	}
	for (i = 0; i < 5; i++)
		for (j = 0; j < 10; j++)
		{
			WriteFile(file_id, &party.magic_store_items[i][j].variety, 2, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].item_level, 2, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].awkward, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].bonus, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].protection, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].charges, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].type, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].magic_use_type, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].graphic_num, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].ability, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].ability_strength, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].type_flag, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].is_special, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].a, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].value, 2, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].weight, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].special_class, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].item_loc.x, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].item_loc.y, 1, &dwByteRead, NULL);
			WriteFile(file_id, party.magic_store_items[i][j].full_name, 25, &dwByteRead, NULL);
			WriteFile(file_id, party.magic_store_items[i][j].name, 15, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].treas_class, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].item_properties, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].reserved1, 1, &dwByteRead, NULL);
			WriteFile(file_id, &party.magic_store_items[i][j].reserved2, 1, &dwByteRead, NULL);
		}
	WriteFile(file_id, &party.imprisoned_monst, 2*4, &dwByteRead, NULL);
	WriteFile(file_id, party.m_seen, 256, &dwByteRead, NULL);
	WriteFile(file_id, party.journal_str, 50, &dwByteRead, NULL);
	WriteFile(file_id, party.journal_day, 2*50, &dwByteRead, NULL);
	WriteFile(file_id, party.special_notes_str, 2*140*2, &dwByteRead, NULL);
	for (i = 0; i < 120; i++)
	{
		WriteFile(file_id, &party.talk_save[i].personality, 2, &dwByteRead, NULL);
		WriteFile(file_id, &party.talk_save[i].town_num, 2, &dwByteRead, NULL);
		WriteFile(file_id, &party.talk_save[i].str1, 2, &dwByteRead, NULL);
		WriteFile(file_id, &party.talk_save[i].str2, 2, &dwByteRead, NULL);
	}
	WriteFile(file_id, &party.direction, 2, &dwByteRead, NULL);
	WriteFile(file_id, &party.at_which_save_slot, 2, &dwByteRead, NULL);
	WriteFile(file_id, party.alchemy, 20, &dwByteRead, NULL);
	WriteFile(file_id, party.can_find_town, 200, &dwByteRead, NULL);
	WriteFile(file_id, party.key_times, 2*100, &dwByteRead, NULL);
	WriteFile(file_id, party.party_event_timers, 2*30, &dwByteRead, NULL);
	WriteFile(file_id, party.global_or_town, 2*30, &dwByteRead, NULL);
	WriteFile(file_id, party.node_to_call, 2*30, &dwByteRead, NULL);
	WriteFile(file_id, party.spec_items, 50, &dwByteRead, NULL);
	WriteFile(file_id, party.help_received, 120, &dwByteRead, NULL);
	WriteFile(file_id, party.m_killed, 2*200, &dwByteRead, NULL);
	WriteFile(file_id, &party.total_m_killed, 4, &dwByteRead, NULL);
	WriteFile(file_id, &party.total_dam_done, 4, &dwByteRead, NULL);
	WriteFile(file_id, &party.total_xp_gained, 4, &dwByteRead, NULL);
	WriteFile(file_id, &party.total_dam_taken, 4, &dwByteRead, NULL);
	WriteFile(file_id, party.scen_name, 256, &dwByteRead, NULL);		
	
		for (count = 0; count < sizeof(party_record_type); count++)
			party_encryptor[count] ^= 0x5C;

	// SAVE SETUP
	setup_ptr = &setup_save;	
	if (WriteFile(file_id, setup_ptr, sizeof(setup_save_type), &bytes, NULL) == FALSE)
	{
		CloseHandle(file_id);
		return;
	}

	// SAVE PCS	
	for (i = 0; i < 6; i++) {
		pc_ptr = &adven[i];	

		party_encryptor = (char *) pc_ptr;
		for (count = 0; count < sizeof(pc_record_type); count++)
			party_encryptor[count] ^= 0x6B;
		if (WriteFile(file_id, pc_ptr, sizeof(pc_record_type), &bytes, NULL) == FALSE)
		{
			add_string_to_buf("Save: Couldn't write to file.         ");
			CloseHandle(file_id);
			for (count = 0; count < sizeof(pc_record_type); count++)
				party_encryptor[count] ^= 0x6B;
				MessageBeep(MB_OK); 
			return;
			}
		for (count = 0; count < sizeof(pc_record_type); count++)
			party_encryptor[count] ^= 0x6B;
		}
		
	if (in_startup_mode == FALSE) {
	
	// SAVE OUT DATA
	if (WriteFile(file_id, out_e, sizeof(out_info_type), &bytes, NULL) == FALSE)
	{
		CloseHandle(file_id);
		return;
	}

	if (town_save == TRUE) {	
			town_ptr = &c_town;	
			/*
			len = sizeof(current_town_type);
			if (WriteFile(file_id, town_ptr, sizeof(current_town_type), &bytes, NULL) == FALSE)
			{
				CloseHandle(file_id);
				return;
			}*/
			
/** saving c_town **/

		{
			WriteFile(file_id, &c_town.town_num, 2, &dwByteRead, NULL);
			WriteFile(file_id, &c_town.difficulty, 2, &dwByteRead, NULL);
			WriteFile(file_id, &c_town.town, sizeof(town_record_type), &dwByteRead, NULL);
			WriteFile(file_id, c_town.explored, 64 * 64, &dwByteRead, NULL);
			WriteFile(file_id, &c_town.hostile, 1, &dwByteRead, NULL);
			{
				for (j = 0; j < 60; j++)
				{
					WriteFile(file_id, &c_town.monst.dudes[j].active, 2, &dwByteRead, NULL);
					WriteFile(file_id, &c_town.monst.dudes[j].attitude, 2, &dwByteRead, NULL);
					WriteFile(file_id, &c_town.monst.dudes[j].number, 1, &dwByteRead, NULL);
					WriteFile(file_id, &c_town.monst.dudes[j].m_loc.x, 1, &dwByteRead, NULL);
					WriteFile(file_id, &c_town.monst.dudes[j].m_loc.y, 1, &dwByteRead, NULL);
					{
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.m_num, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.level, 1, &dwByteRead, NULL);
						WriteFile(file_id, c_town.monst.dudes[j].m_d.m_name, 26, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.health, 2, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.m_health, 2, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.mp, 2, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.max_mp, 2, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.armor, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.skill, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.a, 2*3, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.a1_type, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.a23_type, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.m_type, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.speed, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.ap, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.mu, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.cl, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.breath, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.breath_type, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.treasure, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.spec_skill, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.poison, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.morale, 2, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.m_morale, 2, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.corpse_item, 2, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.corpse_item_chance, 2, &dwByteRead, NULL);
						WriteFile(file_id, c_town.monst.dudes[j].m_d.status, 2*15, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.direction, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.immunities, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.x_width, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.y_width, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.radiate_1, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.radiate_2, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.default_attitude, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.summon_type, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.default_facial_pic, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.res1, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.res2, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.res3, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].m_d.picture_num, 2, &dwByteRead, NULL);
					}
					WriteFile(file_id, &c_town.monst.dudes[j].mobile, 1, &dwByteRead, NULL);
					WriteFile(file_id, &c_town.monst.dudes[j].summoned, 2, &dwByteRead, NULL);
					{
						WriteFile(file_id, &c_town.monst.dudes[j].monst_start.number, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].monst_start.start_attitude, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].monst_start.start_loc.x, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].monst_start.start_loc.y, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].monst_start.mobile, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].monst_start.time_flag, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].monst_start.extra1, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].monst_start.extra2, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].monst_start.spec1, 2, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].monst_start.spec2, 2, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].monst_start.spec_enc_code, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].monst_start.time_code, 1, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].monst_start.monster_time, 2, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].monst_start.personality, 2, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].monst_start.special_on_kill, 2, &dwByteRead, NULL);
						WriteFile(file_id, &c_town.monst.dudes[j].monst_start.facial_pic, 2, &dwByteRead, NULL);
					}
				}
				WriteFile(file_id, &c_town.monst.which_town, 2, &dwByteRead, NULL);
				WriteFile(file_id, &c_town.monst.friendly, 2, &dwByteRead, NULL);
			}

			WriteFile(file_id, &c_town.in_boat, 1, &dwByteRead, NULL);
			WriteFile(file_id, &c_town.p_loc.x, 1, &dwByteRead, NULL);
			WriteFile(file_id, &c_town.p_loc.y, 1, &dwByteRead, NULL);
		}
			
/** end of saving c_town **/
			if (WriteFile(file_id, &t_d, sizeof(big_tr_type), &bytes, NULL) == FALSE)
			{
				CloseHandle(file_id);
				return;
			}
			
			if (WriteFile(file_id, &t_i, sizeof(stored_items_list), &bytes, NULL) == FALSE)
			{
				CloseHandle(file_id);
				return;
			}	
		}
	
	// Save stored items 
	for (i = 0; i < 3; i++)
	{
		if (WriteFile(file_id, &stored_items[i], sizeof(stored_items_list), &bytes, NULL) == FALSE)
		{
			CloseHandle(file_id);
			return;
		}
	}
			
	// If saving maps, save maps
	if (save_maps == TRUE) {
		if (WriteFile(file_id, &(town_maps), sizeof(stored_town_maps_type), &bytes, NULL) == FALSE)
		{
			CloseHandle(file_id);
			return;
		}

		if (WriteFile(file_id, &o_maps, sizeof(stored_outdoor_maps_type), &bytes, NULL) == FALSE)
		{
			CloseHandle(file_id);
			return;
		}
	} 
	
	// SAVE SFX and MISC_I
		if (WriteFile(file_id, sfx, (64 * 64), &bytes, NULL) == FALSE)
		{
			CloseHandle(file_id);
			return;
		}
		if (WriteFile(file_id, misc_i, (64 * 64), &bytes, NULL) == FALSE)
		{
			CloseHandle(file_id);
			return;
		}
	}

	CloseHandle(file_id);
		
	if (in_startup_mode == FALSE)
		add_string_to_buf("Save: Game saved.              ");
}

void set_terrain(location l, unsigned char terrain_type)
{
	t_d.terrain[l.x][l.y] = terrain_type;
	combat_terrain[l.x][l.y] = terrain_type;
}

// mode 0 want town and talking, 1 talking only, 2 want a string only, and extra is string num
// Hey's let's be kludgy and overload these value again! If extra is -1, and mode 2, that
// means we want to load all the strings and only the strings
void load_town(short town_num,short mode,short extra,char *str)
{
	HANDLE file_id;
	short i,j;
	long store;
	DWORD len, dwBytesRead;
	long len_to_jump = 0;
	short which_town;
	char file_name[256];

	if (town_num != minmax(0,scenario.num_towns - 1,(int)town_num))
	{
		give_error("The scenario tried to place you into a non-existant town.","",0);
		return;
	}

	which_town = town_num;

	sprintf(file_name,"scenarios/%s",party.scen_name);

	file_id = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file_id == INVALID_HANDLE_VALUE)
	{
		FCD(949,0);
		return;
	}

	len_to_jump = sizeof(scenario_data_type);
	len_to_jump += sizeof(scen_item_data_type);
	
	for (i = 0; i < 300; i++) len_to_jump += (long) scenario.scen_str_len[i];
	store = 0;
	for (i = 0; i < 100; i++)
		for (j = 0; j < 2; j++)
			store += (long) (scenario.out_data_size[i][j]);
	for (i = 0; i < which_town; i++)
		for (j = 0; j < 5; j++)
			store += (long) (scenario.town_data_size[i][j]);
	len_to_jump += store;

	SetFilePointer(file_id, len_to_jump, NULL, FILE_BEGIN);

	// len = 3506;
	if (mode == 0)
	{
		ReadFile(file_id, &c_town.town, sizeof(town_record_type), &dwBytesRead, NULL);
		port_town(0);
	}
	else ReadFile(file_id, &dummy_town, sizeof(town_record_type), &dwBytesRead, NULL);

	switch (scenario.town_size[which_town]) {
		case 0:
			if (mode == 0)
			{
				ReadFile(file_id, &t_d, sizeof(big_tr_type), &dwBytesRead, NULL);
				port_t_d(0);
			}
			else SetFilePointer(file_id, sizeof(big_tr_type), NULL, FILE_CURRENT);
			break;
			
		case 1:
				if (mode == 0) {
					ReadFile(file_id, &ave_t, sizeof(ave_tr_type), &dwBytesRead, NULL);
					for (i = 0; i < 48; i++)
						for (j = 0; j < 48; j++) {
							t_d.terrain[i][j] = ave_t.terrain[i][j];
							t_d.lighting[i / 8][j] = ave_t.lighting[i / 8][j];					
							}
					for (i = 0; i < 16; i++) t_d.room_rect[i] = ave_t.room_rect[i];
					for (i = 0; i < 40; i++) t_d.creatures[i] = ave_t.creatures[i];
					for (i = 40; i < 60; i++) t_d.creatures[i].number = 0;
					port_t_d(0);
					}
					else SetFilePointer(file_id, sizeof(ave_tr_type), NULL, FILE_CURRENT);
			break;
			
		case 2:
			if (mode == 0) {
				ReadFile(file_id, &tiny_t, sizeof(tiny_tr_type), &dwBytesRead, NULL);
				for (i = 0; i < 32; i++)
					for (j = 0; j < 32; j++) {
						t_d.terrain[i][j] = tiny_t.terrain[i][j];
						t_d.lighting[i / 8][j] = tiny_t.lighting[i / 8][j];					
						}
				for (i = 0; i < 16; i++) t_d.room_rect[i] = tiny_t.room_rect[i];
				for (i = 0; i < 30; i++) t_d.creatures[i] = tiny_t.creatures[i];
				for (i = 30; i < 60; i++) t_d.creatures[i].number = 0;
				port_t_d(0);
				}
				else SetFilePointer(file_id, sizeof(tiny_tr_type), NULL, FILE_CURRENT);
			break;
		}
		
	for (i = 0; i < 140; i++) {
		len = (mode == 0) ? (long) (c_town.town.strlens[i]) : (long) (dummy_town.strlens[i]);
		switch (mode) {
			case 0:
				ReadFile(file_id, &(data_store->town_strs[i]), len, &dwBytesRead, NULL);
				data_store->town_strs[i][len] = 0;
				break;
				
			case 1:
				SetFilePointer(file_id, len, NULL, FILE_CURRENT);
				break;
				
			case 2:
				if (extra < 0) {
					ReadFile(file_id, &(data_store->town_strs[i]), len, &dwBytesRead, NULL);
					data_store->town_strs[i][len] = 0;
					}
				else if (i == extra) {
					ReadFile(file_id, str, len, &dwBytesRead, NULL);
					str[len] = 0;
					}
					else SetFilePointer(file_id, len, NULL, FILE_CURRENT);
				break;
			}
		}

	if (mode < 2)
	{
		ReadFile(file_id, &talking, sizeof(talking_record_type), &dwBytesRead, NULL);
		port_talk_nodes();
	
		for (i = 0; i < 170; i++)
		{
			len = (long) (talking.strlens[i]);
			ReadFile(file_id, &(data_store3->talk_strs[i]), len, &dwBytesRead, NULL);
			data_store3->talk_strs[i][len] = 0;
		}
		
		cur_town_talk_loaded = town_num;
	}
		
	if (mode == 0) town_type = scenario.town_size[which_town];
	CloseHandle(file_id);
	
	// Now more initialization is needed. First need to properly create the misc_i array.
	
	// Initialize barriers, etc. Note non-sfx gets forgotten if this is a town recently visited.
	if (mode == 0) {
		for (i = 0; i < 64; i++)
			for (j = 0; j < 64; j++) {
				misc_i[i][j] = 0;
				sfx[i][j] = 0;
				}
		for (i = 0; i < 50; i++)
			if (/* GK (c_town.town.spec_id[i] >= 0) && */(c_town.town.special_locs[i].x < 100)){
				make_special(c_town.town.special_locs[i].x,c_town.town.special_locs[i].y);
				}
	for (i = 0; i < 50; i++) {
			if ((c_town.town.preset_fields[i].field_type > 0) && (c_town.town.preset_fields[i].field_type < 9))
				misc_i[(short) c_town.town.preset_fields[i].field_loc.x][(short) c_town.town.preset_fields[i].field_loc.y] = 
				 misc_i[(short) c_town.town.preset_fields[i].field_loc.x][(short) c_town.town.preset_fields[i].field_loc.y] | 
			 	(unsigned char) (s_pow(2,c_town.town.preset_fields[i].field_type - 1));
			if ((c_town.town.preset_fields[i].field_type >= 14) && (c_town.town.preset_fields[i].field_type <= 21))
				sfx[(short) c_town.town.preset_fields[i].field_loc.x][(short) c_town.town.preset_fields[i].field_loc.y] = 
				 sfx[(short) c_town.town.preset_fields[i].field_loc.x][(short) c_town.town.preset_fields[i].field_loc.y] | 
				 (unsigned char) (s_pow(2,c_town.town.preset_fields[i].field_type - 14));
			}
		}
}

void shift_universe_left()
{
	short i,j;
	
	make_cursor_watch();
	
	save_outdoor_maps();
	party.outdoor_corner.x--;
	party.i_w_c.x++;
	party.p_loc.x += 48;
	outdoors[1][0] = outdoors[0][0];
	outdoors[1][1] = outdoors[0][1];
	data_store4->outdoor_text[1][0] = data_store4->outdoor_text[0][0];
	data_store4->outdoor_text[1][1] = data_store4->outdoor_text[0][1];

	for (i = 48; i < 96; i++)
		for (j = 0; j < 96; j++)
			out_e[i][j] = out_e[i - 48][j];

	for (i = 0; i < 48; i++)
		for (j = 0; j < 96; j++)
			out_e[i][j] = 0;

	for (i = 0; i < 10; i++) {
		if (party.out_c[i].m_loc.x > 48)
			party.out_c[i].exists = FALSE;
		if (party.out_c[i].exists == TRUE) 
			party.out_c[i].m_loc.x += 48;
		}

	load_outdoors(party.outdoor_corner.x,party.outdoor_corner.y,0,0,0,0,NULL);
	load_outdoors(party.outdoor_corner.x,party.outdoor_corner.y + 1,0,1,0,0,NULL);
	build_outdoors();

	SetCursor(sword_curs);
}

void shift_universe_right()
{
	short i,j;
	
	make_cursor_watch();
	save_outdoor_maps();
	party.outdoor_corner.x++;
	party.i_w_c.x--;
	party.p_loc.x -= 48;
	outdoors[0][0] = outdoors[1][0];
	outdoors[0][1] = outdoors[1][1];
	data_store4->outdoor_text[0][0] = data_store4->outdoor_text[1][0];
	data_store4->outdoor_text[0][1] = data_store4->outdoor_text[1][1];
	for (i = 0; i < 48; i++)
		for (j = 0; j < 96; j++)
			out_e[i][j] = out_e[i + 48][j];
	for (i = 48; i < 96; i++)
		for (j = 0; j < 96; j++)
			out_e[i][j] = 0;


	for (i = 0; i < 10; i++) {
		if (party.out_c[i].m_loc.x < 48)
			party.out_c[i].exists = FALSE;
		if (party.out_c[i].exists == TRUE) 
			party.out_c[i].m_loc.x -= 48;
		}
	load_outdoors(party.outdoor_corner.x + 1,party.outdoor_corner.y,1,0,0,0,NULL);
	load_outdoors(party.outdoor_corner.x + 1,party.outdoor_corner.y + 1,1,1,0,0,NULL);
	build_outdoors();

	SetCursor(sword_curs);

}

void shift_universe_up()
{
	short i,j;
	
	make_cursor_watch();
	save_outdoor_maps();
	party.outdoor_corner.y--;
	party.i_w_c.y++;
	party.p_loc.y += 48;
	outdoors[0][1] = outdoors[0][0];
	outdoors[1][1] = outdoors[1][0];

	data_store4->outdoor_text[0][1] = data_store4->outdoor_text[0][0];
	data_store4->outdoor_text[1][1] = data_store4->outdoor_text[1][0];
		for (i = 0; i < 96; i++)
			for (j = 48; j < 96; j++)
				out_e[i][j] = out_e[i][j - 48];
		for (i = 0; i < 96; i++)
			for (j = 0; j < 48; j++)
				out_e[i][j] = 0;	 

	for (i = 0; i < 10; i++) {
		if (party.out_c[i].m_loc.y > 48)
			party.out_c[i].exists = FALSE;
		if (party.out_c[i].exists == TRUE)
			party.out_c[i].m_loc.y += 48; 
		}
	load_outdoors(party.outdoor_corner.x,party.outdoor_corner.y,0,0,0,0,NULL);
	load_outdoors(party.outdoor_corner.x + 1,party.outdoor_corner.y,1,0,0,0,NULL);

	build_outdoors();

	SetCursor(sword_curs);

}

void shift_universe_down()
{
	short i,j;

	make_cursor_watch();
	
	save_outdoor_maps();
	party.outdoor_corner.y++;
	party.i_w_c.y--;
	party.p_loc.y = party.p_loc.y - 48;
	outdoors[0][0] = outdoors[0][1];
	outdoors[1][0] = outdoors[1][1];

	data_store4->outdoor_text[0][0] = data_store4->outdoor_text[0][1];
	data_store4->outdoor_text[1][0] = data_store4->outdoor_text[1][1];
		for (i = 0; i < 96; i++)
			for (j = 0; j < 48; j++)
				out_e[i][j] = out_e[i][j + 48];
		for (i = 0; i < 96; i++)
			for (j = 48; j < 96; j++)
				out_e[i][j] = 0;	

	for (i = 0; i < 10; i++) {
		if (party.out_c[i].m_loc.y < 48)
			party.out_c[i].exists = FALSE;
		if (party.out_c[i].exists == TRUE) 
			party.out_c[i].m_loc.y = party.out_c[i].m_loc.y - 48;
		}
	load_outdoors(party.outdoor_corner.x,party.outdoor_corner.y + 1,0,1,0,0,NULL);
	load_outdoors(party.outdoor_corner.x + 1,party.outdoor_corner.y + 1,1,1,0,0,NULL);
	
	build_outdoors();

	SetCursor(sword_curs);

}
void position_party(short out_x,short out_y,short pc_pos_x,short pc_pos_y)
{
	short i,j;

	save_outdoor_maps();
	party.p_loc.x = pc_pos_x;
	party.p_loc.y = pc_pos_y;
	party.loc_in_sec = party.p_loc.toLocal();
	
	if ((party.outdoor_corner.x != out_x) || (party.outdoor_corner.y != out_y))
	{
		party.outdoor_corner.x = out_x;
		party.outdoor_corner.y = out_y;	
		load_outdoors(party.outdoor_corner.x + 1,party.outdoor_corner.y + 1,1,1,0,0,NULL);
		load_outdoors(party.outdoor_corner.x,party.outdoor_corner.y + 1,0,1,0,0,NULL);
		load_outdoors(party.outdoor_corner.x + 1,party.outdoor_corner.y,1,0,0,0,NULL);
		load_outdoors(party.outdoor_corner.x,party.outdoor_corner.y,0,0,0,0,NULL);
  	}
	party.i_w_c.x = (party.p_loc.x > 47) ? 1 : 0;
	party.i_w_c.y = (party.p_loc.y > 47) ? 1 : 0;
	for (i = 0; i < 10; i++) party.out_c[i].exists = FALSE;
	for (i = 0; i < 96; i++)
		for (j = 0; j < 96; j++) 	
			out_e[i][j] = 0;
	build_outdoors();	
}

void build_outdoors()
{
	short i,j;

	for (i = 0; i < 48; i++)  
		for (j = 0; j < 48; j++)
		{
			out[i][j] = outdoors[0][0].terrain[i][j];
			out[48 + i][j] = outdoors[1][0].terrain[i][j];
			out[i][48 + j] = outdoors[0][1].terrain[i][j];
			out[48 + i][48 + j] = outdoors[1][1].terrain[i][j];	
		}

	fix_boats();
	add_outdoor_maps(); 
	make_out_trim();
	if (in_startup_mode == FALSE) erase_out_specials();

	for (i = 0; i < 10; i++)
		if (party.out_c[i].exists == TRUE)
			if ((party.out_c[i].m_loc.x < 0) || (party.out_c[i].m_loc.y < 0) ||
				(party.out_c[i].m_loc.x > 95) || (party.out_c[i].m_loc.y > 95))
				party.out_c[i].exists = FALSE;
}

short onm(char x_sector,char y_sector)
{
	return y_sector * scenario.out_width + x_sector;
}

// This adds the current outdoor map info to the saved outdoor map info
void save_outdoor_maps()
{
	short i,j;
	
	for (i = 0; i < 48; i++)
		for (j = 0; j < 48; j++) {
			if (out_e[i][j] > 0) 
				o_maps.outdoor_maps[onm(party.outdoor_corner.x,party.outdoor_corner.y)][i / 8][j] =
				 o_maps.outdoor_maps[onm(party.outdoor_corner.x,party.outdoor_corner.y)][i / 8][j] |
				 (char) (s_pow(2,i % 8));	
			if (party.outdoor_corner.x + 1 < scenario.out_width) {
				if (out_e[i + 48][j] > 0) 
					o_maps.outdoor_maps[onm(party.outdoor_corner.x + 1,party.outdoor_corner.y)][i / 8][j] =
					 o_maps.outdoor_maps[onm(party.outdoor_corner.x + 1,party.outdoor_corner.y)][i / 8][j] |
					 (char) (s_pow(2,i % 8));
				}
			if (party.outdoor_corner.y + 1 < scenario.out_height) {
				if (out_e[i][j + 48] > 0) 
					o_maps.outdoor_maps[onm(party.outdoor_corner.x,party.outdoor_corner.y + 1)][i / 8][j] =
					 o_maps.outdoor_maps[onm(party.outdoor_corner.x,party.outdoor_corner.y + 1)][i / 8][j] |
					 (char) (s_pow(2,i % 8));
				}
			if ((party.outdoor_corner.y + 1 < scenario.out_height) &&
				(party.outdoor_corner.x + 1 < scenario.out_width)) {
				if (out_e[i + 48][j + 48] > 0) 
					o_maps.outdoor_maps[onm(party.outdoor_corner.x + 1,party.outdoor_corner.y + 1)][i / 8][j] =
					 o_maps.outdoor_maps[onm(party.outdoor_corner.x + 1,party.outdoor_corner.y + 1)][i / 8][j] |
					 (char) (s_pow(2,i % 8));
				}
			}
}

void add_outdoor_maps()  // This takes the existing outdoor map info and supplements it
						// with the saved map info
{
	short i,j;
	
	for (i = 0; i < 48; i++)
		for (j = 0; j < 48; j++) {
			if ((out_e[i][j] == 0) && 
			 ((o_maps.outdoor_maps[onm(party.outdoor_corner.x,party.outdoor_corner.y)][i / 8][j] &
			  (char) (s_pow(2,i % 8))) != 0))
			 	out_e[i][j] = 1;
			if (party.outdoor_corner.x + 1 < scenario.out_width) {
				if ((out_e[i + 48][j] == 0) && 
				 ((o_maps.outdoor_maps[onm(party.outdoor_corner.x + 1,party.outdoor_corner.y)][i / 8][j] &
				  (char) (s_pow(2,i % 8))) != 0))
				 	out_e[i + 48][j] = 1;		
			 		}	 	
			if (party.outdoor_corner.y + 1 < scenario.out_height) {
				if ((out_e[i][j + 48] == 0) && 
				 ((o_maps.outdoor_maps[onm(party.outdoor_corner.x,party.outdoor_corner.y + 1)][i / 8][j] &
				  (char) (s_pow(2,i % 8))) != 0))
				 	out_e[i][j + 48] = 1;
				}
			if ((party.outdoor_corner.y + 1 < scenario.out_height) &&
				(party.outdoor_corner.x + 1 < scenario.out_width)) {
				if ((out_e[i + 48][j + 48] == 0) && 
				 ((o_maps.outdoor_maps[onm(party.outdoor_corner.x + 1,party.outdoor_corner.y + 1)][i / 8][j] &
				  (char) (s_pow(2,i % 8))) != 0))
				 	out_e[i + 48][j + 48] = 1;
			 	}
			 }
} 

void fix_boats()
{
	short i;

	for (i = 0; i < NUM_OF_BOATS; i++)
		if ((party.boats[i].exists == TRUE) && (party.boats[i].which_town == INVALID_TOWN))
		{
			if (party.boats[i].boat_sector.x == party.outdoor_corner.x)
				party.boats[i].boat_loc.x = party.boats[i].boat_loc_in_sec.x;
			else if (party.boats[i].boat_sector.x == party.outdoor_corner.x + 1)
				party.boats[i].boat_loc.x = party.boats[i].boat_loc_in_sec.x + 48;
				else party.boats[i].boat_loc.x = 500;
				
			if (party.boats[i].boat_sector.y == party.outdoor_corner.y) 
				party.boats[i].boat_loc.y = party.boats[i].boat_loc_in_sec.y;
				else if (party.boats[i].boat_sector.y == party.outdoor_corner.y + 1)
				party.boats[i].boat_loc.y = party.boats[i].boat_loc_in_sec.y + 48;
					else party.boats[i].boat_loc.y = 500;
		}
		
	for (i = 0; i < NUM_OF_HORSES; i++)
		if ((party.horses[i].exists == TRUE) && (party.horses[i].which_town == INVALID_TOWN)) {
			if (party.horses[i].horse_sector.x == party.outdoor_corner.x)
				party.horses[i].horse_loc.x = party.horses[i].horse_loc_in_sec.x;
				else if (party.horses[i].horse_sector.x == party.outdoor_corner.x + 1)
				party.horses[i].horse_loc.x = party.horses[i].horse_loc_in_sec.x + 48;
					else party.horses[i].horse_loc.x = 500;
			if (party.horses[i].horse_sector.y == party.outdoor_corner.y) 
				party.horses[i].horse_loc.y = party.horses[i].horse_loc_in_sec.y;
				else if (party.horses[i].horse_sector.y == party.outdoor_corner.y + 1)
				party.horses[i].horse_loc.y = party.horses[i].horse_loc_in_sec.y + 48;
					else party.horses[i].horse_loc.y = 500;
			}
}


void load_outdoors(short to_create_x, short to_create_y, short targ_x, short targ_y,
	short mode,short extra,char *str)
{
	HANDLE file_id;
	short i,j,out_sec_num;
	char file_name[256];
	DWORD len, dwBytesRead;
	long len_to_jump = 0,store = 0;

	if ((to_create_x != minmax(0,scenario.out_width - 1,(int)to_create_x)) ||
		(to_create_y != minmax(0,scenario.out_height - 1,(int)to_create_y))) { // not exist
			for (i = 0; i < 48; i++)
				for (j = 0; j < 48; j++)
					outdoors[targ_x][targ_y].terrain[i][j] = 5;
			for (i = 0; i < 18; i++) {
				//outdoors[targ_x][targ_y].special_id[i] = -1;
				outdoors[targ_x][targ_y].special_id[i] = 0xFF;
				outdoors[targ_x][targ_y].special_locs[i].x = 100;
				}
			return;
			}

	sprintf(file_name,"scenarios/%s",party.scen_name);

	file_id = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file_id == INVALID_HANDLE_VALUE) return;

	out_sec_num = scenario.out_width * to_create_y + to_create_x;

	len_to_jump = sizeof(scenario_data_type);
	len_to_jump += sizeof(scen_item_data_type);
	for (i = 0; i < 300; i++)
		len_to_jump += (long) scenario.scen_str_len[i];
	store = 0;
	for (i = 0; i < out_sec_num; i++)
		for (j = 0; j < 2; j++)
			store += (long) (scenario.out_data_size[i][j]);
	len_to_jump += store;

	SetFilePointer(file_id, len_to_jump, NULL, FILE_BEGIN);

	if (mode == 0)
	{
		ReadFile(file_id, &outdoors[targ_x][targ_y], sizeof(outdoor_record_type), &dwBytesRead, NULL);
		outdoors[targ_x][targ_y].flip();
	}
	else ReadFile(file_id, &dummy_out, sizeof(outdoor_record_type), &dwBytesRead, NULL);
		
	if (mode == 0) {
		for (i = 0; i < 9; i++) {
			len = (long) (outdoors[targ_x][targ_y].strlens[i]);
			ReadFile(file_id, &(data_store4->outdoor_text[targ_x][targ_y].out_strs[i]), len, &dwBytesRead, NULL);
			data_store4->outdoor_text[targ_x][targ_y].out_strs[i][len] = 0;
			}
		}
		
	if (mode == 1) 
	{
		for (i = 0; i < 120; i++)
		{
			len = (long) (dummy_out.strlens[i]);
			if (i == extra)
			{
				ReadFile(file_id, str, len, &dwBytesRead, NULL);
				str[len] = 0;
			}
			SetFilePointer(file_id, len, NULL, FILE_CURRENT);
		}	
	}
	
	CloseHandle(file_id);
}


void get_reg_data()
{	
	const int BUFFER_LEN = 64;
	char buffer[BUFFER_LEN];
	const char * iniFile = "./blades.ini";
	const char * section = "Blades of Exile";
	GetPrivateProfileString(section, "give_intro_hint", "1", buffer, BUFFER_LEN, iniFile);
	give_intro_hint = (atoi(buffer))? TRUE : FALSE;
	GetPrivateProfileString(section, "play_sounds", "1", buffer, BUFFER_LEN, iniFile);
	play_sounds = (atoi(buffer))? TRUE : FALSE;
	GetPrivateProfileString(section, "game_run_before", "0", buffer, BUFFER_LEN, iniFile);
	game_run_before = (atoi(buffer))? TRUE : FALSE;
	GetPrivateProfileString(section, "display_mode", "0", buffer, BUFFER_LEN, iniFile);
	display_mode = atoi(buffer);
	GetPrivateProfileString(section, "no_instant_help", "0", buffer, BUFFER_LEN, iniFile);
	party.stuff_done[SDF_NO_INSTANT_HELP] = atoi(buffer);
	GetPrivateProfileString(section, "fancy_startup", "1", buffer, BUFFER_LEN, iniFile);
	fry_startup = atoi(buffer);
	GetPrivateProfileString(section, "darker_graphics", "0", buffer, BUFFER_LEN, iniFile);
	party.stuff_done[307][0] = atoi(buffer); //1 is darker
	GetPrivateProfileString(section, "legacy_day_reached", "0", buffer, BUFFER_LEN, iniFile);
	party.stuff_done[309][0] = atoi(buffer); //1 is legacy
	GetPrivateProfileString(section, "legacy_kill_node", "1", buffer, BUFFER_LEN, iniFile);
	party.stuff_done[309][1] = (atoi(buffer))? FALSE : TRUE; //0 is legacy to preserve backwards compatibility, since SDF are initialized at 0.
	GetPrivateProfileString(section, "town_waterfalls", "1", buffer, BUFFER_LEN, iniFile);
	party.stuff_done[309][2] = (atoi(buffer))? FALSE : TRUE; //0 is legacy to preserve backwards compatibility, since SDF are initialized at 0.
	GetPrivateProfileString(section, "display_grass_trims", "0", buffer, BUFFER_LEN, iniFile);
	party.stuff_done[309][3] = atoi(buffer); //0 is display

}

void build_data_file(short which) // 1 - compatibility ; 2 - preferences
{
	char tmp[] = "?";
	const char * iniFile = "./blades.ini";
	const char * section = "Blades of Exile";
	if(which == 2){
	WritePrivateProfileString(section, "give_intro_hint", (give_intro_hint)? "1" : "0", iniFile);
	WritePrivateProfileString(section, "play_sounds", (play_sounds)? "1" : "0", iniFile);
	WritePrivateProfileString(section, "game_run_before", (game_run_before)? "1" : "0", iniFile);
	WritePrivateProfileString(section, "display_mode", itoa((int) display_mode, tmp, 10), iniFile);
	WritePrivateProfileString(section, "no_instant_help", itoa((int) party.stuff_done[SDF_NO_INSTANT_HELP], tmp, 10), iniFile);
	WritePrivateProfileString(section, "fancy_startup", (fry_startup == TRUE) ? "1" : "0", iniFile);
	WritePrivateProfileString(section, "darker_graphics", (party.stuff_done[307][0] == 0) ? "0": "1", iniFile);
    }
    if(which == 1){
    WritePrivateProfileString(section, "legacy_day_reached", (party.stuff_done[309][0] == 0)? "0" : "1", iniFile);
    WritePrivateProfileString(section, "legacy_kill_node", (party.stuff_done[309][1] == 0)? "1" : "0", iniFile);
    WritePrivateProfileString(section, "town_waterfalls", (party.stuff_done[309][2] == 0)? "1" : "0", iniFile);
    WritePrivateProfileString(section, "display_grass_trims", (party.stuff_done[309][3] == 0)? "0" : "1", iniFile);
    }
}

// expecting party record to contain name of proper scenario to load
Boolean load_scenario()	// OK
{
	short i;
	HANDLE file_id;
	Boolean file_ok = FALSE;
	DWORD len, dwByteRead;
	char file_name[256];
	
	sprintf(file_name,"scenarios/%s",party.scen_name);
	
	file_id = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file_id == INVALID_HANDLE_VALUE) return FALSE;
	
	if (ReadFile(file_id, &scenario, sizeof(scenario_data_type), &dwByteRead, NULL) == FALSE)
	{
		CloseHandle(file_id);
		return FALSE;
	}
	
	if ((scenario.flag1 == 10) && (scenario.flag2 == 20) && (scenario.flag3 == 30)
	  && (scenario.flag4 == 40))
	  {
	  	file_ok = TRUE;
		cur_scen_is_win = FALSE;
		port_scenario();
		}
	if ((scenario.flag1 == 20) && (scenario.flag2 == 40) && (scenario.flag3 == 60)
	  && (scenario.flag4 == 80))
	  {
		file_ok = TRUE;
		cur_scen_is_win = TRUE;
		}
		
	 if (file_ok == FALSE)
	 {
		CloseHandle(file_id);
		give_error("This is not a legitimate Blades of Exile scenario.","",0);
		return FALSE;	 
	}
	 	
	// item data	
	if (ReadFile(file_id, scen_item_list, sizeof(scen_item_data_type), &dwByteRead, NULL) == FALSE)
	{
		CloseHandle(file_id);
		return FALSE;
	}
	
	port_item_list();
	
	for (i = 0; i < 270; i++) {
		len = (DWORD) (scenario.scen_str_len[i]);
		if (i < 160)
		{
			ReadFile(file_id, &(data_store5->scen_strs[i]), len, &dwByteRead, NULL);
			data_store5->scen_strs[i][len] = 0;
		}
		else {
			ReadFile(file_id, &(scen_strs2[i - 160]), len, &dwByteRead, NULL);
			scen_strs2[i - 160][len] = 0;
			}
		}
		
	CloseHandle(file_id);
	
	if (spec_scen_g != NULL)
	{
		DeleteObject(spec_scen_g);
		spec_scen_g = NULL;
	}
	
	sprintf(file_name,"scenarios/%s",party.scen_name);
	for (i = 0; i < 256; i++) {
		if (file_name[i] == '.') {
			file_name[i + 1] = 'b';
			file_name[i + 2] = 'm';
			file_name[i + 3] = 'p';
			i = 256;
			}
		}
		
	spec_scen_g = ReadBMP(file_name);

	set_up_ter_pics();
	return TRUE;
}

void set_up_ter_pics()
{
	for (int i = 0; i < 256; i++)
		terrain_blocked[i] = scenario.ter_types[i].blockage;
	for (int i = 0; i < 256; i++)
		terrain_pic[i] = scenario.ter_types[i].picture;
}

void oops_error(short error)
{
	char error_str[256];
	
	MessageBeep(MB_OK);
	sprintf((char *) error_str,"Giving the scenario editor more memory might also help. Be sure to back your scenario up often. Error number: %d.",error);
	give_error("The program encountered an error while loading/saving/creating the scenario. To prevent future problems, the program will now terminate. Trying again may solve the problem.",(char *) error_str,0);
}

void ListFiles(char *path, HWND listbox){

    short len;
	HANDLE find_file_id;
    WIN32_FIND_DATA lpFindFileData;
    char copy_str[256];
    
    sprintf(copy_str,"scenarios/%s*.*",path);
    
    if((find_file_id = FindFirstFile(copy_str,&lpFindFileData)) == INVALID_HANDLE_VALUE){// is directory empty ?
        FindClose(find_file_id);
        return;
        }
		
	do{
    len = strlen(lpFindFileData.cFileName);
    if(((lpFindFileData.cFileName[len-1] == 's') ||( lpFindFileData.cFileName[len-1] == 'S')) 
    && ((lpFindFileData.cFileName[len-2] == 'x') || (lpFindFileData.cFileName[len-2] == 'X'))
    && ((lpFindFileData.cFileName[len-3] == 'e') || (lpFindFileData.cFileName[len-3] == 'E'))){// is extension .exs ?
        sprintf(copy_str,"%s%s",path,lpFindFileData.cFileName);
        SendMessage(listbox,LB_ADDSTRING,0,(LPARAM) (LPCTSTR) copy_str);
        }
    else if((lpFindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) && (strcmp(lpFindFileData.cFileName,".") != 0) && (strcmp(lpFindFileData.cFileName,"..") != 0)){
        sprintf(copy_str,"%s%s/",path,lpFindFileData.cFileName);
        ListFiles(copy_str,listbox);//so list scenarios under the subdirectory
        }
    }while(FindNextFile(find_file_id,&lpFindFileData) != 0);

    FindClose(find_file_id);
    
}

void build_scen_headers()
{
	short i,current_entry = 0;
	HWND listbox;
	WORD count;
	char filename[256],filename2[256];
	
	listbox = CreateWindow("listbox", NULL,	WS_CHILDWINDOW,	// 3
		 0,0,0,0,	// 7
		mainPtr, // 8
		(HMENU) 1, // 9
		(HINSTANCE) GetWindowLong(mainPtr, GWL_HINSTANCE), // 10
		NULL); // 11
	
    ListFiles("", listbox);//First list the scenarios under the scenarios folder
		
//	SendMessage(listbox,LB_DIR,0x0,(LPARAM) (LPCTSTR) "scenarios/*.exs");

	count = (WORD) SendMessage(listbox,LB_GETCOUNT,0,0L);

	if(count == 0){
    store_num_scen = 0;
    return;    
    }
// memory allocation (if there's an invalid file, the memory is allocated too, but unused)
    if(data_store2 != NULL){
   		delete[] data_store2;
        data_store2 = NULL;
        }

    data_store2 = new piles_of_stuff_dumping_type2[count];	

    if(scen_headers != NULL){
   		delete[] scen_headers;
        scen_headers = NULL;
    }
	
	scen_headers = new scen_header_type[count];
// end of memory allocation	

	for (i = 0; i < count; i++)
	{
		SendMessage(listbox,LB_GETTEXT,i,(LONG) (LPSTR) filename2);

		sprintf(filename,"scenarios/%s",filename2);

		if (load_scenario_header(filename,current_entry) == TRUE){
        	// now we need to store the file name, first stripping any path that occurs
    	   	// before it
	       	strcpy(data_store2[current_entry].scen_names, filename2);	
    		current_entry++;
            }
	}

    store_num_scen = current_entry; //number of valid scenarios
    
	/*for (i = 0; i < count ; i++)
		if (scen_headers[i].flag1 != 0)
			store_num_scen++;*/

    DestroyWindow(listbox);

}

// This is only called at startup, when bringing headers of active scenarios.
// This wipes out the scenario record, so be sure not to call it while in an active scenario.
Boolean load_scenario_header(char *filename,short header_entry)
{
	short i;
	HANDLE file_id;
	WORD store;
	Boolean file_ok = FALSE;
	DWORD len, dwByteRead;
	char load_str[256];
	Boolean mac_header = TRUE;
	
	file_id = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file_id == INVALID_HANDLE_VALUE) return FALSE;
	
	if (ReadFile(file_id, &(scen_headers[header_entry]), sizeof(scen_header_type), &dwByteRead, NULL) == FALSE)
	{
		CloseHandle(file_id);
		return FALSE;
	}
	
	if ((scen_headers[header_entry].flag1 == 10) && (scen_headers[header_entry].flag2 == 20)
	 && (scen_headers[header_entry].flag3 == 30)
	  && (scen_headers[header_entry].flag4 == 40))
	{
	  	file_ok = TRUE;
	  	mac_header = TRUE;
	}
	if ((scen_headers[header_entry].flag1 == 20) && (scen_headers[header_entry].flag2 == 40)
	 && (scen_headers[header_entry].flag3 == 60)
	  && (scen_headers[header_entry].flag4 == 80))
	{
	  	file_ok = TRUE;
	  	mac_header = FALSE;
	}
	
	if (file_ok == FALSE)
	{
		scen_headers[header_entry].flag1 = 0;
		CloseHandle(file_id);
		return FALSE;
	}

	// So file is OK, so load in string data and close it.
	SetFilePointer(file_id, 0, NULL, FILE_BEGIN);
	if (ReadFile(file_id, &scenario, sizeof(scenario_data_type), &dwByteRead, NULL) == FALSE)
	{
		CloseHandle(file_id);
		oops_error(29);
		return FALSE;
	}
	store = scenario.rating;
	if (mac_header == TRUE)	flip_short(&store);
	scen_headers[header_entry].default_ground = store;
	
	SetFilePointer(file_id, sizeof(scen_item_data_type), NULL, FILE_CURRENT);
		
	for (i = 0; i < 3; i++)
	{
		store = (WORD) scenario.scen_str_len[i];
		len = (DWORD) (store);
		ReadFile(file_id, load_str, len, &dwByteRead, NULL);
		load_str[len] = 0;
		if (i == 0) load_str[29] = 0;
		else load_str[59] = 0;
		
		strcpy(data_store2[header_entry].scen_header_strs[i],load_str);
	}
	
	CloseHandle(file_id);
	return TRUE;
}

void port_talk_nodes()
{
	short i;

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

void port_town(short mode)//0 scenario port, 1 save port <= check already done in load_file
{
	short i,j;

	if (cur_scen_is_win == TRUE && mode == 0)
		return;
	flip_short(&c_town.town.town_chop_time);
	flip_short(&c_town.town.town_chop_key);
	flip_short(&c_town.town.lighting);
	for (i =0 ; i < 4; i++)
		flip_short(&c_town.town.exit_specs[i]);
	flip_rect(&c_town.town.in_town_rect);
	for (i =0 ; i < 64; i++) {
		flip_short(&c_town.town.preset_items[i].item_code);
		flip_short(&c_town.town.preset_items[i].ability);
		}
	for (i =0 ; i < 50; i++) {
		flip_short(&c_town.town.preset_fields[i].field_type);
		}
	flip_short(&c_town.town.max_num_monst);
	flip_short(&c_town.town.spec_on_entry);
	flip_short(&c_town.town.spec_on_entry_if_dead);
	for (i =0 ; i < 8; i++) 
		flip_short(&c_town.town.timer_spec_times[i]);
	for (i =0 ; i < 8; i++) 
		flip_short(&c_town.town.timer_specs[i]);
	flip_short(&c_town.town.difficulty);
	for (i =0 ; i < 100; i++) 
		c_town.town.specials[i].flip();

	if(mode == 1){
    flip_short(&c_town.monst.friendly);
    flip_short(&c_town.monst.which_town);
    
    for(i=0;i<60;i++){
        flip_short(&c_town.monst.dudes[i].active);
        flip_short(&c_town.monst.dudes[i].attitude);        
        flip_short(&c_town.monst.dudes[i].summoned);
        
        flip_short(&c_town.monst.dudes[i].monst_start.spec1);
		flip_short(&c_town.monst.dudes[i].monst_start.spec2);
		flip_short(&c_town.monst.dudes[i].monst_start.monster_time);
		flip_short(&c_town.monst.dudes[i].monst_start.personality);
		flip_short(&c_town.monst.dudes[i].monst_start.special_on_kill);
		flip_short(&c_town.monst.dudes[i].monst_start.facial_pic);
		
        for(j=0;j<3;j++)
    		flip_short(&c_town.monst.dudes[i].m_d.a[j]);
   		flip_short(&c_town.monst.dudes[i].m_d.corpse_item);
    	flip_short(&c_town.monst.dudes[i].m_d.corpse_item_chance);
		flip_short(&c_town.monst.dudes[i].m_d.health);
		flip_short(&c_town.monst.dudes[i].m_d.m_health);
		flip_short(&c_town.monst.dudes[i].m_d.m_morale);		
		flip_short(&c_town.monst.dudes[i].m_d.max_mp);				
		flip_short(&c_town.monst.dudes[i].m_d.morale);		
		flip_short(&c_town.monst.dudes[i].m_d.mp);				
		flip_short(&c_town.monst.dudes[i].m_d.picture_num);						
		for(j=0;j<15;j++)
    		flip_short(&c_town.monst.dudes[i].m_d.status[j]);	
        }					
    }
}

/*void port_town(short mode)//0 scenario port, 1 save port <= is_win ? check already done in load_file
{
	short i;

	if (cur_scen_is_win == TRUE && mode == 0)
		return;
	flip_short(&c_town.town.town_chop_time);
	flip_short(&c_town.town.town_chop_key);
	flip_short(&c_town.town.lighting);
	for (i =0 ; i < 4; i++)
		flip_short(&c_town.town.exit_specs[i]);
	flip_rect(&c_town.town.in_town_rect);
	for (i =0 ; i < 64; i++) {
		flip_short(&c_town.town.preset_items[i].item_code);
		flip_short(&c_town.town.preset_items[i].ability);
		}
	for (i =0 ; i < 50; i++) {
		flip_short(&c_town.town.preset_fields[i].field_type);
		}
	flip_short(&c_town.town.max_num_monst);
	flip_short(&c_town.town.spec_on_entry);
	flip_short(&c_town.town.spec_on_entry_if_dead);
	for (i =0 ; i < 8; i++) 
		flip_short(&c_town.town.timer_spec_times[i]);
	for (i =0 ; i < 8; i++) 
		flip_short(&c_town.town.timer_specs[i]);
	flip_short(&c_town.town.difficulty);
	for (i =0 ; i < 100; i++) 
		c_town.town.specials[i].flip();
}*/

void port_t_d(short mode)
{
	short i;
	if (cur_scen_is_win == TRUE && mode == 0)
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
	short i,j;

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
	flip_short(&scenario.flag_i);
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
		scenario.scen_specials[i].flip();
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

void port_party(){

    int i,j,k;
    
    flip_long(&party.age);
    flip_short(&party.food);
    flip_short(&party.gold);
    flip_short(&party.light_level);
    for (i = 0; i < 30; i++) {
		flip_short(&party.boats[i].which_town);
		}
	for (i = 0; i < 30; i++) {
		flip_short(&party.horses[i].which_town);
        }
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 60; j++)	
		{
			flip_short(&party.creature_save[i].dudes[j].active);
			flip_short(&party.creature_save[i].dudes[j].attitude);
			{
				flip_short(&party.creature_save[i].dudes[j].m_d.health);
				flip_short(&party.creature_save[i].dudes[j].m_d.m_health);
				flip_short(&party.creature_save[i].dudes[j].m_d.mp);
				flip_short(&party.creature_save[i].dudes[j].m_d.max_mp);
                for(k=0;k<3;k++)
    				flip_short(&party.creature_save[i].dudes[j].m_d.a[k]);
				flip_short(&party.creature_save[i].dudes[j].m_d.morale);
				flip_short(&party.creature_save[i].dudes[j].m_d.m_morale);
				flip_short(&party.creature_save[i].dudes[j].m_d.corpse_item);
				flip_short(&party.creature_save[i].dudes[j].m_d.corpse_item_chance);
                for(k=0;k<15;k++)
    				flip_short(&party.creature_save[i].dudes[j].m_d.status[k]);
				flip_short(&party.creature_save[i].dudes[j].m_d.picture_num);
			}
    			flip_short(&party.creature_save[i].dudes[j].summoned);
			{
				flip_short(&party.creature_save[i].dudes[j].monst_start.spec1);
				flip_short(&party.creature_save[i].dudes[j].monst_start.spec2);
				flip_short(&party.creature_save[i].dudes[j].monst_start.monster_time);
				flip_short(&party.creature_save[i].dudes[j].monst_start.personality);
				flip_short(&party.creature_save[i].dudes[j].monst_start.special_on_kill);
				flip_short(&party.creature_save[i].dudes[j].monst_start.facial_pic);
			}
		}
		
	    flip_short(&party.creature_save[i].which_town);
		flip_short(&party.creature_save[i].friendly);
	}
		flip_short(&party.in_boat);
    	flip_short(&party.in_horse);
for (i = 0; i < 10; i++)
	{
		flip_short(&party.out_c[i].direction);
		{
			flip_short(&party.out_c[i].what_monst.spec_on_meet);
			flip_short(&party.out_c[i].what_monst.spec_on_win);
			flip_short(&party.out_c[i].what_monst.spec_on_flee);
			flip_short(&party.out_c[i].what_monst.cant_flee);
			flip_short(&party.out_c[i].what_monst.end_spec1);
			flip_short(&party.out_c[i].what_monst.end_spec2);
		}
	}
	for (i = 0; i < 5; i++)
		for (j = 0; j < 10; j++)
		{
			flip_short(&party.magic_store_items[i][j].variety);
			flip_short(&party.magic_store_items[i][j].item_level);
			flip_short(&party.magic_store_items[i][j].value);
		}
	for(k=0;k<4;k++)
        flip_short(&party.imprisoned_monst[k]);
    for(k=0;k<50;k++)
	flip_short(&party.journal_day[k]);
	for(k=0;k<140;k++){
	       flip_short(&party.special_notes_str[k][0]);
	       flip_short(&party.special_notes_str[k][1]);	       
        }
	for (i = 0; i < 120; i++)
	{
		flip_short(&party.talk_save[i].personality);
		flip_short(&party.talk_save[i].town_num);
		flip_short(&party.talk_save[i].str1);
		flip_short(&party.talk_save[i].str2);
	}
	flip_short(&party.direction);
	flip_short(&party.at_which_save_slot);
    for(k=0;k<100;k++)
    	flip_short(&party.key_times[k]);
    for(k=0;k<30;k++)
    	flip_short(&party.party_event_timers[k]);
    for(k=0;k<30;k++)
	   flip_short(&party.global_or_town[k]);
    for(k=0;k<30;k++)
    	flip_short(&party.node_to_call[k]);
    for(k=0;k<200;k++)
    	flip_short(&party.m_killed[k]);
	flip_long((long *) &party.total_m_killed);
	flip_long((long *) &party.total_dam_done);
	flip_long((long *) &party.total_xp_gained);
	flip_long((long *) &party.total_dam_taken);
}

void port_pc(){

    int i,j;
    
    for(i=0;i<6;i++){
    flip_short(&adven[i].cur_health);
    flip_short(&adven[i].cur_sp);
    flip_short(&adven[i].direction);
    flip_short(&adven[i].exp_adj);
    flip_short(&adven[i].experience);
    flip_short(&adven[i].level);
    flip_short(&adven[i].main_status);
    flip_short(&adven[i].max_health);
    flip_short(&adven[i].max_sp);
    flip_short(&adven[i].race);            
    flip_short(&adven[i].skill_pts);
    for(j=0;j<30;j++)
        flip_short(&adven[i].skills[j]);
    for(j=0;j<15;j++)
        flip_short(&adven[i].status[j]);
    flip_short(&adven[i].weap_poisoned);
    flip_short(&adven[i].which_graphic);    
    for(j=0;j<24;j++){
        flip_short(&adven[i].items[j].item_level);
        flip_short(&adven[i].items[j].value);
        flip_short(&adven[i].items[j].variety);
        }
    }

}

void port_item_list()
{
	short i;

	if (cur_scen_is_win == TRUE)
		return;

	for (i = 0; i < 400; i++) {
		flip_short(&(scen_item_list->scen_items[i].variety));
		flip_short(&(scen_item_list->scen_items[i].item_level));
		flip_short(&(scen_item_list->scen_items[i].value));
		}
}

void port_stored_items(stored_items_list* list)
{
	short i;

	for (i = 0; i < NUM_TOWN_ITEMS; i++) {
		flip_short(&(list->items[i].variety));
		flip_short(&(list->items[i].item_level));
		flip_short(&(list->items[i].value));
		}
}

void outdoor_record_type::flip()
{
	short i;

	if (cur_scen_is_win == TRUE)
		return;

	for (i = 0; i < 4; i++) {
		flip_short(&(wandering[i].spec_on_meet));
		flip_short(&(wandering[i].spec_on_win));
		flip_short(&(wandering[i].spec_on_flee));
		flip_short(&(wandering[i].cant_flee));
		flip_short(&(wandering[i].end_spec1));
		flip_short(&(wandering[i].end_spec2));
		flip_short(&(special_enc[i].spec_on_meet));
		flip_short(&(special_enc[i].spec_on_win));
		flip_short(&(special_enc[i].spec_on_flee));
		flip_short(&(special_enc[i].cant_flee));
		flip_short(&(special_enc[i].end_spec1));
		flip_short(&(special_enc[i].end_spec2));
		}
	for (i = 0; i < 8; i++)
		flip_rect(&info_rect[i]);
	for (i = 0; i < 60; i++)
		specials[i].flip();
}

void special_node_type::flip()
{
	flip_short(&type);
	flip_short(&sd1);
	flip_short(&sd2);
	flip_short(&pic);
	flip_short(&m1);
	flip_short(&m2);
	flip_short(&ex1a);
	flip_short(&ex1b);
	flip_short(&ex2a);
	flip_short(&ex2b);
	flip_short(&jumpto);
}

/*void flip_short(short *s)
{
	char store,*s1, *s2;

	s1 = (char *) s;
	s2 = s1 + 1;
	store = *s1;
	*s1 = *s2;
	*s2 = store;
}*/

void flip_short(short* s){
	*s=((*s&255)<<8)|((*s>>8)&255);
}

void flip_long(long *s)
{
/*        char store,*s1, *s2, *s3, *s4;

        s1 = (char *) s;
        s2 = s1 + 1;
        s3 = s1 + 2;
        s4 = s1 + 3;
        store = *s1;
        *s1 = *s4;
        *s4 = store;
        store = *s2;
        *s2 = *s3;
        *s3 = store;*/
        
*s=((*s&255)<<24)|(((*s>>8)&255) << 16)|(((*s>>16)&255) << 8)|((*s >> 24)&255);
}


void flip_short(WORD *s)
{
	BYTE store, *s1, *s2;

	s1 = (BYTE *) s;
	s2 = s1 + 1;
	store = *s1;
	*s1 = *s2;
	*s2 = store;
}

void flip_rect(RECT16 *s)
{
	flip_short((short *) &(s->top));
	flip_short((short *) &(s->bottom));
	flip_short((short *) &(s->left));
	flip_short((short *) &(s->right));
	alter_rect(s);
}
