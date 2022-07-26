#pragma once

#include <memory>
#include <sgl/sgl_shader.hpp>
#include "RenderObject/Camera.hpp"
#include "ecs/systems/IRenderSystem.hpp"

class ModelRenderSystem : public IRenderSystem {
public:
	
	ModelRenderSystem(entt::registry& registry, std::shared_ptr<Camera> camera);
	~ModelRenderSystem();

	void draw() override;


private:

	void updateBuffers();

	sgl::shader m_shader;
	GLuint m_vao, m_vbo, m_mbo, m_ibo;
};
