#pragma once

#include <GL/gl3w.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <sgl/sgl_shader.hpp>
#include <ecs/components.hpp>
#include <ecs/systems/IUpdateSystem.hpp>
#include <ecs/systems/IRenderSystem.hpp>

using namespace glm;

class WayfindSystem : public IUpdateSystem, public IRenderSystem {
public:
	
	WayfindSystem(entt::registry &registry, std::shared_ptr<Camera> camera);
	WayfindSystem(const WayfindSystem &copy) = delete;
	~WayfindSystem() override;
	
	void update(float dt) override;
	void draw() override;
	
private:
	GLuint lineVAO, lineVBO;
	sgl::shader lineShader;
	
};
