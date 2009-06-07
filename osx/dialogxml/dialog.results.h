/*
 *  dialog.results.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 06/06/09.
 *
 */

// Functions for allowing the dialog to have a result which can be of any type.
// Be sure to call setResult and getResult with the exact same template parameters!
// If you don't, bad things could happen. (Especially if you get the result as
// a pointer when it was set as an integral type.)
// Note however that for non-integral types it's safe to set it as type pointer to X and then
// get it as type X or vice versa, since setting it as type X stores the address anyway.

template<typename type> inline void cDialog::setResult(const type& val){
	const type* ptr = &val;
	result = reinterpret_cast<const unsigned long long&>(ptr);
}

template<> inline void cDialog::setResult<char>(const char& val){
	result = reinterpret_cast<const unsigned long long&>(val);
}

template<> inline void cDialog::setResult<signed char>(const signed char& val){
	result = reinterpret_cast<const unsigned long long&>(val);
}

template<> inline void cDialog::setResult<unsigned char>(const unsigned char& val){
	result = reinterpret_cast<const unsigned long long&>(val);
}

template<> inline void cDialog::setResult<signed short>(const signed short& val){
	result = reinterpret_cast<const unsigned long long&>(val);
}

template<> inline void cDialog::setResult<unsigned short>(const unsigned short& val){
	result = reinterpret_cast<const unsigned long long&>(val);
}

template<> inline void cDialog::setResult<signed int>(const signed int& val){
	result = reinterpret_cast<const unsigned long long&>(val);
}

template<> inline void cDialog::setResult<unsigned int>(const unsigned int& val){
	result = reinterpret_cast<const unsigned long long&>(val);
}

template<> inline void cDialog::setResult<signed long>(const signed long& val){
	result = reinterpret_cast<const unsigned long long&>(val);
}

template<> inline void cDialog::setResult<unsigned long>(const unsigned long& val){
	result = reinterpret_cast<const unsigned long long&>(val);
}

template<> inline void cDialog::setResult<signed long long>(const signed long long& val){
	result = reinterpret_cast<const unsigned long long&>(val);
}

template<> inline void cDialog::setResult<unsigned long long>(const unsigned long long& val){
	result = reinterpret_cast<const unsigned long long&>(val);
}

template<> inline void cDialog::setResult<bool>(const bool& val){
	result = reinterpret_cast<const unsigned long long&>(val);
}

template<typename type> inline type& cDialog::getResult(){
	return *reinterpret_cast<type*>(result);
}

template<> inline char& cDialog::getResult<char>(){
	return reinterpret_cast<char&>(result);
}

template<> inline signed char& cDialog::getResult<signed char>(){
	return reinterpret_cast<signed char&>(result);
}

template<> inline unsigned char& cDialog::getResult<unsigned char>(){
	return reinterpret_cast<unsigned char&>(result);
}

template<> inline signed short& cDialog::getResult<signed short>(){
	return reinterpret_cast<signed short&>(result);
}

template<> inline unsigned short& cDialog::getResult<unsigned short>(){
	return reinterpret_cast<unsigned short&>(result);
}

template<> inline signed int& cDialog::getResult<signed int>(){
	return reinterpret_cast<signed int&>(result);
}

template<> inline unsigned int& cDialog::getResult<unsigned int>(){
	return reinterpret_cast<unsigned int&>(result);
}

template<> inline signed long& cDialog::getResult<signed long>(){
	return reinterpret_cast<signed long&>(result);
}

template<> inline unsigned long& cDialog::getResult<unsigned long>(){
	return reinterpret_cast<unsigned long&>(result);
}

template<> inline signed long long& cDialog::getResult<signed long long>(){
	return reinterpret_cast<signed long long&>(result);
}

template<> inline unsigned long long& cDialog::getResult<unsigned long long>(){
	return reinterpret_cast<unsigned long long&>(result);
}

template<> inline bool& cDialog::getResult<bool>(){
	return reinterpret_cast<bool&>(result);
}