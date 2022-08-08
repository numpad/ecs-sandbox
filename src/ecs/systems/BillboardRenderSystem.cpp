#include <ecs/systems/BillboardRenderSystem.hpp>
#include "Engine/Engine.hpp"

BillboardRenderSystem::BillboardRenderSystem(entt::registry& registry, std::shared_ptr<Camera> camera)
    : IUpdateSystem(registry), IRenderSystem(registry, camera) {
	glGenBuffers(1, &instanceBuffer);

	// load shader
	instanceShader.load("res/glsl/2d/billboard_instance_vert.glsl", sgl::shader::VERTEX);
	instanceShader.load("res/glsl/2d/billboard_instance_frag.glsl", sgl::shader::FRAGMENT);
	instanceShader.compile();
	instanceShader.link();

	Blackboard::write("billboardShader", &instanceShader);
}

BillboardRenderSystem::~BillboardRenderSystem() {
	glDeleteBuffers(1, &instanceBuffer);
	registry.ctx<entt::dispatcher>().trigger<LogEvent>("BillboardRenderSystem: destructed.", LogEvent::DEBUG);
}

void BillboardRenderSystem::update(float dt) {
	// TODO: this is no longer needed if we discard pixels in the shader (or do blending right?)
	//       the drawback is that transparency is no longer working, but this should be fine.
	// TODO: maybe add a separate draw call & sort for semi-transparent billboards?
	// depthsort
	registry.sort<CPosition>([this](const auto& lhs, const auto& rhs) {
		constexpr glm::vec3 noY(1.0f, 0.0f, 1.0f);
		float l1 = glm::length2((lhs.pos - this->camera->getPos()) * noY);
		float l2 = glm::length2((rhs.pos - this->camera->getPos()) * noY);

		return l1 > l2;
	});

	// required if entities exist with and without billboards, but both with postitions
	registry.sort<CBillboard, CPosition>();
}

void BillboardRenderSystem::draw() {
	// collect per instance data
	aInstanceModels.clear();
	aInstanceTexOffsets.clear();
	aInstanceTextures.clear();
	boundTextures.clear();

	instanceShader.use();
	cregistry.view<const CPosition, const CBillboard>().each([this](auto entity, auto& pos, auto& bb) {
		// optional texture region
		glm::vec4 texture_region = glm::vec4(0.f, 0.f, 1.f, 1.f);
		const CTextureRegion* texture_region_component = cregistry.try_get<CTextureRegion>(entity);
		const COrientedTexture* oriented_texture_component = cregistry.try_get<COrientedTexture>(entity);
		if (texture_region_component) {
			texture_region = texture_region_component->region;
			if (oriented_texture_component) {
				texture_region.x += texture_region.z * oriented_texture_component->get_sprite_offset();
			}
		}

		glm::mat4 modelmatrix = Billboard::calcModelMatrix(this->camera->getView(), pos.pos, bb.size);

		this->aInstanceModels.push_back(modelmatrix);
		this->aInstanceTexOffsets.push_back(texture_region);

		// collect required entitiy textures to bind
		const Texture* texture = bb.texture;
		// check if opengl texture id is already bound
		auto exists = std::find_if(boundTextures.begin(), boundTextures.end(), [&](const auto& o) {
			return texture->getTexture() == o->getTexture();
		});

		if (exists == boundTextures.end()) { // texture is not bound
			this->aInstanceTextures.push_back(boundTextures.size());
			glActiveTexture(GL_TEXTURE0 + (GLuint)boundTextures.size());
			glBindTexture(GL_TEXTURE_2D, (GLuint)*texture);

			std::string uniformName =
			    (std::string("uTextures[") + std::to_string(boundTextures.size()) + std::string("]"));
			instanceShader[uniformName.c_str()] = (GLint)boundTextures.size();

			boundTextures.push_back(texture);
		} else { // texture already bound
			this->aInstanceTextures.push_back(std::distance(boundTextures.begin(), exists));
		}
	});

	if (Engine::Instance->config.imgui_enabled) {
		if (ImGui::Begin("textures")) {
			using namespace ImGui;
			static bool uDebugToggle;
			if (Checkbox("debug: draw instance texture id", &uDebugToggle)) {
				instanceShader["uDebugToggle"] = uDebugToggle;
			}

			int j = 0;
			for (auto i : boundTextures) {
				Image((void*)(uintptr_t)i->getTexture(), ImVec2(100.f, 100.f * i->getAspectRatio()), ImVec2(0, 1),
				      ImVec2(1, 0));
				SameLine();
				GLint b;
				glActiveTexture(GL_TEXTURE0 + j);
				glGetIntegerv(GL_TEXTURE_BINDING_2D, &b);
				Text("glActiveTexture(GL_TEXTURE%d)\nglBindTexture(GL_TEXTURE_2D, %d=%d)", j, i->getTexture(), b);
				++j;
			}
		}
		ImGui::End();
	}

	// rendering:

	// disable wireframe
	GLint prevWireframeMode;
	glGetIntegerv(GL_POLYGON_MODE, &prevWireframeMode);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	GLState glState;
	glState.depth_test = true;
	glState.depth_write = true;
	glState.cull_face = true;
	Engine::Instance->graphics.setState(glState);

	// prepare shader
	instanceShader["uView"] = camera->getView();
	instanceShader["uProjection"] = camera->getProjection();

	// resize instance data buffer
	if (aInstanceModels.size() > (size_t)lastMaxInstanceCount || lastMaxInstanceCount < 0) {
		lastMaxInstanceCount = aInstanceModels.size();

		glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
		glBufferData(GL_ARRAY_BUFFER,
		             aInstanceModels.size() * sizeof(glm::mat4) + aInstanceTexOffsets.size() * sizeof(glm::vec4) +
		                 aInstanceTextures.size() * sizeof(GLuint),
		             nullptr, GL_DYNAMIC_DRAW);
	}

	// lastMaxInstanceCount = aInstanceModels.size();
	glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
	// fill model matrices
	glBufferSubData(GL_ARRAY_BUFFER, 0, aInstanceModels.size() * sizeof(glm::mat4), aInstanceModels.data());
	// fill texoffsets
	glBufferSubData(GL_ARRAY_BUFFER, aInstanceModels.size() * sizeof(glm::mat4),
	                aInstanceTexOffsets.size() * sizeof(glm::vec4), aInstanceTexOffsets.data());
	// fill textures
	glBufferSubData(GL_ARRAY_BUFFER,
	                aInstanceModels.size() * sizeof(glm::mat4) + aInstanceTexOffsets.size() * sizeof(glm::vec4),
	                aInstanceTextures.size() * sizeof(GLuint), aInstanceTextures.data());

	// bind vao
	glBindVertexArray(billboardRO.getVAO());
	// enable instancing on vertex attrib
	GLuint attribInstanceMatrix = 2;
	GLuint attribInstanceTexOff = 6;
	GLuint attribInstanceTextures = 7;
	// model matrix
	for (GLuint i = 0; i < 4; ++i) {
		glVertexAttribPointer(attribInstanceMatrix + i, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4),
		                      (void*)(i * sizeof(glm::vec4)));
		glEnableVertexAttribArray(attribInstanceMatrix + i);
		glVertexAttribDivisor(attribInstanceMatrix + i, 1);
	}
	// texoffsets
	glVertexAttribPointer(attribInstanceTexOff, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4),
	                      (void*)(aInstanceModels.size() * sizeof(glm::mat4)));
	glEnableVertexAttribArray(attribInstanceTexOff);
	glVertexAttribDivisor(attribInstanceTexOff, 1);
	// textures
	glVertexAttribIPointer(
	    attribInstanceTextures, 1, GL_UNSIGNED_INT, sizeof(GLuint),
	    (void*)(aInstanceModels.size() * sizeof(glm::mat4) + aInstanceTexOffsets.size() * sizeof(glm::vec4)));
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
