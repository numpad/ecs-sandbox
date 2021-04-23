#pragma once

#include <glm/glm.hpp>

#include <ImprovedTerrain/ISignedDistanceFunction.hpp>
#include <ImprovedTerrain/ISignedDistanceBody.hpp>

class DynamicTerrain : public ISignedDistanceFunction {
public:


protected:
	float signed_distance_function(glm::vec3 world_pos) const;

private:

};
