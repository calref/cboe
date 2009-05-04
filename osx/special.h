/*
 *  special.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

namespace legacy { struct special_node_type; };

class cSpecial {
public:
	short type;
	short sd1;
	short sd2;
	short pic;
	short m1;
	short m2;
	short ex1a;
	short ex1b;
	short ex2a;
	short ex2b;
	short jumpto;
	
	cSpecial();
	cSpecial& operator = (legacy::special_node_type& old);
	void writeTo(ostream& file);
};
