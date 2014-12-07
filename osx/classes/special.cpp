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
	type = eSpecType::NONE;
	sd1 = -1;
	sd2 = -1;
	pic = -1;
	pictype = 4;
	m1 = -1;
	m2 = -1;
	ex1a = -1;
	ex1b = -1;
	ex2a = -1;
	ex2b = -1;
	jumpto = -1;
}

cSpecial& cSpecial::operator = (legacy::special_node_type& old){
	type = (eSpecType)old.type;
	sd1 = old.sd1;
	sd2 = old.sd2;
	pic = old.pic;
	// Large dialogs with 36x36 dialog graphics
	if(old.type == 55 || old.type == 58 || old.type == 189)
		pic -= 700;
	// Large dialogs with monster graphics
	else if(old.type == 57 || old.type == 60)
		pic -= 400;
	m1 = old.m1;
	m2 = old.m2;
	ex1a = old.ex1a;
	ex1b = old.ex1b;
	ex2a = old.ex2a;
	ex2b = old.ex2b;
	jumpto = old.jumpto;
	return *this;
}

std::ostream& operator << (std::ostream& out, eSpecType& e) {
	return out << (int) e;
}

// TODO: This should probably understand symbolic names as well?
std::istream& operator >> (std::istream& in, eSpecType& e) {
	int i;
	in >> i;
	e = (eSpecType) i;
	if(getNodeCategory(e) == eSpecCat::INVALID)
		e = eSpecType::ERROR;
	return in;
}
