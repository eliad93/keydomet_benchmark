#include "benchmark/benchmark.h"
#include <iostream>
#include "nemo/count_primes.hh"

using std::cout;
using std::endl;

using namespace nemo;

static void BM_StringCompare_set_string(benchmark::State& state, const size_t size, const size_t strLen) {
	const vector<string>& input = getInput(size, strLen);
	const vector<string>& lookups = getInput(size, strLen);
	while(state.KeepRunning()){
		stringCompare<set<string>>(input, lookups);
	}
}

static void BM_StringCompare_set_keydomet_64(benchmark::State& state, const size_t size, const size_t strLen) {
	const vector<string>& input = getInput(size, strLen);
	const vector<string>& lookups = getInput(size, strLen);
	while(state.KeepRunning()){
		stringCompare<set<KeyDometStr64>>(input, lookups);
	}
}

BENCHMARK_CAPTURE(BM_StringCompare_set_string, test_generated_str_set_string, 1000000, 1024);
BENCHMARK_CAPTURE(BM_StringCompare_set_keydomet_64, test_generated_str_set_keydomet64, 1000000, 1024);


BENCHMARK_MAIN();
