/*
 *  undo.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 29/05/09.
 *
 */

#include "undo.hpp"

cAction::~cAction() {}

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

bool cUndoList::noUndo() {
	return cur == theList.end();
}

bool cUndoList::noRedo() {
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
