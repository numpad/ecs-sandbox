#pragma once

#include <glm/glm.hpp>

class ISignedDistanceBody {
public:
	ISignedDistanceBody(glm::vec3 position) : m_position{position} {

	}

	// evaluate the SDF
	float get_distance(glm::vec3 p) const { return signed_distance_function(p); }

	// get the body's AABB
	virtual glm::mat2x3 get_bounding_box() const = 0;

protected:
	glm::vec3 m_position;

	virtual float signed_distance_function(glm::vec3 p) const = 0;

};
