#include "ImprovedTerrain/SphereBody.hpp"

SphereBody::SphereBody(glm::vec3 position, float radius)
 : ISignedDistanceBody(position), m_radius{radius}
{

}

glm::mat2x3 SphereBody::get_bounding_box() const {
	return glm::mat2x3(m_position + glm::vec3(-m_radius),
	                   m_position + glm::vec3( m_radius));
}

///////////////
// PROTECTED //
///////////////

float SphereBody::signed_distance_function(glm::vec3 p) const {
	return glm::length(m_position - p) - m_radius;
}

