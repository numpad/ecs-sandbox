#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <par/par_shapes.h>

#include "sgl/sgl_shader.hpp"
#include "ecs/components.hpp"
#include "RenderObject/Camera.hpp"

#include "ecs/systems/IRenderSystem.hpp"

class LightVolumeRenderSystem : public IRenderSystem {
public:

	LightVolumeRenderSystem(const entt::registry &registry, std::shared_ptr<Camera> camera);
	~LightVolumeRenderSystem();

	void draw() override;

private:
	std::vector<glm::vec4> m_aInstancePositionsWithRadiuses;
	std::vector<glm::vec3> m_aInstanceColors;
 
	GLuint m_vao, m_ibo, m_ebo;
	GLsizei m_triangleCount;
	sgl::shader m_shader;

	void setupBuffer();
	void destroyBuffer();
};
