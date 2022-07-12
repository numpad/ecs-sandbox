#include <Util/Random.hpp>

Random::Random(float min, float max) : engine(rd()), dist(min, max) {
}

Random::Random(float max) : Random(0.0f, max) {
}

Random::Random() : Random(0.0f, 1.0f) {
}

float Random::operator()() {
	return dist(engine);
}
