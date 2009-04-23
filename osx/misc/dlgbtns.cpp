/*
 *  dlgbtns.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include "dlgconsts.h"

struct btn_t {
	short type;
	char* str;
	short left_adj;
	char def_key;
};

short available_dlog_buttons[NUM_DLOG_B] = { // This array is a list of indices into the following array.
	0,  63, 64, 65, 1,  4,  5,  8,  128,9,
	10, 11, 12, 13,	14, 15, 16, 17, 29, 51,
	60, 61, 62,	66, 69, 70, 71, 72, 73, 74,
	79, 80, 83, 86, 87, 88, 91, 92, 93, 99,
	100,101,102,104,129,130,131,132,133,134,
	135,136,137
};

btn_t buttons[] = {
	{DLG_BTN_DONE, " ", 2, 0}, // Formerly DLG_BTN_REG with "Done " as the string
	{DLG_BTN_REG, "Ask", 0, 0},
	{DLG_BTN_LEFT, " ", 0, DLG_KEY_LEFT},
	{DLG_BTN_RIGHT, " ", 0, DLG_KEY_RIGHT},
	{DLG_BTN_REG, "Keep", 6, 'k'},
	{DLG_BTN_REG, "Cancel", 0, DLG_KEY_ESC},
	{DLG_BTN_SM, "+", 0, 0},
	{DLG_BTN_SM, "-", 0, 0},
	{DLG_BTN_REG, "Buy", 4, 0},
	{DLG_BTN_REG, "Leave", 5, 0},
	//10
	{DLG_BTN_REG, "Get", 0, 'g'},
	{DLG_BTN_REG, "1", 0, '1'},
	{DLG_BTN_REG, "2", 0, '2'},
	{DLG_BTN_REG, "3", 0, '3'},
	{DLG_BTN_REG, "4", 0, '4'},
	{DLG_BTN_REG, "5", 0, '5'},
	{DLG_BTN_REG, "6", 0, '6'},
	{DLG_BTN_REG, "Cast", 4, 0},
	{DLG_BTN_LED1, " ", 0, 0},
	{DLG_BTN_LED1, " ", 0, 0},
	//20
	{DLG_BTN_LED2, " ", 0, 0},
	{DLG_BTN_LED2, " ", 0, 0},
	{DLG_BTN_LED2, " ", 0, 0},
	{DLG_BTN_REG, "Buy", 4, 0},
	{DLG_BTN_REG, "Sell", 0, 0},
	{DLG_BTN_LG, "Other Spells", 5, ' '},
	{DLG_BTN_REG, "Buy x10", 0, 0},
	{DLG_BTN_UP, " ", 0, DLG_KEY_UP},
	{DLG_BTN_DOWN, " ", 0, DLG_KEY_DOWN},
	{DLG_BTN_REG, "Save", 6, 0},
	//30
	{DLG_BTN_REG, "Race", 6, 0},
	{DLG_BTN_TALL, "Train", 6, 0},
	{DLG_BTN_REG, "Items", 0, 0},
	{DLG_BTN_REG, "Spells", 0, 0},
	{DLG_BTN_LG, "Heal Party", 0, 0},
	{DLG_BTN_SM, "1", 0, '1'},
	{DLG_BTN_SM, "2", 0, '2'},
	{DLG_BTN_SM, "3", 0, '3'},
	{DLG_BTN_SM, "4", 0, '4'},
	{DLG_BTN_SM, "5", 0, '5'},
	//40
	{DLG_BTN_SM, "6", 0, '6'},
	{DLG_BTN_SM, "7", 0, '7'},
	{DLG_BTN_SM, "8", 0, '8'},
	{DLG_BTN_SM, "9", 0, '9'},
	{DLG_BTN_SM, "10", 6, 'a'},
	{DLG_BTN_SM, "11", 6, 'b'},
	{DLG_BTN_SM, "12", 6, 'c'},
	{DLG_BTN_SM, "13", 6, 'd'},
	{DLG_BTN_SM, "14", 6, 'e'},
	{DLG_BTN_SM, "15", 6, 'f'},
	//50
	{DLG_BTN_SM, "16", 6, 'g'},
	{DLG_BTN_REG, "Take", 6, 0},
	{DLG_BTN_REG, "Create", 0, 0},
	{DLG_BTN_REG, "Delete", 2, 0},
	{DLG_BTN_LG, "Race/Special", 0, 0},
	{DLG_BTN_REG, "Skill", 0, 0},
	{DLG_BTN_REG, "Name", 0, 0},
	{DLG_BTN_REG, "Graphic", 2, 0},
	{DLG_BTN_LG, "Bash Door", 3, 0},
	{DLG_BTN_LG, "Pick Lock", 3, 0},
	//60
	{DLG_BTN_REG, "Leave", 6, 0},
	{DLG_BTN_REG, "Steal", 6, 0},
	{DLG_BTN_REG, "Attack", 0, 0},
	{DLG_BTN_REG, "OK", 7, 0},
	{DLG_BTN_REG, "Yes", 5, 'y'},
	{DLG_BTN_REG, "No", 5, 'n'},
	{DLG_BTN_LG, "Step In", 0, 0},
	{DLG_BTN_HELP, " ", 0, '?'},
	{DLG_BTN_REG, "Record", 2, 'r'},
	{DLG_BTN_REG, "Climb", 6, 0},
	//70
	{DLG_BTN_REG, "Flee", 4, 0},
	{DLG_BTN_REG, "Onward", 2, 0},
	{DLG_BTN_REG, "Answer", 0, 0},
	{DLG_BTN_REG, "Drink", 5, 0},
	{DLG_BTN_LG, "Approach", 0, 0},
	{DLG_BTN_LG, "Mage Spells", 4, 0},
	{DLG_BTN_LG, "Priest Spells", 10, 0},
	{DLG_BTN_LG, "Advantages", 4, 0},
	{DLG_BTN_LG, "New Game", 0, 0},
	{DLG_BTN_REG, "Land", 6, 0},
	//80
	{DLG_BTN_REG, "Under", 6, 0},
	{DLG_BTN_REG, "Restore", 2, 0},
	{DLG_BTN_REG, "Restart", 1, 0},
	{DLG_BTN_REG, "Quit", 6, 0},
	{DLG_BTN_LG, "Save First", 4, 0},
	{DLG_BTN_LG, "Just Quit", 3, 0},
	{DLG_BTN_REG, "Rest", 6, 0},
	{DLG_BTN_REG, "Read", 4, 0},
	{DLG_BTN_REG, "Pull", 6, 0},
	{DLG_BTN_LG, "Alchemy", 4, 0},
	//90
	{DLG_BTN_SM, "17", 6, 'g'},
	{DLG_BTN_REG, "Push", 6, 0},
	{DLG_BTN_REG, "Pray", 6, 0},
	{DLG_BTN_REG, "Wait", 6, 0},
	{DLG_BTN_PUSH, "", 0, 0},
	{DLG_BTN_TRAIT, "", 0, 0},
	{DLG_BTN_TALL, "Delete", 0, 0},
	{DLG_BTN_TALL, "Graphic", 2, 0},
	{DLG_BTN_TALL, "Create", 0, 0},
	{DLG_BTN_REG, "Give", 4, 0},
	//100
	{DLG_BTN_REG, "Destroy", 2, 0},
	{DLG_BTN_REG, "Pay", 6, 0},
	{DLG_BTN_REG, "Free", 6, 0},
	{DLG_BTN_LG, "Next Tip", 3, 0},
	{DLG_BTN_REG, "Touch", 6, 0},
	{DLG_BTN_LG, "Select Icon", 7, 0},
	{DLG_BTN_LG, "Create/Edit", 3, 0},
	{DLG_BTN_LG, "Clear Special", 0, 0},
	{DLG_BTN_LG, "Edit Abilities", 0, 0},
	{DLG_BTN_REG, "Choose", 0, 0},
	//110
	{DLG_BTN_LG, "Go Back", 0, 0},
	{DLG_BTN_LG, "Create New", 5, 0},
	{DLG_BTN_LG, "General", 3, 0},
	{DLG_BTN_LG, "One Shots", 3, 0},
	{DLG_BTN_LG, "Affect PCs", 3, 0},
	{DLG_BTN_LG, "If-Thens", 3, 0},
	{DLG_BTN_LG, "Town Specs", 3, 0},
	{DLG_BTN_LG, "Out Specs", 4, 0},
	{DLG_BTN_LG, "Advanced", 4, 0},
	{DLG_BTN_LG, "Weapon Abil", 4, 0},
	//120
	{DLG_BTN_LG, "General Abil.", 6, 0},
	{DLG_BTN_LG, "NonSpell Use", 4, 0},
	{DLG_BTN_LG, "Spell Usable", 4, 0},
	{DLG_BTN_LG, "Reagents", 2, 0},
	{DLG_BTN_LG, "Missiles", 2, 0},
	{DLG_BTN_LG, "Abilities", 3, 0},
	{DLG_BTN_LG, "Pick Picture", 6, 0},
	{DLG_BTN_LG, "Animated", 5, 0},
	{DLG_BTN_REG, "Enter", 6, 0},
	{DLG_BTN_REG, "Burn", 6, 0},
	//130
	{DLG_BTN_REG, "Insert", 0, 0},
	{DLG_BTN_REG, "Remove", 4, 0},
	{DLG_BTN_REG, "Accept", 2, 0},
	{DLG_BTN_REG, "Refuse", 2, 0},
	{DLG_BTN_REG, "Open", 6, 0},
	{DLG_BTN_REG, "Close", 4, 0},
	{DLG_BTN_REG, "Sit", 6, 0},
	{DLG_BTN_REG, "Stand", 3, 0},
	{DLG_BTN_SM, "", 0, 0},
	{DLG_BTN_SM, "", 0, 0},
	//140
	{DLG_BTN_SM, "18", 0, 0},
	{DLG_BTN_SM, "19", 0, 0},
	{DLG_BTN_SM, "20", 0, 0},
	{DLG_BTN_SM, "", 0, 0}, // invisible button; text was "Invisible!"
	{DLG_BTN_SM, "", 0, 0},
	{DLG_BTN_SM, "", 0, 0},
	{DLG_BTN_SM, "", 0, 0},
	{DLG_BTN_SM, "", 0, 0},
	{DLG_BTN_SM, "", 0, 0},
	{DLG_BTN_SM, "", 0, 0},
	//150
	{DLG_BTN_LG, "Open File", 7, 0},
	{DLG_BTN_SM, " ", 0, 0},
};