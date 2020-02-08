#include <ecs/systems/BillboardRenderSystem.hpp>

BillboardRenderSystem::BillboardRenderSystem() {
	glGenBuffers(1, &instanceBuffer);
	
	// load shader
	instanceShader.load("res/glsl/2d/billboard_instance_vert.glsl", sgl::shader::VERTEX);
	instanceShader.load("res/glsl/2d/billboard_instance_frag.glsl", sgl::shader::FRAGMENT);
	instanceShader.compile();
	instanceShader.link();
	
}

BillboardRenderSystem::~BillboardRenderSystem() {
	glDeleteBuffers(1, &instanceBuffer);
}

void BillboardRenderSystem::depthSort(entt::registry &registry, glm::vec3 camPos) {
	registry.sort<CPosition>([camPos](const auto &lhs, const auto &rhs) {
		const glm::vec3 noY(1.0f, 0.0f, 1.0f);
		float l1 = glm::length2((lhs.pos - camPos) * noY);
		float l2 = glm::length2((rhs.pos - camPos) * noY);
		
		return l1 > l2;
	});
	registry.sort<CBillboard, CPosition>();
}

void BillboardRenderSystem::drawInstanced(entt::registry &registry,
	glm::mat4 &uView, glm::mat4 &uProjection) {
	
	#if CFG_IMGUI_ENABLED
		if (ImGui::Begin("bbRenderSystem")) {
			ImGui::Text("DrawMode: glDrawElementsInstanced");
			ImGui::Text("#entities: %zu / %d", aInstanceModels.size(), lastMaxInstanceCount);
		}
		ImGui::End();
	#endif
	
	// collect per instance data
	aInstanceModels.clear();
	aInstanceColors.clear();
	aInstanceTexOffsets.clear();
	aInstanceTextures.clear();
	boundTextures.clear();
	
	instanceShader.use();
	registry.view<CPosition, CBillboard>().each(
		[this, &uView](auto entity, auto &pos, auto &bb) {
		
		this->aInstanceModels.push_back(Billboard::calcModelMatrix(uView, pos.pos, bb.size));
		this->aInstanceColors.push_back(bb.color);
		this->aInstanceTexOffsets.push_back(bb.getSubRect());
		
		// collect required entitiy textures to bind
		const Texture *texture = bb.texture;
		// check if opengl texture id is already bound
		auto exists = std::find_if(boundTextures.begin(), boundTextures.end(),
			[&](const auto &o) { return texture->getTexture() == o->getTexture(); });
		
		if (exists == boundTextures.end()) { // texture is not bound
			this->aInstanceTextures.push_back(boundTextures.size());
			glActiveTexture(GL_TEXTURE0 + (GLuint)boundTextures.size());
			glBindTexture(GL_TEXTURE_2D, (GLuint)*texture);
			
			std::string uniformName = (std::string("uTextures[") + std::to_string(boundTextures.size()) + std::string("]"));
			instanceShader[uniformName.c_str()] = (GLint)boundTextures.size();
			
			boundTextures.push_back(texture);
		} else { // texture already bound
			this->aInstanceTextures.push_back(std::distance(boundTextures.begin(), exists));
		}
	});
	
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
	
	// resize instance data buffer
	if (aInstanceModels.size() > (size_t)lastMaxInstanceCount || lastMaxInstanceCount < 0) {
		lastMaxInstanceCount = aInstanceModels.size();
		
		glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
		glBufferData(GL_ARRAY_BUFFER,
			aInstanceModels.size() * sizeof(glm::mat4)
			+ aInstanceColors.size() * sizeof(glm::vec3)
			+ aInstanceTexOffsets.size() * sizeof(glm::vec4)
			+ aInstanceTextures.size() * sizeof(GLuint),
			nullptr, GL_DYNAMIC_DRAW);
	}
	
	//lastMaxInstanceCount = aInstanceModels.size();
	glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
	// fill model matrices
	glBufferSubData(GL_ARRAY_BUFFER, 0,
		aInstanceModels.size() * sizeof(glm::mat4), aInstanceModels.data());
	// fill colors
	glBufferSubData(GL_ARRAY_BUFFER, aInstanceModels.size() * sizeof(glm::mat4),
		aInstanceColors.size() * sizeof(glm::vec3), aInstanceColors.data());
	// fill texoffsets
	glBufferSubData(GL_ARRAY_BUFFER, aInstanceModels.size() * sizeof(glm::mat4) + aInstanceColors.size() * sizeof(glm::vec3),
		aInstanceTexOffsets.size() * sizeof(glm::vec4), aInstanceTexOffsets.data());
	// fill textures
	glBufferSubData(GL_ARRAY_BUFFER, aInstanceModels.size() * sizeof(glm::mat4) + aInstanceColors.size() * sizeof(glm::vec3) + aInstanceTexOffsets.size() * sizeof(glm::vec4),
		aInstanceTextures.size() * sizeof(GLuint), aInstanceTextures.data());
	
	// bind vao
	glBindVertexArray(billboardRO.getVAO());
	// enable instancing on vertex attrib
	GLuint attribInstanceMatrix = 2;
	GLuint attribInstanceColor = 6;
	GLuint attribInstanceTexOff = 7;
	GLuint attribInstanceTextures = 8;
	// model matrix
	for (GLuint i = 0; i < 4; ++i) {
		glVertexAttribPointer(attribInstanceMatrix + i, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(i * sizeof(glm::vec4)));
		glEnableVertexAttribArray(attribInstanceMatrix + i);
		glVertexAttribDivisor(attribInstanceMatrix + i, 1);
	}
	// color
	glVertexAttribPointer(attribInstanceColor, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)(aInstanceModels.size() * sizeof(glm::mat4)));
	glEnableVertexAttribArray(attribInstanceColor);
	glVertexAttribDivisor(attribInstanceColor, 1);
	// texoffsets
	glVertexAttribPointer(attribInstanceTexOff, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void *)(aInstanceModels.size() * sizeof(glm::mat4) + aInstanceColors.size() * sizeof(glm::vec3)));
	glEnableVertexAttribArray(attribInstanceTexOff);
	glVertexAttribDivisor(attribInstanceTexOff, 1);
	// textures
	glVertexAttribIPointer(attribInstanceTextures, 1, GL_UNSIGNED_INT, sizeof(GLuint), (void *)(aInstanceModels.size() * sizeof(glm::mat4) + aInstanceColors.size() * sizeof(glm::vec3) + aInstanceTexOffsets.size() * sizeof(glm::vec4)));
	glEnableVertexAttribArray(attribInstanceTextures);
	glVertexAttribDivisor(attribInstanceTextures, 1);

	// draw
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, aInstanceModels.size());
	
	// cleanup
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// restore polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, prevWireframeMode);
}

