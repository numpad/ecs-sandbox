#include "ImprovedTerrain/CSGTree.hpp"

////////////
// PUBLIC //
////////////

glm::mat2x3 CSGNode::get_bounding_box() const {
	/* can we optimize the bounding box for INTERSECT & DIFF?
	 * 1d example: imagine the following geometry
	 *
	 * Operation: DIFF
	 * Body A:   ########
	 * Body B:       #############
	 * Result:   ####
	 * Chunks: |   1   |   2   |   3   |
	 *
	 * Currently the AABB covers chunks 1, 2, 3...
	 * But the resulting body only covers the first chunk. this means we need to
	 * update 66% more chunks than required! and the resulting geometry looks just the same!
	 * BUT: this might fuck up the SDF and could result in buggy collision detection?
	 */
	const glm::mat2x3 bb_left = m_left->get_bounding_box();
	const glm::mat2x3 bb_right = m_right->get_bounding_box();

	const glm::vec3 left_min = bb_left[0];
	const glm::vec3 left_max = bb_left[1];
	const glm::vec3 right_min = bb_right[0];
	const glm::vec3 right_max = bb_right[1];

	return glm::mat2x3(glm::vec3(glm::min(left_min.x, right_min.x), glm::min(left_min.y, right_min.y),
	                             glm::min(left_min.z, right_min.z)),
	                   glm::vec3(glm::max(left_max.x, right_max.x), glm::max(left_max.y, right_max.y),
	                             glm::max(left_max.z, right_max.z)));
}

///////////////
// PROTECTED //
///////////////

float CSGNode::signed_distance_function(glm::vec3 p) const {
	return combine(m_left->get_distance(p), m_right->get_distance(p));
}

/////////////
// PRIVATE //
/////////////

float CSGNode::combine(float a, float b) const {
	switch (m_op) {
	default:
	case Operator::UNION:
		return glm::min(a, b);
	case Operator::INTERSECT:
		return glm::max(a, b);
	case Operator::DIFF:
		return glm::max(a, -b);
	};
}
