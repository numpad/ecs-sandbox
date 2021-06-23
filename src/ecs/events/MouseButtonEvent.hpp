#pragma once

#include <glm/glm.hpp>

struct MouseButtonEvent {
	int button;
	bool is_down;
	int modifiers;

	glm::vec2 pos;
};
