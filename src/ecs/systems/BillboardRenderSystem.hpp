#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <RenderObject/Billboard.hpp>
#include <ecs/components.hpp>
#include <imgui/imgui.h>

class BillboardRenderSystem {
public:
	BillboardRenderSystem() = default;
	
	void draw(entt::registry &, Billboard &, glm::mat4 &uView, glm::mat4 &uProj);

private:
	
	
};

