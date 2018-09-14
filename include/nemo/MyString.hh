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
		strcpy(_str, s.c_str());
		_size = s.length(); 
		//_size = sprintf(_str, s.c_str());
	}

	MyString(const char* s) :
		_str(NULL),
		_size(0){
		if(s == NULL){
			return;
		}
		_str = new char[strlen(s)+1];
		strcpy(_str, s);
		_size = strlen(s); 
		//_size = sprintf(_str, s);
	}

	MyString(const MyString& s) :
		_str(NULL),
		_size(0){
		if(s.empty()){
			return;
		}
		_str = new char[s.size()+1];
		strcpy(_str, s.c_str());
		_size = s.length(); 
		//_size = sprintf(_str, s.c_str());
	}

	MyString& operator= (const MyString& str){
	if(this == &str){
		return *this; 
	}
	if(_str != NULL){
		delete[] _str; 
	}
	_str = new char[str.size()+1];
	strcpy(_str, str.c_str());
	_size = str.length(); 
	//_size = sprintf(_str, str.c_str());
	return *this; 
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

inline ostream& operator<< (ostream& os, const MyString& str){
	return os << str.c_str();
}

inline bool operator< (const MyString str1, const MyString str2){
	if(!str1.empty() && !str2.empty()){
		return strcmp(str1.c_str(), str2.c_str()) < 0;
	}
	return false; 
}



inline const bool operator== (const MyString s1,const MyString s2){
	if(s1.size() == s2.size() && s1.c_str() == s2.c_str()){
		return true;
	} 
	return false;
}

#endif /* MYSTRING_H_ */