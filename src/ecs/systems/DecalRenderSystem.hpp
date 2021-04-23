#pragma once

#include <config.hpp>

#include <memory>
#include <vector>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <sgl/sgl_shader.hpp>
#include <ecs/systems.hpp>
#include <ecs/components.hpp>
#include <RenderObject/Camera.hpp>
#include <Util/Blackboard.hpp>

class DecalRenderSystem : public IRenderSystem {
public:

	DecalRenderSystem(const entt::registry &registry, std::shared_ptr<Camera> camera);
	~DecalRenderSystem();
	
	void draw();

private:
	GLuint m_instanceBuffer;
	size_t m_instanceBufferSize = 0;
	std::vector<glm::vec4> m_aInstanceTexOffsets;
	std::vector<glm::mat4> m_aInstanceModels;
	std::vector<GLuint> m_aInstanceTextures;
	std::vector<const Texture *> m_boundTextures;

	GLuint m_vao, m_vbo, m_ebo;
	sgl::shader m_shader;

	void initCube();
	void destroyCube();

	void collectInstanceData();
};
