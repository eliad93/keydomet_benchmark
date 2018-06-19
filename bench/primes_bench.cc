#include "benchmark/benchmark.h"
#include <iostream>
 #include "nemo/count_primes.hh"
using std::cout;
using std::endl;


static void BM_StringCompare(benchmark::State& state) {
  while (state.KeepRunning()) {
    nemo::stringCompare();
  }
  cout << "END OF STRING COMPARE" << endl;
}

BENCHMARK(BM_StringCompare);

BENCHMARK_MAIN();
