#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <ecs/components.hpp>

class PositionUpdateSystem {
public:
	
	PositionUpdateSystem() = default;
	
	void update(entt::registry &reg);
	
private:
		
};
