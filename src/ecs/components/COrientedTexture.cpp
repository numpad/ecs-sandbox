#include <ecs/components/COrientedTexture.hpp>

int COrientedTexture::get_sprite_offset() const {
	const float half_direction_angle = ((glm::pi<float>() * 2.f) / float(directions)) * 0.5f;

	float rad = angle;
	rad -= (camera_angle - half_direction_angle);

	// convert to unsigned angle
	while (rad < 0.f) rad += glm::pi<float>() * 2.f;
	
	int direction = glm::floor((rad / (glm::pi<float>() * 2.f)) * float(directions));
	return glm::mod(direction, directions);
}
