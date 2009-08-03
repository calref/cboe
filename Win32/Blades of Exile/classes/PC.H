#ifndef _PC_H
	#define _PC_H
	
#include "consts.h"
#include "item.h"			//item_record_type
//#include "consts.h"

/* do not change */
/* dummy_pc_record_type is temporarily used in file reading/writing operations */
struct dummy_pc_record_type
{
	short main_status;
	char name[20];
	short skills[30];
	short max_health, cur_health, max_sp, cur_sp, experience, skill_pts, level;
	short status[15];
	item_record_type items[24];
	char equip[24];									// boolean
	char priest_spells[62], mage_spells[62];		// boolean
	short which_graphic, weap_poisoned;
	char advan[15], traits[15];						// boolean
	short race, exp_adj, direction;
};
	
class pc_record_type
{
public:
	/* DO NOT CHANGE */
	short main_status;
	char name[20];
	short skills[30];
	short max_health, cur_health, max_sp, cur_sp, experience, skill_pts, level;
	short status[15];
	item_record_type items[24];
	char equip[24];									// boolean
	char priest_spells[62], mage_spells[62];		// boolean
	short which_graphic, weap_poisoned;
	char advan[15], traits[15];						// boolean
	short race, exp_adj, direction;
	/* end of DO NOT CHANGE */
	
	/* functions */
	void acid(short how_much);
	void cure(short how_much);			// cure
	void curse(short how_much);			// curse
	void disease(short how_much);		// disease
	void dumbfound(short how_much);		// dumbfound
	void heal(short how_much);			// heal
	void kill(short type);				// kill
	void poison(short how_much);
	void sleep(short how_much,short what_type,short adjust);
	void slow(short how_much);
	void web(short how_much);
	
	bool damage(short how_much, short damage_type, short type_of_attacker);
	
	void drainXP(short how_much);		// drain experience points
	void giveXP(short how_much);		// give experience points
	void restoreSP(short how_much);		// restore spell points
	//bool takeSP(short how_much);		// take spell points
	void takeItem(short which_item);
	void removeCharge(short which_item);
	void enchantWeapon(short item_hit,short enchant_type,short new_val);
	void equipItem(short item_num);
	void dropItem(short item_num,location where_drop);
	void giveThing(short item_num);
	bool giveToPC(item_record_type  item, bool print_result);
	void combineThings();
	
	bool runTrap(short trap_type, short trap_level, short diff);
	
	short getProtLevel(short ability);
	short hasAbilEquip(short ability);
	short hasAbil(short ability);
	short hasSpace();
	short okToBuy(short cost, item_record_type item);
	short statAdj(short which);
	
	short amountCanCarry();
	short amountCarried();
	
	bool isAlive() { return (main_status == MAIN_STATUS_ALIVE); }
	
	void sortItems();
	
	short getNum();
};
	
#endif
