#pragma once

#include <config.hpp>

#include <vector>
#include <unordered_map>
#include <algorithm>

#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <imgui/imgui.h>
#include <util/sgl_shader.hpp>

#include <ecs/systems/BaseUpdateSystem.hpp>
#include <ecs/systems/BaseRenderSystem.hpp>
#include <ecs/components.hpp>

#include <RenderObject/Billboard.hpp>
#include <RenderObject/Camera.hpp>

class BillboardRenderSystem : public BaseUpdateSystem, public BaseRenderSystem {
public:
	
	BillboardRenderSystem(entt::registry &registry, Camera &camera);
	BillboardRenderSystem(const BillboardRenderSystem &copy) = delete;
	~BillboardRenderSystem();
	
	void update();
	void draw();
	
		
private:
	sgl::shader instanceShader;
	
	Billboard billboardRO;
	GLuint instanceBuffer;
	
	std::vector<glm::mat4> aInstanceModels;
	std::vector<glm::vec3> aInstanceColors;
	std::vector<glm::vec4> aInstanceTexOffsets;
	std::vector<GLuint> aInstanceTextures;
	std::vector<const Texture *> boundTextures;
	
	GLint lastMaxInstanceCount = -1;
};

