#include "ecs/systems/LightVolumeRenderSystem.hpp"

////////////
// PUBLIC //
////////////

LightVolumeRenderSystem::LightVolumeRenderSystem(const entt::registry &registry, std::shared_ptr<Camera> camera)
	: IRenderSystem(registry, camera)
{
	m_shader.load("res/glsl/proto/lightvolume_instance_vert.glsl", sgl::shader::VERTEX);
	m_shader.load("res/glsl/proto/lightvolume_instance_frag.glsl", sgl::shader::FRAGMENT);
	m_shader.compile();
	m_shader.link();

	setupBuffer();
}

LightVolumeRenderSystem::~LightVolumeRenderSystem() {

}

void LightVolumeRenderSystem::draw() {
	const GLsizei count = m_aInstancePositionsWithRadiuses.size();

	m_shader.use();
	m_shader["uProjection"] = camera->getProjection();
	m_shader["uView"] = camera->getView();
	glBindVertexArray(m_vao);
	glDrawElementsInstanced(GL_TRIANGLES, m_triangleCount * 3, GL_UNSIGNED_SHORT, 0, 2);
}

/////////////
// PRIVATE //
/////////////

void LightVolumeRenderSystem::setupBuffer() {
	m_aInstancePositionsWithRadiuses = {glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(2.0f, 0.0f, 2.0f, 1.5f)};
	m_aInstanceColors = {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)};

	// setup sphere
	par_shapes_mesh *sphereMesh = par_shapes_create_subdivided_sphere(2);

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_ibo);
	glGenBuffers(1, &m_ebo);

	m_triangleCount = sphereMesh->ntriangles;
	size_t meshVerticesSize = sphereMesh->npoints * sizeof(glm::vec3);
	size_t instancePositionAndRadiusSize = m_aInstancePositionsWithRadiuses.size() * sizeof(glm::vec4);
	size_t instanceColorSize = m_aInstanceColors.size() * sizeof(glm::vec3);
	
	glBindVertexArray(m_vao);

	// ibo
	glBindBuffer(GL_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ARRAY_BUFFER, meshVerticesSize + instancePositionAndRadiusSize + instanceColorSize, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, meshVerticesSize, sphereMesh->points);
	glBufferSubData(GL_ARRAY_BUFFER, meshVerticesSize, instancePositionAndRadiusSize, m_aInstancePositionsWithRadiuses.data());
	glBufferSubData(GL_ARRAY_BUFFER, meshVerticesSize + instancePositionAndRadiusSize, instanceColorSize, m_aInstanceColors.data());
	
	// ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereMesh->ntriangles * 3 * sizeof(PAR_SHAPES_T), sphereMesh->triangles, GL_STATIC_DRAW);

	// vao
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)(meshVerticesSize));
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(meshVerticesSize + instancePositionAndRadiusSize));

	glBindVertexArray(0);

	// cleanup
	par_shapes_free_mesh(sphereMesh);
}

void LightVolumeRenderSystem::destroyBuffer() {
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_ibo);
	glDeleteBuffers(1, &m_ebo);

}
