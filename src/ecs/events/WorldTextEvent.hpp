#pragma once

#include <string>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

struct WorldTextEvent {
	entt::entity who;
	glm::vec3 offset;
	std::string text;
	
};
