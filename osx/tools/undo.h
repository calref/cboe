/*
 *  undo.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 29/05/09.
 *
 *  For use in creating an undo history in the scenario editor.
 *  Could also be used in the PC editor.
 *
 */

#include <list>

class cAction {
public:
	virtual void undo() = 0; // undoes this action if it has not already been undone
	virtual void redo() = 0; // redoes this action if it has been undone
	virtual bool isDone() = 0; // checks to see whether the action has been undone; returns false if it has
	virtual std::string getActionName() = 0; // returns the name of this action for display in the Edit menu
	virtual ~cAction();
};

class cUndoList {
	std::list<cAction*> theList;
	std::list<cAction*>::iterator cur, lastSave;
	size_t num_actions;
public:
	cUndoList();
	void undo(); // undoes the current action and decrements the cur pointer
	void redo(); // increments the cur pointer and redoes the current action
	void save(); // sets the last saved action to the current action
	void revert(); // undoes all actions back to (but excluding) the last saved action
	void add(cAction* what);
	static size_t maxUndoSize;
};

// As a special convention, I will prefix action classes with 'a' instead of 'c'
/*
class aEditMonster : public cAction {
	cMonster oldMonst, newMonst;
};

class aEditItem : public cAction {
	cItemRec oldItem, newItem;
};*/
