#pragma once

#include <GLFW/glfw3.h>
#include <entt/entt.hpp>
#include <ecs/components.hpp>
#include <Util/Random.hpp>

class RandomJumpSystem {
public:
	
	RandomJumpSystem() = default;
	
	void update(entt::registry &registry);
private:
	
};
