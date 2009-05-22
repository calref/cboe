/*
 *  special.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>


#include "classes.h"
#include "oldstructs.h"

cSpecial::cSpecial(){
	type = 0;
	sd1 = -1;
	sd2 = -1;
	pic = -1;
	m1 = -1;
	m2 = -1;
	ex1a = -1;
	ex1b = -1;
	ex2a = -1;
	ex2b = -1;
	jumpto = -1;
}

__attribute__((deprecated))
cSpecial& cSpecial::operator = (legacy::special_node_type& old){
	type = old.type;
	sd1 = old.sd1;
	sd2 = old.sd2;
	pic = old.pic;
	m1 = old.m1;
	m2 = old.m2;
	ex1a = old.ex1a;
	ex1b = old.ex1b;
	ex2a = old.ex2a;
	ex2b = old.ex2b;
	jumpto = old.jumpto;
	return *this;
}
