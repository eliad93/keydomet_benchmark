#include "benchmark/benchmark.h"
#include <iostream>
#include <type_traits>
#include "nemo/count_primes.hh"
#include <locale>
#include <codecvt>

using std::cout;
using std::endl;

using namespace nemo;

		// size_t memorySize = 0;
	// for(auto s : c){
 //        memorySize += s.size() * sizeof(std::string::value_type) + sizeof(s);
 //    }
	// size_t memoryCapacity = 0;
	// 	for(auto s : c){
 //        memoryCapacity += s.capacity() * sizeof(std::string::value_type) + sizeof(s);
 //    }

	// std::ofstream outFile;
	// outFile.open(sizeMeasurmentsPath.c_str(), std::ios_base::app);

	// outFile << "memory consumption (using size) of " << containerName<C>() << " with " << c.size()
	// << " strings is: " << memorySize << " bytes." << std::endl;

	// outFile << "memory consumption (using capacity) of " << containerName<C>() << " with " << c.size()
	// << " strings is: " << memoryCapacity << " bytes." << std::endl;

	// outFile.close();

const string Csv6MStringsPath = string("/home/eliad93/keydomet_benchmark/6M_keys_+_vals.csv");
// const string sizeMeasurmentsPath = string("/home/yanivbaldi/keydomet_benchmark/results/size_overhead");
const vector<string>& csv = parseCSV<vector<string>>(Csv6MStringsPath);
const vector<string>& input90Percent = getPortion<string>(csv, 0.9);

template <class C> static void BM_generated(benchmark::State& state) {
	const vector<string>& input = getInput(state.range(0), state.range(1));
	const vector<string>& lookups = getInput(state.range(0), state.range(1));
	C c = buildContainerWrapper<C>(input);
	while(state.KeepRunning()){
		stringCompare<C>(c, lookups);
	}
}

template <class C> static void BM_parsed_data(benchmark::State& state) {
	const vector<string>& lookups = getInputFromData(state.range(0),input90Percent);
	C c = buildContainerWrapper<C>(input90Percent);
	while(state.KeepRunning()){
		stringCompare<C>(c, lookups);
	}
}

template <class C> static void BM_90_percent_parsed_data_10_percent_unparsed_lookups(benchmark::State& state) {
	const vector<string>& lookups = getInputFromData(state.range(0),getPortion<string>(csv, 0.1, true));
	C c = buildContainerWrapper<C>(input90Percent);
	while(state.KeepRunning()){
		stringCompare<C>(c, lookups);
	}
}

template <class C> static void BM_90_percent_parsed_data_5050_parsed_unparsed_lookups(benchmark::State& state) {
	const vector<string>& lookups = getInputFromParsedAndUnparsed(state.range(0), csv);
	C c = buildContainerWrapper<C>(input90Percent);
	while(state.KeepRunning()){
		stringCompare<C>(c, lookups);
	}
}

template <class C, class T> static void BM_90_percent_parsed_data_5050_parsed_unparsed_lookups_with_generated_writes(benchmark::State& state) {
	const vector<string>& lookups = getInputFromParsedAndUnparsed(state.range(0), csv);
	auto writes = input90Percent;
	int readToWriteRatio = state.range(1);
	int strLen = state.range(2);
	C c = buildContainerWrapper<C>(writes);
	while(state.KeepRunning()){
		stringCompareAndInsert<C,T>(c, lookups, readToWriteRatio, strLen);
	}
}


static void BM_90_percent_parsed_data_5050_parsed_unparsed_lookups_wstring(benchmark::State& state) {
	const vector<wstring>& lookups = getInputFromParsedAndUnparsed_wstring(state.range(0), csv);
	set<wstring> c;
	buildContainerWstring(c,input90Percent);
	while(state.KeepRunning()){
		wstringCompare(c, lookups);
	}
}

// static void BM_90_percent_parsed_data_5050_parsed_unparsed_lookups_keyDometMystring(benchmark::State& state) {
// 	const vector<KeyDometMyString64>& lookups = getInputFromParsedAndUnparsed_KeyDometMyString64(state.range(0), csv);
// 	C c = buildContainerWrapper(input);
// 	while(state.KeepRunning()){
// 		KeyDometMyString64Compare(c, lookups);
// 	}
// }

static void BM_90_percent_parsed_data_5050_parsed_unparsed_lookups_mystring(benchmark::State& state) {
	const vector<MyString>& lookups = getInputFromParsedAndUnparsedMyString(state.range(0), csv);
	set<MyString> c;
	buildContainerMyString(c, csv);
	while(state.KeepRunning()){
		MyStringCompare(c, lookups);
	} 
}

#define BENCH_COMPARE_2_TYPES_3_SETS_OF_2_ARGS(func, type1, type2, args) BENCHMARK_TEMPLATE(func, type1)->Args({args[0][0], args[0][1]}); BENCHMARK_TEMPLATE(func, type2)->Args({args[0][0], args[0][1]}); \
				BENCHMARK_TEMPLATE(func, type1)->Args({args[1][0], args[1][1]}); BENCHMARK_TEMPLATE(func, type2)->Args({args[1][0], args[1][1]}); \
				BENCHMARK_TEMPLATE(func, type1)->Args({args[2][0], args[2][1]}); BENCHMARK_TEMPLATE(func, type2)->Args({args[2][0], args[2][1]})

#define BENCH_2_TYPES_COMPARE_2_TYPES_3_SETS_OF_3_ARGS(func, cmp1, cmp2, type1, type2, args) BENCHMARK_TEMPLATE(func, cmp1, type1)->Args({args[0][0], args[0][1], args[0][2]}); \
				BENCHMARK_TEMPLATE(func, cmp2, type2)->Args({args[0][0], args[0][1], args[0][2]}); \
				BENCHMARK_TEMPLATE(func, cmp1, type1)->Args({args[1][0], args[1][1], args[1][2]}); BENCHMARK_TEMPLATE(func, cmp2, type2)->Args({args[1][0], args[1][1], args[1][2]}); \
				BENCHMARK_TEMPLATE(func, cmp1, type1)->Args({args[2][0], args[2][1], args[2][2]}); BENCHMARK_TEMPLATE(func, cmp2, type2)->Args({args[2][0], args[2][1], args[2][2]})

#define BENCH_COMPARE_1_TYPES_3_SETS_OF_2_ARGS(func, type, args) BENCHMARK_TEMPLATE(func, type)->Args({args[0][0], args[0][1]});  \
				BENCHMARK_TEMPLATE(func, type)->Args({args[1][0], args[1][1]}); \
				BENCHMARK_TEMPLATE(func, type)->Args({args[2][0], args[2][1]})

#define BENCH_NORMAL_COMPARE_1_TYPES_3_SETS_OF_2_ARGS(func, args) BENCHMARK(func)->Args({args[0][0]});  \
				BENCHMARK(func)->Args({args[1][0]}); \
				BENCHMARK(func)->Args({args[2][0]})

int argsArr[][2] = {{1000,1024},{100000,1024},{1000000,1024}};

BENCH_COMPARE_2_TYPES_3_SETS_OF_2_ARGS(BM_generated, set<string>, set<KeyDometStr64>, argsArr);

BENCH_COMPARE_2_TYPES_3_SETS_OF_2_ARGS(BM_parsed_data, set<string>, set<KeyDometStr64>, argsArr);

BENCH_COMPARE_2_TYPES_3_SETS_OF_2_ARGS(BM_90_percent_parsed_data_10_percent_unparsed_lookups, set<string>, set<KeyDometStr64>, argsArr);

BENCHMARK_TEMPLATE(BM_90_percent_parsed_data_5050_parsed_unparsed_lookups, 
	set<KeyDometStr16>)->Args({1000,1024})->Args({100000,1024})->Args({1000000,1024});

BENCHMARK_TEMPLATE(BM_90_percent_parsed_data_5050_parsed_unparsed_lookups, 
	set<KeyDometStr32>)->Args({1000,1024})->Args({100000,1024})->Args({1000000,1024});

BENCHMARK_TEMPLATE(BM_90_percent_parsed_data_5050_parsed_unparsed_lookups, 
	set<KeyDometStr64>)->Args({1000,1024})->Args({100000,1024})->Args({1000000,1024});

BENCHMARK_TEMPLATE(BM_90_percent_parsed_data_5050_parsed_unparsed_lookups, 
	set<KeyDometStr128>)->Args({1000,1024})->Args({100000,1024})->Args({1000000,1024});

BENCHMARK_TEMPLATE(BM_90_percent_parsed_data_5050_parsed_unparsed_lookups, 
	set<KeyDometStr128>)->Args({1000000,1024})->Args({1000000,1024});

BENCH_COMPARE_2_TYPES_3_SETS_OF_2_ARGS(BM_90_percent_parsed_data_5050_parsed_unparsed_lookups, 
	set<string>, set<KeyDometStr64>, argsArr);

BENCHMARK(BM_90_percent_parsed_data_5050_parsed_unparsed_lookups_wstring)->Args({1000})->Args({100000})->Args({1000000});

// BENCHMARK(BM_90_percent_parsed_data_5050_parsed_unparsed_lookups_keyDometMystring)->Args({1000});
// BENCHMARK(BM_90_percent_parsed_data_5050_parsed_unparsed_lookups_keyDometMystring)->Args({100000});
// BENCHMARK(BM_90_percent_parsed_data_5050_parsed_unparsed_lookups_keyDometMystring)->Args({1000000});

BENCHMARK(BM_90_percent_parsed_data_5050_parsed_unparsed_lookups_mystring)->Args({1000})->Args({100000})->Args({1000000});

// can't assign in non exceutable code so must declare a new array
int argsArr2[][3] = {{1000,100,1024},{1000,10,1024},{1000,1,1024}};
BENCH_2_TYPES_COMPARE_2_TYPES_3_SETS_OF_3_ARGS(BM_90_percent_parsed_data_5050_parsed_unparsed_lookups_with_generated_writes, set<string>,
	set<KeyDometStr64>, string, KeyDometStr64, argsArr2);

int argsArr3[][3] = {{100000,100,1024},{100000,10,1024},{100000,1,1024}};
BENCH_2_TYPES_COMPARE_2_TYPES_3_SETS_OF_3_ARGS(BM_90_percent_parsed_data_5050_parsed_unparsed_lookups_with_generated_writes, set<string>,
	set<KeyDometStr64>, string, KeyDometStr64, argsArr3);

int argsArr4[][3] = {{1000000,100,1024},{1000000,10,1024},{1000000,1,1024}};
BENCH_2_TYPES_COMPARE_2_TYPES_3_SETS_OF_3_ARGS(BM_90_percent_parsed_data_5050_parsed_unparsed_lookups_with_generated_writes, set<string>,
	set<KeyDometStr64>, string, KeyDometStr64, argsArr4);

BENCHMARK_MAIN();
