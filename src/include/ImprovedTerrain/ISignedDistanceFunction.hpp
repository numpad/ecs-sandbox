#pragma once

#include <glm/glm.hpp>

class ISignedDistanceFunction {
public:

	float get_distance(glm::vec3 p) const { return signed_distance_function(p); };

protected:
	virtual float signed_distance_function(glm::vec3 p) const = 0;

};
