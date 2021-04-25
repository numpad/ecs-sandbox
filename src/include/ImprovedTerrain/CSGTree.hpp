#pragma once

#include <glm/glm.hpp>
#include <ImprovedTerrain/ISignedDistanceBody.hpp>
#include <Util/Math3d.hpp>

class CSGNode : public ISignedDistanceBody {
public:

	enum class Operator {
		UNION, INTERSECT, DIFF
	};

	CSGNode(ISignedDistanceBody *left, ISignedDistanceBody *right, Operator op)
	 : ISignedDistanceBody{glm::vec3(0.f)}, m_left{left}, m_right{right}, m_op{op}
	{}

	glm::mat2x3 get_bounding_box() const;

	ISignedDistanceBody *m_left, *m_right;
	Operator m_op;

protected:
	static inline float sdf_union(float a, float b) { return glm::min(a, b); }
	static inline float sdf_diff(float a, float b) { return glm::max(a, -b); }
	static inline float sdf_intersect(float a, float b) { return glm::max(a, b); }

	float signed_distance_function(glm::vec3 p) const;

};
