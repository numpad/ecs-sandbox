#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <ImprovedTerrain/ISignedDistanceFunction.hpp>
#include <ImprovedTerrain/ISignedDistanceBody.hpp>
#include <ImprovedTerrain/SparseGrid3D.hpp>
#include <ImprovedTerrain/CSGTree.hpp>
#include <ImprovedTerrain/SphereBody.hpp>
#include <ImprovedTerrain/CubeBody.hpp>
#include <iostream>

class DynamicTerrain : public ISignedDistanceFunction {
public:

	DynamicTerrain() {

	}

protected:
	float signed_distance_function(glm::vec3 world_pos) const;

private:
	SparseGrid3D<ISignedDistanceFunction> m_chunks;

};
