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
#include "location.hpp"

class cAction {
	std::string actname;
	bool done = true;
protected:
	/// Construct a named action
	/// @param name The name of the action to show in the Edit menu
	cAction(std::string name) : actname(name) {}
public:
	/// Undoes this action if it has not already been undone.
	/// If it has already been undone, does nothing.
	void undo();
	/// Redoes this action if it has been undone.
	/// If it has not been undone, does nothing.
	void redo();
	/// Cchecks to see whether the action has been undone.
	/// @return false if it the action has been undone.
	bool isDone() {return done;};
	/// Get the name of this action for display in the Edit menu.
	/// @return The action name
	std::string getActionName() {return actname;}
	virtual ~cAction();
private:
	/// Implementation of @ref undo().
	/// Must be overridden by subclasses to undo the action.
	/// It can assume that the action has not been undone yet.
	/// @return whether the undo succeeded.
	virtual bool undo_me() = 0;
	/// Implementation of @ref redo().
	/// Must be overridden by subclasses to redo the action.
	/// It can assume that the action has been undone already.
	/// @return whether the redo succeeded.
	virtual bool redo_me() = 0;
};

using action_ptr = std::shared_ptr<cAction>;

class cUndoList {
	std::list<action_ptr> theList;
	std::list<action_ptr>::iterator cur, lastSave;
public:
	/// Construct a new undo list.
	cUndoList();
	/// Undoes the current action and decrements the cur pointer.
	void undo();
	/// Increments the cur pointer and redoes the current action.
	void redo();
	/// Sets the last saved action to the current action.
	void save(); 
	/// Undoes all actions back to (but excluding) the last saved action.
	void revert();
	/// Clears the undo list.
	void clear();
	/// Check whether there's an action to undo.
	/// @return true if there are no actions to undo.
	bool noUndo() const;
	/// Check whether there's an action to redo.
	/// @return true if there are no actions to redo.
	bool noRedo() const;
	/// Get the action name of the next action to undo.
	/// @return The name of the action to be undone.
	std::string undoName() const;
	/// Get the action name of the next action to redo.
	/// @return The name of the action to be redone.
	std::string redoName() const;
	/// Add a new action to the undo list.
	/// Any actions at the end of the list that have been undone are dropped.
	/// May also drop actions from the front of the list.
	/// @param what The action to add.
	void add(action_ptr what);
	/// Controls the maximum size of the undo list.
	/// If more actions are added after this point, actions will be
	/// dropped from the front of the list.
	/// Actions are only dropped when adding new actions.
	static size_t maxUndoSize;
};

// As a special convention, I will prefix non-abstract action classes with 'a' instead of 'c'

#endif
