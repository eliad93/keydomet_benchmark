
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
#include "nemo/count_primes.hh"

using namespace std;
using namespace nemo;

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

template<template<class, typename...> class Container,
        typename StrT, KeyDometSize Size, typename... ContainerArgs>
void buildContainer(Container<KeyDometStr<StrT, Size>, ContainerArgs...>& container, const vector<string>& input)
{
    transform(input.begin(), input.end(), std::inserter(container, container.begin()), [](const string& str) {
        return KeyDometStr<StrT, Size>{str};
    });
}

template<template<class, typename...> class Container,
        typename... ContainerArgs>
void buildContainer(Container<string, ContainerArgs...>& container, const vector<string>& input)
{
    transform(input.begin(), input.end(), std::inserter(container, container.begin()), [](const string& str) {
        return str;
    });
}

#if BENCH_FOLLY
template<template<typename> class Container>
void buildContainer(Container<folly::fbstring>& container, const vector<string>& input)
{
    transform(input.begin(), input.end(), container.begin(), [] (const string& str) {
        return folly::fbstring{str};
    });
}
#endif

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


template <class Container>
void nemo::stringCompare(const vector<string>& input, const vector<string>& lookups){
    Container container;
    buildContainer(container, input);
    for (const string& s : lookups){
        lookup(container, s);
    }
}
