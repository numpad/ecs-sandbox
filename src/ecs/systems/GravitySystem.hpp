#pragma once

#include <entt/entt.hpp>
#include <ecs/components.hpp>

class GravitySystem {
public:
	
	GravitySystem(float gravity);
	
	void update(entt::registry &registry);
	
private:
	float gravity;
	
};
