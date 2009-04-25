/*
 *  talking.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

namespace legacy {
	struct talking_record_type;
	struct talking_node_type;
};

class cSpeech { // formerly talking_record_type
public:
	class cNode { // formerly talking_node_type
	public:
		short personality,type;
		char link1[4],link2[4];
		short extras[4];
	};
	unsigned char strlens[200];
	char talk_strs[170][256];
	cNode talk_nodes[60];
	
	cSpeech& operator = (legacy::talking_record_type& old);
};