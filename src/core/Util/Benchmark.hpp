#pragma once

#include <chrono>

class Benchmark {
public:
	Benchmark();

	void start();
	void stop();
	void stop_and_print(const char* name = "time elapsed");

	double ns() const {
		return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
	}
	double us() const {
		return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / 1000.f;
	}
	double ms() const {
		return std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1000.f;
	}
	double s() const {
		return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.f;
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> begin, end;
	std::chrono::duration<double> duration;
};
