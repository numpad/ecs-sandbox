#pragma once

#include <ecs/components.hpp>
#include <entt/entt.hpp>
#include <GLFW/glfw3.h>

class CHealthbarRenderSystem {
public:
	
	CHealthbarRenderSystem();
	
	void draw(entt::registry &registry);
	
private:
	
};
