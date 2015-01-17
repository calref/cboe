/*
 *  talking.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "classes.h"
#include "oldstructs.h"

void cSpeech::append(legacy::talking_record_type& old){
	int i,j;
	for(i = 0; i < 200; i++)
		strlens[i] = old.strlens[i];
	for(i = 0; i < 60; i++){
		talk_nodes[i].personality = old.talk_nodes[i].personality;
		talk_nodes[i].type = eTalkNode(old.talk_nodes[i].type);
		for(j = 0; j < 4; j++){
			talk_nodes[i].link1[j] = old.talk_nodes[i].link1[j];
			talk_nodes[i].link2[j] = old.talk_nodes[i].link2[j];
			talk_nodes[i].extras[j] = old.talk_nodes[i].extras[j];
		}
		// Now, convert data if necessary
		switch(old.talk_nodes[i].type) {
			case 9: case 10: // Spell shops TODO: Merge these by adding 100 if it's priest spells
				talk_nodes[i].extras[1] += 30;
				break;
		}
	}
}
