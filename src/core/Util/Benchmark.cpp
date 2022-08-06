#include <Util/Benchmark.hpp>
#include <fmt/core.h>
#include <fmt/color.h>

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

void Benchmark::stop_and_print(const char* name) {
	stop();
	double time = s();
	const char* unit = "s";
	
	if (time < 0.1) {
		time = ms();
		unit = "ms";

		if (time < 1.0) {
			time = us();
			unit = "us";

			if (time < 1.0) {
				time = ns();
				unit = "ns";
			}
		}
	}

	fmt::print(fmt::fg(fmt::terminal_color::blue), "[TIME] \"{}\": {:.2f}{}\n", name, time, unit);
}

