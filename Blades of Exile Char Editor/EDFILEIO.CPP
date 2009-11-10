#include <Windows.h>
#include <commdlg.h>

#include <string.h>
#include "stdio.h"

#include "global.h"
#include "edfileio.h"
#include "graphics.h"
#include "edsound.h"
#include "editors.h"


/* Adventure globals */
extern party_record_type party;
extern pc_record_type adven[6];
extern outdoor_record_type outdoors[2][2];
extern current_town_type c_town;
extern big_tr_type t_d;
extern stored_items_list_type t_i;
extern unsigned char misc_i[64][64],sfx[64][64];
extern unsigned char out[96][96];
extern unsigned char out_e[96][96];
extern setup_save_type setup_save;
extern stored_items_list_type stored_items[3];
extern stored_town_maps_type maps;
extern stored_outdoor_maps_type o_maps;

extern scen_item_data_type scen_item_list;
extern char file_path_name[256];
extern short item_menus_lock;

extern Boolean play_sounds,sys_7_avail,party_in_scen;
extern short current_active_pc;
extern HWND mainPtr;

extern Boolean file_in_mem;


typedef struct {
	char expl[96][96];
	}	out_info_type;

extern short store_flags[3];

// Big waste!
out_info_type store_map;
char last_load_file[63] = "blades.sav";
char szFileName [128] = "blades.sav";
char szTitleName [128] = "blades.sav";
OPENFILENAME ofn;
extern stored_town_maps_type town_maps;
extern char town_strs[180][256];

short store_size;


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
static char *szFilter[] = {"Blades of Exile Save Files (*.SAV)","*.sav",
		"Text Files (*.TXT)","*.txt",
		"All Files (*.*)","*.*",
		""};


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
		ofn.lpstrDefExt = "txt";
		ofn.lCustData = 0L;
		ofn.lpfnHook = NULL;
		ofn.lpTemplateName = NULL;
}

void load_file()
{
	long file_size;
	HANDLE file_id;
	short i, j;
	Boolean town_restore = FALSE;
	Boolean maps_there = FALSE;
	Boolean in_scen = FALSE;

	DWORD dwByteRead;

	UINT count;

	char *party_ptr;
	char *pc_ptr;
	short flag;


	short flags[3][2] = {{5790,1342}, // slot 0 ... 5790 - out  1342 - town
					{100,200}, // slot 1 100  in scenario, 200 not in
					{3422,5567}}; // slot 2 ... 3422 - no maps  5567 - maps

	ofn.hwndOwner = mainPtr;
	ofn.lpstrFile = szFileName;
	ofn.lpstrFileTitle = szTitleName;
	ofn.Flags = 0;

	if (GetOpenFileName(&ofn) == 0)
		return;

    SetCurrentDirectory(file_path_name);

	file_id = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file_id == INVALID_HANDLE_VALUE) return;

	file_size = sizeof(party_record_type);


	for (i = 0; i < 3; i++)
	{
		if (ReadFile(file_id, &flag, sizeof(short), &dwByteRead, NULL) == FALSE)
		{
			CloseHandle(file_id);
			return;
		}

		if ((flag != flags[i][0]) && (flag != flags[i][1]))
		{
			CloseHandle(file_id);
			FCD(1063,0);
			return;
		}

        store_flags[i]=flag;

		if ((i == 0) && (flag == flags[i][1])) town_restore = TRUE;
		if ((i == 1) && (flag == flags[i][0])) in_scen = TRUE;
		if ((i == 2) && (flag == flags[i][1])) maps_there = TRUE;
		}

	// LOAD PARTY
	party_ptr = (char *) &party;

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
			ReadFile(file_id, &party.out_c[i].what_monst.end_spec1, 2, &dwByteRead, NULL);
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

	for (count = 0; count < sizeof(party_record_type); count++)	party_ptr[count] ^= 0x5C;

	// LOAD SETUP

	if (ReadFile(file_id, &setup_save, sizeof(setup_save_type), &dwByteRead, NULL) == FALSE)
		{
		CloseHandle(file_id);
		FCD(1064,0);
		return;
		}

	// LOAD PCS

	for (i = 0; i < 6; i++)
	{
		pc_ptr = (char *) &adven[i];
		if (ReadFile(file_id, pc_ptr, sizeof(pc_record_type), &dwByteRead, NULL) == FALSE)
		{
			CloseHandle(file_id);
			FCD(1064,0);
			return;
		}

		for (count = 0; count < sizeof(pc_record_type); count++)
			pc_ptr[count] ^= 0x6B;
	}

	if (in_scen == TRUE) {

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
		/*if (ReadFile(file_id, &c_town, sizeof(current_town_type), &dwByteRead, NULL) == FALSE)
		{
			CloseHandle(file_id);
			FCD(1064,0);
			return;
		}*/

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

		if (ReadFile(file_id, &t_d, sizeof(big_tr_type), &dwByteRead, NULL) == FALSE)
		{
			CloseHandle(file_id);
			FCD(1064,0);
			return;
		}

		if (ReadFile(file_id, &t_i, sizeof(stored_items_list_type), &dwByteRead, NULL) == FALSE)
		{
			CloseHandle(file_id);
			FCD(1064,0);
			return;
		}

        load_town_strings(c_town.town_num); // load town strings

	}

	// LOAD STORED ITEMS
	for (i = 0; i < 3; i++)
	{
		if (ReadFile(file_id, &stored_items[i], sizeof(stored_items_list_type), &dwByteRead, NULL) == FALSE)
		{
			CloseHandle(file_id);
			FCD(1064,0);
			return;
		}
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

	for (i = 0; i < 6; i++)
		if (adven[i].main_status > 0) {
			current_active_pc = i;
			i = 6;
			}

	file_in_mem = TRUE;
	party_in_scen = in_scen;

    item_menus_lock = load_items_list();

	redraw_screen();
}

short load_items_list(){    //LOAD SCENARIO ITEMS LIST

    HANDLE scen_file_id;
	DWORD dwScenByteRead;

    scenario_data_type scenario;
    char scen_name[256]="";

    if(party_in_scen)
        sprintf(scen_name,"scenarios/%s",party.scen_name);
    else
        sprintf(scen_name,"bladbase.exs");

	scen_file_id = CreateFile(scen_name, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (scen_file_id == INVALID_HANDLE_VALUE)
	{
      	CloseHandle(scen_file_id);
        MessageBox(mainPtr,"Couldn't open the scenario file, trying to use bladbase.exs instead.",
	  "File Error",MB_OK | MB_ICONEXCLAMATION);
        scen_file_id = CreateFile("bladbase.exs", GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(scen_file_id == INVALID_HANDLE_VALUE){
           	CloseHandle(scen_file_id);
            MessageBox(mainPtr,"Couldn't open bladbase.exs, can't give items.",
	         "File Error",MB_OK | MB_ICONEXCLAMATION);
        return 1;
        }
    }

    if (ReadFile(scen_file_id, &scenario, sizeof(scenario_data_type), &dwScenByteRead, NULL) == FALSE) //get scenario data
	{
	   CloseHandle(scen_file_id);
        MessageBox(mainPtr,"Invalid scenario file !",
	  "File Error",MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}

// item data

	if (ReadFile(scen_file_id, &scen_item_list, sizeof(scen_item_data_type), &dwScenByteRead, NULL) == FALSE)
	{
		CloseHandle(scen_file_id);
MessageBox(mainPtr,"Couldn't read the scenario item list !",
	  "File Error",MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}

	if ((scenario.flag1 == 10) && (scenario.flag2 == 20)
	 && (scenario.flag3 == 30)
	  && (scenario.flag4 == 40)) {
		port_item_list(); //Mac scenario, so let's make sure to adjust items properties
		}

	CloseHandle(scen_file_id);

	return 0;
}//END SCENARIO ITEMS LIST LOADING

void load_town_strings(short which_town){    //LOAD TOWNS STRINGS LIST

    HANDLE scen_file_id;
	long len, len_to_jump=0;
    DWORD dwScenByteRead;

    scenario_data_type scenario;

    char scen_name[256]="";
    int i,j;

    sprintf(scen_name,"scenarios/%s",party.scen_name);

	scen_file_id = CreateFile(scen_name, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (scen_file_id == INVALID_HANDLE_VALUE)
	{
        MessageBox(mainPtr,"Couldn't open the scenario file !",
	  "File Error",MB_OK | MB_ICONEXCLAMATION);
		return;
	}

if (ReadFile(scen_file_id, &scenario, sizeof(scenario_data_type), &dwScenByteRead, NULL) == FALSE)
	{
	   CloseHandle(scen_file_id);
        MessageBox(mainPtr,"Invalid scenario file !",
	  "File Error",MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	if ((scenario.flag1 == 10) && (scenario.flag2 == 20)
	 && (scenario.flag3 == 30)
	  && (scenario.flag4 == 40)) {
		port_scenario(&scenario); //Mac scenario, so let's make sure we can read it properly
		}

	len_to_jump += sizeof(scen_item_data_type);//now let's jump to the town_strings position
	for (i = 0; i < 300; i++)
		len_to_jump += (long) scenario.scen_str_len[i];
	long store = 0;
	for (i = 0; i < 100; i++)
		for (j = 0; j < 2; j++)
			store += (long) (scenario.out_data_size[i][j]);
	for (i = 0; i < which_town; i++)
		for (j = 0; j < 5; j++)
			store += (long) (scenario.town_data_size[i][j]);
	len_to_jump += store;
	len_to_jump += sizeof(town_record_type);

    SetFilePointer(scen_file_id, len_to_jump, NULL, FILE_CURRENT);

    switch (scenario.town_size[which_town]) {
		case 0:
			SetFilePointer(scen_file_id, sizeof(big_tr_type), NULL, FILE_CURRENT);
			break;

		case 1:
            SetFilePointer(scen_file_id, sizeof(ave_tr_type), NULL, FILE_CURRENT);
			break;

		case 2:
            SetFilePointer(scen_file_id, sizeof(tiny_tr_type), NULL, FILE_CURRENT);
			break;
		}

        // town strings

		for (i = 0; i < 140; i++) {
		len = (long) (c_town.town.strlens[i]);
        ReadFile(scen_file_id,  &town_strs[i], len, &dwScenByteRead, NULL);
   		town_strs[i][len] = 0;
		}

	CloseHandle(scen_file_id);

}//END TOWNS STRINGS LOADING

void save_file(short mode)
//short mode;  // 0 - normal  1 - save as
{
	HANDLE file_id;

	short i,j;

	DWORD count, bytes, dwByteRead;
	short flag;
	short *store;
	party_record_type *party_ptr;
	setup_save_type	*setup_ptr;
	pc_record_type *pc_ptr;

	char *party_encryptor;

		Boolean town_save = FALSE,in_scen = FALSE,save_maps = FALSE;

	if (file_in_mem == FALSE)
		return;

	if (store_flags[0] == 1342)
		town_save = TRUE;
	if (store_flags[1] == 100)
		in_scen = TRUE;
	if (store_flags[2] == 5567)  {
		save_maps = TRUE;
  		}

	ofn.hwndOwner = mainPtr;
	ofn.lpstrFile = szFileName;
	ofn.lpstrFileTitle = szTitleName;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if (mode == 1) {
		if (GetSaveFileName(&ofn) == 0)
			return;
		}

	file_id = CreateFile(szFileName, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    SetCurrentDirectory(file_path_name);

	if (file_id == INVALID_HANDLE_VALUE) return;

	store = &flag;

	flag = (town_save == TRUE) ? 1342 : 5790;
	if (WriteFile(file_id, store, sizeof(short), &bytes, NULL) == FALSE)
	{
		CloseHandle(file_id);
		FCD(1069,0);
		return;
	}
	flag = (in_scen == TRUE) ? 100 : 200;
	if (WriteFile(file_id, store, sizeof(short), &bytes, NULL) == FALSE)
	{
		CloseHandle(file_id);
		FCD(1069,0);
		return;
	}
	flag = (save_maps == TRUE) ? 5567 : 3422;
	if (WriteFile(file_id, store, sizeof(short), &bytes, NULL) == FALSE)
	{
		CloseHandle(file_id);
		FCD(1069,0);
		return;
	}

	// SAVE PARTY
	party_ptr = &party;

	party_encryptor = (char *) party_ptr;
	for (count = 0; count < sizeof(party_record_type); count++)
		party_encryptor[count] ^= 0x5C;

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
			WriteFile(file_id, &party.out_c[i].what_monst.end_spec1, 2, &dwByteRead, NULL);
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
	if (WriteFile(file_id, &setup_save, sizeof(setup_save_type), &bytes, NULL) == FALSE)
	{
		CloseHandle(file_id);
		FCD(1069,0);
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
			CloseHandle(file_id);
			for (count = 0; count < sizeof(pc_record_type); count++)
				party_encryptor[count] ^= 0x6B;
    			FCD(1069,0);
				MessageBeep(MB_OK);
			return;
			}
		for (count = 0; count < sizeof(pc_record_type); count++)
			party_encryptor[count] ^= 0x6B;
		}

	if (party_in_scen == TRUE) {

	// SAVE OUT DATA
	if (WriteFile(file_id, out_e, sizeof(out_info_type), &bytes, NULL) == FALSE)
	{
		CloseHandle(file_id);
		FCD(1069,0);
		return;
	}

	if (town_save == TRUE) {
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

			if (WriteFile(file_id, &t_d, sizeof(big_tr_type), &bytes, NULL) == FALSE)
			{
				CloseHandle(file_id);
    			FCD(1069,0);
				return;
			}
			if (WriteFile(file_id, &t_i, sizeof(stored_items_list_type), &bytes, NULL) == FALSE)
			{
				CloseHandle(file_id);
    			FCD(1069,0);
				return;
			}
		}

	// Save stored items
	for (i = 0; i < 3; i++) {
		if (WriteFile(file_id, &stored_items[i], sizeof(stored_items_list_type), &bytes, NULL) == FALSE)
		{
			CloseHandle(file_id);
			FCD(1069,0);
			return;
		}
	}

	// If saving maps, save maps
	if (save_maps == TRUE) {
		if (WriteFile(file_id, &(town_maps), sizeof(stored_town_maps_type), &bytes, NULL) == FALSE)
		{
			CloseHandle(file_id);
			FCD(1069,0);
			return;
		}

		if (WriteFile(file_id, &o_maps, sizeof(stored_outdoor_maps_type), &bytes, NULL) == FALSE)
		{
			CloseHandle(file_id);
			FCD(1069,0);
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
			FCD(1069,0);
			return;
		}
	}

	CloseHandle(file_id);
}


void leave_town()
{
	store_flags[0] = 5790;
}


void remove_party_from_scen()
{
	store_flags[1] = 200;
	party_in_scen = FALSE;
}

void get_reg_data()
{
	const int BUFFER_LEN = 64;
	char buffer[BUFFER_LEN];
	const char * iniFile = "./blades.ini";
	const char * section = "Blades of Exile";

	GetPrivateProfileString(section, "play_sounds", "1", buffer, BUFFER_LEN, iniFile);
	play_sounds = (atoi(buffer))? TRUE : FALSE;
}

//Mac porting stuff

void port_scenario(scenario_data_type *scenario)
{
	short i,j;

	flip_short(&scenario->flag_a);
	flip_short(&scenario->flag_b);
	flip_short(&scenario->flag_c);
	flip_short(&scenario->flag_d);
	flip_short(&scenario->flag_e);
	flip_short(&scenario->flag_f);
	flip_short(&scenario->flag_g);
	flip_short(&scenario->flag_h);
	flip_short(&scenario->flag_i);
	flip_short(&scenario->intro_mess_pic);
	flip_short(&scenario->intro_mess_len);
	flip_short(&scenario->which_town_start);
	for (i = 0; i < 200; i++)
		for (j = 0; j < 5; j++)
			flip_short(&scenario->town_data_size[i][j]);
	for (i = 0; i < 10; i++)
		flip_short(&scenario->town_to_add_to[i]);
	for (i = 0; i < 10; i++)
		for (j = 0; j < 2; j++)
			flip_short(&scenario->flag_to_add_to_town[i][j]);
	for (i = 0; i < 100; i++)
		for (j = 0; j < 2; j++)
			flip_short(&scenario->out_data_size[i][j]);
	for (i = 0; i < 3; i++)
		flip_rect(&scenario->store_item_rects[i]);
	for (i = 0; i < 3; i++)
		flip_short(&scenario->store_item_towns[i]);
	for (i = 0; i < 50; i++)
		flip_short(&scenario->special_items[i]);
	for (i = 0; i < 50; i++)
		flip_short(&scenario->special_item_special[i]);
	flip_short(&scenario->rating);
	flip_short(&scenario->uses_custom_graphics);
	for (i = 0; i < 256; i++) {
		flip_short(&scenario->scen_monsters[i].health);
		flip_short(&scenario->scen_monsters[i].m_health);
		flip_short(&scenario->scen_monsters[i].max_mp);
		flip_short(&scenario->scen_monsters[i].mp);
		flip_short(&scenario->scen_monsters[i].a[1]);
		flip_short(&scenario->scen_monsters[i].a[0]);
		flip_short(&scenario->scen_monsters[i].a[2]);
		flip_short(&scenario->scen_monsters[i].morale);
		flip_short(&scenario->scen_monsters[i].m_morale);
		flip_short(&scenario->scen_monsters[i].corpse_item);
		flip_short(&scenario->scen_monsters[i].corpse_item_chance);
		flip_short(&scenario->scen_monsters[i].picture_num);
		}

	for (i = 0; i < 256; i++) {
		flip_short(&scenario->ter_types[i].picture);
		}
	for (i = 0; i < 30; i++) {
		flip_short(&scenario->scen_boats[i].which_town);
		}
	for (i = 0; i < 30; i++) {
		flip_short(&scenario->scen_horses[i].which_town);
		}
	for (i = 0; i < 20; i++)
		flip_short(&scenario->scenario_timer_times[i]);
	for (i = 0; i < 20; i++)
		flip_short(&scenario->scenario_timer_specs[i]);
	for (i = 0; i < 256; i++) {
		flip_spec_node(&scenario->scen_specials[i]);
		}
		for (i = 0; i < 10; i++)  {
		flip_short(&scenario->storage_shortcuts[i].ter_type);
		flip_short(&scenario->storage_shortcuts[i].property);
		for (j = 0; j < 10; j++)  {
			flip_short(&scenario->storage_shortcuts[i].item_num[j]);
			flip_short(&scenario->storage_shortcuts[i].item_odds[j]);
			}
		}
	flip_short(&scenario->last_town_edited);
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

void flip_rect(RECT16 *s)
{
   	short a;
	flip_short((short *) &(s->top));
	flip_short((short *) &(s->bottom));
	flip_short((short *) &(s->left));
	flip_short((short *) &(s->right));
    a = s->top;
	s->top = s->left;
	s->left = a;
	a = s->bottom;
	s->bottom = s->right;
	s->right = a;
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

void port_item_list()
{
	short i;

	for (i = 0; i < 400; i++) {
		flip_short(&(scen_item_list.scen_items[i].variety));
		flip_short(&(scen_item_list.scen_items[i].item_level));
		flip_short(&(scen_item_list.scen_items[i].value));
		}
}
