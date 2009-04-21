/*
 *  scenario.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include "scenario.h"

cScenario::cItemStorage::cItemStorage() : ter_type(-1), property(0) {
	for(int i = 0; i < 10; i++)
		item_num[i] = -1;
	for(int i = 0; i < 10; i++)
		item_odds[i] = 0;
}

//{-1,{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{0,0,0,0,0,0,0,0,0,0},0}