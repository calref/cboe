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

#ifndef BOE_UNDO_HPP
#define BOE_UNDO_HPP

#include <list>
#include <memory>
#include <string>

class cAction {
	std::string actname;
protected:
	bool done = true;
public:
	cAction(std::string name) : actname(name) {}
	virtual void undo() = 0; ///< Undoes this action if it has not already been undone
	virtual void redo() = 0; ///< Redoes this action if it has been undone
	bool isDone() {return done;}; ///< checks to see whether the action has been undone; returns false if it has
	std::string getActionName() {return actname;} ///< returns the name of this action for display in the Edit menu
	virtual ~cAction();
};

using action_ptr = std::shared_ptr<cAction>;

class cUndoList {
	std::list<action_ptr> theList;
	std::list<action_ptr>::iterator cur, lastSave;
	size_t num_actions = 0;
public:
	cUndoList();
	void undo(); ///< Undoes the current action and decrements the cur pointer
	void redo(); ///< Increments the cur pointer and redoes the current action
	void save(); ///< Sets the last saved action to the current action
	void revert(); ///< Undoes all actions back to (but excluding) the last saved action
	void clear(); ///< Clears the list
	bool noUndo() const; ///< Check whether there's an action to undo
	bool noRedo() const; ///< Check whether there's an action to redo
	std::string undoName() const; ///< Get the action name of the next action to undo
	std::string redoName() const; ///< Get the action name of the next action to redo
	void add(action_ptr what);
	static size_t maxUndoSize;
};

// As a special convention, I will prefix action classes with 'a' instead of 'c'

#endif
