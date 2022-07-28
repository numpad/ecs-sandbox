#include "ecs/systems/ModelRenderSystem.hpp"
#include "Assets/Loaders/ObjLoader.hpp"
#include "Assets/Vertex.hpp"
#include "Engine/Engine.hpp"
#include "Graphics/GLState.hpp"
#include "ecs/components/CModel.hpp"
#include "ecs/components/COrientation.hpp"
#include "ecs/components/CPosition.hpp"
#include "ecs/systems/IRenderSystem.hpp"
#include <GL/gl3w.h>
#include <GL/glcorearb.h>

////////////
// PUBLIC //
////////////

ModelRenderSystem::ModelRenderSystem(entt::registry& registry, std::shared_ptr<Camera> camera)
    : IRenderSystem(registry, camera) {

	// prepare buffers
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_mbo);
	glGenBuffers(1, &m_ibo);

	// load shader
	m_shader.load("res/glsl/proto/model_multi_vert.glsl", sgl::shader::VERTEX);
	m_shader.load("res/glsl/proto/model_multi_frag.glsl", sgl::shader::FRAGMENT);
	m_shader.compile();
	m_shader.link();

	initializeBuffers();
}

ModelRenderSystem::~ModelRenderSystem() {
	// delete buffers
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_mbo);
	glDeleteBuffers(1, &m_ibo);
}

void ModelRenderSystem::draw() {
	updateBuffers();

	GLState state;
	state.cull_face = true;
	state.depth_test = true;
	state.depth_write = true;
	Engine::Instance->getGraphics().setState(state);
	
	m_shader.use();
	m_shader["uProjection"] = camera->getProjection();
	m_shader["uView"] = camera->getView();
	
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_ibo);
	glBindVertexArray(m_vao);
	glMultiDrawArraysIndirect(GL_TRIANGLES, NULL, m_meshTransforms.size(), 0);
	glBindVertexArray(0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}

/////////////
// PRIVATE //
/////////////

static inline void _addMeshTransform(MeshTransformMap& meshTransforms, Mesh* mesh, const glm::mat4& transform) {
	meshTransforms.try_emplace(mesh, std::vector<glm::mat4>{});
	meshTransforms.at(mesh).push_back(transform);
}

static inline GLuint _getMeshTransformCount(const MeshTransformMap& meshTransforms) {
	GLuint count = 0;
	
	for (auto it = meshTransforms.begin(); it != meshTransforms.end(); ++it) {
		count += it->second.size();
	}

	return count;
}

void ModelRenderSystem::initializeBuffers() {
	// upload to gpu
	glBindVertexArray(m_vao);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_ibo);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, 1 * sizeof(GLuint), nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 1 * sizeof(Vertex), nullptr, GL_STATIC_DRAW);

	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	// normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// color
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	// texcoord
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoords));

	glBindBuffer(GL_ARRAY_BUFFER, m_mbo);
	glBufferData(GL_ARRAY_BUFFER, 1 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);

	// model matrix
	glEnableVertexAttribArray(4 + 0);
	glEnableVertexAttribArray(4 + 1);
	glEnableVertexAttribArray(4 + 2);
	glEnableVertexAttribArray(4 + 3);
	glVertexAttribPointer(4 + 0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * 0));
	glVertexAttribPointer(4 + 1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * 1));
	glVertexAttribPointer(4 + 2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * 2));
	glVertexAttribPointer(4 + 3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * 3));
	glVertexAttribDivisor(4 + 0, 1);
	glVertexAttribDivisor(4 + 1, 1);
	glVertexAttribDivisor(4 + 2, 1);
	glVertexAttribDivisor(4 + 3, 1);
	

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModelRenderSystem::updateBuffers() {
	// collect meshes & transforms
	m_meshTransforms.clear();
	cregistry.view<const CPosition, const COrientation, const CModel>().each([this](const CPosition& cpos, const COrientation& corientation, const CModel& cmodel) {
		const glm::mat4 modelMatrix = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), cpos.pos), corientation.amount, corientation.orientation), glm::vec3(0.75f));
		
		_addMeshTransform(m_meshTransforms, cmodel.mesh, modelMatrix);
	});
	
	
	// order mesh vertices & produce indirect draw calls for gpu
	std::vector<Vertex> vertices;
	std::vector<GLuint> indirectData;
	GLuint verticesOffset = 0;
	GLuint instancesOffset = 0;
	for (auto it = m_meshTransforms.begin(); it != m_meshTransforms.end(); ++it) {
		vertices.insert(vertices.end(), it->first->vertices.begin(), it->first->vertices.end());

		indirectData.push_back(it->first->vertices.size());
		indirectData.push_back(it->second.size());
		indirectData.push_back(verticesOffset);
		indirectData.push_back(instancesOffset);

		verticesOffset += it->first->vertices.size();
		instancesOffset += it->second.size();
	}
	
	// upload indirect draw calls
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_ibo);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, indirectData.size() * sizeof(GLuint), &indirectData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	
	// upload mesh vertices
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	// upload model matrices
	glBindBuffer(GL_ARRAY_BUFFER, m_mbo);
	glBufferData(GL_ARRAY_BUFFER, _getMeshTransformCount(m_meshTransforms) * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
	GLintptr offset = 0;
	for (auto it = m_meshTransforms.begin(); it != m_meshTransforms.end(); ++it) {
		glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(glm::mat4), it->second.size() * sizeof(glm::mat4), it->second.data());
		offset += it->second.size();
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

