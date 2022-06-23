#pragma once

#include <GL/gl3w.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <par/par_shapes.h>

#include "sgl/sgl_shader.hpp"
#include "ecs/components.hpp"
#include "ecs/systems/IRenderSystem.hpp"
#include "RenderObject/Camera.hpp"
#include "Graphics/GLState.hpp"
#include "Util/Math3d.hpp"

class LightVolumeRenderSystem : public IRenderSystem {
public:
	LightVolumeRenderSystem(const entt::registry& registry, std::shared_ptr<Camera> camera);
	~LightVolumeRenderSystem();

	void draw() override;

private:
	std::vector<glm::vec4> m_aInstancePositionsWithRadiuses;
	std::vector<glm::vec3> m_aInstanceColors;

	GLuint m_vao, m_ibo, m_ebo;
	par_shapes_mesh* m_sphereMesh;
	GLsizei m_triangleCount, m_meshVerticesSize;
	sgl::shader m_shader;

	void setupBuffer();
	void destroyBuffer();
	void updateBuffer();

	void onPointLightConstructed(const entt::registry& registry, entt::entity entity);
};
