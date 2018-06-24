#include "benchmark/benchmark.h"
#include <iostream>
 #include "nemo/count_primes.hh"

using std::cout;
using std::endl;

// //constexpr auto KeyDometSizeToUse = KeyDometSize::SIZE_128BIT;
// constexpr auto KeyDometSizeToUse = KeyDometSize::SIZE_64BIT;
// //constexpr auto KeyDometSizeToUse = KeyDometSize::SIZE_16BIT;

// using KeyDometStr64 = KeyDometStr<std::string, KeyDometSizeToUse>;

using namespace nemo;

template <class Container>
void stringCompare(const vector<string>& input, const vector<string>& lookups){
    Container container;
    buildContainer(container, input);
    for (const string& s : lookups){
        lookup(container, s);
    }
}

using HkSet = set<KeyDometStr64, less<>>;

static void BM_StringCompare(benchmark::State& state, const size_t size, const size_t strLen) {
	const vector<string>& input = getInput(size, strLen);
	const vector<string>& lookups = getInput(size, strLen);
	while(state.KeepRunning()){
		stringCompare<set<std::string, less<>>>(input, lookups);
	}
}

static void BM_StringCompareKeydomet(benchmark::State& state, const size_t size, const size_t strLen) {
	const vector<string>& input = getInput(size, strLen);
	const vector<string>& lookups = getInput(size, strLen);
	while(state.KeepRunning()){
		stringCompare<HkSet>(input, lookups);
	}
}

BENCHMARK_CAPTURE(BM_StringCompare, eran_basic_test, 1000000, 16);
BENCHMARK_CAPTURE(BM_StringCompareKeydomet, eran_basic_test_keydomet, 1000000, 16);


BENCHMARK_MAIN();
