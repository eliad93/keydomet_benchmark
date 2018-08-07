
#if BENCH_FOLLY
#   include "../folly/folly/FBString.h"
#   define IF_TEST_FOLLY(s) , s
#else
#   define IF_TEST_FOLLY(s)
#endif

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

string nemo::getRandStr(size_t len)
{
    static mt19937 gen{random_device{}()};
    uniform_int_distribution<short> dis('A', 'z');
    string s;
    s.reserve(len);
    for (size_t i = 0; i < len; ++i)
        s += (char)dis(gen);
    return s;
}

vector<string> nemo::getInput(size_t keysNum, size_t keyLen)
{
    vector<string> input{keysNum};
    generate(input.begin(), input.end(), [keyLen] {
        return getRandStr(keyLen);
    });
    return input;
}





//////////////////////////////////////////////////////////////////////////////////////

vector<string> nemo::getInputFromData(size_t keysNum, vector<string> data)
{
    if(keysNum > data.size()){
        keysNum = data.size(); 
    }
    vector<string> input{keysNum};
    for(int i=0; i<keysNum; i++){
        input.push_back(data[i]);
    }
    random_shuffle(input.begin(), input.end());
    return input;
}



vector<string> nemo::getInputFromParsedAndUnparsed(size_t keysNum, const string& path)
{
    
    vector<string> input90 = nemo::parseCSV<vector<string>>(path, 0.9);
    vector<string> input10 = nemo::parseCSV<vector<string>>(path, 0.1);

    random_shuffle(input90.begin(), input90.end());
    random_shuffle(input10.begin(), input10.end());

    if(keysNum/2 > min(input90.size(), input10.size())){
        keysNum = min(input90.size(), input10.size())*2; 
    }
    vector<string> input{keysNum};

    for(int i=0; i<keysNum/2; i++){
        input.push_back(input90[i]);
        input.push_back(input10[i]);
    }

    random_shuffle(input.begin(), input.end());
    return input;
}
//////////////////////////////////////////////////////////////////////////////////////////






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
