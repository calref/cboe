/*
 *  talking.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include "classes.h"
#include "oldstructs.h"

__attribute__((deprecated))
cSpeech& cSpeech::operator = (legacy::talking_record_type& old){
	int i,j;
	for(i = 0; i < 200; i++)
		strlens[i] = old.strlens[i];
	for(i = 0; i < 60; i++){
		talk_nodes[i].personality = old.talk_nodes[i].personality;
		talk_nodes[i].type = old.talk_nodes[i].type;
		for(j = 0; j < 4; j++){
			talk_nodes[i].link1[j] = old.talk_nodes[i].link1[j];
			talk_nodes[i].link2[j] = old.talk_nodes[i].link2[j];
			talk_nodes[i].extras[j] = old.talk_nodes[i].extras[j];
		}
	}
	return *this;
}
