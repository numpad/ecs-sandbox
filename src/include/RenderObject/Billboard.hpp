#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <GL/gl3w.h>
#include <util/sgl_shader.hpp>

class Billboard {
public:
	
	Billboard(glm::vec2 size);
	Billboard(const Billboard &copy) = delete;
	
	~Billboard();
	
	void draw(glm::mat4 &uView, glm::mat4 &uProjection, glm::vec3 pos);

private:
	glm::vec2 size;
	
	sgl::shader bbShader;
	GLuint bbVAO, bbVBO, bbEBO;
	void setupBillboard();
	void destroyBillboard();
};
