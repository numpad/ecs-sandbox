#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <RenderObject/Billboard.hpp>
#include <ecs/components.hpp>

namespace BillboardRender {
	
	void draw(entt::registry &, Billboard &, glm::mat4 &uView, glm::mat4 &uProj);
}

