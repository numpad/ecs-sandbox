#pragma once

#include <chrono>

using namespace std;

class Benchmark {
public:
	Benchmark();
	
	void start();
	void stop();
	
	double ns() { return chrono::duration_cast<chrono::nanoseconds>(duration).count(); }
	double us() { return chrono::duration_cast<chrono::nanoseconds>(duration).count()  / 1000.f; }
	double ms() { return chrono::duration_cast<chrono::microseconds>(duration).count() / 1000.f; }
	double s()  { return chrono::duration_cast<chrono::milliseconds>(duration).count() / 1000.f; }
	
private:
	chrono::time_point<chrono::high_resolution_clock> begin, end;
	chrono::duration<double> duration;
};