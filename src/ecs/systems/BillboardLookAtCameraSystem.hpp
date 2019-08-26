#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <RenderObject/Billboard.hpp>

class BillboardLookAtCameraSystem {
public:
	
	BillboardLookAtCameraSystem() = default;
	
	void update(entt::registry &registry, glm::mat4 &uView, glm::vec3 renderpos);
	
private:
	
};
