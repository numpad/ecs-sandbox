#pragma once

#include <ecs/components.hpp>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <ecs/systems/BaseUpdateSystem.hpp>

class PressAwaySystem : public BaseUpdateSystem {
public:
	
	PressAwaySystem() = default;
	~PressAwaySystem();
	
	void update(entt::registry &);
	
private:
	
};
