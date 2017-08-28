/*
 *  dlogutil.buttons.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 07/06/09.
 *
 */

#define BTNS_DEFINED
#include "3choice.hpp"

/// @file
/// Preset button specifications for cThreeChoice

/// A list of preset button types.
bbtt basic_buttons[71] = {
	{BTN_DONE, " ", {true,key_enter,mod_none}, "Done"},
	{BTN_REG, "OK", {true,key_enter,mod_none}},
	{BTN_REG, "Yes", {false,'y',mod_none}},
	{BTN_REG, "No", {false,'n',mod_none}},
	{BTN_REG, "Ask", {false,0,mod_none}},
	{BTN_REG, "Keep", {false,'k',mod_none}},
	{BTN_REG, "Cancel", {true,key_esc,mod_none}},
	{BTN_REG, "Buy", {false,0,mod_none}},
	{BTN_REG, "Enter", {false,0,mod_none}},
	{BTN_REG, "Leave", {true,key_enter,mod_none}},
	{BTN_REG, "Get", {false,'g',mod_none}},
	{BTN_REG, "1", {false,'1',mod_none}},
	{BTN_REG, "2", {false,'2',mod_none}},
	{BTN_REG, "3", {false,'3',mod_none}},
	{BTN_REG, "4", {false,'4',mod_none}},
	{BTN_REG, "5", {false,'5',mod_none}},
	{BTN_REG, "6", {false,'6',mod_none}},
	{BTN_REG, "Cast", {false,0,mod_none}},
	{BTN_REG, "Save", {false,0,mod_none}},
	{BTN_REG, "Take", {false,0,mod_none}},
	{BTN_REG, "Stay", {true,key_enter,mod_none}},
	{BTN_REG, "Steal", {false,0,mod_none}},
	{BTN_REG, "Attack", {false,0,mod_none}},
	{BTN_LG, "Step In", {false,0,mod_none}},
	{BTN_REG, "Climb", {false,0,mod_none}},
	{BTN_REG, "Flee", {false,0,mod_none}},
	{BTN_REG, "Onward", {false,0,mod_none}},
	{BTN_REG, "Answer", {false,0,mod_none}},
	{BTN_REG, "Drink", {false,0,mod_none}},
	{BTN_LG, "Approach", {false,0,mod_none}},
	{BTN_REG, "Land", {false,0,mod_none}},
	{BTN_REG, "Under", {false,0,mod_none}},
	{BTN_REG, "Quit", {false,0,mod_none}},
	{BTN_REG, "Rest", {false,0,mod_none}},
	{BTN_REG, "Read", {false,0,mod_none}},
	{BTN_REG, "Pull", {false,0,mod_none}},
	{BTN_REG, "Push", {false,0,mod_none}},
	{BTN_REG, "Pray", {false,0,mod_none}},
	{BTN_REG, "Wait", {false,0,mod_none}},
	{BTN_REG, "Give", {false,0,mod_none}},
	{BTN_REG, "Destroy", {false,0,mod_none}},
	{BTN_REG, "Pay", {false,0,mod_none}},
	{BTN_REG, "Free", {false,0,mod_none}},
	{BTN_REG, "Touch", {false,0,mod_none}},
	{BTN_REG, "Burn", {false,0,mod_none}},
	{BTN_REG, "Insert", {false,0,mod_none}},
	{BTN_REG, "Remove", {false,0,mod_none}},
	{BTN_REG, "Accept", {false,0,mod_none}},
	{BTN_REG, "Refuse", {false,0,mod_none}},
	{BTN_REG, "Open", {false,0,mod_none}},
	{BTN_REG, "Close", {false,0,mod_none}},
	{BTN_REG, "Sit", {false,0,mod_none}},
	{BTN_REG, "Stand", {false,0,mod_none}},
	{BTN_LEFT, " ", {true,key_left,mod_none}, "Left Arrow"},
	{BTN_RIGHT, " ", {true,key_right,mod_none}, "Right Arrow"},
	{BTN_UP, " ", {true,key_up,mod_none}, "Up Arrow"},
	{BTN_DOWN, " ", {true,key_down,mod_none}, "Down Arrow"},
	{BTN_REG, "Sell", {false,0,mod_none}},
	{BTN_REG, "Identify", {false,0,mod_none}},
	{BTN_REG, "Enchant", {false,0,mod_none}},
	{BTN_REG, "Train", {false,0,mod_none}},
	{BTN_LG, "Heal Party", {false,0,mod_none}},
	{BTN_LG, "Bash Door", {false,0,mod_none}},
	{BTN_LG, "Pick Lock", {false,0,mod_none}},
	{BTN_REG, "Record", {false,'r',mod_none}},
	{BTN_REG, "Climb", {false,0,mod_none}},
	{BTN_REG, "Restore", {false,0,mod_none}},
	{BTN_REG, "Restart", {false,0,mod_none}},
	{BTN_REG, "Create", {false,0,mod_none}},
	{BTN_REG, "Choose", {false,0,mod_none}},
	{BTN_LG, "Go Back", {false,0,mod_none}},
};