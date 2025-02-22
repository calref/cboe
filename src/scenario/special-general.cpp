//
//  special-general.cpp
//  BoE
//
//  Created by Celtic Minstrel on 2025-02-18.
//

#include "special.hpp"

// Note: If adding a new node type below, be sure to adjust the end point here too.
node_category_info_t CAT_GENERAL{eSpecType::NONE, eSpecType::STR_BUF_TO_SIGN};

namespace{
	node_properties_t S_NONE = node_builder_t(eSpecType::NONE);
	node_properties_t S_SETFLAG = node_builder_t(eSpecType::SET_SDF)
		.sdf()
		.msg();
	node_properties_t S_INCFLAG = node_builder_t(eSpecType::INC_SDF)
		.sdf()
		.msg()
		.ex1b(eSpecPicker::TOGGLE);
	node_properties_t S_MSG = node_builder_t(eSpecType::DISPLAY_MSG)
		.msg();
	node_properties_t S_SHOP = node_builder_t(eSpecType::ENTER_SHOP)
		.msg1(eSpecPicker::MSG_SINGLE)
		.ex1a(STRT_SHOP)
		.ex1b(STRT_COST_ADJ)
		.jump(eSpecPicker::NONE);
	node_properties_t S_MSG_SM = node_builder_t(eSpecType::DISPLAY_SM_MSG)
		.msg();
	node_properties_t S_FLIPFLAG = node_builder_t(eSpecType::FLIP_SDF)
		.sdf()
		.msg();
	node_properties_t S_RANDFLAG = node_builder_t(eSpecType::SDF_RANDOM)
		.sdf()
		.msg();
	node_properties_t S_ADDFLAG = node_builder_t(eSpecType::SDF_ADD)
		.sdf()
		.msg();
	node_properties_t S_SUBFLAG = node_builder_t(eSpecType::SDF_DIFF)
		.sdf()
		.msg();
	node_properties_t S_STORY = node_builder_t(eSpecType::STORY_DIALOG)
		.msg1(eSpecPicker::MSG_SINGLE)
		.field_pair(eSpecField::MSG2, eSpecField::MSG3, eSpecPicker::MSG_SEQUENCE_VAR)
		.pic();
	node_properties_t S_PREVENT = node_builder_t(eSpecType::CANT_ENTER)
		.msg()
		.ex1a(eSpecPicker::TOGGLE)
		.ex2a(eSpecPicker::TOGGLE);
	node_properties_t S_TIME = node_builder_t(eSpecType::CHANGE_TIME)
		.msg();
	node_properties_t S_TIMER = node_builder_t(eSpecType::SCEN_TIMER_START)
		.msg()
		.ex1b(+eSpecPicker::NODE);
	node_properties_t S_SND = node_builder_t(eSpecType::PLAY_SOUND)
		.ex1a(eSpecPicker::SOUND)
		.ex1b(eSpecPicker::TOGGLE);
	node_properties_t S_HORSE_OWN = node_builder_t(eSpecType::CHANGE_HORSE_OWNER)
		.msg()
		.ex1a(eSpecPicker::HORSE)
		.ex2a(eSpecPicker::TOGGLE);
	node_properties_t S_BOAT_OWN = node_builder_t(eSpecType::CHANGE_BOAT_OWNER)
		.msg()
		.ex1a(eSpecPicker::BOAT)
		.ex2a(eSpecPicker::TOGGLE);
	node_properties_t S_TOWN_VIS = node_builder_t(eSpecType::SET_TOWN_VISIBILITY)
		.msg()
		.ex1a(STRT_TOWN)
		.ex2a(eSpecPicker::TOGGLE);
	node_properties_t S_EVENT = node_builder_t(eSpecType::MAJOR_EVENT_OCCURRED)
		.msg()
		.ex1a(eSpecPicker::EVENT);
	node_properties_t S_FORCEGIVE = node_builder_t(eSpecType::FORCED_GIVE)
		.msg()
		.ex1a(STRT_ITEM)
		.ex1b(eSpecPicker::NODE);
	node_properties_t S_BUYTYPE = node_builder_t(eSpecType::BUY_ITEMS_OF_TYPE)
		.msg()
		.ex1a(eSpecPicker::ITEM_CLASS)
		.ex1b(eSpecPicker::NODE);
	node_properties_t S_GLOBAL = node_builder_t(eSpecType::CALL_GLOBAL)
		.jump(+eSpecPicker::NODE);
	node_properties_t S_SETROW = node_builder_t(eSpecType::SET_SDF_ROW);
	node_properties_t S_COPYFLAG = node_builder_t(eSpecType::COPY_SDF)
		.sdf()
		.sdf(eSpecField::EX1A, eSpecField::EX1B);
	node_properties_t S_PICTURE = node_builder_t(eSpecType::DISPLAY_PICTURE)
		.msg1(eSpecPicker::MSG_SINGLE)
		.ex1a(PIC_FULL);
	node_properties_t S_REST = node_builder_t(eSpecType::REST)
		.msg();
	node_properties_t S_MSG_TITLE = node_builder_t(eSpecType::TITLED_MSG)
		.msg()
		.msg3(eSpecPicker::MSG_SINGLE)
		.pic();
	node_properties_t S_END_SCEN = node_builder_t(eSpecType::END_SCENARIO);
	node_properties_t S_SETPTR = node_builder_t(eSpecType::SET_POINTER)
		.sdf()
		.ex1a(eSpecPicker::POINTER);
	node_properties_t S_CAMPFLAG = node_builder_t(eSpecType::SET_CAMP_FLAG)
		.sdf()
		.msg1(+eSpecPicker::MSG_SINGLE)
		.ex2a(eSpecPicker::TOGGLE);
	node_properties_t S_DEBUG = node_builder_t(eSpecType::PRINT_NUMS)
		.sdf()
		.pict(STRT_DEBUG_PRINT);
	node_properties_t S_MULFLAG = node_builder_t(eSpecType::SDF_TIMES)
		.sdf()
		.msg();
	node_properties_t S_DIVFLAG = node_builder_t(eSpecType::SDF_DIVIDE)
		.sdf()
		.sdf(eSpecField::EX1C, eSpecField::EX2C)
		.msg();
	node_properties_t S_EXPFLAG = node_builder_t(eSpecType::SDF_POWER)
		.sdf()
		.msg();
	node_properties_t S_TERCHANGE = node_builder_t(eSpecType::CHANGE_TER)
		.msg()
		.loc(eSpecField::EX1A, eSpecField::EX1B)
		.ex2a(STRT_TER);
	node_properties_t S_TERSWAP = node_builder_t(eSpecType::SWAP_TER)
		.msg()
		.loc(eSpecField::EX1A, eSpecField::EX1B)
		.ex2a(STRT_TER)
		.ex2b(STRT_TER);
	node_properties_t S_TERTRANS = node_builder_t(eSpecType::TRANS_TER)
		.msg()
		.loc(eSpecField::EX1A, eSpecField::EX1B);
	node_properties_t S_BUF_CLEAR = node_builder_t(eSpecType::CLEAR_BUF);
	node_properties_t S_BUF_ADDSTR = node_builder_t(eSpecType::APPEND_STRING)
		.ex1a(eSpecPicker::MSG_SINGLE)
		.pict(eSpecPicker::TOGGLE);
	node_properties_t S_BUF_ADDNUM = node_builder_t(eSpecType::APPEND_NUM)
		.pict(eSpecPicker::TOGGLE);
	node_properties_t S_BUF_ADDMONST = node_builder_t(eSpecType::APPEND_MONST)
		.ex1a(STRT_MONST)
		.pict(eSpecPicker::TOGGLE);
	node_properties_t S_BUF_ADDITEM = node_builder_t(eSpecType::APPEND_ITEM)
		.ex1a(STRT_ITEM)
		.pict(eSpecPicker::TOGGLE)
		.ex1b(eSpecPicker::TOGGLE);
	node_properties_t S_BUF_ADDTER = node_builder_t(eSpecType::APPEND_TER)
		.ex1a(STRT_TER)
		.pict(eSpecPicker::TOGGLE);
	node_properties_t S_PAUSE = node_builder_t(eSpecType::PAUSE);
	node_properties_t S_TALK = node_builder_t(eSpecType::START_TALK)
		.pict(PIC_TALK)
		.ex1b(STRT_MONST)
		.jump(eSpecPicker::NONE);
	node_properties_t S_QUEST = node_builder_t(eSpecType::UPDATE_QUEST)
		.msg()
		.ex1a(STRT_QUEST)
		.ex1b(STRT_QUEST_STATUS);
	node_properties_t S_BUF_SWAP = node_builder_t(eSpecType::SWAP_STR_BUF)
		.msg();
	node_properties_t S_ALTER_SIGN = node_builder_t(eSpecType::STR_BUF_TO_SIGN)
		.msg1(eSpecPicker::MSG_SINGLE);
}
