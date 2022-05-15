#include <ecs/systems/DecalRenderSystem.hpp>
#include <Engine/Engine.hpp>

//////////////
//  PUBLIC  //
//////////////



DecalRenderSystem::DecalRenderSystem(const entt::registry &registry, std::shared_ptr<Camera> camera)
 : IRenderSystem(registry, camera) {

	m_shader.load("res/glsl/proto/decal_instance_vert.glsl", sgl::shader::VERTEX);
	m_shader.load("res/glsl/proto/decal_instance_frag.glsl", sgl::shader::FRAGMENT);
	m_shader.compile();
	m_shader.link();

	Blackboard::write("decalShader", &m_shader);

	glGenBuffers(1, &m_instanceBuffer);
	initCube();
}

DecalRenderSystem::~DecalRenderSystem() {
	glDeleteBuffers(1, &m_instanceBuffer);
	destroyCube();
}

void DecalRenderSystem::draw() {
	GLboolean prev_depthmask;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &prev_depthmask);
	glDepthMask(GL_FALSE);

	// collect required buffer & shader data
	m_shader.use();
	collectInstanceData();
	
	m_shader["uProjection"] = camera->getProjection();
	m_shader["uView"] = camera->getView();
	m_shader["uResolution"] = camera->getScreenSize();

	GLint texture_slot = m_boundTextures.size(); // probably makes more sense to store this as GL_TEXTURE0
	glActiveTexture(GL_TEXTURE0 + texture_slot);
	glBindTexture(GL_TEXTURE_2D, Engine::Instance->getGBuffer().m_position->get_texture());
	m_shader["uTexDepth"] = texture_slot;

	// draw
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_instanceBuffer);
	glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, m_aInstanceModels.size());
	glBindVertexArray(0);

	glDepthMask(prev_depthmask);
}


/////////////
// PRIVATE //
/////////////

void DecalRenderSystem::initCube() {
	GLfloat vertices[] = {
		-.5f, -.5f, -.5f,
		 .5f, -.5f, -.5f,
		-.5f,  .5f, -.5f,
		 .5f,  .5f, -.5f,

		-.5f, -.5f,  .5f,
		 .5f, -.5f,  .5f,
		-.5f,  .5f,  .5f,
		 .5f,  .5f,  .5f
	};
	GLuint indices[] = {
		0, 2, 1, 1, 2, 3, // back
		0, 4, 2, 4, 6, 2, // side L
		5, 1, 7, 7, 1, 3, // side R
		4, 5, 6, 6, 5, 7, // front
		0, 1, 4, 4, 1, 5, // top
		6, 3, 2, 6, 7, 3  // bottom
	};

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
		glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DecalRenderSystem::destroyCube() {
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);
}

void DecalRenderSystem::collectInstanceData() {
	m_aInstanceTexOffsets.clear();
	m_aInstanceModels.clear();
	m_aInstanceTextures.clear();
	m_boundTextures.clear();

	cregistry.view<const CPosition, const COrientation, const CDecal>().each([this](const auto &pos, const auto &orient, const auto &decal) {
		glm::mat4 model =
			glm::scale(
				glm::rotate(
					glm::translate(
						glm::mat4(1.f),
						pos.pos),
					orient.amount * glm::pi<float>() * 2.f,
					orient.orientation
					),
				decal.size);
		
		const Texture *texture = decal.texture;
		auto exists = std::find_if(m_boundTextures.begin(), m_boundTextures.end(),
			[&](const auto &o) { return texture->getTexture() == o->getTexture(); });
		
		if (exists == m_boundTextures.end()) { // texture is not bound
			m_aInstanceTextures.push_back(m_boundTextures.size());
			glActiveTexture(GL_TEXTURE0 + (GLuint)m_boundTextures.size());
			glBindTexture(GL_TEXTURE_2D, (GLuint)*texture);
			
			std::string uniformName = (std::string("uTextures[") + std::to_string(m_boundTextures.size()) + std::string("]"));
			m_shader[uniformName.c_str()] = (GLint)m_boundTextures.size();
			
			m_boundTextures.push_back(texture);
		} else { // texture already bound
			m_aInstanceTextures.push_back(std::distance(m_boundTextures.begin(), exists));
		}

		m_aInstanceTexOffsets.push_back(decal.texture_offset);
		m_aInstanceModels.push_back(model);
	});

	// move instance data to gpu
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_instanceBuffer);

	size_t required_instancebuffer_size = m_aInstanceTexOffsets.size() * sizeof(glm::vec4)
	                                      + m_aInstanceModels.size() * sizeof(glm::mat4)
										  + m_aInstanceTextures.size() * sizeof(GLuint);
	
	if (required_instancebuffer_size > m_instanceBufferSize) {
		m_instanceBufferSize = required_instancebuffer_size;
		glBufferData(GL_ARRAY_BUFFER, required_instancebuffer_size, NULL, GL_DYNAMIC_DRAW);
	}

	glBufferSubData(GL_ARRAY_BUFFER, 0, m_aInstanceTexOffsets.size() * sizeof(glm::vec4), m_aInstanceTexOffsets.data());
	glBufferSubData(GL_ARRAY_BUFFER, m_aInstanceTexOffsets.size() * sizeof(glm::vec4), m_aInstanceModels.size() * sizeof(glm::mat4), m_aInstanceModels.data());
	glBufferSubData(GL_ARRAY_BUFFER, m_aInstanceTexOffsets.size() * sizeof(glm::vec4) + m_aInstanceModels.size() * sizeof(glm::mat4), m_aInstanceTextures.size() * sizeof(GLuint), m_aInstanceTextures.data());

	// enable instancing
	GLuint attribInstanceTexOffsets = 1;
	GLuint attribInstanceModel = 2;
	GLuint attribTextureIndex = 6;

	glVertexAttribPointer(attribInstanceTexOffsets, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), NULL);
	glEnableVertexAttribArray(attribInstanceTexOffsets);
	glVertexAttribDivisor(attribInstanceTexOffsets, 1);

	for (GLuint i = 0; i < 4; ++i) {
		glVertexAttribPointer(attribInstanceModel + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(m_aInstanceTexOffsets.size() * sizeof(glm::vec4) + i * sizeof(glm::vec4)));
		glEnableVertexAttribArray(attribInstanceModel + i);
		glVertexAttribDivisor(attribInstanceModel + i, 1);
	}

	glVertexAttribIPointer(attribTextureIndex, 4, GL_UNSIGNED_INT, sizeof(GLuint), (void *)(m_aInstanceTexOffsets.size() * sizeof(glm::vec4) + m_aInstanceModels.size() * sizeof(glm::mat4)));
	glEnableVertexAttribArray(attribTextureIndex);
	glVertexAttribDivisor(attribTextureIndex, 1);

	glBindVertexArray(0);
}
