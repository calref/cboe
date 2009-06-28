/*
 *  classes.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 * This file simply includes all the classes in an order that satisfies all dependencies.
 *
 */

#include "simpletypes.h"
#include "location.h"
#include "terrain.h"
#include "vehicle.h"
#include "item.h"
#include "special.h"
#include "monster.h"
#include "talking.h"
#include "town.h"
#include "regtown.h"
#include "tmpltown.h"
#include "outdoors.h"
#include "scenario.h"
#include "pc.h"
#include "creatlist.h"
#include "party.h"
#include "universe.h"

extern cScenario scenario;
template<class type> type cOutdoors::cWandering::get(m_num_t who,bool hostile,type cMonster::* what){
	if(hostile) return scenario.scen_monsters[monst[who]].*what;
	return scenario.scen_monsters[friendly[who]].*what;
}
