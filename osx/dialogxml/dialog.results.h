/*
 *  dialog.results.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 06/06/09.
 *
 */

// Functions for allowing the dialog to have a result which can be of any type.

template<typename type> inline void cDialog::setResult(const type& val){
	result = val;
}

template<typename type> inline type cDialog::getResult(){
	return boost::any_cast<type>(result);
}
