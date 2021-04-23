#pragma once

#include <glm/glm.hpp>
#include <ImprovedTerrain/ISignedDistanceBody.hpp>

class SphereBody : public ISignedDistanceBody {
public:

	SphereBody(glm::vec3 position, float radius);

	glm::mat2x3 get_bounding_box() const;

protected:
	float signed_distance_function(glm::vec3 p) const;
	float m_radius;

};
