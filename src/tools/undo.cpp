/*
 *  undo.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 29/05/09.
 *
 */

#include "undo.hpp"

cAction::~cAction() {}

void cAction::undo() {
	if(done && undo_me())
		done = false;
}

void cAction::redo() {
	if(!done && redo_me())
		done = true;
}

cUndoList::cUndoList(){
	lastSave = cur = theList.begin();
}

size_t cUndoList::maxUndoSize = 50;

void cUndoList::undo(){
	if(noUndo()) return;
	(*cur)->undo();
	cur++;
}

void cUndoList::redo(){
	if(noRedo()) return;
	cur--;
	(*cur)->redo();
}

std::string cUndoList::undoName() const {
	if(noUndo()) return "";
	return (*cur)->getActionName();
}

std::string cUndoList::redoName() const {
	if(noRedo()) return "";
	auto next = std::next(cur);
	return (*next)->getActionName();
}

bool cUndoList::noUndo() const {
	return cur == theList.end();
}

bool cUndoList::noRedo() const {
	return cur == theList.begin();
}

void cUndoList::save(){
	lastSave = cur;
}

void cUndoList::revert(){
	while(cur != lastSave) undo();
}

void cUndoList::clear() {
	theList.clear();
}

void cUndoList::add(action_ptr what){
	if(!what) return;
	theList.erase(theList.begin(), cur);
	theList.push_front(what);
	num_actions++;
	while(num_actions > maxUndoSize) {
		theList.pop_back();
		num_actions--;
	}
	cur = theList.begin();
}
