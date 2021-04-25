#pragma once

#include <glm/glm.hpp>
#include <ImprovedTerrain/ISignedDistanceBody.hpp>

class DiskBody : public ISignedDistanceBody {
public:

	DiskBody(glm::vec3 position, float r, float h);

	glm::mat2x3 get_bounding_box() const;

protected:
	float signed_distance_function(glm::vec3 p) const;
	float m_radius, m_height;

};
