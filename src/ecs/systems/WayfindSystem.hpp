#pragma once

#include <GL/gl3w.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <sgl/sgl_shader.hpp>
#include <ecs/components.hpp>
#include <ecs/systems/BaseUpdateSystem.hpp>
#include <ecs/systems/BaseRenderSystem.hpp>

using namespace glm;

class WayfindSystem : public BaseUpdateSystem, public BaseRenderSystem {
public:
	
	WayfindSystem(entt::registry &registry, std::shared_ptr<Camera> camera);
	WayfindSystem(const WayfindSystem &copy) = delete;
	~WayfindSystem() override;
	
	void update() override;
	void draw() override;
	
private:
	GLuint lineVAO, lineVBO;
	sgl::shader lineShader;
	
};
