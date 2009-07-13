#include <iostream>
#include "Global.h"
#include "TFILEIO.h"

extern scenario_data_type scenario;
short jl = 0;

// outdoor_record_type current_terrain;
Boolean cur_scen_is_win = 1;



void print_write_position ();

short wd_to_pwd(char *str)
{
	char pwd[8] = "aaaaaa";
	short i;
	long val = 0,pow[6] = {1,10,100,1000,9999,99999};
	
	for (i = 0; i < 6; i++) {
		if (str[i] == 0) 
			i = 6;
			else {
				if ((str[i] >= 65) && (str[i] <= 90))
					pwd[i] = str[i] + 32;
				else if ((str[i] >= 48) && (str[i] <= 57))
					pwd[i] = str[i] + 49;
				else if ((str[i] >= 97) && (str[i] <= 122))
					pwd[i] = str[i];
				}
		}
	for (i = 0; i < 6; i++)
		val = val + pow[i] * (long) (pwd[i] - 97);
	val = val % 30000;
	return (short) val;
}

short town_s(short flag)
{
	long k = 0;
		
	k = (long) flag;
	k = k * k * k;
	jl = jl * 54;
	jl = jl * 2 + 1234 + k;
	k = k + 51;
	k = k % 3000;
	jl = jl * 2 + 1234 + k;
	k = k * scenario.num_towns;
	k = k % 10000;
	jl = jl * jl + 84 + k;
	k = k + 10000;
	
	return (short) k;
}

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

short out_s(short flag)
{
	long k = 0;
		
	k = (long) flag;
	k = k * k * k;
	jl = jl * jl + 84 + k;
	k = k + scenario.out_data_size[0][1];
	k = k % 3000;
	k = k * 4;
	jl = jl * 2 + 1234 + k;
	jl = jl * 54;
	jl = jl * 2 + 1234 + k;
	k = k % 10000;
	k = k + 4;
	
	return (short) k;
}

short str_size_1(short flag)
{
	long k = 0;
		
	k = (long) flag;
	k = k * k;
	jl = jl * 2 + 1234 + k;
	jl = jl * jl + 84 + k;
	k = k + scenario.scen_str_len[0] + scenario.scen_str_len[1] + scenario.scen_str_len[2];
	jl = jl * 2 + 1234 + k;
	k = k % 3000;
	jl = jl * 54;
	jl = jl * jl + 84 + k;
	k = k * 4;
	k = k % 5000;
	k = k - 9099;
	
	return (short) k;
}

short str_size_2(short flag)
{
	long k = 0;
		
	k = (long) flag;
	jl = jl * jl + 84 + k;
	k = k * k * k * k;
	jl = jl * 54;
	k = k + 80;
	k = k % 3000;
	jl = jl * 2 + 1234 + k;
	k = k * scenario.out_width * scenario.out_height;
	jl = jl * jl + 84 + k;
	k = k % 3124;
	k = k - 5426;
	
	return (short) k;
}

short str_size_3(short flag)
{
	long k = 0;
		
	k = (long) flag;
	k = k * (scenario.town_data_size[0][0] +  scenario.town_data_size[0][1] +  scenario.town_data_size[0][2] +  scenario.town_data_size[0][3]);
	k = k + 80;
	jl = jl * jl + 84 + k;
	k = k % 3000;
	jl = jl * 2 + 1234 + k;
	k = k * 45;
	jl = jl * 54;
	jl = jl * jl + 84 + k;
	k = k % 887;
	k = k + 9452;
	
	return (short) k;
}

Boolean save_scenario(char * argv[])
{
	Boolean file_ok = FALSE;
	FILE *pFile;
	char error;

	pFile = fopen(argv[1], "rb+");

	if(pFile == NULL){
        return FALSE;
        }

	short pass = wd_to_pwd(argv[2]); //set the password variables
    scenario.flag_a = sizeof(scenario_data_type) + get_ran(1,-1000,1000);
	scenario.flag_b=town_s(pass);
	scenario.flag_c=out_s(pass);
	scenario.flag_e=str_size_1(pass);
	scenario.flag_f=str_size_2(pass);
    scenario.flag_h=str_size_3(pass);
    scenario.flag_g = 10000 + get_ran(1,0,5000);
   	scenario.flag_d=init_data(pass);
/*   	printf("flag b : %d\nflag c : %d\n flag e : %d \n flag f : %d \n flag h : %d\n flag d : %d\n",town_s(pass),out_s(pass),str_size_1(pass),str_size_2(pass),str_size_3(pass),init_data(pass));*/

	if(cur_scen_is_win == FALSE)//if Mac made scenario flip it back
		port_scenario();

    if((error = fwrite(&scenario, sizeof(scenario_data_type),1, pFile)) != 1 )
    {
      	fclose(pFile);
      	return FALSE;
    }

    fclose(pFile);

	return TRUE;
}

Boolean load_scenario(char * argv[])
{
	Boolean file_ok = FALSE;
	FILE *pFile;
	char error;

	
	pFile = fopen(argv[1], "rb");

	if(pFile == NULL){
        return FALSE;
        }

    if((error = fread(&scenario, sizeof(scenario_data_type),1, pFile)) != 1 )
    {
      	fclose(pFile);
      	return FALSE;
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
        fclose(pFile);
		printf("This is not a legitimate Blades of Exile scenario.\n","",0);
		return FALSE;
		}

    fclose(pFile);
    return TRUE;
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


void flip_short(short *s)
{
	char store,*s1, *s2;

	s1 = (char *) s;
	s2 = s1 + 1;
	store = *s1;
	*s1 = *s2;
	*s2 = store;

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
