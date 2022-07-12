#pragma once

#include <GL/gl3w.h>
#include <vector>
#include <fmt/core.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <sgl/sgl_shader.hpp>

class Chunk {
private:
	std::vector<glm::vec3> m_points;
	sgl::shader m_shader;
	GLuint m_vao, m_vbo;

public:
	Chunk();
	~Chunk();

	float get(glm::vec3 p) const {
		if (p.y > 0.5f)
			return -1.0f;

		return 1.0f;
	}

	void polygonize(glm::ivec3 chunk);
	void draw(const glm::mat4& projection, const glm::mat4& view);
};
