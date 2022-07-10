#include "UI/ImageWidget.hpp"
#include <GL/gl3w.h>
#include <glm/gtc/matrix_transform.hpp>

ImageWidget::ImageWidget(GLuint texture) : m_texture{texture} {
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	GLfloat vertices[] = {
	    0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	};

	GLuint indices[] = {0, 1, 2, 2, 1, 3};

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_shader.load("res/glsl/ui/image_vert.glsl", sgl::shader::VERTEX);
	m_shader.load("res/glsl/ui/image_frag.glsl", sgl::shader::FRAGMENT);
	m_shader.compile();
	m_shader.link();
}

ImageWidget::~ImageWidget() {
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);
}

void ImageWidget::draw(const glm::mat3& transform) const {
	m_shader.use();

	float width = 800, height = 600;
	m_shader["uProjection"] = glm::ortho(0.0f, width, height, 0.0f);
	m_shader["uView"] = glm::mat4(transform);
	m_shader["uModel"] = glm::mat4(1.0f);
	m_shader["uTexture"] = 0;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
