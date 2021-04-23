#pragma once

#include <ecs/components.hpp>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <ecs/systems/IUpdateSystem.hpp>

class PressAwaySystem : public IUpdateSystem {
public:
	
	PressAwaySystem(entt::registry &registry);
	~PressAwaySystem();
	
	void update(float dt);
	
private:
	
};
