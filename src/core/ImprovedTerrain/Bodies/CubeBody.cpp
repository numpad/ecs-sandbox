#include "ImprovedTerrain/Bodies/CubeBody.hpp"

CubeBody::CubeBody(glm::vec3 position, glm::vec3 size) : ISignedDistanceBody(position), m_size{size} {
}

glm::mat2x3 CubeBody::get_bounding_box() const {
	return glm::mat2x3(m_position - m_size, m_position + m_size);
}

///////////////
// PROTECTED //
///////////////

float CubeBody::signed_distance_function(glm::vec3 p) const {
	glm::vec3 q = glm::abs(m_position - p) - m_size;
	return glm::length(glm::max(q, 0.0f)) + glm::min(glm::max(q.x, glm::max(q.y, q.z)), 0.0f);
}
