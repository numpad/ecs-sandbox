#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <ecs/components.hpp>

class WayfindSystem {
public:
	
	WayfindSystem() = default;
	WayfindSystem(const WayfindSystem &copy) = delete;
	
	void update(entt::registry &registry);
	
private:
	
};
