/*
 *  special.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#ifndef SPECIAL_H
#define SPECIAL_H

#include <iosfwd>

namespace legacy { struct special_node_type; };

class cSpecial {
public:
	short type;
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
	eSpecContext mode;
	unsigned char type; // 0 - scen, 1 - out, 2 - town
	location where;
	long long trigger_time;
};

#endif