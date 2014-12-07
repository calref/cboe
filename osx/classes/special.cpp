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
	m1 = old.m1;
	m2 = old.m2;
	ex1a = old.ex1a;
	ex1b = old.ex1b;
	ex2a = old.ex2a;
	ex2b = old.ex2b;
	jumpto = old.jumpto;
	// Now apply any needed conversions.
	switch(old.type) {
		case 55: case 58: case 189: // Large dialogs with 36x36 dialog graphics
			pic -= 700;
			break;
		case 57: case 60: // Large dialogs with monster graphics
			pic -= 400;
			break;
			// TODO: Originally the block nodes supported messages; the new version doesn't.
			// (Will probably need to make special nodes a dynamic vector before fixing this.)
		case 7: case 8: case 9: case 10: // out, town, combat, look block
			type = eSpecType::IF_CONTEXT;
			ex1b = ex1a;
			if(old.type == 7) ex1a = (int) eSpecCtx::OUT_MOVE;
			if(old.type == 8) ex1a = (int) eSpecCtx::TOWN_MOVE;
			if(old.type == 9) ex1a = (int) eSpecCtx::COMBAT_MOVE;
			if(old.type == 10) ex1a = 100;
			break;
		case 24: // ritual of sanctification
			type = eSpecType::IF_CONTEXT;
			ex1c = jumpto;
			jumpto = ex1b;
			ex1a = (int) eSpecCtx::TARGET;
			ex1b = 108; // Spell ID for ritual of sanctification, as seen in cast_town_spell()
			break;
	}
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
