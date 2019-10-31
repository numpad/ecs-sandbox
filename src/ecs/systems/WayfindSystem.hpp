#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <ecs/components.hpp>
#include <ecs/systems/BaseUpdateSystem.hpp>

class WayfindSystem : public BaseUpdateSystem {
public:
	
	WayfindSystem() = default;
	WayfindSystem(const WayfindSystem &copy) = delete;
	
	void update(entt::registry &registry);
	
private:
	
};
