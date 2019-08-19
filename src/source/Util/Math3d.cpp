#include <Util/Math3d.hpp>

namespace m3d {

	glm::vec3 raycastPlaneXZ(glm::vec3 pos_world, glm::vec3 lookdir,
		glm::vec2 screenpos_pixel, glm::vec2 screensize_pixel,
		float plane_y_world)
	{
		lookdir = glm::normalize(lookdir);
		
		glm::vec3 up(0.0f, 1.0f, 0.0f);
		glm::vec3 right = glm::cross(lookdir, up);
		glm::vec3 forward = glm::cross(right, up);
		glm::vec3 upwards = glm::cross(lookdir, right);
		glm::vec2 screenpos = (screenpos_pixel / screensize_pixel) * 2.0f - 1.0f;
		
		pos_world += right * screenpos.x;
		pos_world += upwards * screenpos.y;
		
		float dist = ((plane_y_world - pos_world.y) / lookdir.y);
		glm::vec3 collision = pos_world + lookdir * dist;
		// (x, 0, z) = (0.5, 2, 0) + d * (0, -1, 1)
		//     0     =       2 +    d *     (-1)     | -2
		//    -2     =              d *     (-1)     | / (-1)
		// -2 / -1 = d
		return collision;
	}

}
