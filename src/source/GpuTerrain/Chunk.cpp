#include "GpuTerrain/Chunk.hpp"

Chunk::Chunk() {
	m_shader.load("res/glsl/primitives/points_vert.glsl", sgl::shader::VERTEX);
	m_shader.load("res/glsl/primitives/points_geom.glsl", sgl::shader::GEOMETRY);
	m_shader.load("res/glsl/primitives/points_frag.glsl", sgl::shader::FRAGMENT);
	m_shader.compile();
	m_shader.link();

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), NULL);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Chunk::~Chunk() {
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}

void Chunk::polygonize(glm::ivec3 chunk) {
	float size = 1.0f;
	float step = 0.1f;

	m_points.clear();
	for (float x = 0; x <= size; x += step) {
		for (float y = 0; y <= size; y += step) {
			for (float z = 0; z <= size; z += step) {
				m_points.push_back(glm::vec3(x, y, z));
			}
		}
	}


	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_points.size() * sizeof(glm::vec3), m_points.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Chunk::draw(const glm::mat4 &projection, const glm::mat4 &view) {
	m_shader["uProjection"] = projection;
	m_shader["uView"] = view;
	m_shader["uTime"] = (float)glfwGetTime();

	m_shader.use();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glDisable(GL_CULL_FACE);
	glDrawArrays(GL_POINTS, 0, m_points.size());
	glEnable(GL_CULL_FACE);
}
