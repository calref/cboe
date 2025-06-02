/*
 *  undo.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 29/05/09.
 *
 */

#include "undo.hpp"

// Uncomment this line when debugging undo/redo operations
#define DEBUG_UNDO true

#ifdef DEBUG_UNDO
#define UNDO_LOG(x) LOG(x);
#define UNDO_LOG_VALUE(x) LOG_VALUE(x);
#else
#define UNDO_LOG(x) ;
#define UNDO_LOG_VALUE(x) ;
#endif

cAction::~cAction() {}

void cAction::undo() {
	UNDO_LOG("Undoing " + actname);
	if(done && reversed ? redo_me() : undo_me())
		done = false;
}

void cAction::redo() {
	UNDO_LOG("Redoing " + actname);
	if(!done && reversed ? undo_me() : redo_me())
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
	auto next = std::prev(cur);
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
	UNDO_LOG("Performing " + what->getActionName());
	theList.erase(theList.begin(), cur);
	theList.push_front(what);
	while(theList.size() > maxUndoSize) {
		theList.pop_back();
	}
	cur = theList.begin();
}
