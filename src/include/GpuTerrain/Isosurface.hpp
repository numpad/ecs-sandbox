#pragma once

#include "GpuTerrain/ISignedDistanceFunction.hpp"

class IsoSurface : public ISignedDistanceFunction {
private:

protected:
	virtual float signed_distance_function(glm::vec3 p) const {
		
	}

public:


};
