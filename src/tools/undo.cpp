/*
 *  undo.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 29/05/09.
 *
 */

#include "undo.h"

cUndoList::cUndoList(){
	lastSave = cur = theList.begin();
}

size_t cUndoList::maxUndoSize = 0;

//TODO: These functions should have error checking to ensure they do not access an out of bounds action
void cUndoList::undo(){
	(*cur)->undo();
	cur--;
}

void cUndoList::redo(){
	cur++;
	(*cur)->redo();
}

void cUndoList::save(){
	lastSave = cur;
}

void cUndoList::revert(){
	while(cur != lastSave) undo();
}

void cUndoList::add(cAction* what){
	theList.push_back(what);
	num_actions++;
	while(num_actions > maxUndoSize) theList.pop_front(), num_actions--;
}
