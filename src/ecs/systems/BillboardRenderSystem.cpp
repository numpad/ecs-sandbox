#include <ecs/systems/BillboardRenderSystem.hpp>

BillboardRenderSystem::BillboardRenderSystem(entt::registry &registry) {
	glGenBuffers(1, &instanceBuffer);
	glGenBuffers(1, &instanceBuffer2);
	currentBuffer = instanceBuffer;
	backBuffer = instanceBuffer2;
	
	// load shader
	instanceShader.load("res/glsl/2d/billboard_instance_vert.glsl", sgl::shader::VERTEX);
	instanceShader.load("res/glsl/2d/billboard_instance_frag.glsl", sgl::shader::FRAGMENT);
	instanceShader.compile();
	instanceShader.link();
	
}

BillboardRenderSystem::~BillboardRenderSystem() {
	glDeleteBuffers(1, &instanceBuffer);
	glDeleteBuffers(1, &instanceBuffer2);
}

void BillboardRenderSystem::depthSort(entt::registry &registry, vec3 camPos) {
	registry.sort<CPosition>([camPos](const auto &lhs, const auto &rhs) {
		constexpr vec3 noY(1.0f, 0.0f, 1.0f);
		float l1 = length2((lhs.pos - camPos) * noY);
		float l2 = length2((rhs.pos - camPos) * noY);
		
		return l1 > l2;
	});
	//registry.sort<CBillboard, CPosition>();
}

void BillboardRenderSystem::drawInstanced(entt::registry &registry,
	mat4 &uView, mat4 &uProjection) {
	
	auto renderables = registry.view<CPosition, CBillboard>();
	const size_t instances = renderables.size();
	
	#if CFG_IMGUI_ENABLED
		if (ImGui::Begin("bbRenderSystem")) {
			ImGui::Text("DrawMode: glDrawElementsInstanced");
			ImGui::Text("#entities: %zu / %d", instances, lastMaxInstanceCount);
		}
		ImGui::End();
	#endif
	
	// collect per instance data
	//aInstanceModels.clear();
	//aInstanceColors.clear();
	//aInstanceTexOffsets.clear();
	//aInstanceTextures.clear();
	boundTextures.clear();
	
	instanceShader.use();
		
	// resize instance data buffer
	if ((instances > (size_t)lastMaxInstanceCount || lastMaxInstanceCount < 0)) {
		lastMaxInstanceCount = instances;
		
		glBindBuffer(GL_ARRAY_BUFFER, backBuffer);
		glBufferData(GL_ARRAY_BUFFER,
			instances * sizeof(mat4)
			+ instances * sizeof(vec3)
			+ instances * sizeof(vec4)
			+ instances * sizeof(GLuint),
			nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, currentBuffer);
		glBufferData(GL_ARRAY_BUFFER,
			instances * sizeof(mat4)
			+ instances * sizeof(vec3)
			+ instances * sizeof(vec4)
			+ instances * sizeof(GLuint),
			nullptr, GL_DYNAMIC_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, currentBuffer);
	void *instanceData = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	mat4   *modelMatrixData = (mat4   *)(instanceData + 0);
	vec3   *colorData       = (vec3   *)(instanceData + instances * (sizeof(mat4)));
	vec4   *texoffsetData   = (vec4   *)(instanceData + instances * (sizeof(mat4) + sizeof(vec3)));
	GLuint *textureData     = (GLuint *)(instanceData + instances * (sizeof(mat4) + sizeof(vec3) + sizeof(vec4)));
	
	size_t currentInstance = 0;
	renderables.each([=, this, &uView, &currentInstance,
		modelMatrixData, colorData, texoffsetData, textureData
		](auto entity, auto &pos, auto &bb) {
		
		//this->aInstanceModels.push_back(Billboard::calcModelMatrix(uView, pos.pos, bb.size));
		modelMatrixData[currentInstance] = Billboard::calcModelMatrix(uView, pos.pos, bb.size);
		//this->aInstanceColors.push_back(bb.color);
		colorData[currentInstance] = bb.color;
		//this->aInstanceTexOffsets.push_back(bb.getSubRect());
		texoffsetData[currentInstance] = bb.getSubRect();
		// collect required entitiy textures to bind
		const Texture *texture = bb.texture;
		// check if opengl texture id is already bound
		auto exists = std::find_if(boundTextures.begin(), boundTextures.end(),
			[&](const auto &o) { return texture->getTexture() == o->getTexture(); });
		
		if (exists == boundTextures.end()) { // texture is not bound
			//this->aInstanceTextures.push_back(boundTextures.size());
			textureData[currentInstance] = boundTextures.size();
			glActiveTexture(GL_TEXTURE0 + (GLuint)boundTextures.size());
			glBindTexture(GL_TEXTURE_2D, (GLuint)*texture);
			
			std::string uniformName = (std::string("uTextures[") + std::to_string(boundTextures.size()) + std::string("]"));
			instanceShader[uniformName.c_str()] = (GLint)boundTextures.size();
			
			boundTextures.push_back(texture);
		} else { // texture already bound
			//this->aInstanceTextures.push_back(std::distance(boundTextures.begin(), exists));
			textureData[currentInstance] = std::distance(boundTextures.begin(), exists);
		}
		
		++currentInstance;
	});
	swapBuffers();
	
	// debug
	#if CFG_IMGUI_ENABLED
		if (ImGui::Begin("textures")) {
			using namespace ImGui;
			static bool uDebugToggle;
			if (Checkbox("debug: draw instance texture id", &uDebugToggle)) {
				instanceShader["uDebugToggle"] = uDebugToggle;
			}
			
			int j = 0;
			for (auto i : boundTextures) {
				Image((void *)i->getTexture(),
					ImVec2(100.f, 100.f * i->getAspectRatio()),
					ImVec2(0, 1), ImVec2(1, 0));
				SameLine();
				GLint b;
				glActiveTexture(GL_TEXTURE0 + j);
				glGetIntegerv(GL_TEXTURE_BINDING_2D, &b);
				Text("glActiveTexture(GL_TEXTURE%d)\nglBindTexture(GL_TEXTURE_2D, %d=%d)", j, i->getTexture(), b);
				++j;
			}
		}
		ImGui::End();
	#endif
	
	
	// rendering:
	
	// disable wireframe
	GLint prevWireframeMode;
	glGetIntegerv(GL_POLYGON_MODE, &prevWireframeMode);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	// prepare shader
	instanceShader["uView"] = uView;
	instanceShader["uProjection"] = uProjection;
	
	//lastMaxInstanceCount = instances;
	//glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
	// fill model matrices
	//glBufferSubData(GL_ARRAY_BUFFER, 0,
	//	instances * sizeof(mat4), aInstanceModels.data());
	//// fill colors
	//glBufferSubData(GL_ARRAY_BUFFER, instances * sizeof(mat4),
	//	aInstanceColors.size() * sizeof(vec3), aInstanceColors.data());
	//// fill texoffsets
	//glBufferSubData(GL_ARRAY_BUFFER, instances * sizeof(mat4) + instances * sizeof(vec3),
	//	aInstanceTexOffsets.size() * sizeof(vec4), aInstanceTexOffsets.data());
	//// fill textures
	//glBufferSubData(GL_ARRAY_BUFFER, instances * sizeof(mat4) + instances * sizeof(vec3) + instances * sizeof(vec4),
	//	aInstanceTextures.size() * sizeof(GLuint), aInstanceTextures.data());
	glUnmapBuffer(GL_ARRAY_BUFFER);
	
	// bind vao
	glBindVertexArray(billboardRO.getVAO());
	// enable instancing on vertex attrib
	GLuint attribInstanceMatrix = 2;
	GLuint attribInstanceColor = 6;
	GLuint attribInstanceTexOff = 7;
	GLuint attribInstanceTextures = 8;
	// model matrix
	for (GLuint i = 0; i < 4; ++i) {
		glVertexAttribPointer(attribInstanceMatrix + i, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(vec4), (void *)(i * sizeof(vec4)));
		glEnableVertexAttribArray(attribInstanceMatrix + i);
		glVertexAttribDivisor(attribInstanceMatrix + i, 1);
	}
	// color
	glVertexAttribPointer(attribInstanceColor, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)(instances * sizeof(mat4)));
	glEnableVertexAttribArray(attribInstanceColor);
	glVertexAttribDivisor(attribInstanceColor, 1);
	// texoffsets
	glVertexAttribPointer(attribInstanceTexOff, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), (void *)(instances * sizeof(mat4) + instances * sizeof(vec3)));
	glEnableVertexAttribArray(attribInstanceTexOff);
	glVertexAttribDivisor(attribInstanceTexOff, 1);
	// textures
	glVertexAttribIPointer(attribInstanceTextures, 1, GL_UNSIGNED_INT, sizeof(GLuint), (void *)(instances * sizeof(mat4) + instances * sizeof(vec3) + instances * sizeof(vec4)));
	glEnableVertexAttribArray(attribInstanceTextures);
	glVertexAttribDivisor(attribInstanceTextures, 1);

	// draw
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, instances);
	
	// cleanup
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// restore polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, prevWireframeMode);
}

