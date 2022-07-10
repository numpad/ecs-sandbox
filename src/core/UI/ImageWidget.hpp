#pragma once

#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "sgl/sgl_shader.hpp"
#include "UI/IWidget.hpp"

class ImageWidget : public IWidget {
public:
	ImageWidget(GLuint texture);
	~ImageWidget();

	void draw(const glm::mat3& view, const glm::mat3& model) const;

private:
	GLuint m_texture;
	GLuint m_vao, m_vbo, m_ebo;
	sgl::shader m_shader;
};
