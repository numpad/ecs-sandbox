#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <config.hpp>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <RenderObject/Billboard.hpp>
#include <ecs/components.hpp>
#include <imgui/imgui.h>

class BillboardRenderSystem {
public:
	
	BillboardRenderSystem();
	BillboardRenderSystem(const BillboardRenderSystem &copy) = delete;
	~BillboardRenderSystem();
	
	void depthSort(entt::registry &, glm::vec3 camPos);
	void drawInstanced(entt::registry &, glm::mat4 &uView, glm::mat4 &uProj);
	
private:
	Billboard billboardRO;
	GLuint instanceBuffer;
		
	std::vector<glm::mat4> aInstanceModels;
	std::vector<glm::vec3> aInstanceColors;
	std::vector<glm::vec4> aInstanceTexOffsets;
	std::vector<GLuint> aInstanceTextures;
	std::vector<const Texture *> boundTextures;
	
	GLint lastMaxInstanceCount = -1;
	
};

