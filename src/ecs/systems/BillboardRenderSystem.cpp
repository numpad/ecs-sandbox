#include <ecs/systems/BillboardRenderSystem.hpp>

BillboardRenderSystem::BillboardRenderSystem()
	: billboardRO(Billboard("res/images/sprites/default_soldier_s.png"))
{
	glGenBuffers(1, &instanceBuffer);
	
}

BillboardRenderSystem::~BillboardRenderSystem() {
	glDeleteBuffers(1, &instanceBuffer);
}

void BillboardRenderSystem::draw(entt::registry &registry,
	glm::mat4 &uView, glm::mat4 &uProjection) {
	
	static glm::vec3 zero(0.0f);
	glm::vec3 *renderTarget;
	static bool checked = false;
	if (ImGui::Begin("bbRenderSystem")) {
		ImGui::Text("DrawMode: default");
		ImGui::Checkbox("Flat billboards?", &checked);
	}
	if (checked) {
		renderTarget = &zero;
	} else {
		renderTarget = nullptr;
	}
	ImGui::End();
	
	
	registry.view<CPosition, CBillboard>().each(
		[this, &uView, &uProjection, renderTarget](auto entity, auto &pos, auto &bb) {
		
		this->billboardRO.draw(uView, uProjection, pos.pos, bb.size, renderTarget);
	});
}


void BillboardRenderSystem::drawInstanced(entt::registry &registry,
	glm::mat4 &uView, glm::mat4 &uProjection) {
	
	static glm::vec3 zero(0.0f);
	glm::vec3 *renderTarget;
	static bool checked = false;
	
	if (ImGui::Begin("bbRenderSystem")) {
		ImGui::Text("DrawMode: instanced");
		ImGui::Checkbox("Flat billboards?", &checked);
	}
	if (checked) {
		renderTarget = &zero;
	} else {
		renderTarget = nullptr;
	}
	ImGui::End();
	
	std::vector<glm::mat4> uInstanceModels;
	uInstanceModels.clear();
	// collect model matrices
	registry.view<CPosition, CBillboard>().each(
		[this, &uView, &uProjection, renderTarget, &uInstanceModels](auto entity, auto &pos, auto &bb) {
		
		glm::vec3 rt;
		if (!renderTarget)
			rt = pos.pos;
		else rt = *renderTarget;
		
		uInstanceModels.push_back(billboardRO.calcModelMatrix(uView, pos.pos, rt, bb.size));
	});
	
	
	// rendering:
	
	// prepare shader
	auto &shader = billboardRO.getInstanceShader();
	shader.use();
	shader["uTexture"] = 0;
	shader["uView"] = uView;
	shader["uProjection"] = uProjection;
	
	// load instance model matrices into array buffer
	glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
	glBufferData(GL_ARRAY_BUFFER, uInstanceModels.size() * sizeof(glm::mat4), &uInstanceModels[0], GL_STATIC_DRAW);
	// bind vao
	glBindVertexArray(billboardRO.getVAO());
	// enable instancing on vertex attrib
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)0);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(1 * sizeof(glm::vec4)));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(2 * sizeof(glm::vec4)));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(3 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	
	glActiveTexture(GL_TEXTURE0);
	billboardRO.getTexture().bind();
	
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, uInstanceModels.size());
	
	glBindVertexArray(0);
	billboardRO.getTexture().unbind();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

