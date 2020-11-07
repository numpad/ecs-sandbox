#pragma once

#include <vector>

#include <GL/gl3w.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <sgl/sgl_shader.hpp>
#include <ecs/systems.hpp>
#include <ecs/components.hpp>
#include <ecs/events.hpp>

using namespace glm;

class PrimitiveRenderSystem : public BaseUpdateSystem, public BaseRenderSystem {
public:
	
	PrimitiveRenderSystem(entt::registry &registry, std::shared_ptr<Camera> camera);
	~PrimitiveRenderSystem();
	
	void receive(const DrawPrimitiveEvent &event);
	
	void update();
	void draw();
	
private:
	GLuint lineVAO, lineVBO;
	sgl::shader lineShader;
	
	std::vector<vec3> vertexList;
	
};
