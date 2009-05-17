#include <stdio.h>
#include <string.h>
#include "scen.global.h"
#include "classes.h"
#include "graphtool.h"
#include "scen.graphics.h"
#include "scen.dlgutil.h"
//#include "itemlist.h"
#include "scen.core.h"
#include "dlgtool.h"
#include "dlgconsts.h"
#include "scen.keydlgs.h"
#include "scen.townout.h"
#include "scen.fileio.h"
#include "scen.actions.h"

extern short cen_x, cen_y,/* overall_mode,*/cur_town;
extern bool mouse_button_held;
extern short cur_viewing_mode,dialog_answer;//,given_password,user_given_password;
extern cTown* town;
//extern short town_type;  // 0 - big 1 - ave 2 - small
extern short max_dim[3];
extern short mode_count,to_create;
extern unsigned char template_terrain[64][64];
cItemRec item_list[400];
extern cScenario scenario;
extern cSpecial null_spec_node;
extern cSpeech null_talk_node;
//extern piles_of_stuff_dumping_type *data_store;
extern location cur_out;
extern short start_volume, start_dir;

short store_which_ter;
cTerrain store_ter;
short store_which_monst;
cMonster store_monst,store_monst2;
short store_which_item;
cItemRec store_item,store_item2;
short store_which_spec_item ;
short spec_item_spec,store_horse_page,store_boat_page ;
cScenario::cItemStorage store_storage;
short cur_shortcut;

char *item_types[] = {"No Item","1-Handed weapon","2-Handed weapon","Gold","Bow","Arrows","Thrown missile",
	"Potion/Magic Item","Scroll/Magic Item","Wand","Tool","Food","Shield","Armor","Helm",
	"Gloves","Shield","Boots","Ring","Necklace",
"Weapon Poison","Non-Use Object","Pants","Crossbow","Bolts","Missile (no ammo)","Unused","Unused"};

unsigned char m_level[200] = {
	0,1,1,1,1,1,1,1,1,1,  // 0
	1,1,30,4,8,12,20,25,3,5,   // 10
	28,3,8,20,4,8,20,5,10,25,  // 20
	35,8,6,2,2,7,6,12,2,5,  // 30
	4,6,5,6,8,7,18,7,10,8,  // 40
	8,12,12,15,18,22,28,26,3,10,  // 50
	4,3,6,8,12,11,10,14,18,35,  // 60
	14,10,4,8,8,3,7,3,2,4,  // 70
	3,18,2,18,7,20,30,35,5,3,  // 80
	1,9,8,7,7,18,18,25,35,3,  // 90
	5,24,18,10,25,7,2,28,1,1,  // 100
	1,1,1,1,1,3,7,8,4,8,  // 110
	3,12,18,4,7,5,8,18,22,10,  // 120
	12,6,12,16,7,25,28,25,3,3,  // 130
	3,4,14,2,6,8,8,10,4,5,  // 140
	8,7,10,12,14,10,12,14,24,13,  // 150
	18,18,22,22,10,30,20,27,30,7,  // 160
	30,15,12,18,3,5,7,39,20,24,  // 170
	24,8,7,25,2,10,18,0,0,0,  // 180
	0,0,0,0,0,0,0,0,0,0
};  // 190

short m_health[200] = {
	0,6,6,6,6,6,6,6,6,6,  // 0
	3,3,140,20,40,75,115,140,14,16,   // 10
	90,18,40,90,18,40,90,16,40,90,  // 20
	160,40,35,6,6,35,30,80,10,22,  // 30
	15,25,18,30,45,35,70,30,50,40,  // 40
	40,70,70,100,90,110,140,130,15,50,  // 50
	20,15,30,40,60,55,50,70,90,170,  // 60
	70,50,20,40,40,15,35,30,10,20,  // 70
	15,100,10,90,35,70,150,200,25,20,  // 80
	5,90,75,70,70,120,120,190,400,15,  // 90
	25,120,90,50,120,35,10,200,3,3,  // 100
	3,7,7,1,5,15,35,40,14,40,  // 110
	15,50,90,150,35,25,40,90,110,50,  // 120
	60,25,50,80,35,125,140,150,8,8,  // 130
	10,10,140,10,30,40,40,60,30,25,  // 140
	40,35,50,60,70,50,60,70,200,70,  // 150
	70,70,90,120,100,200,100,150,200,40,  // 160
	500,100,80,110,17,30,30,2500,100,125,  // 170
	140,40,10,80,6,30,50,150,0,0,  // 180
	0,0,0,0,0,0,0,0,0,0
};  // 190

unsigned char m_armor[200] = {
	1,1,1,1,1,1,1,1,1,1,  // 0
	1,1,30,8,12,18,22,25,6,6,   // 10
	20,8,14,20,8,14,20,5,10,20,  // 20
	30,4,4,2,2,9,7,15,3,5,  // 30
	4,7,6,12,18,9,15,7,3,5,  // 40
	10,18,18,24,10,14,24,25,2,6,  // 50
	0,0,2,2,2,7,8,10,5,15,  // 60
	7,10,4,7,7,0,0,0,2,2,  // 70
	2,1,10,12,8,14,20,20,4,2,  // 80
	2,10,10,14,14,14,14,18,25,2,  // 90
	2,8,8,5,9,12,12,18,1,1,  // 100
	1,1,1,1,1,1,1,3,1,1,  // 110
	1,6,6,12,8,5,5,7,10,0,  // 120
	7,2,5,13,6,20,18,25,0,0,  // 130
	0,0,10,2,3,4,5,6,0,6,  // 140
	10,10,12,20,30,8,14,8,18,20,  // 150
	14,14,18,25,25,25,20,30,9,0,  // 160
	15,6,10,14,3,1,10,40,14,14,  // 170
	10,6,30,20,1,2,4,12,0,0,  // 180
	0,0,0,0,0,0,0,0,0,0
};  // 190

unsigned char m_skill[200] = {
	2,2,2,2,2,2,2,2,2,1,  // 0
	1,2,30,8,10,14,20,23,5,6,   // 10
	28,2,4,8,2,4,8,2,4,5,  // 20
	5,3,3,1,1,12,8,25,5,8,  // 30
	3,9,8,4,18,10,10,7,14,6,  // 40
	6,6,6,28,12,10,28,28,4,10,  // 50
	5,3,9,13,18,7,10,17,20,30,  // 60
	8,16,7,12,12,4,7,7,3,5,  // 70
	5,15,4,18,12,18,24,33,5,5,  // 80
	3,12,12,8,8,8,8,23,35,4,  // 90
	6,7,7,5,12,0,0,0,1,1,  // 100
	1,1,1,1,1,5,9,12,3,25,  // 110
	5,9,20,25,10,5,12,15,24,13,  // 120
	8,3,12,18,10,4,28,33,3,3,  // 130
	3,3,12,4,6,7,7,13,30,5,  // 140
	8,6,8,14,22,12,18,10,30,20,  // 150
	15,15,13,26,0,0,30,36,30,4,  // 160
	26,18,15,17,6,5,10,35,20,20,  // 170
	17,7,2,16,2,8,14,18,0,0,  // 180
	0,0,0,0,0,0,0,0,0,0
};  // 190

short m_a1[200] = {
	6,6,6,6,6,6,6,6,6,1,  // 0
	1,2,210,107,109,110,208,212,8,6,   // 10
	10,8,8,208,8,8,208,6,8,10,  // 20
	12,6,6,3,3,12,106,108,7,9,  // 30
	8,107,8,8,110,206,206,10,208,12,  // 40
	10,10,10,310,408,408,510,410,8,108,  // 50
	104,8,108,208,308,10,108,0,110,0,  // 60
	210,0,106,206,206,8,110,12,6,10,  // 70
	8,0,7,215,0,208,0,510,106,8,  // 80
	3,212,16,18,18,112,112,410,514,8,  // 90
	10,10,10,106,206,0,0,0,2,2,  // 100
	2,2,2,2,2,106,206,306,8,206,  // 110
	8,106,0,0,12,8,10,206,306,409,  // 120
	306,106,407,410,106,12,310,0,106,8,  // 130
	8,7,110,8,108,208,108,308,108,10,  // 140
	112,8,10,210,310,208,308,208,410,0,  // 150
	310,310,10,410,0,0,810,913,913,8,  // 160
	710,512,210,212,12,0,208,840,308,308,  // 170
	308,209,8,912,8,108,210,312,0,0,  // 180
	0,0,0,0,0,0,0,0,0,0
};  // 190

short m_a2[200] = {
	0,0,0,0,0,0,0,0,0,0,  // 0
	0,0,210,0,0,110,208,212,0,0,   // 10
	0,0,0,0,0,0,0,0,0,0,  // 20
	0,0,0,0,0,0,106,108,0,0,  // 30
	0,0,0,0,110,0,0,0,208,0,  // 40
	0,0,0,310,0,0,0,0,0,0,  // 50
	0,0,10,13,405,0,0,306,0,308,  // 60
	0,208,8,10,10,8,0,0,0,0,  // 70
	0,310,0,110,10,15,310,310,0,0,  // 80
	0,0,0,0,0,0,0,310,0,0,  // 90
	0,0,0,8,10,0,0,0,0,0,  // 100
	0,0,0,0,0,0,0,12,0,0,  // 110
	0,0,210,409,0,0,0,8,106,0,  // 120
	0,0,0,0,0,0,0,410,0,0,  // 130
	0,0,110,0,0,0,0,0,0,0,  // 140
	0,0,0,0,0,0,0,0,0,212,  // 150
	0,0,0,0,0,0,0,0,0,0,  // 160
	310,0,108,12,0,105,0,730,308,308,  // 170
	308,209,0,0,0,0,0,212,0,0,  // 180
	0,0,0,0,0,0,0,0,0,0
};  // 190

short m_a3[200] = {
	0,0,0,0,0,0,0,0,0,0,  // 0
	0,0,0,0,0,0,0,0,0,0,   // 10
	0,0,0,0,0,0,0,0,0,0,  // 20
	0,0,0,0,00,0,0,108,0,0,  // 30
	0,0,0,0,0,00,00,0,0,0,  // 40
	0,0,0,0,0,0,0,0,0,0,  // 50
	00,0,10,13,405,0,0,306,0,308,  // 60
	0,208,8,10,10,8,0,0,0,0,  // 70
	0,310,0,110,10,15,310,310,0,0,  // 80
	0,0,0,0,0,0,0,0,0,0,  // 90
	0,0,0,8,10,0,0,0,0,0,  // 100
	0,0,0,0,0,0,0,12,0,0,  // 110
	0,0,210,409,0,0,0,8,106,0,  // 120
	0,0,0,0,0,0,0,410,0,0,  // 130
	0,0,0,0,0,0,0,0,0,0,  // 140
	0,0,0,0,0,0,0,0,0,212,  // 150
	0,0,0,0,0,0,0,0,0,0,  // 160
	310,0,108,12,0,105,0,730,308,308,  // 170
	308,0,0,0,0,0,0,212,0,0,  // 180
	0,0,0,0,0,0,0,0,0,0
};  // 190

unsigned char m_a1t[200] = {
	0,0,0,0,0,0,0,0,0,0,  // 0
	0,0,0,0,0,0,0,0,0,0,   // 10
	0,0,0,0,0,0,0,0,0,0,  // 20
	0,0,0,0,0,0,4,4,0,0,  // 30
	0,0,0,0,0,0,0,0,0,0,  // 40
	0,0,0,0,0,0,0,0,1,1,  // 50
	1,1,2,2,2,1,1,0,1,0,  // 60
	1,0,2,2,2,3,3,3,2,2,  // 70
	2,0,2,2,0,2,0,2,2,0,  // 80
	0,0,0,0,0,0,0,0,0,2,  // 90
	2,2,2,2,2,0,0,0,2,2,  // 100
	2,2,2,2,2,2,2,2,2,2,  // 110
	4,1,0,0,0,0,0,2,2,3,  // 120
	2,2,2,2,4,4,2,0,3,3,  // 130
	3,3,3,2,2,2,2,2,2,0,  // 140
	0,0,0,0,0,0,0,0,0,0,  // 150
	0,0,0,0,0,0,2,2,2,4,  // 160
	2,2,2,2,0,0,3,2,2,2,// 170
	2,2,7,9,0,0,0,2,0,0,  // 180
	0,0,0,0,0,0,0,0,0,0
};  // 190

unsigned char m_a23t[200] = {
	0,0,0,0,0,0,0,0,0,0,  // 0
	0,0,0,0,0,0,0,0,0,0,   // 10
	0,0,0,0,0,0,0,0,0,0,  // 20
	0,0,0,0,0,0,4,4,0,0,  // 30
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0, // 50
	0,0,1,1,1,0,0,1,0,1,
	0,1,1,1,1,3,3,3,0,0,
	0,1,0,1,1,1,4,1,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,1,1,0,0,0,0,0, //100
	0,0,0,0,0,0,0,1,0,0,
	0,0,0,1,1,0,0,1,1,0,
	0,0,0,0,0,0,0,1,0,0,
	0,0,3,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,  // 150
	0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,0,1,0,1,2,2,
	2,2,0,0,0,0,0,1,0,0,
	0,0,0,0,0,0,0,0,0,0
};  // 190


unsigned char m_type[200] = {
	0,0,0,0,0,0,0,0,0,0,  // 0
	0,0,0,0,0,0,0,0,0,0,   // 10
	0,5,5,5,5,5,5,4,4,4,  // 20
	4,4,4,0,0,0,5,5,6,6,  // 30
	6,6,6,6,6,6,6,6,6,6,  // 40
	6,6,6,6,9,9,9,9,8,8,  // 50
	8,8,8,8,8,8,8,8,8,8,  // 60
	8,8,1,1,1,10,10,10,2,2,  // 70
	2,2,2,1,7,7,7,7,7,6,  // 80
	6,6,6,6,6,6,6,6,6,1,  // 90
	1,2,2,1,1,11,11,11,2,2,  // 100
	2,2,2,2,2,2,2,2,12,12,  // 110
	6,2,2,3,2,8,8,1,1,10,  // 120
	12,12,12,12,2,2,2,2,10,10,  // 130
	10,10,10,12,12,12,12,12,12,6,  // 140
	6,6,6,6,6,9,9,9,9,11,  // 150
	11,11,11,11,11,11,2,2,2,6,  // 160
	1,2,2,2,1,8,10,13,1,1,  // 170
	1,1,14,14,6,6,6,7,0,0,  // 180
	0,0,0,0,0,0,0,0,0,0
};  // 190

unsigned char m_speed[200] = {
	0,4,4,4,4,4,4,4,4,4,  // 0
	4,4,6,4,4,5,8,8,4,4,   // 10
	4,4,4,4,4,4,4,4,5,6,  // 20
	7,4,4,4,4,4,5,8,3,3,  // 30
	4,6,4,4,6,4,4,4,4,4,  // 40
	4,4,4,4,3,4,4,4,4,4,  // 50
	4,4,4,4,9,4,4,4,4,5,  // 60
	4,4,4,4,4,3,3,3,4,4,  // 70
	4,3,8,4,4,4,4,4,4,4,  // 80
	4,7,6,4,4,4,4,6,8,4,  // 90
	4,5,6,4,4,4,4,5,2,3,  // 100
	3,3,3,2,2,4,4,4,4,4,  // 110
	7,4,4,4,3,4,4,4,4,3,  // 120
	4,4,4,4,4,5,5,5,3,3,  // 130
	3,3,3,3,3,4,4,4,4,4,  // 140
	4,4,4,4,4,4,4,4,4,4,  // 150
	4,4,4,7,4,4,4,5,3,4,  // 160
	4,4,3,4,4,3,4,4,4,4,  // 170
	4,4,9,8,3,5,5,6,4,4,  // 180
	4,4,4,4,4,4,4,4,4,4
};  // 190

unsigned char m_mu[200] = {
	0,0,0,0,0,0,0,0,0,0,  // 0
	0,0,0,0,0,0,0,0,0,0,   // 10
	0,0,0,0,0,0,0,2,4,7,  // 20
	7,5,3,0,0,0,0,0,0,0,  // 30
	0,0,0,3,0,0,6,0,0,0,
	4,0,6,0,0,4,0,0,0,0, // 50
	0,0,0,0,0,3,0,0,5,7,
	4,0,0,0,0,0,0,0,0,0,
	0,0,0,4,2,5,0,7,1,0,
	0,0,0,3,0,6,0,0,7,0,
	0,6,0,0,0,1,0,7,0,0, //100
	0,0,0,0,0,0,0,0,0,0,
	0,3,0,0,0,0,0,0,3,0,
	0,0,0,0,0,6,5,4,0,0,
	0,1,0,0,0,0,0,0,0,0,
	0,4,6,0,0,0,0,3,0,0,  // 150
	0,0,0,0,0,7,0,0,0,0,
	5,0,0,0,0,0,0,7,0,0,
	0,0,3,2,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0
};  // 190

unsigned char m_cl[200] = {
	0,0,0,0,0,0,0,0,0,0,  // 0
	0,0,0,0,0,0,0,0,0,0,   // 10
	0,2,4,6,2,4,6,0,0,0,  // 20
	0,0,0,0,0,0,0,0,0,0,  // 30
	0,0,0,3,0,0,0,0,0,4,
	0,7,0,0,0,4,0,0,0,0, // 50
	0,0,0,0,0,0,0,0,0,7,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,2,3,0,7,1,0,
	0,0,0,0,3,0,6,0,7,0,
	0,3,0,0,0,1,0,7,0,0, //100
	0,0,0,0,0,0,0,0,0,0,
	0,5,0,0,0,0,2,0,3,0,
	0,0,0,0,0,0,4,0,0,0,
	0,1,0,0,0,0,0,0,0,0,
	0,5,6,0,0,0,0,3,0,0,  // 150
	0,0,0,5,0,7,0,0,0,0,
	7,0,0,0,0,0,0,7,0,0,
	0,0,0,2,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0
};  // 190

unsigned char m_breath[200] = {
	0,0,0,0,0,0,0,0,0,0,  // 0
	0,0,0,0,0,0,0,0,0,0,   // 10
	0,0,0,0,0,0,0,0,0,0,  // 20
	0,0,0,0,0,0,0,0,0,0,  // 30
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0, // 50
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,4,104,0,0,0,0,0,
	0,0,0,6,0,0,0,8,1,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0, //100
	0,0,0,0,0,0,0,0,0,0,
	0,0,5,0,0,0,0,4,107,0,
	0,0,0,0,0,0,0,7,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,// 150
	7,107,208,9,209,0,0,0,0,0,  
	9,0,0,0,0,0,0,35,0,15,
	115,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0
};  // 190

unsigned char m_poison[200] = {
	0,0,0,0,0,0,0,0,0,0,  // 0
	0,0,0,0,0,0,0,0,0,0,   // 10
	0,0,0,0,0,0,0,0,0,0,  // 20
	0,0,0,0,0,4,0,0,0,2,  // 30
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0, // 50
	0,0,0,0,0,0,0,0,0,0,
	3,0,0,0,0,0,0,2,0,0,
	2,0,0,0,0,0,6,8,1,0,
	0,0,0,0,0,0,0,0,0,0,
	5,0,0,0,0,0,0,0,0,0, //100
	0,0,0,0,0,0,0,0,3,9,
	0,0,0,0,7,0,0,0,0,0,
	0,0,4,0,0,0,7,0,0,0,
	0,0,0,0,0,0,4,0,0,0,
	0,0,0,0,0,0,0,0,0,0,  // 150
	0,0,0,0,0,0,0,6,7,0,
	0,0,0,4,1,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0
};  // 190

unsigned char m_treas[200] = {
	0,0,0,0,0,0,0,0,0,0,  // 0
	0,0,0,2,2,2,2,2,1,1,   // 10
	2,1,2,3,1,2,3,1,2,3,  // 20
	4,2,1,1,1,1,1,2,1,1,  // 30
	1,2,1,2,3,1,2,1,2,2,  // 40
	2,3,3,4,1,2,4,2,0,0,  // 50
	0,0,0,0,0,0,1,2,3,4,  // 60
	0,0,0,2,2,0,0,0,0,0,  // 70
	0,0,0,2,2,2,3,4,2,1,  // 80
	0,2,2,2,2,2,2,3,4,0,  // 90
	0,2,2,0,0,0,0,0,0,0,  // 100
	0,0,0,0,0,0,0,0,0,0,  // 110
	1,0,0,0,0,0,0,2,3,0,  // 120
	0,0,0,0,0,3,3,3,0,0,  // 130
	0,0,0,0,0,0,0,0,0,1,  // 140
	2,2,2,2,4,1,2,2,4,0,  // 150
	0,0,0,0,0,0,0,0,3,2,  // 160
	3,2,1,2,0,0,0,4,2,3,  // 170
	3,0,0,0,1,2,3,0,0,0,  // 180
	0,0,0,0,0,0,0,0,0,0
};  // 190

unsigned char m_abil[200] = {
	0,0,0,0,0,0,0,0,0,0,  // 0
	0,0,3,0,0,0,0,0,0,2,   // 10
	20,0,0,0,0,0,0,0,0,0,  // 20
	0,0,0,0,0,0,0,0,0,0,  // 30
	0,0,2,0,0,0,0,0,3,0,  // 40
	0,0,0,0,4,5,6,5,0,10,  // 50
	0,0,18,18,18,0,16,17,17,17,  // 60
	0,0,0,0,0,0,15,12,0,14,  // 70
	0,27,0,0,0,0,9,0,0,0,  // 80
	0,0,7,0,0,0,0,0,0,0,  // 90
	0,0,9,8,8,13,13,0,0,0,  // 100
	0,0,0,0,0,0,0,0,19,19,  // 110
	21,11,11,12,0,0,0,22,23,23,  // 120
	27,0,24,33,25,26,29,22,27,31,  // 130
	28,0,35,30,30,25,0,25,25,0,  // 140
	0,0,0,0,0,4,5,5,6,0,  // 150
	0,0,33,28,24,9,0,0,29,0,  // 160
	0,34,0,32,0,0,31,13,0,0,  // 170
	0,0,28,29,0,0,0,33,0,0,  // 180
	0,0,0,0,0,0,0,0,0,0
};  // 190

unsigned char m_pict[200] = {
	0,1,2,3,4,5,6,7,8,9,  // 0
	10,11,12,13,14,15,16,17,18,19,   // 10
	20,21,22,22,23,24,24,25,26,27,  // 20
	28,29,30,31,32,4,33,34,35,36,  // 30
	37,38,39,40,41,42,44,46,47,48,  // 40
	49,48,49,50,51,53,55,57,59,60,  // 50
	61,43,63,64,64,62,65,66,67,68,  // 60
	69,70,71,72,73,74,75,74,76,76,  // 70
	76,77,78,79,81,82,83,83,85,86,  // 80
	87,88,89,90,91,92,93,94,95,96,  // 90
	96,97,97,98,98,99,99,100,101,102,  // 100
	103,104,105,106,107,108,116,109,111,111,  // 110
	112,0,0,113,77,62,66,72,114,75,  // 120
	117,118,119,120,77,122,123,125,127,128,  // 130
	129,130,131,135,136,137,136,137,139,140,  // 140
	141,142,143,144,145,51,55,53,55,146,  // 150
	147,148,149,150,151,152,153,154,155,159,  // 160
	160,164,166,168,170,61,74,95,160,160,  // 170
	171,172,173,174,160,3,5,173,143,0,  // 180
	0,0,0,0,0,0,0,0,0,0
};  // 190


unsigned char m_magic_r[200] = {
	0,0,0,0,0,0,0,0,0,0,  // 0
	0,0,1,0,0,0,0,0,0,0,   // 10
	0,1,1,1,1,1,1,1,1,2,  // 20
	2,1,1,0,0,0,0,1,0,0,  // 30
	0,0,0,0,0,0,1,0,0,0,
	0,1,1,0,0,0,0,0,0,0, // 50
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,1,0,0,1,1,0,0,
	0,0,0,0,0,0,0,1,1,0,
	0,2,2,0,0,0,0,2,0,0, //100
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,2,0,0,0,0,0,0,
	0,0,0,0,0,2,1,1,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,1,  // 150
	1,1,1,1,2,2,0,0,1,2,
	1,0,0,1,0,0,1,2,0,0,
	0,0,2,1,0,0,0,2,0,0,
	0,0,0,0,0,0,0,0,0,0
};  // 190

unsigned char m_fire_r[200] = {
	0,0,0,0,0,0,0,0,0,0,  // 0
	0,0,1,0,0,0,0,0,0,0,   // 10
	0,0,0,1,0,0,1,1,1,1,  // 20
	2,0,0,0,0,0,0,1,0,0,  // 30
	0,0,0,0,0,0,1,0,0,0,
	0,1,1,0,0,0,0,0,0,2, // 50
	0,0,0,0,0,0,0,0,0,1,
	0,0,0,2,0,0,0,0,0,0,
	0,0,0,2,2,2,2,2,2,0,
	0,0,0,0,0,1,1,1,1,0,
	0,1,1,0,0,0,0,2,0,0, //100
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,2,0,0,0,2,0,0,
	0,0,0,0,0,2,1,2,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,  // 150
	2,0,0,1,2,2,0,0,0,0,
	2,0,0,0,0,0,1,2,0,2,
	0,0,2,1,0,0,0,1,0,0,
	0,0,0,0,0,0,0,0,0,0
};  // 190

unsigned char m_cold_r[200] = {
	0,0,0,0,0,0,0,0,0,0,  // 0
	0,0,1,0,0,0,0,0,0,0,   // 10
	0,0,0,1,0,0,1,0,1,1,  // 20
	2,0,0,0,0,0,0,1,0,0,  // 30
	0,0,0,0,0,0,1,0,0,0,
	0,1,1,0,0,0,0,0,2,2, // 50
	2,2,2,2,2,2,2,2,2,2,
	2,2,0,0,2,0,2,0,0,0,
	0,1,0,0,0,0,1,1,0,0,
	0,0,0,0,0,1,1,1,1,0,
	0,1,1,0,0,0,0,2,0,0, //100
	0,0,0,0,0,0,0,0,0,0,
	0,2,2,2,0,2,2,0,2,2,
	1,1,1,0,0,2,1,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,  // 150
	0,2,0,1,2,2,0,0,0,0,
	0,0,0,0,0,2,0,1,0,0,
	2,0,2,1,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0
};  // 190

unsigned char m_poison_r[200] = {
	0,0,0,0,0,0,0,0,0,0,  // 0
	0,0,1,0,0,0,0,0,0,0,   // 10
	0,0,0,0,0,0,0,0,0,0,  // 20
	1,0,0,0,0,0,1,1,0,0,  // 30
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,2,2,2, // 50
	2,2,2,2,2,2,2,2,2,2,
	2,2,0,0,0,2,2,2,0,0,
	2,2,0,0,2,2,2,2,2,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,2,2,2,0,0, //100
	0,0,0,0,0,0,0,0,0,0,
	0,2,2,2,2,2,2,0,0,0,
	0,0,0,0,2,1,1,2,2,2,
	2,2,2,1,1,1,1,0,0,0,
	0,0,0,0,0,0,0,0,0,2,  // 150
	2,2,2,1,2,2,0,0,0,0,
	1,1,0,0,0,2,2,1,0,0,
	0,0,2,2,0,0,0,2,0,0,
	0,0,0,0,0,0,0,0,0,0
};  // 190

short ter_pics[256] = {
	0,1,2,3,4,5,6,6,7,8,
	8,9,10,10,11,12,12,13,14,15,
	16,17,18,19,20,21,22,23,24,25,
	26,27,28,29,30,31,32,33,34,35,
	36,37,38,39,40,41,42,43,44,45,
	46,47,48,49,50,51,52,53,54,55,
	56,57,58,59,60,61,62,63,64,65,
	66,400,401,402,403,404,404,405,406,202,
	203,204,215,216,67,68,69,85,86,87,
	74,70,71,72,73,75,76,83,84,121,
	122,178,179,180,411,182,183,184,185,186,
	79,80,81,82,83,84,198,199,181,205,
	206,227,88,88,89,90,90,90,90,91,
	92,93,94,95,96,88,97,98,99,100,
	100,101,102,102,102,102,103,104,105,106,
	107,108,100,109,110,110,111,112,112,112,
	112,113,114,115,116,117,118,110,119,120,
	123,123,123,124,125,126,127,128,129,150,
	151,152,146,147,148,149,130,131,132,133,
	134,135,136,137,138,139,140,141,142,143,
	144,145,153,154,155,156,412,413,213,214,
	157,157,158,159,160,161,162,163,164,165,
	166,167,168,169,170,171,172,173,174,175,
	176,177,187,188,189,190,2,192,193,195,
	194,196,197,191,200,201,207,208,209,210,
	211,212,407,408,409,410
};

short ter_block[256] = {
	0,0,0,0,0,5,5,1,5,5,
	1,5,5,1,5,5,1,5,5,5,
	5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	
	3,3,3,3,3,3,3,3,3,3,
	3,3,3,0,3,0,0,0,0,0,
	0,3,0,0,3,2,2,4,2,0,
	0,0,0,0,0,0,0,0,0,0,
	3,4,0,0,0,4,0,4,0,4,
	
	4,4,0,0,4,0,4,4,0,4,
	4,0,4,4,0,0,5,5,0,4,
	4,4,5,5,1,5,5,5,5,0,
	4,0,5,4,5,5,5,5,4,5,
	5,1,5,5,5,5,0,4,0,5,
	
	5,5,5,4,5,5,1,5,5,5,
	5,0,4,0,5,5,5,5,5,4,
	0,2,2,0,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,
	4,5,4,4,4,0,0,0,0,0,
	
	0,4,4,4,0,4,0,0,2,2,
	0,2,4,4,4,0,4,0,4,0,
	0,0,4,4,0,0,4,4,4,4,
	4,4,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,2,2,2,2,
	2,2,0,0,0,0
};

short ter_traits[256] = {
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,2,0,15,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,5,0,0,0,
	
	0,14,14,0,0,0,11,11,0,0,
	0,5,0,0,0,0,0,0,0,0,
	0,11,0,1,0,1,10,10,10,22,
	9,8,11,7,7,7,0,0,0,0,
	1,0,1,10,10,10,22,9,8,11,
	
	7,7,7,0,0,1,0,1,10,10,
	10,22,9,8,11,7,7,7,0,0,
	0,20,4,20,0,14,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,14,0,0,14,0,0,0,0,0,
	
	0,14,0,0,0,0,0,0,0,0,			
	0,20,0,0,0,0,0,0,0,0,
	0,14,0,14,20,0,0,0,0,0,
	0,0,21,21,21,21,21,21,21,21,
	21,21,21,21,21,21,0,0,0,0,
	0,0,16,17,18,19
};

cMonster return_monster_template(unsigned char store) {
	cMonster monst;
	short m_num,i;
	
	m_num = store;
	monst.m_num = m_num;
	if (m_num >= 189)
		m_num = 189;
	monst.level = m_level[m_num];
	monst.m_health = m_health[m_num];
	monst.health = monst.m_health;
	monst.max_mp = 0;
	monst.mp = monst.max_mp;
	monst.armor = m_armor[m_num];
	monst.skill = m_skill[m_num];
	monst.a[0] = m_a1[m_num];
	monst.a[1] = m_a2[m_num];
	monst.a[2] = m_a3[m_num];
	if (monst.a[0] > 0)
		monst.a[0] += 100;
	if (monst.a[1] > 0)
		monst.a[1] += 100;
	if (monst.a[2] > 0)
		monst.a[2] += 100;
	monst.a1_type = m_a1t[m_num];
	monst.a23_type = m_a23t[m_num];
	monst.m_type = (eMonsterType) m_type[m_num];
	monst.speed = m_speed[m_num];
	monst.ap = 0;
	monst.mu = m_mu[m_num];
	monst.cl = m_cl[m_num];
	if ((monst.mu > 0) || (monst.cl > 0))
		monst.max_mp = monst.mp = 14 * m_level[m_num];
	monst.breath = m_breath[m_num] % 100;
	monst.breath_type = m_breath[m_num] / 100;
	monst.treasure = m_treas[m_num];
	monst.spec_skill = m_abil[m_num];
	monst.m_morale = 10 * m_level[m_num];
	if (m_level[m_num] >= 20)
		monst.m_morale += 10 * (m_level[m_num] - 20);
	
	monst.morale = monst.m_morale;
	monst.poison = m_poison[m_num];
	monst.picture_num =  m_pict[m_num];
	monst.direction = 0;
	monst.corpse_item = -1;
	monst.corpse_item_chance = -1;
	for (i = 0; i < 15; i++)
		monst.status[i] = 0;
	monst.x_width = monst.y_width = 1;
	for (i = 0; i < 200; i++)
		if (m_pict[m_num] == m_pic_index[i].i) {
			monst.picture_num = i;
			monst.x_width = m_pic_index[i].x;
			monst.y_width = m_pic_index[i].y;
			i = 200;
		}
	//	monst.x_width = xdim[m_num];
	//	monst.y_width = ydim[m_num];
	monst.radiate_1 = 0;
	monst.radiate_2 = 0;
	monst.default_attitude = 0;
	monst.summon_type = 0;
	monst.default_facial_pic = 0;
	monst.res1 = 0;
	monst.res2 = 0;
	monst.res3 = 0;
	
	monst.immunities = 0;
	if (m_magic_r[m_num] == 1)
		monst.immunities = monst.immunities | 1;
	if (m_magic_r[m_num] == 2)
		monst.immunities = monst.immunities | 2;
	if (m_fire_r[m_num] == 1)
		monst.immunities = monst.immunities | 4;
	if (m_fire_r[m_num] == 2)
		monst.immunities = monst.immunities | 8;
	if (m_cold_r[m_num] == 1)
		monst.immunities = monst.immunities | 16;
	if (m_cold_r[m_num] == 2)
		monst.immunities = monst.immunities | 32;
	if (m_poison_r[m_num] == 1)
		monst.immunities = monst.immunities | 64;
	if (m_poison_r[m_num] == 2)
		monst.immunities = monst.immunities | 128;
	
	return monst;
}

//item_record_type convert_item (short_item_record_type s_item) {
//	item_record_type i;
//	location l = {0,0};
//	short temp_val;
//	
//	i.variety = (short) s_item.variety;
//	i.item_level = (short) s_item.item_level;
//	i.awkward = (short) s_item.awkward;
//	i.bonus = (short) s_item.bonus;
//	i.protection = (short) s_item.protection;
//	i.charges = (short) s_item.charges;
//	i.type = (short) s_item.type;
//	i.graphic_num = (short) s_item.graphic_num;
//	if (i.graphic_num >= 25)
//		i.graphic_num += 20;
//	i.ability = (short) s_item.real_abil;
//	i.type_flag = (short) s_item.type_flag;
//	i.is_special = (short) s_item.is_special;
//	i.value = (short) s_item.value;
//	i.weight = s_item.weight;
//	i.special_class = 0;
//	i.item_loc = l;
//	strcpy((char *)i.full_name,(char *)s_item.full_name);
//	strcpy((char *)i.name,(char *)s_item.name);
//
//	if (i.charges > 0)
//		temp_val = i.value * i.charges;
//		else temp_val = i.value;
//	if (temp_val >= 15)
//		i.treas_class = 1;
//	if (temp_val >= 100)
//		i.treas_class = 2;
//	if (temp_val >= 900)
//		i.treas_class = 3;
//	if (temp_val >= 2400)
//		i.treas_class = 4;
//		
//	i.magic_use_type = s_item.magic_use_type;
//	i.ability_strength = s_item.ability_strength;
//	i.reserved1 = 0;
//	i.reserved2 = 0;
//	i.item_properties = 0;
//	if (s_item.identified == true)
//		i.item_properties = i.item_properties | 1;
//	if ((s_item.ability == 14) || (s_item.ability == 129) || (s_item.ability == 95))
//		i.item_properties = i.item_properties | 16;
//	if (s_item.magic == true)
//		i.item_properties = i.item_properties | 4;
//
//	return i;
//}

void init_scenario() {
	short i;
	rectangle dummy_rect;
	Str255 temp_str;
	cVehicle null_boat;// = {{0,0},{0,0},{0,0},-1,false,false};
	cVehicle null_horse;// = {{0,0},{0,0},{0,0},-1,false,false};
	cScenario::cItemStorage null_item_s;// ={-1,{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{0,0,0,0,0,0,0,0,0,0},0};
	short j,item_to_hide[13] = {214,233,270,271,280,281,286,290,291,297,304,311,348};
	
	scenario.format.ver[0] = 1;
	scenario.format.ver[1] = 0;
	scenario.format.ver[2] = 0;
	scenario.format.min_run_ver = 1;
	scenario.format.prog_make_ver[0] = 1;
	scenario.format.prog_make_ver[1] = 0;
	scenario.format.prog_make_ver[2] = 0;
	scenario.num_towns = 1;
	scenario.out_width = 1;
	scenario.out_height = 1;
	scenario.difficulty = 0;
	scenario.intro_pic = 0;
	scenario.default_ground = 1;
	for (i = 0; i < 200; i++) {
		scenario.town_size[i] = 1;
		scenario.town_hidden[i] = 0;
		scenario.town_data_size[i][0] = 0;
		scenario.town_data_size[i][1] = 0;
		scenario.town_data_size[i][2] = 0;
		scenario.town_data_size[i][3] = 0;
		scenario.town_data_size[i][4] = 0;
	}
	scenario.intro_mess_len = 0;
	scenario.where_start.x = 24;
	scenario.where_start.y = 24;
	scenario.out_sec_start.x = 0;
	scenario.out_sec_start.y = 0;
	scenario.out_start = scenario.where_start;
	scenario.which_town_start = 0;
	for (i = 0; i < 10; i++) {
		scenario.town_to_add_to[i] = -1;
		scenario.flag_to_add_to_town[i][0] = 0;
		scenario.flag_to_add_to_town[i][1] = 0;
	}
	for (i = 0; i < 100; i++) {
		scenario.out_data_size[i][0] = 0;
		scenario.out_data_size[i][1] = 0;
	}
	for (i = 0; i < 3; i++) {
		scenario.store_item_rects[i] = dummy_rect;
		scenario.store_item_towns[i] = -1;
	}
	for (i = 0; i < 50; i++) {
		scenario.special_items[i] = 0;
		scenario.special_item_special[i] = -1;
	}
	scenario.rating = 0;
	scenario.uses_custom_graphics = 0;
	for (i = 0; i < 256; i++) {
		scenario.scen_monsters[i] = return_monster_template(i);
		get_str(temp_str,2,i);
		if ((i > 187) || (i == 0))
			scenario.scen_monsters[i].m_name = "Unused";
		else scenario.scen_monsters[i].m_name = (char*)temp_str;
		if (i == 0)
			scenario.scen_monsters[i].m_name = "Empty";
	}
	for (i = 0; i < 30; i++) {
		scenario.boats[i] = null_boat;
		scenario.horses[i] = null_horse;
	}
	for (i = 0; i < 256; i++) {
		scenario.ter_types[i].picture = ter_pics[i];
		scenario.ter_types[i].blockage = ter_block[i];
		scenario.ter_types[i].special = (eTerSpec) ter_traits[i]; // ???: What on earth is all this for?
		get_str(temp_str,1,i + 1);
		sprintf((char *)scenario.ter_types[i].name.c_str(), "%s", temp_str);
		
		scenario.scen_specials[i] = null_spec_node;
	}
	for (i = 0; i < 20; i++) {
		scenario.scenario_timer_times[i] = 0;
		scenario.scenario_timer_specs[i] = -1;
	}
	for (i = 0; i < 10; i++) {
		scenario.storage_shortcuts[i] = null_item_s;
	}
	scenario.last_out_edited.x = 0;
	scenario.last_out_edited.y = 0;
	scenario.last_town_edited = 0;
	for (i = 0; i < 400; i++) {
		scenario.scen_items[i] = item_list[i]; //convert_item(item_list[i]);
		for (j = 0; j < 13; j++)
			if (i == item_to_hide[j]) 
				scenario.scen_items[i].item_properties =
				scenario.scen_items[i].item_properties | 32;
	}
	for (i = 0; i < 270; i++) {
		get_str(temp_str,35,i + 1);
		sprintf((char *)scenario.scen_strs(i), "%s", temp_str);
		scenario.scen_str_len[i] = strlen((char *) scenario.scen_strs(i));
	}
}	
void put_ter_info_in_dlog() {
	Str255 str;
	strcpy((char*)str,(char*)scenario.ter_types[store_which_ter].name.c_str());
	
	cdsin(813,12,store_which_ter);
	CDST(813,2,(char*)str);
	CDSN(813,5,store_ter.picture);
	cd_set_led_range(813,19,24,store_ter.blockage);
	cd_set_led(813,25,store_ter.fly_over);
	cd_set_led(813,26,store_ter.boat_over);
	cd_set_led(813,27,store_ter.block_horse);
	cd_set_led_range(813,28,31,store_ter.step_sound);
	str[0] = store_ter.shortcut_key;
	str[1] = 0;
	CDST(813,3,(char *) str);
	CDSN(813,4,store_ter.trans_to_what);
	CDSN(813,8,store_ter.light_radius);
	cd_set_led_range(813,32,55,store_ter.special);
	CDSN(813,6,store_ter.flag1);
	CDSN(813,7,store_ter.flag2);
	get_str(str,21,40 + store_ter.special);
	csit(813,67,(char *) str);
	get_str(str,21,80 + store_ter.special);
	csit(813,68,(char *) str);
	if (store_ter.picture >= 1000)
		csp(813,14,store_ter.picture % 1000,PICT_CUSTOM + PICT_TER);
	else if (store_ter.picture >= 400)
		csp(813,14,store_ter.picture - 400,PICT_TER_ANIM);
	else csp(813,14,store_ter.picture,PICT_TER);
}

bool save_ter_info() {
	Str255 str;
	short i;
	
	
	store_ter.picture = CDGN(813,5);
	if  ((store_ter.picture < 0) ||
		 ((store_ter.picture > PICT_N_TER) && (store_ter.picture < 400)) ||
		 ((store_ter.picture > 400 + PICT_N_TER_ANIM) && (store_ter.picture < 1000))) {
		give_error("Picture number isn't in the proper range.","",813);
		return false;
	}
	
	if (store_which_ter > 90) {
		store_ter.blockage = cd_get_led_range(813,19,24);
		store_ter.special = (eTerSpec) cd_get_led_range(813,32,55);
		i = CDGN(813,6);
		/*if ((store_ter.special < 2) || (store_ter.special > 6)) {
			if (cre(i,0,256,"First special flag must be from 0 to 255.","",813) == true) return false;
		}
		else*/ if (store_ter.special == TER_SPEC_DAMAGING) {
			if (cre(i,0,256,"First special flag must be from 0 to 100.","",813) == true) return false;
		}
		else if (store_ter.special == TER_SPEC_DANGEROUS) {
			if (cre(i,0,256,"First special flag must be from 0 to 8.","",813) == true) return false;
		}
		store_ter.flag1 = CDGN(813,6);
		
		i = CDGN(813,7);
		if (store_ter.special == TER_SPEC_TOWN_ENTRANCE) {
			if (cre(i,0,256,"Second special flag must be from 0 to 200.","",813) == true) return false;
		}
		else if ((store_ter.special == TER_SPEC_DAMAGING) || (store_ter.special == TER_SPEC_DANGEROUS)) {
			if (cre(i,0,256,"Second special flag must be from 0 to 100.","",813) == true) return false;
		}
		store_ter.flag2 = CDGN(813,7);
		
		i = CDGN(813,4);
		if (cre(i,0,255,"Transform To What must be from 0 to 255.","",813) == true) return false;
		store_ter.trans_to_what = CDGN(813,4);
		store_ter.fly_over = cd_get_led(813,25);
		store_ter.boat_over = cd_get_led(813,26);
		store_ter.block_horse = cd_get_led(813,27);
		store_ter.light_radius = CDGN(813,8);
		if (cre(store_ter.light_radius,0,8,"Light radius must be from 0 to 8.","",813) == true) return false;
		
		store_ter.step_sound = cd_get_led_range(813,28,31);
	}
	CDGT(813,3,(char *) str);
	store_ter.shortcut_key = str[0];
	
	CDGT(813,2,(char *) str);
	str[29] = 0;
	scenario.ter_types[store_which_ter].name = (char*)str;
	
	scenario.ter_types[store_which_ter] = store_ter;
	return true;
}

void edit_ter_type_event_filter (short item_hit) {
	Str255 str;
	short i;
	
	switch (item_hit) {
		case 9:
			toast_dialog();
			break;
		case 62:
			if (save_ter_info() == true)
				toast_dialog();
			break;
		case 10:
			if (save_ter_info() == false) break;
			store_which_ter--;
			if (store_which_ter < 0) store_which_ter = 255;
			store_ter = scenario.ter_types[store_which_ter];
			put_ter_info_in_dlog();
			break;
		case 11:
			if (save_ter_info() == false) break;
			store_which_ter++;
			if (store_which_ter > 255) store_which_ter = 0;
			store_ter = scenario.ter_types[store_which_ter];
			put_ter_info_in_dlog();
			break;
			
		case 13: case 70:
			if (item_hit == 13)
				i = choose_graphic(0,PICT_N_TER,store_ter.picture,PICT_TER,813);
			else i = 400 + choose_graphic(/*300,313*/0,PICT_N_TER_ANIM,store_ter.picture,PICT_TER_ANIM,813);
			if (i >= 0) {
				//if (i >= 300) i += 100;
				store_ter.picture = i;
			}
			else break;
			CDSN(813,5,store_ter.picture);
			if (store_ter.picture >= 1000)
				csp(813,14,store_ter.picture % 1000,PICT_CUSTOM + PICT_TER);
			else if (store_ter.picture >= 400)
				csp(813,14,store_ter.picture - 400,PICT_TER_ANIM);
			else csp(813,14,store_ter.picture,PICT_TER);
			break;
		default:
			cd_hit_led_range(813,32,55,item_hit);
			cd_hit_led_range(813,28,31,item_hit);
			cd_hit_led_range(813,19,24,item_hit);
			cd_flip_led(813,25,item_hit);
			cd_flip_led(813,26,item_hit);
			cd_flip_led(813,27,item_hit);
			if ((item_hit >= 32) && (item_hit <= 55)) {
				get_str(str,21,40 + item_hit - 32);
				csit(813,67,(char *) str);
				get_str(str,21,80 + item_hit - 32);
				csit(813,68,(char *) str);
			}
			break;
			
	}
}

short edit_ter_type(short which_ter) {
	// ignore parent in Mac version
	short item_hit,i,store_dialog_answer;
	Str255 temp_str;
	char *blocked_strs[6] = {"Clear","Walk through, Opaque","Clear, Special","Clear, Blocked","Blocked, Obstructed",
	"Blocked, Opaque"};
	char *sound_strs[4] = {"Footstep","Squish","Crunch","Silence"};
	
	store_which_ter = which_ter;
	store_ter = scenario.ter_types[which_ter];
	//make_cursor_sword();
	
	cd_create_dialog_parent_num(813,0);
	printf("Created dialog.");
	put_ter_info_in_dlog();
	for (i = 0; i < 24; i++) {
		get_str(temp_str,21,i + 1);
		cd_add_label(813,32 + i,(char *) temp_str,57);
	}
	for (i = 0; i < 4; i++) 
		cd_add_label(813,28 + i,sound_strs[i],35);
	for (i = 0; i < 6; i++) 
		cd_add_label(813,19 + i,blocked_strs[i],57);
	cd_attach_key(813,10,0);
	cd_attach_key(813,11,0);
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(813);
	return 0;
}

void put_monst_info_in_dlog() {
	Str255 str;
	
	if (store_monst.picture_num < 1000)
		csp(814,34,/*400 + */store_monst.picture_num,PICT_MONST);
	else {
		short type_g = PICT_CUSTOM + PICT_MONST;
		short size_g = store_monst.picture_num / 1000;
		switch(size_g){
			case 2:
				type_g += PICT_WIDE_MONSTER;
				break;
			case 3:
				type_g += PICT_TALL_MONSTER;
				break;
			case 4:
				type_g += PICT_WIDE_MONSTER + PICT_TALL_MONSTER;
				break;
		}
		csp(814,34,store_monst.picture_num % 1000,type_g);
	}
	cdsin(814,33,store_which_monst);
	CDST(814,2,(char*)scenario.scen_monsters[store_which_monst].m_name.c_str());
	CDSN(814,3,store_monst.picture_num);
	sprintf((char *) str,"Width = %d",store_monst.x_width);
	csit(814,40,(char *) str);
	sprintf((char *) str,"Height = %d",store_monst.y_width);
	csit(814,41,(char *) str);
	CDSN(814,4,store_monst.level);
	CDSN(814,5,store_monst.health);
	CDSN(814,6,store_monst.armor);
	CDSN(814,7,store_monst.skill);
	CDSN(814,8,store_monst.speed);
	CDSN(814,9,store_monst.mu);
	CDSN(814,10,store_monst.cl);
	CDSN(814,11,store_monst.a[0] / 100);
	CDSN(814,12,store_monst.a[0] % 100);
	CDSN(814,13,store_monst.a[1] / 100);
	CDSN(814,14,store_monst.a[1] % 100);
	CDSN(814,15,store_monst.a[2] / 100);
	CDSN(814,16,store_monst.a[2] % 100);
	CDSN(814,17,store_monst.default_facial_pic);
	CDSN(814,18,store_monst.treasure);
	
	cd_set_led_range(814,29,32,store_monst.default_attitude);
	
	get_str(str,20,150 + store_monst.m_type);
	csit(814,42,(char *) str);
	get_str(str,20,130 + store_monst.a1_type);
	csit(814,43,(char *) str);
	get_str(str,20,130 + store_monst.a23_type);
	csit(814,44,(char *) str);
}

bool save_monst_info() {
	Str255 str;
	short i;
	
	CDGT(814,2,(char *) str);
	str[19] = 0;
	scenario.scen_monsters[store_which_monst].m_name = (char*)str;
	//CDGT(814,2,data_store->scen_item_list.monst_names[store_which_monst]);
	store_monst.picture_num = CDGN(814,3);
	if (cre(store_monst.picture_num,0,5000,"Monster pic must be from 0 to 5000.","",814) > 0) return false;
	// later check pic num for error, and assign widths if custom
	if (store_monst.picture_num >= 1000) {
		if ((store_monst.picture_num >= 1000) && (store_monst.picture_num < 2000)) {
			store_monst.x_width = 1;
			store_monst.y_width = 1;
		}
		if ((store_monst.picture_num >= 2000) && (store_monst.picture_num < 3000)) {
			store_monst.x_width = 2;
			store_monst.y_width = 1;
		}
		if ((store_monst.picture_num >= 3000) && (store_monst.picture_num < 4000)) {
			store_monst.x_width = 1;
			store_monst.y_width = 2;
		}
		if ((store_monst.picture_num >= 4000) && (store_monst.picture_num < 5000)) {
			store_monst.x_width = 2;
			store_monst.y_width = 2;
		}
	}
	else {
		if (cre(store_monst.picture_num,0,174,"Non-customized monster pic must be from 0 to 173.","",814) > 0) return false;
		store_monst.x_width = m_pic_index[store_monst.picture_num].x;
		store_monst.y_width = m_pic_index[store_monst.picture_num].y;
		
	}
	store_monst.level = CDGN(814,4);
	if (cre(store_monst.level,0,40,"Level must be from 0 to 40.","",814) > 0) return false;
	store_monst.m_health = store_monst.health = CDGN(814,5);
	if (cre(store_monst.health,0,2500,"Health must be from 0 to 2500.","",814) > 0) return false;
	store_monst.armor = CDGN(814,6);
	if (cre(store_monst.armor,0,50,"Armor must be from 0 to 50.","",814) > 0) return false;
	store_monst.skill = CDGN(814,7);
	if (cre(store_monst.skill,0,40,"Skill must be from 0 to 40.","",814) > 0) return false;
	store_monst.speed = CDGN(814,8);
	if (cre(store_monst.speed,1,12,"Speed must be from 2 to 12.","",814) > 0) return false;
	store_monst.mu = CDGN(814,9);
	if (cre(store_monst.mu,0,7,"Magic Spells must be from 0 to 7.","",814) > 0) return false;
	store_monst.cl = CDGN(814,10);
	if (cre(store_monst.cl,0,7,"Priest Spells must be from 0 to 7.","",814) > 0) return false;
	
	i = CDGN(814,11);
	if (cre(i,0,20,"Attack 1 number of dice must be from 0 to 20 (0 means no attack).","",814) > 0) return false;
	i = CDGN(814,13);
	if (cre(i,0,20,"Attack 2 number of dice must be from 0 to 20 (0 means no attack).","",814) > 0) return false;
	i = CDGN(814,15);
	if (cre(i,0,20,"Attack 3 number of dice must be from 0 to 20 (0 means no attack).","",814) > 0) return false;
	i = CDGN(814,12);
	if (cre(i,0,50,"Attack 1 damage per die must be from 1 to 50.","",814) > 0) return false;
	i = CDGN(814,14);
	if (cre(i,0,50,"Attack 2 damage per die must be from 1 to 50.","",814) > 0) return false;
	i = CDGN(814,16);
	if (cre(i,0,50,"Attack 3 damage per die must be from 1 to 50.","",814) > 0) return false;
	
	
	store_monst.a[0] = CDGN(814,11) * 100 + CDGN(814,12);
	store_monst.a[1] = CDGN(814,13) * 100 + CDGN(814,14);
	store_monst.a[2] = CDGN(814,15) * 100 + CDGN(814,16);
	
	store_monst.default_facial_pic = CDGN(814,17); // later
	store_monst.treasure = CDGN(814,18);
	if (cre(store_monst.treasure,0,4,"Treasure must be from 0 to 4.","",814) > 0) return false;
	store_monst.default_attitude = cd_get_led_range(814,29,32);
	
	return true;
}

void edit_monst_type_event_filter (short item_hit) {
	Str255 str;
	short i;
	cMonster temp_monst;
	
	switch (item_hit) {
		case 20:
			toast_dialog();
			break;
		case 19:
			if (save_monst_info() == true) {
				scenario.scen_monsters[store_which_monst] = store_monst;
				toast_dialog();
			}
			break;
		case 23: // abils
			if (save_monst_info() == false) break;
			temp_monst = edit_monst_abil(store_monst,814);
			if ((temp_monst.level >= 0) && (temp_monst.level < 255))
				store_monst = temp_monst;
			put_monst_info_in_dlog();
			break;
		case 21:
			if (save_monst_info() == false) break;
			scenario.scen_monsters[store_which_monst] = store_monst;
			store_which_monst--;
			if (store_which_monst < 1) store_which_monst = 255;
			store_monst = scenario.scen_monsters[store_which_monst];
			put_monst_info_in_dlog();
			break;
		case 22:
			if (save_monst_info() == false) break;
			scenario.scen_monsters[store_which_monst] = store_monst;
			store_which_monst++;
			if (store_which_monst > 255) store_which_monst = 1;
			store_monst = scenario.scen_monsters[store_which_monst];
			put_monst_info_in_dlog();
			break;
		case 24: // picture
			if (save_monst_info() == false) break;
			i = choose_graphic(/*400,572*/0,PICT_N_MONST,store_monst.picture_num/* + 400*/,PICT_MONST,814);
			if (i >= 0) {
				store_monst.picture_num = i - 400;
			}
			else break;
			store_monst.x_width = m_pic_index[i].x;
			store_monst.y_width = m_pic_index[i].y;
			put_monst_info_in_dlog();
			break;
		case 25: // m type
			if (save_monst_info() == false) break;
			i = choose_text_res(20,150,164,store_monst.m_type + 150,814,"Choose Monster Type:");
			if (i >= 0) {
				i -= 150;
				store_monst.m_type = (eMonsterType) i;
				put_monst_info_in_dlog();
			}
			break;
		case 26: // att type 1
			if (save_monst_info() == false) break;
			i = choose_text_res(20,130,139,store_monst.a1_type + 130,814,"Choose Attack 1 Type:");
			if (i >= 0) {
				store_monst.a1_type = i - 130;
				put_monst_info_in_dlog();
			}
			break;
		case 27: // att type 23
			if (save_monst_info() == false) break;
			i = choose_text_res(20,130,139,store_monst.a23_type + 130,814,"Choose Attack 2 & 3 Type:");
			if (i >= 0) {
				store_monst.a23_type = i - 130;
				put_monst_info_in_dlog();
			}
			break;
		case 28: // talk pic
			if (save_monst_info() == false) break;
			i = choose_graphic(/*1001,1084*/1,1+PICT_N_TALK,store_monst.default_facial_pic/* + 1000*/,PICT_TALK,814);
			if (i >= 0) {
				store_monst.default_facial_pic = i - 1000;
			}
			else break;
			put_monst_info_in_dlog();
			break;
		default:
			cd_hit_led_range(814,29,32,item_hit);
			break;
			
	}
}

short edit_monst_type(short which_monst) {
	// ignore parent in Mac version
	short item_hit,i,store_dialog_answer;
	Str255 temp_str;
	char *attitude[4] = {"Friendly, Docile","Hostile, Type A","Friendly, Will Fight","Hostile, Type B"};
	
	store_which_monst = which_monst;
	store_monst = scenario.scen_monsters[which_monst];
	//make_cursor_sword();
	
	cd_create_dialog_parent_num(814,0);
	
	put_monst_info_in_dlog();
	
	for (i = 0; i < 4; i++) 
		cd_add_label(814,29 + i,attitude[i],57);
	cd_attach_key(814,21,0);
	cd_attach_key(814,22,0);
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(814);
	
	return 0;
}

void put_monst_abils_in_dlog() {
	Str255 str;
	short i;
	
	cdsin(815,24,store_which_monst);
	
	
	CDSN(815,2,store_monst2.poison);
	CDSN(815,3,store_monst2.breath);
	
	get_str(str,20,store_monst2.spec_skill + 1);
	csit(815,30,(char *) str);
	get_str(str,20,store_monst2.radiate_1 + 50);
	csit(815,33,(char *) str);
	get_str(str,20,store_monst2.radiate_1 + 80);
	csit(815,36,(char *) str);
	CDSN(815,4,store_monst2.radiate_2);
	CDSN(815,5,store_monst2.corpse_item);
	CDSN(815,6,store_monst2.corpse_item_chance);
 	cd_set_led_range(815,9,12,store_monst2.breath_type);
 	cd_set_led_range(815,41,44,store_monst2.summon_type);
 	for (i = 0; i < 8; i++)
 		cd_set_led(815,13 + i,0);
 	if (store_monst2.immunities & 1 ) cd_set_led(815,13,1);
 	if (store_monst2.immunities & 2 ) cd_set_led(815,14,1);
 	if (store_monst2.immunities & 4 ) cd_set_led(815,15,1);
 	if (store_monst2.immunities & 8 ) cd_set_led(815,16,1);
 	if (store_monst2.immunities & 16 ) cd_set_led(815,17,1);
 	if (store_monst2.immunities & 32 ) cd_set_led(815,18,1);
 	if (store_monst2.immunities & 64 ) cd_set_led(815,19,1);
 	if (store_monst2.immunities & 128 ) cd_set_led(815,20,1);
}

bool save_monst_abils() {
	Str255 str;
	short i;
	
 	store_monst2.poison = CDGN(815,2);
 	if (cre(store_monst2.poison,0,8,"Poison must be from 0 to 8.","",815) > 0) return false;
	store_monst2.breath = CDGN(815,3);
  	if (cre(store_monst2.poison,0,40,"Breath Damage must be from 0 to 4.","",815) > 0) return false;
	store_monst2.breath_type = cd_get_led_range(815,9,12);
	store_monst2.summon_type = cd_get_led_range(815,41,44);
 	store_monst2.radiate_2 = CDGN(815,4);
 	if ((store_monst2.radiate_1 >= 1) && (store_monst2.radiate_1 <= 6))
   		if (cre(store_monst2.radiate_2,0,100,"Radiation Chance must be from 0 to 100.","",815) > 0) return false;
 	if ((store_monst2.radiate_1 >= 10) && (store_monst2.radiate_1 <= 12))
   		if (cre(store_monst2.radiate_2,0,255,"Summoned Monster must be from 0 to 255.","",815) > 0) return false;
	
 	store_monst2.corpse_item = CDGN(815,5);
  	if (cre(store_monst2.corpse_item,-1,399,"Item To Drop must be from 0 to 399 (or -1 for no item).","",815) > 0) return false;
	store_monst2.corpse_item_chance = CDGN(815,6);
  	if (cre(store_monst2.corpse_item_chance,-1,100,"Dropping Chance must be from 0 to 100 (or -1 for no item).","",815) > 0) return false;
	store_monst2.immunities = 0;
	if (cd_get_led(815,13) == 1) store_monst2.immunities = store_monst2.immunities | 1;
	if (cd_get_led(815,14) == 1) store_monst2.immunities = store_monst2.immunities | 2;
	if (cd_get_led(815,15) == 1) store_monst2.immunities = store_monst2.immunities | 4;
	if (cd_get_led(815,16) == 1) store_monst2.immunities = store_monst2.immunities | 8;
	if (cd_get_led(815,17) == 1) store_monst2.immunities = store_monst2.immunities | 16;
	if (cd_get_led(815,18) == 1) store_monst2.immunities = store_monst2.immunities | 32;
	if (cd_get_led(815,19) == 1) store_monst2.immunities = store_monst2.immunities | 64;
	if (cd_get_led(815,20) == 1) store_monst2.immunities = store_monst2.immunities | 128;
	return true;
}

void edit_monst_abil_event_filter (short item_hit) {
	Str255 str;
	short i;
	
	switch (item_hit) {
		case 8:
			store_monst2.level = -1;
			toast_dialog();
			break;
		case 7:
			if (save_monst_abils() == true)
				toast_dialog();
			break;
		case 31: // abils
			if (save_monst_abils() == false) break;
			i = choose_text_res(20,1,38,store_monst2.spec_skill + 1,815,"Choose Monster Ability:");
			if (i >= 0) {
				store_monst2.spec_skill = i - 1;
				put_monst_abils_in_dlog();
			}	
			break;
		case 34: // radiate
			if (save_monst_abils() == false) break;
			i = choose_text_res(20,50,65,store_monst2.radiate_1 + 50,815,"Choose Radiation Ability:");
			if (i >= 0) {
				store_monst2.radiate_1 = i - 50;
				put_monst_abils_in_dlog();
			}	
			break;
		default:
			cd_hit_led_range(815,9,12,item_hit);
			cd_hit_led_range(815,41,44,item_hit);
			for (i = 13; i < 21; i++)
				cd_flip_led(815,i,item_hit);
			break;
			
	}
}

cMonster edit_monst_abil(cMonster starting_record,short parent_num) {
	// ignore parent in Mac version
	short item_hit,i,store_dialog_answer;
	Str255 temp_str;
	
	//store_which_monst = which_monst;
	store_monst2 = starting_record;
	//make_cursor_sword();
	
	cd_create_dialog_parent_num(815,parent_num);
	
	put_monst_abils_in_dlog();
	
	cd_add_label(815,9,"Fire",35);
	cd_add_label(815,10,"Cold",35);
	cd_add_label(815,11,"Electricity",35);
	cd_add_label(815,12,"Darkness",35);
	cd_add_label(815,41,"No Summon",35);
	cd_add_label(815,42,"Type 1",35);
	cd_add_label(815,43,"Type 2",35);
	cd_add_label(815,44,"Type 3",35);
	
	cd_add_label(815,13,"Resist Magic",55);
	cd_add_label(815,15,"Resist Fire",55);
	cd_add_label(815,17,"Resist Cold",55);
	cd_add_label(815,19,"Resist Poison",55);
	cd_add_label(815,14,"Immune To Magic",55);
	cd_add_label(815,16,"Immune To Fire",55);
	cd_add_label(815,18,"Immune To Cold",55);
	cd_add_label(815,20,"Immune To Poison",55);
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(815);
	return store_monst2;
}

void put_item_info_in_dlog() {
	Str255 str;
	
	cdsin(818,52,store_which_item);
	CDST(818,2,store_item.full_name.c_str());
	CDST(818,3,store_item.name.c_str());
	if (store_item.graphic_num >= 1000) // was 150
		csp(818,49,store_item.graphic_num % 1000,PICT_CUSTOM + PICT_ITEM);
	else csp(818,49,/*1800 + */store_item.graphic_num,PICT_ITEM);
	CDSN(818,4,store_item.graphic_num);
	cd_set_led_range(818,18,45,store_item.variety);
	CDSN(818,5,store_item.item_level);
	CDSN(818,6,store_item.awkward);
	CDSN(818,7,store_item.bonus);
	CDSN(818,8,store_item.protection);
	CDSN(818,9,store_item.charges);
	CDSN(818,10,store_item.type_flag);
	CDSN(818,11,store_item.value);
	CDSN(818,12,store_item.weight);
	CDSN(818,13,store_item.special_class);
	cd_set_led_range(818,46,48,store_item.type - 1);
	
}

bool save_item_info() {
	Str255 str;
	short i;
	
	CDGT(818,2,(char *) str);
	str[24] = 0;
	store_item.full_name = (char*)str;
	CDGT(818,3,(char *) str);
	str[14] = 0;
	store_item.name = (char*)str;
	store_item.graphic_num = CDGN(818,4);
	
	store_item.variety = (eItemType) cd_get_led_range(818,18,45);
	store_item.type = (eWeapType) (cd_get_led_range(818,46,48) + 1);
	
	store_item.item_level = CDGN(818,5);
	if (cre(store_item.item_level,0,50,"Item Level must be from 0 to 50.","",818) > 0) return false;
	if (((store_item.variety == 3) || (store_item.variety == 11)) && (store_item.item_level == 0))
		store_item.item_level = 1;
	
	store_item.awkward = CDGN(818,6);
	if (cre(store_item.awkward,0,20,"Awkward must be from 0 to 20.","",818) > 0) return false;
	store_item.bonus = CDGN(818,7);
	if (cre(store_item.bonus,0,60,"Bonus must be from 0 to 60.","",818) > 0) return false;
	store_item.protection = CDGN(818,8);
	if (cre(store_item.protection,-10,20,"Protection must be from -10 to 20.","",818) > 0) return false;
	store_item.charges = CDGN(818,9);
	if (cre(store_item.charges,0,100,"Charges must be from 0 to 100.","",818) > 0) return false;
	store_item.type_flag = CDGN(818,10);
	if (cre(store_item.type_flag,0,1000,"Type Flag must be from 0 to 1000.","",818) > 0) return false;
	store_item.value  = CDGN(818,11);
	if (cre(store_item.value,0,10000,"Value must be from 0 to 10000.","",818) > 0) return false;
	store_item.weight = CDGN(818,12);
	if (cre(store_item.weight,0,250,"Weight must be from 0 to 250.","",818) > 0) return false;
	store_item.special_class = CDGN(818,13);
	if (cre(store_item.special_class,0,100,"Special Class must be from 0 to 100.","",818) > 0) return false;
	
	if ((store_item.type_flag > 0) && (store_item.charges == 0)) {
		give_error("If the Type Flag is greater than 0, the Charges must also be greater than 0.","",818);
		return false;
	}
	if (store_item.variety > 25) {
		give_error("The Unused item varieties are reserved for later expansions, and can't be used now.","",818);
		return false;
	}
	if ((store_item.ability >= 70) && (store_item.ability < 170) && (store_item.charges == 0)) {
		give_error("An item with the special ability selected must have at least 1 charge.","",818);
		return false;
	}
	scenario.scen_items[store_which_item] = store_item ;
	
	return true;
}

void edit_item_type_event_filter (short item_hit) {
	Str255 str;
	short i;
	cItemRec temp_item;
	
	switch (item_hit) {
		case 15:
			toast_dialog();
			break;
		case 14:
			if (save_item_info() == true)
				toast_dialog();
			break;
			
		case 16:
			if (save_item_info() == false) break;
			store_which_item--;
			if (store_which_item < 0) store_which_item = 399;
			store_item = scenario.scen_items[store_which_item];
			put_item_info_in_dlog();
			break;
		case 17:
			if (save_item_info() == false) break;
			store_which_item++;
			if (store_which_item > 399) store_which_item = 0;
			store_item = scenario.scen_items[store_which_item];
			put_item_info_in_dlog();
			break;
		case 56:
			if (save_item_info() == false) break;
			i = choose_graphic(/*1800,1922*/0,PICT_N_ITEM,store_item.graphic_num/* + 1800*/,PICT_ITEM,818);
			if (i < 0) break;
			store_item.graphic_num = i;
			put_item_info_in_dlog();
			break;
		case 69:
			if (store_item.variety == 0) {
				give_error("You must give the item a type (weapon, armor, etc.) before you can choose its abilities.","",818);
				break;
			}
			if ((store_item.variety == 3) || (store_item.variety == 11)) {
				give_error("Gold and Food cannot be given special abilities.","",818);
				break;
			}
			temp_item = edit_item_abil(store_item,818);
			if (temp_item.variety != 0)
				store_item = temp_item;
			break;
		default:
			cd_hit_led_range(818,18,45,item_hit);
			store_item.variety = (eItemType) cd_get_led_range(818,18,45);
			if ((store_item.variety == ITEM_TYPE_ONE_HANDED) || (store_item.variety == ITEM_TYPE_TWO_HANDED))
				store_item.type = ITEM_EDGED;
			else store_item.type = ITEM_NOT_MELEE;
			cd_hit_led_range(818,46,48,item_hit);
			break;
			
	}
}

short edit_item_type(short which_item) {
	// ignore parent in Mac version
	short item_hit,i,store_dialog_answer;
	Str255 temp_str;
	
	store_which_item = which_item;
	store_item = scenario.scen_items[store_which_item];
	//make_cursor_sword();
	
	cd_create_dialog_parent_num(818,0);
	
	put_item_info_in_dlog();
	
	for (i = 18; i < 46; i++) 
		cd_add_label(818, i,item_types[i - 18],47);
	cd_add_label(818,46,"Edged",23);
	cd_add_label(818,47,"Bashing",23);
	cd_add_label(818,48,"Pole",23);
	cd_attach_key(818,16,0);
	cd_attach_key(818,17,0);
	
	item_hit = cd_run_dialog();
	
	cd_kill_dialog(818);
	return 0;
}

void put_item_abils_in_dlog() {
	Str255 str;
	short i;
	
	cdsin(824,16,store_which_item);
	csit(824,32,store_item2.full_name.c_str());
	csit(824,34,item_types[store_item2.variety]);
	get_str(str,23,store_item2.ability + 1);
	csit(824,19,(char *) str);
	
	cd_set_led_range(824,5,8,store_item2.magic_use_type);
	cd_set_led_range(824,26,30,store_item2.treas_class);
	CDSN(824,2,store_item2.ability_strength);
	if (store_item2.item_properties & 1)
		cd_set_led(824,9,1);
	else cd_set_led(824,9,0);
	if (store_item2.item_properties & 4)
		cd_set_led(824,10,1);
	else cd_set_led(824,10,0);
	if (store_item2.item_properties & 16)
		cd_set_led(824,11,1);
	else cd_set_led(824,11,0);
	if (store_item2.item_properties & 32)
		cd_set_led(824,12,1);
	else cd_set_led(824,12,0);
}

bool save_item_abils() {
	Str255 str;
	short i;
	
	store_item2.magic_use_type = cd_get_led_range(824,5,8);
	store_item2.treas_class = cd_get_led_range(824,26,30);
	store_item2.ability_strength = CDGN(824,2);
	if ((store_item2.ability != 119) && (store_item2.ability != 120)) {
		if (cre(store_item2.ability_strength,0,10,"Ability Strength must always be a number from 0 to 10.","",824) > 0) return false;
	}
	else if (cre(store_item2.ability_strength,0,255,"Ability Strength must be 0 to 255 - the number of the monster to summon.","",824) > 0) return false;
	store_item2.item_properties = 0;
	store_item2.item_properties |= (cd_get_led(824,9) == 1) ? 1 : 0;
	store_item2.item_properties |= (cd_get_led(824,10) == 1) ? 4 : 0;
	store_item2.item_properties |= (cd_get_led(824,11) == 1) ? 16 : 0;
	store_item2.item_properties |= (cd_get_led(824,12) == 1) ? 32 : 0;
	return true;
}

void edit_item_abil_event_filter (short item_hit) {
	Str255 str;
	short i;
	
	switch (item_hit) {
		case 4:
			store_item2.ability = ITEM_NO_ABILITY;
			toast_dialog();
			break;
		case 3:
			if (save_item_abils() == true)
				toast_dialog();
			break;
		case 17:
			if (save_item_abils() == false) break;
			if (store_item.variety > 2) {
				give_error("You can only give an ability of this sort to a melee weapon.","",824);
				break;
			}
			i = choose_text_res(23,1,15,store_item2.ability + 1,824,"Choose Weapon Ability (inherent)");
			if (i >= 0) store_item2.ability = (eItemAbil) (i - 1);
			else store_item2.ability = ITEM_NO_ABILITY;
			put_item_abils_in_dlog();
			break;
		case 35:
			if (save_item_abils() == false) break;
			if ((store_item.variety == 5) || (store_item.variety == 6)|| (store_item.variety == 7) || (store_item.variety == 8) ||
				(store_item.variety == 9) || (store_item.variety == 10) || (store_item.variety == 20) ||
				(store_item.variety == 21) || (store_item.variety == 24)){
				give_error("You can only give an ability of this sort to an non-missile item which can be equipped (like armor, or a ring).","",824);
				break;
			}
			i = choose_text_res(23,31,63,store_item2.ability + 1,824,"Choose General Ability (inherent)");
			if (i >= 0) store_item2.ability = (eItemAbil) (i - 1);
			else store_item2.ability = ITEM_NO_ABILITY;
			put_item_abils_in_dlog();
			break;
		case 36:
			if (save_item_abils() == false) break;
			if ((store_item.variety == 5) || (store_item.variety == 6) || (store_item.variety == 24)){
				give_error("You can only give an ability of this sort to an item which isn't a missile.","",824);
				break;
			}
			i = choose_text_res(23,71,95,store_item2.ability + 1,824,"Choose Usable Ability (Not spell)");
			if (i >= 0) store_item2.ability = (eItemAbil) (i - 1);
			else store_item2.ability = ITEM_NO_ABILITY;
			put_item_abils_in_dlog();
			break;
		case 39:
			if (save_item_abils() == false) break;
			if ((store_item.variety == 5) || (store_item.variety == 6) || (store_item.variety == 24)){
				give_error("You can only give an ability of this sort to an item which isn't a missile.","",824);
				break;
			}
			i = choose_text_res(23,111,136,store_item2.ability + 1,824,"Choose Usable Ability (Spell)");
			if (i >= 0) store_item2.ability = (eItemAbil) (i - 1);
			else store_item2.ability = ITEM_NO_ABILITY;
			put_item_abils_in_dlog();
			break;
		case 38:
			if (save_item_abils() == false) break;
			if (store_item.variety != 21){
				give_error("You can only give an ability of this sort to an item of type Non-Use Object.","",824);
				break;
			}
			i = choose_text_res(23,151,162,store_item2.ability + 1,824,"Choose Reagent Ability");
			if (i >= 0) store_item2.ability = (eItemAbil) (i - 1);
			else store_item2.ability = ITEM_NO_ABILITY;
			put_item_abils_in_dlog();
			break;
		case 37:
			if (save_item_abils() == false) break;
			if ((store_item.variety != 5) && (store_item.variety != 6) && (store_item.variety != 24)){
				give_error("You can only give an ability of this sort to an item which isn't a missile.","",824);
				break;
			}
			i = choose_text_res(23,171,177,store_item2.ability + 1,824,"Choose Missile Ability");
			if (i >= 0) store_item2.ability = (eItemAbil) (i - 1);
			else store_item2.ability = ITEM_NO_ABILITY;
			put_item_abils_in_dlog();
			break;
		default:
			cd_hit_led_range(824,26,30,item_hit);
			cd_hit_led_range(824,5,8,item_hit);
			for (i = 9; i < 13; i++)
				cd_flip_led(824,i,item_hit);
			break;
			
	}
}

cItemRec edit_item_abil(cItemRec starting_record,short parent_num) {
	// ignore parent in Mac version
	short item_hit,i,store_dialog_answer;
	Str255 temp_str;
	
	//store_which_item = which_item;
	store_item2 = starting_record;
	//make_cursor_sword();
	
	cd_create_dialog_parent_num(824,parent_num);
	
	put_item_abils_in_dlog();
	
	cd_add_label(824,26,"Type 0: Junk, Not left",68);
	cd_add_label(824,27,"Type 1: Lousy, 1 - 20 gp",68);
	cd_add_label(824,28,"Type 2: So-so, 20-200 gp",68);
	cd_add_label(824,29,"Type 3: Good, 200+ gp",68);
	cd_add_label(824,30,"Type 4: Great, 2500+ gp",68);
	
	cd_add_label(824,5,"Help using PC",50);
	cd_add_label(824,6,"Harm using PC",50);
	cd_add_label(824,7,"Help whole party",50);
	cd_add_label(824,8,"Harm whole party",50);
	
	cd_add_label(824,9,"Always identified",50);
	cd_add_label(824,10,"Magical",50);
	cd_add_label(824,11,"Cursed",50);
	cd_add_label(824,12,"Conceal ability",50);
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(824);
	return store_item2;
}

void put_spec_item_in_dlog() {
	Str255 str;
	short i;
	
	cdsin(806,19,store_which_spec_item);
	CDST(806,2,scenario.scen_strs(60 + store_which_spec_item * 2));
	CDST(806,3,scenario.scen_strs(60 + store_which_spec_item * 2 + 1));
	CDSN(806,4,scenario.special_item_special[store_which_spec_item]);
	if (scenario.special_items[store_which_spec_item] >= 10)
		cd_set_led(806,15,1);
	else cd_set_led(806,15,0);
	if (scenario.special_items[store_which_spec_item] % 10 > 0)
		cd_set_led(806,17,1);
	else cd_set_led(806,17,0);
}

bool save_spec_item() {
	Str255 str;
	short i;
	
	CDGT(806,2,(char *) str);
	str[25] = 0;
	sprintf(scenario.scen_strs(60 + store_which_spec_item * 2 + 0),"%s",str);
	CDGT(806,3,scenario.scen_strs(60 + store_which_spec_item * 2 + 1));
	spec_item_spec = CDGN(806,4);
	if (cre(scenario.special_item_special[store_which_spec_item],
			-1,255,"Scenario special node called must be from 0 to 255 (or -1 for no special).","",806) > 0) return false;
	scenario.special_item_special[store_which_spec_item] = spec_item_spec;
	scenario.special_items[store_which_spec_item] = 0;
	if (cd_get_led(806,15) == 1)
		scenario.special_items[store_which_spec_item] += 10;
	if (cd_get_led(806,17) == 1)
		scenario.special_items[store_which_spec_item] += 1;
	return true;
}

void edit_spec_item_event_filter (short spec_item_hit) {
	Str255 str;
	short i,spec;
	
	switch (spec_item_hit) {
		case 11:
			toast_dialog();
			break;
		case 5:
			if (save_spec_item() == true)
				toast_dialog();
			break;
			
		case 20:
			if (save_spec_item() == false) break;
			store_which_spec_item--;
			if (store_which_spec_item < 0) store_which_spec_item = 49;
			spec_item_spec = scenario.special_item_special[store_which_spec_item];
			put_spec_item_in_dlog();
			break;
		case 21:
			if (save_spec_item() == false) break;
			store_which_spec_item++;
			if (store_which_spec_item > 49) store_which_spec_item = 0;
			spec_item_spec = scenario.special_item_special[store_which_spec_item];
			put_spec_item_in_dlog();
			break;
		case 13: //choose edit
			if (save_spec_item() == false)
				break;
			spec = CDGN(806,4);
			if ((spec < 0) || (spec >= 256)) {
				spec = get_fresh_spec(0);
				if (spec < 0) {
					give_error("You can't create a new special encounter because there are no more free scenario special nodes.",
							   "To free a special node, set its type to No Special and set its Jump To special to -1.",806);
					break;
				}
				CDSN(806,4,spec);
			}	
			edit_spec_enc(spec,0,806);
			if ((spec >= 0) && (spec < 256) && (scenario.scen_specials[spec].pic < 0))
				CDSN(806,4,-1);
			if (save_spec_item() == false)
				break;
			break;
		default:
			cd_flip_led(806,15,spec_item_hit);
			cd_flip_led(806,17,spec_item_hit);
			break;
			
	}
}

void edit_spec_item(short which_item) {
	// ignore parent in Mac version
	short spec_item_hit,i,store_dialog_answer;
	Str255 temp_str;
	
	//store_which_spec_item = which_spec_item;
	store_which_spec_item = which_item;
	spec_item_spec = scenario.special_item_special[store_which_spec_item];
	
	cd_create_dialog_parent_num(806,0);
	
	put_spec_item_in_dlog();
	cd_attach_key(806,20,0);
	cd_attach_key(806,21,0);
	cd_activate_item(806,12,0);
	
	spec_item_hit = cd_run_dialog();
	
	cd_kill_dialog(806);
}

void put_save_rects_in_dlog() {
	Str255 str;
	short i;
	
	for (i = 0; i < 3; i++) {
		CDSN(807,3 + 5 * i,scenario.store_item_rects[i].top);
		CDSN(807,4 + 5 * i,scenario.store_item_rects[i].left);
		CDSN(807,5 + 5 * i,scenario.store_item_rects[i].bottom);
		CDSN(807,6 + 5 * i,scenario.store_item_rects[i].right);
		CDSN(807,2 + 5 * i,scenario.store_item_towns[i]);
		
	}
	
}

bool save_save_rects() {
	Str255 str;
	short i;
	
	for (i = 0; i < 3; i++) {
		scenario.store_item_rects[i].top = CDGN(807,3 + 5 * i);
		scenario.store_item_rects[i].left = CDGN(807,4 + 5 * i);
		scenario.store_item_rects[i].bottom = CDGN(807,5 + 5 * i);
		scenario.store_item_rects[i].right = CDGN(807,6 + 5 * i);
		scenario.store_item_towns[i] = CDGN(807,2 + 5 * i);
		if ((scenario.store_item_towns[i] < -1) || (scenario.store_item_towns[i] >= 200)) {
			give_error("Towns must be in 0 to 200 range (or -1 for no save items rectangle).","",807);
			return false;
		}
	}
	if (((scenario.store_item_towns[0] == scenario.store_item_towns[1]) && 
		 (scenario.store_item_towns[0] >= 0) && (scenario.store_item_towns[1] >= 0))
		||
		((scenario.store_item_towns[2] == scenario.store_item_towns[1]) && 
		 (scenario.store_item_towns[2] >= 0) && (scenario.store_item_towns[1] >= 0))
		||
		((scenario.store_item_towns[2] == scenario.store_item_towns[0]) && 
		 (scenario.store_item_towns[2] >= 0) && (scenario.store_item_towns[0] >= 0))
		) {
		give_error("The three towns towns with saved item rectangles must be different.","",807);
		return false;
	}
	return true;
}

void edit_save_rects_event_filter (short save_rects_hit) {
	Str255 str;
	short i;
	
	switch (save_rects_hit) {
		case 18:
			toast_dialog();
			break;
		case 17:
			if (save_save_rects() == true)
				toast_dialog();
			break;
			
	}
}

void edit_save_rects() {
	// ignore parent in Mac version
	short save_rects_hit,i,store_dialog_answer;
	Str255 temp_str;
	
	cd_create_dialog_parent_num(807,0);
	
	put_save_rects_in_dlog();
	
	save_rects_hit = cd_run_dialog();
	cd_kill_dialog(807);
}

bool save_horses() {
	Str255 str;
	short i;
	
	for (i = 0; i < 6; i++) {
		scenario.horses[6 * store_horse_page + i].which_town = CDGN(808,2 + i);
		if (cre(scenario.horses[6 * store_horse_page + i].which_town,
				-1,199,"Town number must be from 0 to 199 (or -1 for horse to not exist).","",808) == true) return false;
		scenario.horses[6 * store_horse_page + i].loc.x = CDGN(808,8 + i);
		if (cre(scenario.horses[6 * store_horse_page + i].loc.x,
				0,63,"Horse location coordinates must be from 0 to 63.","",808) == true) return false;
		scenario.horses[6 * store_horse_page + i].loc.y = CDGN(808,14 + i);
		if (cre(scenario.horses[6 * store_horse_page + i].loc.y,
				0,63,"Horse location coordinates must be from 0 to 63.","",808) == true) return false;
		scenario.horses[6 * store_horse_page + i].property = cd_get_led(808,43 + i);
	}
	return true;
}

void put_horses_in_dlog() {
	Str255 str;
	short i;
	
	for (i = 0; i < 6; i++) {
		cdsin(808,23 + i,6 * store_horse_page + i);
		CDSN(808,2 + i,scenario.horses[6 * store_horse_page + i].which_town);
		CDSN(808,8 + i,scenario.horses[6 * store_horse_page + i].loc.x);
		CDSN(808,14 + i,scenario.horses[6 * store_horse_page + i].loc.y);
		cd_set_led(808,43 + i,scenario.horses[6 * store_horse_page + i].property);
	}
	
}

void edit_horses_event_filter (short item_hit) {
	Str255 str;
	short i;
	
	switch (item_hit) {
		case 20:
			if (save_horses() == true)
				toast_dialog();
			break;
		case 21:
			if (save_horses() == false) break;
			store_horse_page--;
			if (store_horse_page < 0) store_horse_page = 4;
			put_horses_in_dlog();
			break;
		case 22:
			if (save_horses() == false) break;
			store_horse_page++;
			if (store_horse_page > 4) store_horse_page = 0;
			put_horses_in_dlog();
			break;
		default:
			for (i = 0; i < 6; i++)
				cd_flip_led(808,43 + i,item_hit);
			break;
	}
}

void edit_horses() {
	// ignore parent in Mac version
	short horses_hit,i,store_dialog_answer;
	Str255 temp_str;
	
	store_horse_page = 0;
	
	cd_create_dialog_parent_num(808,0);
	
	put_horses_in_dlog();
	
	horses_hit = cd_run_dialog();
	cd_kill_dialog(808);
}

bool save_boats() {
	Str255 str;
	short i;
	
	for (i = 0; i < 6; i++) {
		scenario.boats[6 * store_boat_page + i].which_town = CDGN(809,2 + i);
		if (cre(scenario.boats[6 * store_boat_page + i].which_town,
				-1,199,"Town number must be from 0 to 199 (or -1 for boat to not exist).","",809) == true) return false;
		scenario.boats[6 * store_boat_page + i].loc.x = CDGN(809,8 + i);
		if (cre(scenario.boats[6 * store_boat_page + i].loc.x,
				0,63,"boat location coordinates must be from 0 to 63.","",809) == true) return false;
		scenario.boats[6 * store_boat_page + i].loc.y = CDGN(809,14 + i);
		if (cre(scenario.boats[6 * store_boat_page + i].loc.y,
				0,63,"boat location coordinates must be from 0 to 63.","",809) == true) return false;
		scenario.boats[6 * store_boat_page + i].property = cd_get_led(809,43 + i);
	}
	return true;
}

void put_boats_in_dlog() {
	Str255 str;
	short i;
	
	for (i = 0; i < 6; i++) {
		cdsin(809,24 + i,6 * store_boat_page + i);
		CDSN(809,2 + i,scenario.boats[6 * store_boat_page + i].which_town);
		CDSN(809,8 + i,scenario.boats[6 * store_boat_page + i].loc.x);
		CDSN(809,14 + i,scenario.boats[6 * store_boat_page + i].loc.y);
		cd_set_led(809,43 + i,scenario.boats[6 * store_boat_page + i].property);
	}
	
}

void edit_boats_event_filter (short item_hit) {
	Str255 str;
	short i;
	
	switch (item_hit) {
		case 20:
			if (save_boats() == true)
				toast_dialog();
			break;
		case 22:
			if (save_boats() == false) break;
			store_boat_page--;
			if (store_boat_page < 0) store_boat_page = 4;
			put_boats_in_dlog();
			break;
		case 23:
			if (save_boats() == false) break;
			store_boat_page++;
			if (store_boat_page > 4) store_boat_page = 0;
			put_boats_in_dlog();
			break;
		default:
			for (i = 0; i < 6; i++)
				cd_flip_led(809,43 + i,item_hit);
			break;
	}
}

void edit_boats() {
	// ignore parent in Mac version
	short boats_hit,i,store_dialog_answer;
	Str255 temp_str;
	
	store_boat_page = 0;
	
	cd_create_dialog_parent_num(809,0);
	
	put_boats_in_dlog();
	
	boats_hit = cd_run_dialog();
	cd_kill_dialog(809);
}

bool save_add_town() {
	Str255 str;
	short i;
	
	for (i = 0; i < 10; i++) {
		scenario.town_to_add_to[i] = CDGN(810,2 + i);
		if (cre(scenario.town_to_add_to[i],
				-1,199,"Town number must be from 0 to 199 (or -1 for no effect).","",810) == true) return false;
		scenario.flag_to_add_to_town[i][0] = CDGN(810,12 + i);
		if (cre(scenario.flag_to_add_to_town[i][0],
				0,299,"First part of flag must be from 0 to 299.","",810) == true) return false;
		scenario.flag_to_add_to_town[i][1] = CDGN(810,22 + i);
		if (cre(scenario.flag_to_add_to_town[i][1],
				0,9,"Second part of flag must be from 0 to 9.","",810) == true) return false;
	}
	return true;
}

void put_add_town_in_dlog() {
	Str255 str;
	short i;
	
	for (i = 0; i < 10; i++) {
		CDSN(810,2 + i,scenario.town_to_add_to[i]);
		CDSN(810,12 + i,scenario.flag_to_add_to_town[i][0]);
		CDSN(810,22 + i,scenario.flag_to_add_to_town[i][1]);
	}
	
}

void edit_add_town_event_filter (short item_hit) {
	Str255 str;
	short i;
	
	switch (item_hit) {
		case 32:
			if (save_add_town() == true)
				toast_dialog();
			break;
			
	}
}

void edit_add_town() {
	// ignore parent in Mac version
	short add_town_hit,i,store_dialog_answer;
	Str255 temp_str;
	
	cd_create_dialog_parent_num(810,0);
	
	put_add_town_in_dlog();
	
	add_town_hit = cd_run_dialog();
	cd_kill_dialog(810);
}

bool save_item_placement() {
	Str255 str;
	short i;
	
	store_storage.property = cd_get_led(812,38);
	store_storage.ter_type = CDGN(812,22);
	if (cre(store_storage.ter_type,
			-1,255,"Terrain Type must be from 0 to 255 (or -1 for No Shortcut).","",812) == true) return false;
	for (i = 0; i < 10; i++) {
		store_storage.item_num[i] = CDGN(812,2 + i);
		if (cre(store_storage.item_num[i],
				-1,399,"All item numbers must be from 0 to 399 (or -1 for No Item).","",812) == true) return false;
		store_storage.item_odds[i] = CDGN(812,12 + i);
		if (cre(store_storage.item_odds[i],
				0,100,"All item chances must bve from 0 to 100.","",812) == true) return false;
	}
	scenario.storage_shortcuts[cur_shortcut] = store_storage;
	
	return true;
}

void put_item_placement_in_dlog() {
	Str255 str;
	short i;
	
	cdsin(812,27,cur_shortcut);
	cd_set_led(812,38,store_storage.property);
	CDSN(812,22,store_storage.ter_type);
	for (i = 0; i < 10; i++) {
		CDSN(812,2 + i,store_storage.item_num[i]);
		CDSN(812,12 + i,store_storage.item_odds[i]);
	}
	
}

void edit_item_placement_event_filter (short item_hit) {
	Str255 str;
	short i;
	
	switch (item_hit) {
		case 23:
			if (save_item_placement() == true)
				toast_dialog();
			break;
		case 24:
			toast_dialog();
			break;
		case 26:
			if (save_item_placement() == false) break;
			cur_shortcut--;
			if (cur_shortcut < 0) cur_shortcut = 9;
			store_storage = scenario.storage_shortcuts[cur_shortcut];
			put_item_placement_in_dlog();
			break;
		case 25:
			if (save_item_placement() == false) break;
			cur_shortcut++;
			if (cur_shortcut > 9) cur_shortcut = 0;
			store_storage = scenario.storage_shortcuts[cur_shortcut];
			put_item_placement_in_dlog();
			break;
		case 41:
			store_storage.ter_type = CDGN(812,22);
			i = choose_text_res(-4,0,255,store_storage.ter_type,812,"Which Terrain?");
			store_storage.ter_type = i;
			CDSN(812,22,i);
			break;
		default:
			if ((item_hit >= 42) && (item_hit <= 51)) {
				i = CDGN(812,2 + item_hit - 42);
				i = choose_text_res(-2,0,399,i,812,"Place which item?");
				if (i >= 0)
					CDSN(812,2 + item_hit - 42,i);
				break;
			}
			cd_flip_led(812,38,item_hit);
			break;
	}
}

void edit_item_placement() {
	// ignore parent in Mac version
	short item_placement_hit,i,store_dialog_answer;
	Str255 temp_str;
	
	store_storage = scenario.storage_shortcuts[0];
	cur_shortcut = 0;
	
	cd_create_dialog_parent_num(812,0);
	
	put_item_placement_in_dlog();
	
	item_placement_hit = cd_run_dialog();
	cd_kill_dialog(812);
}

bool save_scen_details() {
	Str255 str;
	short i;
	
	scenario.difficulty = cd_get_led_range(803,30,33);
	scenario.rating = cd_get_led_range(803,21,24);
	scenario.format.ver[0] = CDGN(803,2);
	scenario.format.ver[1] = CDGN(803,3);
	scenario.format.ver[2] = CDGN(803,4);
	for (i = 0; i < 3; i++)
		if (cre(scenario.format.ver[i],
				0,9,"The digits in the version number must be in the 0 to 9 range.","",803) == true) return false;
	CDGT(803,5,(char *) str);
	str[59] = 0;
	strcpy(scenario.scen_strs(1),(char *) str);
	CDGT(803,6,(char *) str);
	str[59] = 0;
	strcpy(scenario.scen_strs(2),(char *) str);
	CDGT(803,7,scenario.scen_strs(3));
	
	return true;
}

void put_scen_details_in_dlog() {
	Str255 str;
	short i;
	
	cd_set_led_range(803,30,33,scenario.difficulty);
	cd_set_led_range(803,21,24,scenario.rating);
	CDSN(803,2,scenario.format.ver[0]);
	CDSN(803,3,scenario.format.ver[1]);
	CDSN(803,4,scenario.format.ver[2]);
	CDST(803,5,scenario.scen_strs(1));
	CDST(803,6,scenario.scen_strs(2));
	CDST(803,7,scenario.scen_strs(3));
}

void edit_scen_details_event_filter (short item_hit) {
	Str255 str;
	short i;
	
	switch (item_hit) {
		case 8:
			if (save_scen_details() == true)
				toast_dialog();
			break;
		default:
			cd_hit_led_range(803,21,24,item_hit);
			cd_hit_led_range(803,30,33,item_hit);
			break;
	}
}

void edit_scen_details() {
	// ignore parent in Mac version
	short scen_details_hit,i,store_dialog_answer;
	Str255 temp_str;
	
	cd_create_dialog_parent_num(803,0);
	
	put_scen_details_in_dlog();
	
	scen_details_hit = cd_run_dialog();
	cd_kill_dialog(803);
}



void put_make_scen_1_in_dlog() {
	Str255 str;
	short i;
	
	CDST(800,2,"Scenario name");
	CDST(800,3,"filename");
}

void edit_make_scen_1_event_filter (short item_hit) {
	Str255 str;
	short i,j;
	
	switch (item_hit) {
		case 4:
			CDGT(800,3,(char *) str);
			j = strlen((char *) str);
			if (j == 0) {
				give_error("You've left the file name empty.","",800);
				break;
			}
			if (j > 50) {
				give_error("The file name can be at most 50 characters long.","",800);
				break;
			}
			for (i = 0; i < j; i++)
				if ((str[i] < 97) || (str[i] > 122)) {
					give_error("The file name must consist of only lower case letters.","",800);
					return;
				}
			dialog_answer = 1;
			toast_dialog();
			break;
		case 9:
			dialog_answer = 0;
			toast_dialog();
			break;
		default:
			cd_flip_led(800,11,item_hit);
			break;
	}
}

short edit_make_scen_1(char *filename,char *title,short *grass) {
	// ignore parent in Mac version
	short make_scen_1_hit,i,store_dialog_answer;
	Str255 temp_str;
	
	cd_create_dialog_parent_num(800,0);
	
	put_make_scen_1_in_dlog();
	
	make_scen_1_hit = cd_run_dialog();
	CDGT(800,2,title);
	title[30] = 0;
	CDGT(800,3,filename);
	*grass = cd_get_led(800,11);
	cd_kill_dialog(800);
	return dialog_answer;
}

void put_make_scen_2_in_dlog() {
	Str255 str;
	short i;
	
	CDSN(801,2,1);
	CDSN(801,3,1);
	CDSN(801,4,0);
	CDSN(801,5,1);
	CDSN(801,6,0);
}

void edit_make_scen_2_event_filter (short item_hit) {
	Str255 str;
	short i,j,k;
	
	switch (item_hit) {
		case 11:
			i = CDGN(801,2);
			if (cre(i,
					1,50,"Outdoors width must be between 1 and 50.","",801) == true) return ;
			j = CDGN(801,3);
			if (cre(j,
					1,50,"Outdoors height must be between 1 and 50.","",801) == true) return ;
			if (cre(i * j,
					1,100,"The total number of outdoor sections (width times height) must be between 1 and 100.","",801) == true) return ;
			i = CDGN(801,4);
			j = CDGN(801,5);
			k = CDGN(801,6);
			if (cre(i,
					0,200,"Number of small towns must be between 0 and 200.","",801) == true) return ;
			if (cre(j,
					1,200,"Number of medium towns must be between 0 and 200. The first town (Town 0) must always be of medium size.","",801) == true) return ;
			if (cre(k,
					0,200,"Number of large towns must be between 0 and 200.","",801) == true) return ;
			if (cre(i + j + k,
					1,200,"The total number of towns must be from 1 to 200 (you must have at least 1 town).","",801) == true) return ;
			
			toast_dialog();
			break;
		case 10:
			dialog_answer = 0;
			toast_dialog();
			break;
		default:
			cd_flip_led(801,26,item_hit);
			break;
	}
}

short edit_make_scen_2(short *val_array) {
	// ignore parent in Mac version
	short make_scen_2_hit,i,store_dialog_answer;//array[6];
	Str255 temp_str;
	
	//array = val_array;
	cd_create_dialog_parent_num(801,0);
	
	put_make_scen_2_in_dlog();
	
	make_scen_2_hit = cd_run_dialog();
	
	for (i = 0; i < 5; i++)
		val_array[i] = CDGN(801,2 + i);
	val_array[5] = cd_get_led(801,26);
	cd_kill_dialog(801);
	return dialog_answer;
}

bool build_scenario() {
	printf("Building a scenario currently disabled.\n");
	//	short two_flags[6]; // width, height, large, med, small, default_town
	//	Str255 f_name,f_name2,title;
	//	short grass,password,which_town,error;
	//	FSSpec temp_file_to_load;
	//	short i,j;
	//	long dummy;
	//	
	//	if (edit_make_scen_1((char *) f_name,(char *) title,&grass) == false)
	//		return false;
	//	sprintf((char *) f_name2,"%s.exs",f_name);
	//	if (edit_make_scen_2((short *) two_flags) == false)
	//		return false;
	//	user_given_password = given_password = get_password();
	//	if (fancy_choice_dialog(860,0) == 2)
	//		return false;
	//	town = new cMedTown;
	//	//cMedTown* t_d = (cMedTown*) town;
	//	init_out();
	//	init_scenario();
	//	strcpy((char *) scenario.scen_strs(0),(char *) title);
	//	if (two_flags[5] == 0) {
	//		init_town(1);
	//		if (grass == 0)
	//			for (i = 0; i < 48; i++)
	//				for (j = 0; j < 48; j++)
	//					town->terrain(i,j) = 0;
	//		}
	//	else {
	//		error = FSMakeFSSpec(start_volume,start_dir,"\p::::Blades of Exile Base",&temp_file_to_load);
	//		if (error != 0) {oops_error(40);}
	//		import_town(0,temp_file_to_load);
	//	}
	//	if (two_flags[3] > 0)
	//		two_flags[3]--;
	//	
	//	make_new_scenario(f_name2,two_flags[0],two_flags[1],two_flags[5],grass);
	//	
	//	// now make sure correct outdoors is in memory, because we're going to be saving scenarios
	//	// for a while
	//	overall_mode = 60;
	//	cur_out.x = 0;
	//	cur_out.y = 0;
	//	load_outdoors(cur_out,0);
	//
	//	for (i = 0; i < two_flags[2]; i++) {
	//		which_town = scenario.num_towns;
	//		scenario.num_towns++;
	//		scenario.town_size[which_town] = 0;
	//		scenario.town_hidden[which_town] = 0;
	//		cur_town = which_town;
	//		init_town(0);
	//		strcpy(data_store->town_strs[0],"Large town");
	//		town_type = 0;
	//		reset_pwd();
	//		save_scenario();
	//		}
	//	for (i = 0; i < two_flags[3]; i++) {
	//		which_town = scenario.num_towns;
	//		scenario.num_towns++;
	//		scenario.town_size[which_town] = 1;
	//		scenario.town_hidden[which_town] = 0;
	//		cur_town = which_town;
	//		init_town(1);
	//		strcpy(data_store->town_strs[0],"Medium town");
	//		town_type = 1;
	//		reset_pwd();
	//		save_scenario();
	//		}
	//	for (i = 0; i < two_flags[4]; i++) {
	//		which_town = scenario.num_towns;
	//		scenario.num_towns++;
	//		scenario.town_size[which_town] = 2;
	//		scenario.town_hidden[which_town] = 0;
	//		cur_town = which_town;
	//		init_town(2);
	//		strcpy(data_store->town_strs[0],"Small town");
	//		town_type = 2;
	//		reset_pwd();
	//		save_scenario();
	//		}
	//	//Delay(200,&dummy);
	//	if(load_town(0)) cur_town = 0;
	//	cur_town = 0;
	//	augment_terrain(cur_out);
	//	update_item_menu();
}

//void user_password_filter (short item_hit) {
//	Str255 str;
//	short i;
//	
//	switch (item_hit) {
//		case 4:
//			i = CDGN(802,2);
//			if ((i < 0) || (i > 30000)) {
//				give_error("The password must be from 0 to 30000.","",802);
//				break;
//			}
//			toast_dialog();
//			break;
//			
//	}
//}
//
//short get_password() {
//	// ignore parent in Mac version
//	short town_strs_hit,i,store_dialog_answer;
//	Str255 temp_str,str2;
//	
//	cd_create_dialog_parent_num(802,0);
//	
//	CDSN(802,2,0);
//	
//	town_strs_hit = cd_run_dialog();
//	
//	CDGT(802,2,(char *) temp_str);
//	i = wd_to_pwd(temp_str);
//	
//	cd_kill_dialog(802);
//	
//	return i;
//}

void set_starting_loc_filter (short item_hit) {
	Str255 str;
	short i,j,k;
	
	switch (item_hit) {
		case 5:
			i = CDGN(805,2);
			j = CDGN(805,3);
			k = CDGN(805,4);
			if ((i < 0) || (i >= scenario.num_towns)) {
				sprintf((char *) str,"The starting town must be from 0 to %d.",scenario.num_towns - 1);
				give_error((char *) str,"",805);
				break;
			}
			if ((j < 0) || (j >= max_dim[scenario.town_size[i]] - 1) ||
				(k < 0) || (k >= max_dim[scenario.town_size[i]] - 1)) {
				give_error("This coordinate is not inside the bounds of the town.","",805);
				break;
			}
			scenario.which_town_start = i;
			scenario.where_start.x = j;
			scenario.where_start.y = k;
			toast_dialog();
			break;
		case 12:
			toast_dialog();
			break;
	}
}

void set_starting_loc() {
	// ignore parent in Mac version
	short town_strs_hit,i,store_dialog_answer;
	Str255 temp_str,str2;
	
	cd_create_dialog_parent_num(805,0);
	
	CDSN(805,2,scenario.which_town_start);
	CDSN(805,3,scenario.where_start.x);
	CDSN(805,4,scenario.where_start.y);
	
	town_strs_hit = cd_run_dialog();
	
	
	cd_kill_dialog(805);
	
}

bool save_scenario_events() {
	Str255 str;
	short i;
	
	for (i = 0; i < 10; i++) {
		scenario.scenario_timer_times[i] = CDGN(811,2 + i);
		if ((scenario.scenario_timer_times[i] > 0) &&
			(scenario.scenario_timer_times[i] % 10 != 0)) {
			give_error("All scenario event times must be multiples of 10 (e.g. 100, 150, 1000, etc.).","",811);
			return false;
		}
		scenario.scenario_timer_specs[i] = CDGN(811,12 + i);
		if (cre(scenario.scenario_timer_specs[i],-1,255,"The scenario special nodes must be between 0 at 255 (or -1 for no special)."
				,"",811) == true) return false;
	}
	return true;
}

void put_scenario_events_in_dlog() {
	Str255 str;
	short i;
	
	for (i = 0; i < 10; i++) {
		CDSN(811,2 + i,scenario.scenario_timer_times[i]);
		CDSN(811,12 + i,scenario.scenario_timer_specs[i]);
	}
	
}

void edit_scenario_events_event_filter (short item_hit) {
	Str255 str;
	short i,spec;
	
	switch (item_hit) {
		case 22:
			if (save_scenario_events() == true)
				toast_dialog();
			break;
		default:
			if ((item_hit >= 30) && (item_hit <= 39)) {
				if (save_scenario_events() == false)
					break;
				spec = CDGN(811,item_hit - 30 + 12);
				if ((spec < 0) || (spec > 255)) {
					spec = get_fresh_spec(0);
					if (spec < 0) {
						give_error("You can't create a new scenario special encounter because there are no more free special nodes.",
								   "To free a special node, set its type to No Special and set its Jump To special to -1.",811);
						break;
					}
					CDSN(811,item_hit - 30 + 12,spec);
				}	
				edit_spec_enc(spec,0,811);
				if ((spec >= 0) && (spec < 256) && (scenario.scen_specials[spec].pic < 0))
					CDSN(811,item_hit - 30 + 12,-1);
			}
			break;
	}
}

void edit_scenario_events() {
	// ignore parent in Mac version
	short advanced_town_hit,i,store_dialog_answer;
	Str255 temp_str;
	
	
	cd_create_dialog_parent_num(811,0);
	
	put_scenario_events_in_dlog();
	
	advanced_town_hit = cd_run_dialog();
	
	cd_kill_dialog(811);
}

//void give_password_filter (short item_hit) {
//	Str255 str;
//	short i;
//	
//	switch (item_hit) {
//		default:
//			
//			toast_dialog();
//			break;
//			
//	}
//}
//
//short enter_password() {
//	// ignore parent in Mac version
//	short town_strs_hit,i,store_dialog_answer;
//	Str255 temp_str,str2;
//	
//	cd_create_dialog_parent_num(823,0);
//	
//	CDSN(823,2,0);
//	
//	town_strs_hit = cd_run_dialog();
//	
//	CDGT(823,2,(char *) temp_str);
//	i = wd_to_pwd(temp_str);
//	given_password = user_given_password = i;
//	
//	cd_kill_dialog(823);
//	
//	return i;
//}
//
//short wd_to_pwd(Str255 str) {
//	char pwd[8] = "aaaaaa";
//	short i;
//	long val = 0,pow[6] = {1,10,100,1000,9999,99999};
//	
//	for (i = 0; i < 6; i++) {
//		if (str[i] == 0) 
//			i = 6;
//		else {
//			if ((str[i] >= 65) && (str[i] <= 90))
//				pwd[i] = str[i] + 32;
//			else if ((str[i] >= 48) && (str[i] <= 57))
//				pwd[i] = str[i] + 49;
//			else if ((str[i] >= 97) && (str[i] <= 122))
//				pwd[i] = str[i];
//		}
//	}
//	for (i = 0; i < 6; i++)
//		val = val + pow[i] * (long) (pwd[i] - 97);
//	val = val % 30000;
//	return (short) val;
//}
