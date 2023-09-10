#include <benchmark/benchmark.h>
#include <map>

static void bench_map_insert(benchmark::State& state)
{
    std::map<int, int> mmap;
    for (auto _ : state) {
        // This code gets timed
        mmap.insert(std::make_pair(5,15));
    }
}

BENCHMARK(bench_map_insert);

static void bench_map_emplace(benchmark::State& state)
{
    std::map<int, int> mmap;
    for (auto _ : state) {
        // This code gets timed
        mmap.emplace(5, 15);
    }
}

BENCHMARK(bench_map_emplace);

static void bench_map_operator(benchmark::State& state)
{
    std::map<int, int> mmap;
    for (auto _ : state) {
        // This code gets timed
        mmap[5] = 15;
    }
}

BENCHMARK(bench_map_operator);

BENCHMARK_MAIN();