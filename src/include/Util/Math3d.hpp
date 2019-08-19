#pragma once

#include <glm/glm.hpp>

namespace m3d {
	
	glm::vec3 raycastPlaneXZ(glm::vec3 pos_world, glm::vec3 lookdir,
		glm::vec2 screenpos_pixel, glm::vec2 screensize_pixel,
		float plane_y_world);
}
