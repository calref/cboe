/*
 *  creatlist.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>


#include "classes.h"
#include "oldstructs.h"

__attribute__((deprecated))
cPopulation& cPopulation::operator = (legacy::creature_list_type old){
	for(int i = 0; i < 60; i++)
		dudes[i] = old.dudes[i];
	which_town = old.which_town;
	friendly = old.friendly;
	return *this;
}
