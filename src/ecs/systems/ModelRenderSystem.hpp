#pragma once

#include <memory>
#include <map>
#include <sgl/sgl_shader.hpp>
#include "Assets/Mesh.hpp"
#include "RenderObject/Camera.hpp"
#include "ecs/systems/IRenderSystem.hpp"

using MeshTransformMap = std::map<Mesh*, std::vector<glm::mat4>>;

class ModelRenderSystem : public IRenderSystem {
public:
	ModelRenderSystem(entt::registry& registry, std::shared_ptr<Camera> camera);
	~ModelRenderSystem();

	void draw() override;

private:
	// data for rendering
	sgl::shader m_shader;
	GLuint m_vao, m_vbo, m_mbo, m_ibo;

	// data previous to rendering
	MeshTransformMap m_meshTransforms;

	void initializeBuffers();
	void updateBuffers();
};
