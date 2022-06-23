#include "ecs/systems/LightVolumeRenderSystem.hpp"
#include "Engine/Engine.hpp"

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
	updateBuffer();

	//cregistry.on_construct<CPointLight>().connect<&LightVolumeRenderSystem::onPointLightConstructed>(*this);
}

LightVolumeRenderSystem::~LightVolumeRenderSystem() {
	//cregistry.on_construct<CPointLight>().disconnect<&LightVolumeRenderSystem::onPointLightConstructed>(*this);

}

void LightVolumeRenderSystem::draw() {
	const GLsizei count = m_aInstancePositionsWithRadiuses.size();

	m_shader.use();
	m_shader["uProjection"] = camera->getProjection();
	m_shader["uView"] = camera->getView();
	
	glBindVertexArray(m_vao);

	GLState glState;
	glState.depth_test = true;
	glState.depth_write = false;
	glState.cull_face = true;
	glState.blend = true;
	glState.blend_src = GL_SRC_ALPHA;
	glState.blend_dst = GL_ONE;
	glState.blend_src_alpha = GL_SRC_ALPHA;
	glState.blend_dst_alpha = GL_ONE;

	Engine::Instance->getGraphics().setState(glState);
	glDrawElementsInstanced(GL_TRIANGLES, m_triangleCount * 3, GL_UNSIGNED_SHORT, 0, count);

}

/////////////
// PRIVATE //
/////////////

void LightVolumeRenderSystem::setupBuffer() {
	// setup sphere
	m_sphereMesh = par_shapes_create_subdivided_sphere(2);

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_ibo);
	glGenBuffers(1, &m_ebo);

	m_aInstancePositionsWithRadiuses.clear();
	m_aInstanceColors.clear();
	cregistry.view<CPosition, CPointLight>().each([this](entt::entity entity, auto &position, auto &pointlight) {
		m_aInstancePositionsWithRadiuses.push_back(glm::vec4(position.pos, pointlight.radius));
		m_aInstanceColors.push_back(pointlight.color);
	});

	m_triangleCount = m_sphereMesh->ntriangles;
	m_meshVerticesSize = m_sphereMesh->npoints * sizeof(glm::vec3);
	const size_t instancePositionAndRadiusSize = m_aInstancePositionsWithRadiuses.size() * sizeof(glm::vec4);
	const size_t instanceColorSize = m_aInstanceColors.size() * sizeof(glm::vec3);
	

	glBindVertexArray(m_vao);

	// ibo
	glBindBuffer(GL_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ARRAY_BUFFER, m_meshVerticesSize + instancePositionAndRadiusSize + instanceColorSize, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_meshVerticesSize, m_sphereMesh->points);
	glBufferSubData(GL_ARRAY_BUFFER, m_meshVerticesSize, instancePositionAndRadiusSize, m_aInstancePositionsWithRadiuses.data());
	glBufferSubData(GL_ARRAY_BUFFER, m_meshVerticesSize + instancePositionAndRadiusSize, instanceColorSize, m_aInstanceColors.data());
	
	// ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_sphereMesh->ntriangles * 3 * sizeof(PAR_SHAPES_T), m_sphereMesh->triangles, GL_STATIC_DRAW);

	// vao
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)(m_meshVerticesSize));

	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(m_meshVerticesSize + instancePositionAndRadiusSize));

	glBindVertexArray(0);
}

void LightVolumeRenderSystem::destroyBuffer() {
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_ibo);
	glDeleteBuffers(1, &m_ebo);

	par_shapes_free_mesh(m_sphereMesh);
}

void LightVolumeRenderSystem::updateBuffer() {
	
}

void LightVolumeRenderSystem::onPointLightConstructed(const entt::registry &registry, entt::entity entity) {
	// TODO: just add the newest element to the buffer, instead of completely updating all data
	setupBuffer();
}