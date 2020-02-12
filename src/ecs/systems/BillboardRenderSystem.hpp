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
#include <util/sgl_shader.hpp>

using namespace glm;

class BillboardRenderSystem {
public:
	
	BillboardRenderSystem(entt::registry &registry);
	BillboardRenderSystem(const BillboardRenderSystem &copy) = delete;
	~BillboardRenderSystem();
	
	void depthSort(entt::registry &, vec3 camPos);
	void drawInstanced(entt::registry &, mat4 &uView, mat4 &uProj);
	
private:
	//entt::group<CPosition, CBillboard> drawGroup{};
	sgl::shader instanceShader;
	
	Billboard billboardRO;
	GLuint instanceBuffer, instanceBuffer2;
	GLuint currentBuffer, backBuffer;
	int resizes = 0;
	void swapBuffers() {
		GLuint tmp = currentBuffer;
		currentBuffer = backBuffer;
		backBuffer = tmp;
	}
	//std::vector<mat4> aInstanceModels;
	//std::vector<vec3> aInstanceColors;
	//std::vector<vec4> aInstanceTexOffsets;
	//std::vector<GLuint> aInstanceTextures;
	std::vector<const Texture *> boundTextures;
	
	GLint lastMaxInstanceCount = -1;
};

