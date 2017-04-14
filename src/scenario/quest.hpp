//
//  quest.hpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-13.
//
//

#ifndef BoE_QUEST_HPP
#define BoE_QUEST_HPP

enum class eQuestStatus {AVAILABLE, STARTED, COMPLETED, FAILED};

class cQuest {
public:
	short flags = 0; // 0 - absolute deadline, 1 - relative to when quest started, +10 - start quest when scenario starts
	short deadline = -1;
	short event = -1; // if this event occurs before the deadline, then the deadline is waived
	short xp = 0, gold = 0; // automatically award this much XP and gold to the party when the quest is marked complete
	short bank1 = -1, bank2 = -1; // which job bank(s) this quest is in; -1 for none
	std::string name;
	std::string descr;
};

std::istream& operator>>(std::istream& in, eQuestStatus& type);
std::ostream& operator<<(std::ostream& out, eQuestStatus type);

#endif
