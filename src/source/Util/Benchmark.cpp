#include <Util/Benchmark.hpp>

Benchmark::Benchmark() {
	start();
}

void Benchmark::start() {
	begin = std::chrono::high_resolution_clock::now();
}

void Benchmark::stop() {
	end = std::chrono::high_resolution_clock::now();
	duration = end - begin;
}