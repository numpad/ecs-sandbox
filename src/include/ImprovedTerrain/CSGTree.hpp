#pragma once

#include <glm/glm.hpp>
#include <ImprovedTerrain/ISignedDistanceBody.hpp>

class CSGNode : public ISignedDistanceBody {
public:

	enum class Operator {
		UNION, INTERSECT, DIFF
	};

	CSGNode(ISignedDistanceBody *left, ISignedDistanceBody *right, Operator op)
	 : ISignedDistanceBody{glm::vec3(0.f)}, m_left{left}, m_right{right}, m_op{op}
	{}

	glm::mat2x3 get_bounding_box() const {
		const glm::mat2x3 bb_left = m_left->get_bounding_box();
		const glm::mat2x3 bb_right = m_right->get_bounding_box();
		
		const glm::vec3 left_min = bb_left[0];
		const glm::vec3 left_max = bb_left[1];
		const glm::vec3 right_min = bb_right[0];
		const glm::vec3 right_max = bb_right[1];

		return glm::mat2x3(
			glm::vec3(
				glm::min(left_min.x, right_min.x),
				glm::min(left_min.y, right_min.y),
				glm::min(left_min.z, right_min.z)
			),
			glm::vec3(
				glm::max(left_max.x, right_max.x),
				glm::max(left_max.y, right_max.y),
				glm::max(left_max.z, right_max.z)
			)
		);
	}

	ISignedDistanceBody *m_left, *m_right;
	Operator m_op;

protected:
	static inline float sdf_union(float a, float b) { return glm::min(a, b); }
	static inline float sdf_diff(float a, float b) { return glm::max(a, -b); }
	static inline float sdf_intersect(float a, float b) { return glm::max(a, b); }

	float signed_distance_function(glm::vec3 p) const {
		switch (m_op) {
			case Operator::UNION:     return sdf_union(m_left->get_distance(p), m_right->get_distance(p));
			case Operator::INTERSECT: return sdf_intersect(m_left->get_distance(p), m_right->get_distance(p));
			case Operator::DIFF:      return sdf_diff(m_left->get_distance(p), m_right->get_distance(p));
		};

		return 1.f;
	}

};
