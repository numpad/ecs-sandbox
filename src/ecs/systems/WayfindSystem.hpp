#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <ecs/components.hpp>
#include <ecs/systems/BaseUpdateSystem.hpp>

class WayfindSystem : public BaseUpdateSystem {
public:
	
	WayfindSystem(entt::registry &registry);
	WayfindSystem(const WayfindSystem &copy) = delete;
	
	void update();
	
private:
	
};
