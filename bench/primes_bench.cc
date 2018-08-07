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




/////////////////////////////BENCH 2 - 1////////////////////////////////////
static void BM_StringCompare_90_set_string(benchmark::State& state, const size_t size, const string& path) {
	const vector<string>& input = parseCSV<vector<string>>(path, 0.9);
	const vector<string>& lookups = getInputFromData(size,input);
	while(state.KeepRunning()){
		stringCompare<set<string>>(input, lookups);
	}
}

static void BM_StringCompare_90_set_keydomet_64(benchmark::State& state, const size_t size, const string& path) {
	const vector<string>& input = parseCSV<vector<string>>(path, 0.9);
	const vector<string>& lookups = getInputFromData(size,input);
	while(state.KeepRunning()){
		stringCompare<set<KeyDometStr64>>(input, lookups);
	}
}
////////////////////////////BENCH 2 - 2 END////////////////////////////////////



/////////////////////////////BENCH 2 - 1////////////////////////////////////
static void BM_StringCompare_10_set_string(benchmark::State& state, const size_t size, const string& path) {
	const vector<string>& input = parseCSV<vector<string>>(path, 0.9);
	const vector<string>& lookups = getInputFromData(size,parseCSV<vector<string>>(path, 0.1));
	while(state.KeepRunning()){
		stringCompare<set<string>>(input, lookups);
	}
}

static void BM_StringCompare_10_set_keydomet_64(benchmark::State& state, const size_t size, const string& path) {
	const vector<string>& input = parseCSV<vector<string>>(path, 0.9);
	const vector<string>& lookups = getInputFromData(size,parseCSV<vector<string>>(path, 0.1));
	while(state.KeepRunning()){
		stringCompare<set<KeyDometStr64>>(input, lookups);
	}
}
////////////////////////////BENCH 2 - 2 END////////////////////////////////////



/////////////////////////////BENCH 2 - 1////////////////////////////////////
static void BM_StringCompare_5050_set_string(benchmark::State& state, const size_t size, const string& path) {
	const vector<string>& input = parseCSV<vector<string>>(path, 0.9);
	const vector<string>& lookups = getInputFromParsedAndUnparsed(size, path);
	while(state.KeepRunning()){
		stringCompare<set<string>>(input, lookups);
	}
}

static void BM_StringCompare_5050_set_keydomet_64(benchmark::State& state, const size_t size, const string& path) {
	const vector<string>& input = parseCSV<vector<string>>(path, 0.9);
	const vector<string>& lookups = getInputFromParsedAndUnparsed(size, path);
	while(state.KeepRunning()){
		stringCompare<set<KeyDometStr64>>(input, lookups);
	}
}
////////////////////////////BENCH 2 - 2 END////////////////////////////////////

const string Csv6MStringsPath = string("/home/eliad93/keydomet/keydomet_benchmark/6M_keys_+_vals.csv");


BENCHMARK_CAPTURE(BM_StringCompare_set_string, test_generated_str_set_string, 1000000, 1024);
BENCHMARK_CAPTURE(BM_StringCompare_set_keydomet_64, test_generated_str_set_keydomet64, 1000000, 1024);

BENCHMARK_CAPTURE(BM_StringCompare_90_set_string, test_real_str_90_set_string, 1000000, Csv6MStringsPath);
BENCHMARK_CAPTURE(BM_StringCompare_90_set_keydomet_64, test_real_str_90_set_keydomet64, 1000000, Csv6MStringsPath);

BENCHMARK_CAPTURE(BM_StringCompare_10_set_string, test_real_str_10_set_string, 1000000, Csv6MStringsPath);
BENCHMARK_CAPTURE(BM_StringCompare_10_set_keydomet_64, test_real_str_10_set_keydomet64, 1000000, Csv6MStringsPath);

BENCHMARK_CAPTURE(BM_StringCompare_5050_set_string, test_real_str_5050_set_string, 1000000, Csv6MStringsPath);
BENCHMARK_CAPTURE(BM_StringCompare_5050_set_keydomet_64, test_real_str_5050_set_keydomet64, 1000000, Csv6MStringsPath);


BENCHMARK_MAIN();
