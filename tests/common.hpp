#pragma once

#include <chrono>
#include <string>

// This function is a helper to run benchmarks. It  that takes a lambda function as an argument.
// This lambda function will contain the code to benchmark.
template <typename Func>
void benchmarkSection(Func func, const std::string& sectionName)
{
	auto start = std::chrono::high_resolution_clock::now();

	// BENCHMARK(sectionName.c_str()){ return func(); };

	func();  // Execute the lambda function

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = end - start;

	std::cout << "Benchmark completed for " << sectionName << ": " << std::to_string(elapsed.count()) << " ms\n";
	// SUCCEED("Benchmark completed for " + sectionName + ": " + std::to_string(elapsed.count()) + " ms");
}