#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <ecs/components.hpp>
#include <ecs/systems/BaseUpdateSystem.hpp>

class PositionUpdateSystem : public BaseUpdateSystem {
public:
	
	PositionUpdateSystem(entt::registry &registry);
	
	void update();
	
private:
		
};
