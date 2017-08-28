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

class cJob {
public:
	cJob() : status(eQuestStatus::AVAILABLE), start(0), source(-1) {}
	explicit cJob(int start, int source = -1) : status(eQuestStatus::STARTED), start(start), source(source) {}
	eQuestStatus status;
	int start; // the day the quest was started; used for quests with relative deadlines
	int source; // if gotten from a job board, this is the number of the job board; otherwise -1
};

std::istream& operator>>(std::istream& in, eQuestStatus& type);
std::ostream& operator<<(std::ostream& out, eQuestStatus type);

#endif
