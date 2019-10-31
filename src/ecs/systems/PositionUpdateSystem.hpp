#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <ecs/components.hpp>
#include <ecs/systems/BaseUpdateSystem.hpp>

class PositionUpdateSystem : public BaseUpdateSystem {
public:
	
	PositionUpdateSystem() = default;
	
	void update(entt::registry &reg);
	
private:
		
};
