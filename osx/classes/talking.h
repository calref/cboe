/*
 *  talking.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#ifndef BOE_DATA_TALKING_H
#define BOE_DATA_TALKING_H

#include <iosfwd>

namespace legacy {
	struct talking_record_type;
	struct talking_node_type;
};

class cPersonality {
public:
	std::string title;
	std::string look, name, job, dunno;
};

class cSpeech { // formerly talking_record_type
public:
	class cNode { // formerly talking_node_type
	public:
		short personality,type;
		char link1[4],link2[4];
		short extras[4];
		std::string str1, str2;
	};
	unsigned char strlens[200];
	cPersonality people[10];
	cNode talk_nodes[60];
	
	cSpeech& operator = (legacy::talking_record_type& old);
	void writeTo(std::ostream& file);
};

#endif