/*
 *  dlogutil.buttons.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 07/06/09.
 *
 */

#define BTNS_DEFINED
#include "dialogxml/dialogs/3choice.hpp"

/// @file
/// Preset button specifications for cThreeChoice

cKey no_key = {true,key_none,mod_none};

/// A list of preset button types.
bbtt basic_buttons[71] = {
	{BTN_DONE, " ", {true,key_enter,mod_none}, "Done"},
	{BTN_REG, "OK", {true,key_enter,mod_none}},
	{BTN_REG, "Yes", {false,'y',mod_none}},
	{BTN_REG, "No", {false,'n',mod_none}},
	{BTN_REG, "Ask", no_key},
	{BTN_REG, "Keep", {false,'k',mod_none}},
	{BTN_REG, "Cancel", {true,key_esc,mod_none}},
	{BTN_REG, "Buy", no_key},
	{BTN_REG, "Enter", no_key},
	{BTN_REG, "Leave", no_key},
	{BTN_REG, "Get", {false,'g',mod_none}},
	{BTN_REG, "1", {false,'1',mod_none}},
	{BTN_REG, "2", {false,'2',mod_none}},
	{BTN_REG, "3", {false,'3',mod_none}},
	{BTN_REG, "4", {false,'4',mod_none}},
	{BTN_REG, "5", {false,'5',mod_none}},
	{BTN_REG, "6", {false,'6',mod_none}},
	{BTN_REG, "Cast", no_key},
	{BTN_REG, "Save", no_key},
	{BTN_REG, "Take", no_key},
	{BTN_REG, "Stay", no_key},
	{BTN_REG, "Steal", no_key},
	{BTN_REG, "Attack", no_key},
	{BTN_LG, "Step In", no_key},
	{BTN_REG, "Climb", no_key},
	{BTN_REG, "Flee", no_key},
	{BTN_REG, "Onward", no_key},
	{BTN_REG, "Answer", no_key},
	{BTN_REG, "Drink", no_key},
	{BTN_LG, "Approach", no_key},
	{BTN_REG, "Land", no_key},
	{BTN_REG, "Under", no_key},
	{BTN_REG, "Quit", no_key},
	{BTN_REG, "Rest", no_key},
	{BTN_REG, "Read", no_key},
	{BTN_REG, "Pull", no_key},
	{BTN_REG, "Push", no_key},
	{BTN_REG, "Pray", no_key},
	{BTN_REG, "Wait", no_key},
	{BTN_REG, "Give", no_key},
	{BTN_REG, "Destroy", no_key},
	{BTN_REG, "Pay", no_key},
	{BTN_REG, "Free", no_key},
	{BTN_REG, "Touch", no_key},
	{BTN_REG, "Burn", no_key},
	{BTN_REG, "Insert", no_key},
	{BTN_REG, "Remove", no_key},
	{BTN_REG, "Accept", no_key},
	{BTN_REG, "Refuse", no_key},
	{BTN_REG, "Open", no_key},
	{BTN_REG, "Close", no_key},
	{BTN_REG, "Sit", no_key},
	{BTN_REG, "Stand", no_key},
	{BTN_LEFT, " ", {true,key_left,mod_none}, "Left Arrow"},
	{BTN_RIGHT, " ", {true,key_right,mod_none}, "Right Arrow"},
	{BTN_UP, " ", {true,key_up,mod_none}, "Up Arrow"},
	{BTN_DOWN, " ", {true,key_down,mod_none}, "Down Arrow"},
	{BTN_REG, "Sell", no_key},
	{BTN_REG, "Identify", no_key},
	{BTN_REG, "Enchant", no_key},
	{BTN_REG, "Train", no_key},
	{BTN_LG, "Heal Party", no_key},
	{BTN_LG, "Bash Door", no_key},
	{BTN_LG, "Pick Lock", no_key},
	{BTN_REG, "Record", {false,'r',mod_none}},
	{BTN_REG, "Climb", no_key},
	{BTN_REG, "Restore", no_key},
	{BTN_REG, "Restart", no_key},
	{BTN_REG, "Create", no_key},
	{BTN_REG, "Choose", no_key},
	{BTN_LG, "Go Back", no_key},
};
