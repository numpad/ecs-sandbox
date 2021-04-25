#pragma once

#include <glm/glm.hpp>
#include <ImprovedTerrain/ISignedDistanceBody.hpp>

class CubeBody : public ISignedDistanceBody {
public:

	CubeBody(glm::vec3 position, glm::vec3 size);

	glm::mat2x3 get_bounding_box() const;

protected:
	float signed_distance_function(glm::vec3 p) const;
	glm::vec3 m_size;

};
