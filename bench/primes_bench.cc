#include "benchmark/benchmark.h"
#include <iostream>
#include <type_traits>
#include "nemo/count_primes.hh"

using std::cout;
using std::endl;

using namespace nemo;

const string Csv6MStringsPath = string("/home/yanivbaldi/keydomet_benchmark/6M keys + vals.csv");
const string sizeMeasurmentsPath = string("/home/yanivbaldi/keydomet_benchmark/results/size_overhead");

template <class C> static void BM_generated(benchmark::State& state) {
	const vector<string>& input = getInput(state.range(0), state.range(1));
	const vector<string>& lookups = getInput(state.range(0), state.range(1));
	C c = buildContainerWrapper<C>(input);
	while(state.KeepRunning()){
		stringCompare<C>(c, lookups);
	}
}

/////////////////////////////BENCH 2 - 1////////////////////////////////////

template <class C> static void BM_parsed_data(benchmark::State& state) {
	const vector<string>& input = parseCSV<vector<string>>(Csv6MStringsPath, 0.9);
	const vector<string>& lookups = getInputFromData(state.range(0),input);
	C c = buildContainerWrapper<C>(input);
	while(state.KeepRunning()){
		stringCompare<C>(c, lookups);
	}
}

////////////////////////////BENCH 2 - 2 END////////////////////////////////////



/////////////////////////////BENCH 2 - 1////////////////////////////////////

template <class C> static void BM_90_percent_parsed_data_10_percent_unparsed_lookups(benchmark::State& state) {
	const vector<string>& input = parseCSV<vector<string>>(Csv6MStringsPath, 0.9);
	const vector<string>& lookups = getInputFromData(state.range(0),parseCSV<vector<string>>(Csv6MStringsPath, 0.1));
	C c = buildContainerWrapper<C>(input);
	while(state.KeepRunning()){
		stringCompare<C>(c, lookups);
	}
}

////////////////////////////BENCH 2 - 2 END////////////////////////////////////

/////////////////////////////BENCH 2 - 1////////////////////////////////////

template <class C> static void BM_90_percent_parsed_data_5050_parsed_unparsed_lookups(benchmark::State& state) {
	const vector<string>& input = parseCSV<vector<string>>(Csv6MStringsPath, 0.9);
	const vector<string>& lookups = getInputFromParsedAndUnparsed(state.range(0), Csv6MStringsPath);
	C c = buildContainerWrapper<C>(input);
	size_t memorySize = 0;
	for(auto s : c){
        memorySize += s.size() * sizeof(std::string::value_type) + sizeof(s);
    }
	size_t memoryCapacity = 0;
		for(auto s : c){
        memoryCapacity += s.capacity() * sizeof(std::string::value_type) + sizeof(s);
    }

	std::ofstream outFile;
	outFile.open(sizeMeasurmentsPath.c_str(), std::ios_base::app);

	outFile << "memory consumption (using size) of " << containerName<C>() << " with " << c.size()
	<< " strings is: " << memorySize << " bytes." << std::endl;

	outFile << "memory consumption (using capacity) of " << containerName<C>() << " with " << c.size()
	<< " strings is: " << memoryCapacity << " bytes." << std::endl;

	outFile.close();
	while(state.KeepRunning()){
		stringCompare<C>(c, lookups);
	}
}

////////////////////////////BENCH 2 - 2 END////////////////////////////////////




////////////////////////////BENCH 4///////////////////////////////////////////

template <class C, class T> static void BM_90_percent_parsed_data_5050_parsed_unparsed_lookups_with_generated_writes(benchmark::State& state) {
	const vector<string>& input = parseCSV<vector<string>>(Csv6MStringsPath, 0.9);
	const vector<string>& lookups = getInputFromParsedAndUnparsed(state.range(0), Csv6MStringsPath);;
	auto writes = input;
	int readToWriteRatio = state.range(1);
	int strLen = state.range(2);
	C c = buildContainerWrapper<C>(writes);
	while(state.KeepRunning()){
		stringCompareAndInsert<C,T>(c, lookups, readToWriteRatio, strLen);
	}
}

////////////////////////////BENCH 4 - 2 END////////////////////////////////////

////////////////////////////BENCH 5 ///////////////////////////////////////////


template <class C> static void BM_90_percent_parsed_data_5050_parsed_unparsed_lookups_wstring(benchmark::State& state) {
	const vector<wstring>& input = parseCSV_wstring<vector<wstring>>(Csv6MStringsPath, 0.9);
	const vector<wstring>& lookups = getInputFromParsedAndUnparsed_wstring(state.range(0), Csv6MStringsPath);
	C c = buildContainerWrapper<C>(input);
	while(state.KeepRunning()){
		wstringCompare<C>(c, lookups);
	}
}


template <class C> static void BM_90_percent_parsed_data_5050_parsed_unparsed_lookups_mystring(benchmark::State& state) {
	const vector<MyString>& input = parseCSV_MyString<vector<MyString>>(Csv6MStringsPath, 0.9);
	const vector<MyString>& lookups = getInputFromParsedAndUnparsed_MyString(state.range(0), Csv6MStringsPath);
	C c = buildContainerWrapper<C>(input);
	while(state.KeepRunning()){
		MyStringCompare<C>(c, lookups);
	} 
}


////////////////////////////BENCH 5 END ////////////////////////////////////////

/*
	MACRO to workaround the fact that defining the template benchmark with multiple Args will result
	in unfriendly output where the two containers results are far each other - example:

Benchmark                                                             Time           CPU Iterations
---------------------------------------------------------------------------------------------------
BM_generated_string_compare<set<string>>/1000/1024               492709 ns     492659 ns       1357  <-
BM_generated_string_compare<set<string>>/100000/1024          205902030 ns  205879344 ns          3
BM_generated_string_compare<set<string>>/1000000/1024        3622266219 ns 3621885913 ns          1
BM_generated_string_compare<set<KeyDometStr64>>/1000/1024        440105 ns     440061 ns       1616  <-
BM_generated_string_compare<set<KeyDometStr64>>/100000/1024   170327825 ns  170311805 ns          4
BM_generated_string_compare<set<KeyDometStr64>>/1000000/1024 2937967260 ns 2937675491 ns          1

VS:

Benchmark                                                             Time           CPU Iterations
---------------------------------------------------------------------------------------------------
BM_generated_string_compare<set<string>>/1000/1024               538820 ns     537967 ns       1447  <-
BM_generated_string_compare<set<KeyDometStr64>>/1000/1024        442155 ns     442112 ns       1608  <-
BM_generated_string_compare<set<string>>/100000/1024          183921215 ns  183900382 ns          3
BM_generated_string_compare<set<KeyDometStr64>>/100000/1024   169346937 ns  169326695 ns          3
BM_generated_string_compare<set<string>>/1000000/1024        3528135562 ns 3527768624 ns          1
BM_generated_string_compare<set<KeyDometStr64>>/1000000/1024 2869301162 ns 2868993827 ns          1


*/

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

BENCH_COMPARE_2_TYPES_3_SETS_OF_2_ARGS(BM_90_percent_parsed_data_5050_parsed_unparsed_lookups, set<string>, set<KeyDometStr128>, argsArr);

BENCH_COMPARE_1_TYPES_3_SETS_OF_2_ARGS(BM_90_percent_parsed_data_5050_parsed_unparsed_lookups_wstring, set<wstring>, argsArr);
BENCH_COMPARE_1_TYPES_3_SETS_OF_2_ARGS(BM_90_percent_parsed_data_5050_parsed_unparsed_lookups_mystring, set<MyString>, argsArr);

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
