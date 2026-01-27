#define CATCH_CONFIG_MAIN

/*
To use catch2's BENCHMARK:
1. replace #define CATCH_CONFIG_RUNNER with #define CATCH_CONFIG_MAIN
2. Enable benchmarking: #define CATCH_CONFIG_ENABLE_BENCHMARKING
3. Put BENCHMARK(sectionName.c_str()){ return func(); }; into benchmarkSection()
*/
// #define CATCH_CONFIG_MAIN
// #define CATCH_CONFIG_ENABLE_BENCHMARKING

#define EASYS_ENTITY_LIMIT 1000000

#include <catch2/catch.hpp>
#include <chrono>
#include <easys/sparse_set.hpp>
#include "common.hpp"

TEST_CASE("Sparse set Benchmark", "[SparseSet]")
{
    struct TestComponent {
        int a;
        float b;
    };

    Easys::SparseSet<unsigned, TestComponent> set;

    for (int i = 0; i < EASYS_ENTITY_LIMIT; i++)
        set.set(i, {1, 2.f});

    TestComponent tmp;
    
    SECTION("Benchmarking SparseSet::set()") {
        benchmarkSection([&]()
        {
            for (int i = 0; i < EASYS_ENTITY_LIMIT; i++)
                set.set(i, {1, 2.f});
        },
        "SparseSet::set()");
    }
    
    SECTION("Benchmarking SparseSet::emplace()") {
        benchmarkSection([&]()
        {
            for (int i = 0; i < EASYS_ENTITY_LIMIT; i++)
                set.emplace(i, 1, 2.f);
        },
        "SparseSet::emplace()");
    }
    
    SECTION("Benchmarking SparseSet::operator[]()") {
        benchmarkSection([&]()
        {
            for (int i = 0; i < EASYS_ENTITY_LIMIT; i++)
                tmp = set[i];
        },
        "SparseSet::operator[]()");
    }
    
    SECTION("Benchmarking SparseSet::try_get()") {
        benchmarkSection([&]()
        {
            for (int i = 0; i < EASYS_ENTITY_LIMIT; i++)
                tmp = *set.try_get(i);
        },
        "SparseSet::try_get()");
    }
    
    SECTION("Benchmarking SparseSet::at()") {
        benchmarkSection([&]()
        {
            for (int i = 0; i < EASYS_ENTITY_LIMIT; i++)
                tmp = set.at(i);
        },
        "SparseSet::at()");
    }
    
    SECTION("Benchmarking SparseSet::remove()") {
        benchmarkSection([&]()
        {
            for (int i = 0; i < EASYS_ENTITY_LIMIT; i++)
                set.remove(i);
        },
        "SparseSet::remove()");
    }
}