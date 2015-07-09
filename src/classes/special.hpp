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
#include <string>
#include <functional>
#include "simpletypes.hpp"
#include "location.hpp"

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
	void append(legacy::special_node_type& old);
	void writeTo(std::ostream& file, int n) const;
};

class cTimer {
public:
	long time = 0;
	short node_type = 0;
	short node = -1;
};

struct pending_special_type {
	spec_num_t spec;
	eSpecCtx mode;
	unsigned short type; // 0 - scen, 1 - out, 2 - town
	location where;
	long long trigger_time;
};

struct node_properties_t {
	eSpecType self;
	std::string opcode() const;
	std::string name() const, descr() const;
	std::string sdf1_lbl() const, sdf2_lbl() const, sdf1_hlp() const, sdf2_hlp() const;
	std::string msg1_lbl() const, msg2_lbl() const, msg3_lbl() const, msg1_hlp() const, msg2_hlp() const, msg3_hlp() const;
	std::string pic_lbl() const, pt_lbl() const, pic_hlp() const, pt_hlp() const;
	std::string ex1a_lbl() const, ex1b_lbl() const, ex1c_lbl() const, ex1a_hlp() const, ex1b_hlp() const, ex1c_hlp() const;
	std::string ex2a_lbl() const, ex2b_lbl() const, ex2c_lbl() const, ex2a_hlp() const, ex2b_hlp() const, ex2c_hlp() const;
	std::string jmp_lbl() const, jmp_hlp() const;
	char sd1_btn, sd2_btn, m1_btn, m2_btn, m3_btn, p_btn, pt_btn;
	char x1a_btn, x1b_btn, x1c_btn, x2a_btn, x2b_btn, x2c_btn;
	node_properties_t() {}
	node_properties_t(std::initializer_list<std::function<void(node_properties_t)>>);
};

const node_properties_t& operator* (eSpecType t);

#endif
