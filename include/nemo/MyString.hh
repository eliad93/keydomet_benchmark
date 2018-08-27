/*
 * MyString.h
 *
 *  Created on: Aug 27, 2018
 *      Author: benise
 */

#include <iostream>
#include <string.h>
#include <assert.h>

#ifndef MYSTRING_H_
#define MYSTRING_H_

using std::string;
using std::ostream;

class MyString{

private:

	char* _str;
	size_t _size;

public:

	MyString() :
		_str(NULL),
		_size(0) {}

	MyString(const string& s) :
		_str(NULL),
		_size(0){
		if(s.empty()){
			return;
		}
		_str = new char[s.length()+1];
		_size = sprintf(_str, s.c_str());
	}

	MyString(const char* s) :
		_str(NULL),
		_size(0){
		if(s == NULL){
			return;
		}
		_str = new char[strlen(s)+1];
		_size = sprintf(_str, s);
	}

	size_t size() const noexcept {
		assert((_size==0 && _str==NULL) || (_size>0 && _str != NULL));
		return strlen(_str);
	}

	size_t length() const noexcept {
		assert((_size==0 && _str==NULL) || (_size>0 && _str != NULL));
		return strlen(_str);
	}


	void reverse (){
		if(_size == 0){
			assert(_str == NULL);
			return;
		}
	    for (size_t i=0; i<_size/2; i++){
	    	_str[i]^=_str[_size-i-1];
	    	_str[_size-i-1]^=_str[i];
	    	_str[i]^=_str[_size-i-1];
	    }
	}

	void clear() noexcept{
		delete[] _str;
		_str = NULL;
		_size = 0;
	}

	bool empty() const noexcept{
		assert((_size==0 && _str==NULL) || (_size>0 && _str != NULL));
		return (_size == 0);
	}

	const char* c_str() const noexcept{
		return _str;
	}

	~MyString(){
		delete[] _str;
	}

	friend ostream& operator<< (ostream& os, const MyString& str);
};

ostream& operator<< (ostream& os, const MyString& str){
	return os << str.c_str();
}

#endif /* MYSTRING_H_ */
