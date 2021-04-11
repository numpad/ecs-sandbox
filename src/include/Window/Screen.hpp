#pragma once

#include <GL/gl3w.h>

class Screen {
public:

	void initialize();
	void destroy();

	void bind();
	void draw();

private:
	GLuint m_vao, m_vbo;

	constexpr static GLfloat VERTICES[] = {-1.f,-1.f,1.f,-1.f,-1.f,1.f,-1.f,1.f,1.f,-1.f,1.f,1.f,0.f,0.f,1.f,0.f,0.f,1.f,0.f,1.f,1.f,0.f,1.f,1.f};
};
