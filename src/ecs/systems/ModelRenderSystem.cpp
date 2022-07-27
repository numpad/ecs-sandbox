#include "ecs/systems/ModelRenderSystem.hpp"
#include "Assets/Loaders/ObjLoader.hpp"
#include "Assets/Vertex.hpp"
#include "Engine/Engine.hpp"
#include "Graphics/GLState.hpp"
#include "ecs/components/CModel.hpp"
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

	updateBuffers();
}

ModelRenderSystem::~ModelRenderSystem() {
	// delete buffers
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_mbo);
	glDeleteBuffers(1, &m_ibo);
}

void ModelRenderSystem::draw() {
	GLState state;
	state.cull_face = true;
	state.depth_test = true;
	state.depth_write = true;
	Engine::Instance->getGraphics().setState(state);

	m_shader.use();
	m_shader["uProjection"] = camera->getProjection();
	m_shader["uView"] = camera->getView();

	cregistry.view<const CPosition, const CModel>().each([](const CPosition& cpos, const CModel& cmodel) {

	});

	constexpr int INDIRECT_COUNT = 2;
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_ibo);
	glBindVertexArray(m_vao);
	glMultiDrawArraysIndirect(GL_TRIANGLES, NULL, INDIRECT_COUNT, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}

/////////////
// PRIVATE //
/////////////

void ModelRenderSystem::updateBuffers() {
	std::vector<glm::mat4> modelMatrices = {
		glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.0f)), glm::vec3(0.75f)),
		glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(1.00f)),
		glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.0f, 0.0f)), glm::vec3(1.25f)),
		glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f)), glm::vec3(1.50f)),
		glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(2.5f, 0.0f, 0.0f)), glm::vec3(1.75f))
	};

	std::vector<Vertex> vertices;
	Mesh* mesh1 = ObjLoader::load("res/models/raft/barrel.obj");
	Mesh* mesh2 = ObjLoader::load("res/models/raft/boxOpen.obj");
	vertices.insert(vertices.end(), mesh1->vertices.begin(), mesh1->vertices.end());
	vertices.insert(vertices.end(), mesh2->vertices.begin(), mesh2->vertices.end());

	std::vector<GLuint> indirectData = {
		// count, instanceCount, first, baseInstance
		static_cast<unsigned int>(mesh1->vertices.size()), 3, 0, 0,
		static_cast<unsigned int>(mesh2->vertices.size()), 2, (unsigned int)mesh1->vertices.size(), 3
	};

	// upload to gpu

	glBindVertexArray(m_vao);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_ibo);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, indirectData.size() * sizeof(GLuint), &indirectData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

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
	glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

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

	delete mesh1;
	delete mesh2;
}
