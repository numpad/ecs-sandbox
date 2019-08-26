#pragma once

#include <ecs/components.hpp>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

class PressAwaySystem {
public:
	
	PressAwaySystem() = default;
	~PressAwaySystem();
	
	void update(entt::registry &);
	
private:
	
};
