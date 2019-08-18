#pragma once

#include <random>

class Random {
public:
	
	Random(float min, float max);
	Random(float max);
	Random();
	
	float operator()();
		
private:
	std::random_device rd;
	std::mt19937 engine;
	
	std::uniform_real_distribution<float> dist;
};
