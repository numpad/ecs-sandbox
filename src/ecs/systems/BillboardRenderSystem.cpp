#include <ecs/systems/BillboardRenderSystem.hpp>

BillboardRenderSystem::BillboardRenderSystem(AssetManager &am)
	: assetManager(am)
{
	glGenBuffers(1, &instanceBuffer);
	
}

BillboardRenderSystem::~BillboardRenderSystem() {
	glDeleteBuffers(1, &instanceBuffer);
}

void BillboardRenderSystem::depthSort(entt::registry &registry, glm::vec3 camPos) {
	registry.sort<CBillboard, CPosition>();
	registry.sort<CPosition>([camPos](const auto &lhs, const auto &rhs) {
		const glm::vec3 noY(1.0f, 0.0f, 1.0f);
		float l1 = glm::length2((lhs.pos - camPos) * noY);
		float l2 = glm::length2((rhs.pos - camPos) * noY);
		
		return l1 > l2;
	});
}

void BillboardRenderSystem::drawInstanced(entt::registry &registry,
	glm::mat4 &uView, glm::mat4 &uProjection) {
	
	#if CFG_IMGUI_ENABLED
		if (ImGui::Begin("bbRenderSystem")) {
			ImGui::Text("DrawMode: instanced");
			ImGui::Text("#entities: %zu / %d", aInstanceModels.size(), lastMaxInstanceCount);
		}
		ImGui::End();
	#endif

	// collect per instance data
	aInstanceModels.clear();
	aInstanceColors.clear();
	aInstanceTexOffsets.clear();
	
	// TODO: multiple textures
	registry.view<CPosition, CBillboard>().each(
		[this, &uView](auto entity, auto &pos, auto &bb) {
		
		this->aInstanceModels.push_back(billboardRO.calcModelMatrix(uView, pos.pos, bb.size));
		this->aInstanceColors.push_back(bb.color);
		this->aInstanceTexOffsets.push_back(bb.texture_offset);
	});
	
	
	// rendering:
	
	// prepare shader
	auto &shader = billboardRO.getInstanceShader();
	shader.use();
	shader["uTexture"] = 0;
	shader["uView"] = uView;
	shader["uProjection"] = uProjection;
	
	// resize instance data buffer
	if (aInstanceModels.size() > (size_t)lastMaxInstanceCount || lastMaxInstanceCount < 0) {
		lastMaxInstanceCount = aInstanceModels.size();
		
		glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
		glBufferData(GL_ARRAY_BUFFER,
			aInstanceModels.size() * sizeof(glm::mat4)
			+ aInstanceColors.size() * sizeof(glm::vec3)
			+ aInstanceTexOffsets.size() * sizeof(glm::vec4),
			nullptr, GL_DYNAMIC_DRAW);
		
		#if CFG_DEBUG
			printf("[LOG] BillboardRenderSystem: new buffer data.\n");
		#endif
	}
	
	//lastMaxInstanceCount = aInstanceModels.size();
	glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
	// fill model matrices
	glBufferSubData(GL_ARRAY_BUFFER, 0,
		aInstanceModels.size() * sizeof(glm::mat4), &aInstanceModels[0]);
	// fill colors
	glBufferSubData(GL_ARRAY_BUFFER, aInstanceModels.size() * sizeof(glm::mat4),
		aInstanceColors.size() * sizeof(glm::vec3), &aInstanceColors[0]);
	
	glBufferSubData(GL_ARRAY_BUFFER, aInstanceModels.size() * sizeof(glm::mat4) + aInstanceColors.size() * sizeof(glm::vec3),
		aInstanceTexOffsets.size() * sizeof(glm::vec4), &aInstanceTexOffsets[0]);
	
	// bind vao
	glBindVertexArray(billboardRO.getVAO());
	// enable instancing on vertex attrib
	GLuint attribInstanceMatrix = 2;
	GLuint attribInstanceColor = 6;
	GLuint attribInstanceTexOff = 7;
	
	// model matrix
	glVertexAttribPointer(attribInstanceMatrix + 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(0 * sizeof(glm::vec4)));
	glVertexAttribPointer(attribInstanceMatrix + 1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(1 * sizeof(glm::vec4)));
	glVertexAttribPointer(attribInstanceMatrix + 2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(2 * sizeof(glm::vec4)));
	glVertexAttribPointer(attribInstanceMatrix + 3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(3 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(attribInstanceMatrix + 0);
	glEnableVertexAttribArray(attribInstanceMatrix + 1);
	glEnableVertexAttribArray(attribInstanceMatrix + 2);
	glEnableVertexAttribArray(attribInstanceMatrix + 3);
	glVertexAttribDivisor(attribInstanceMatrix + 0, 1);
	glVertexAttribDivisor(attribInstanceMatrix + 1, 1);
	glVertexAttribDivisor(attribInstanceMatrix + 2, 1);
	glVertexAttribDivisor(attribInstanceMatrix + 3, 1);
	// color
	glVertexAttribPointer(attribInstanceColor, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)(aInstanceModels.size() * sizeof(glm::mat4)));
	glEnableVertexAttribArray(attribInstanceColor);
	glVertexAttribDivisor(attribInstanceColor, 1);
	// texoffsets
	glVertexAttribPointer(attribInstanceTexOff, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void *)(aInstanceModels.size() * sizeof(glm::mat4) + aInstanceColors.size() * sizeof(glm::vec3)));
	glEnableVertexAttribArray(attribInstanceTexOff);
	glVertexAttribDivisor(attribInstanceTexOff, 1);
	
	// textures
	glActiveTexture(GL_TEXTURE0);
	assetManager.getTexture("res/images/textures/dungeon.png")->bind();
	
	// draw
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, aInstanceModels.size());
	
	// cleanup
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

