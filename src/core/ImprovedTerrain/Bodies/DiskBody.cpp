#include "ImprovedTerrain/Bodies/DiskBody.hpp"

DiskBody::DiskBody(glm::vec3 position, float radius, float height)
    : ISignedDistanceBody(position), m_radius{radius}, m_height{height + 0.01f} {
}

glm::mat2x3 DiskBody::get_bounding_box() const {
	return glm::mat2x3(m_position - glm::vec3(m_radius, m_height, m_radius),
	                   m_position + glm::vec3(m_radius, m_height, m_radius));
}

///////////////
// PROTECTED //
///////////////

float DiskBody::signed_distance_function(glm::vec3 p) const {
	glm::vec3 lp = m_position - p;
	return glm::max(glm::length(lp * glm::vec3(1.f, 0.f, 1.f)) - m_radius, glm::abs(lp.y) - m_height);
}
