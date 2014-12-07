/*
 *  special.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#ifndef BOE_DATA_SPECIAL_H
#define BOE_DATA_SPECIAL_H

#include <iosfwd>
#include "simpletypes.h"
#include "location.h"

namespace legacy { struct special_node_type; };

class cSpecial {
public:
	eSpecType type;
	short sd1;
	short sd2;
	short pic;
	short pictype;
	short m1;
	short m2;
	short m3;
	short ex1a;
	short ex1b;
	short ex1c;
	short ex2a;
	short ex2b;
	short ex2c;
	short jumpto;
	
	cSpecial();
	cSpecial& operator = (legacy::special_node_type& old);
	void writeTo(std::ostream& file);
};

struct pending_special_type {
	spec_num_t spec;
	eSpecCtx mode;
	unsigned char type; // 0 - scen, 1 - out, 2 - town
	location where;
	long long trigger_time;
};

struct node_properties_t {
	bool ex1a_choose, ex1b_choose, ex1c_choose, ex2a_choose, ex2b_choose, ex2c_choose;
	short sdf_label, msg_label, pic_label, jmp_label;
	node_properties_t() {}
	node_properties_t(std::initializer_list<std::function<void(node_properties_t)>>);
};

std::ostream& operator << (std::ostream& out, eSpecType& e);
std::istream& operator >> (std::istream& in, eSpecType& e);
const node_properties_t& operator* (eSpecType t);

#endif