#pragma once

#include <glm/glm.hpp>

using namespace glm;

class Terrain {
public:
	
	inline float get(vec3 pos) const { return sampleValue(pos); };
	
protected:
	
	virtual float sampleValue(vec3 pos) const = 0;
	
private:
	
};
