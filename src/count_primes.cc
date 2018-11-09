#include <set>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>
#include "nemo/count_primes.hh"

using namespace std;
using namespace nemo;

std::ostream& operator<<(std::ostream& os, const kdmt128_t& val){
    os << val.msbs << val.lsbs;
    return os;
}

template<template<class, class...> class Container, class StrType, class... Args>
bool lookup(Container<StrType, Args...>& s, const string& key)
{
    auto iter = s.find(key);
    return iter != s.end();
}

// specialized for keydomet-ed containers, which take a keydomet for lookups
template<template<class, class...> class Container, class... Args>
bool lookup(Container<KeyDometStr64, Args...>& s, const string& key)
{
    auto hkey = makeFindKey(s, key);
    auto iter = s.find(hkey);
    return iter != s.end();
}

// bool lookup(set<KeyDometMyString64>& s, const string& key)
// {
//     auto hkey = makeFindKey(s, key);
//     auto iter = s.find(hkey);
//     return iter != s.end();
// }

namespace nemo {
    string getRandStr(size_t len){
        static mt19937 gen{random_device{}()};
        uniform_int_distribution<short> dis('A', 'z');
        string s;
        s.reserve(len);
        for (size_t i = 0; i < len; ++i){
            s += (char)dis(gen);
        }
        return s;
    }

    int getRandInt(int range){
        static mt19937 gen{random_device{}()};
        uniform_int_distribution<int> dis(0, range);
        return (int)dis(gen);
    }

vector<string> getInput(size_t keysNum, size_t keyLen)
{
    vector<string> input;
    for(int i=0; i<keysNum; i++){
        input.push_back(getRandStr(keyLen));
    }
    return input;
}

vector<string> getInputFromData(size_t keysNum, const vector<string>& data){
    int size = data.size();
    vector<string> input;
    for(int i=0; i<keysNum; i++){
        input.push_back(data[getRandInt(size-1)]);
    }
    return input;
}

vector<string> getInputFromParsedAndUnparsed(size_t keysNum, const vector<string>& parsed){
    vector<string> input90 = getPortion<string>(parsed, 0.9);
    vector<string> input10 = getPortion<string>(parsed, 0.1, true);

    int size10 = input10.size();
    int size90 = input90.size();
    vector<string> input;
    for(int i=0; i<keysNum/2; i++){
        input.push_back(input90[getRandInt(size90-1)]);
        input.push_back(input10[getRandInt(size10-1)]);
    }
        return input;
}


/////////////////////////////////////////////////////// FUNCTIONS FOR WSTRING COMPARE /////////////////////////////////////////////////
vector<wstring> getInputFromParsedAndUnparsed_wstring(size_t keysNum, const vector<string>& parsed)
{
    vector<wstring> parsedWstring;
    for(auto s : parsed){
        std::wstring wide(s.begin(), s.end());
        parsedWstring.push_back(wide);
    }
    vector<wstring> input90 = getPortion<wstring>(parsedWstring, 0.9);
    vector<wstring> input10 = getPortion<wstring>(parsedWstring, 0.1, true);

    int size10 = input10.size();
    int size90 = input90.size();
    vector<wstring> input;

    for(int i=0; i<keysNum/2; i++){
        input.push_back(input90[getRandInt(size90-1)]);
        input.push_back(input10[getRandInt(size10-1)]);
    }
    return input;
}

void wstringCompare(set<wstring>& c, const vector<wstring>& lookups){
    for (const wstring& s : lookups){
        lookup(c, s);
    }
} 

void buildContainerWstring(set<wstring>& container, const vector<string>& input){
    for(auto s : input){
        std::wstring wide(s.begin(), s.end());
        container.insert(wide);
    }
}
/////////////////////////////////////////////////////// FUNCTIONS FOR WSTRING COMPARE END/////////////////////////////////////////////////



/////////////////////////////////////////////////////// FUNCTIONS FOR MyString COMPARE /////////////////////////////////////////////////

void buildContainerMyString(set<MyString>& container, const vector<string>& input){
    for(auto s : input){
        MyString newStr(s);
        container.insert(newStr);
    }
}

vector<MyString> getInputFromParsedAndUnparsedMyString(size_t keysNum, const vector<string>& parsed){
    vector<MyString> parsedMyString;
    for(auto s : parsed){
        MyString newStr(s);
        parsedMyString.push_back(newStr);
    }
    vector<MyString> input90 = getPortion<MyString>(parsedMyString, 0.9);
    vector<MyString> input10 = getPortion<MyString>(parsedMyString, 0.9, true);
    vector<MyString> input;

    int size10 = input10.size();
    int size90 = input90.size();
    for(int i=0; i<keysNum/2; i++){
        input.push_back(input90[getRandInt(size90-1)]);
        input.push_back(input10[getRandInt(size10-1)]);
    }
    return input;
}

void MyStringCompare(set<MyString>& c, const vector<MyString>& lookups){
    for(const MyString& s : lookups){
        lookup(c, s);
    }
} 

// vector<KeyDometMyString64> getInputFromParsedAndUnparsed_KeyDometMyString64
// (size_t keysNum, const vector<string>& parsed){
//     vector<KeyDometMyString64> parsedKeyDometMyString64;
//     for(auto s : parsed){
//         KeyDometMyString64 newStr(s);
//         parsedKeyDometMyString64.push_back(newStr);
//     }
//     vector<KeyDometMyString64> input90;
//     vector<KeyDometMyString64> input10;

//     int size10 = input10.size();
//     int size90 = input90.size();
//     vector<KeyDometMyString64> input;

//     for(int i=0; i<keysNum/2; i++){
//         input.push_back(input90[getRandInt(size90-1)]);
//         input.push_back(input10[getRandInt(size10-1)]);
//     }
//     return input;
// }

// void KeyDometMyString64Compare(set<KeyDometMyString64>& c,
//  const vector<KeyDometMyString64>& lookups){
//     for (auto s : lookups){
//         lookup(c, s);
//     }
// } 
/////////////////////////////////////////////////////// FUNCTIONS FOR MyString COMPARE END/////////////////////////////////////////////////

template <typename C>
string containerName(){
    throw std::invalid_argument("bad container type");
}

template <>
string containerName<set<string>>(){
    return string("set<string>");
}

template <>
string containerName<set<KeyDometStr16>>(){
    return string("set<KeyDometStr16>");
}

template <>
string containerName<set<KeyDometStr32>>(){
    return string("set<KeyDometStr32>");
}

template <>
string containerName<set<KeyDometStr64>>(){
    return string("set<KeyDometStr64>");
}

template <>
string containerName<set<KeyDometStr128>>(){
    return string("set<KeyDometStr128>");
}

}
