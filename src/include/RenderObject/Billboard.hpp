#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <GL/gl3w.h>
#include <util/sgl_shader.hpp>
#include <Util/Texture.hpp>

class Billboard {
public:
	
	Billboard(std::string spritepath);
	Billboard(const Billboard &copy) = delete;
	
	~Billboard();
	
	void lookAtCamera();
	
	void draw(glm::mat4 &uView, glm::mat4 &uProjection, glm::vec3 pos,
		glm::vec2 size, glm::vec3 *camtarget = nullptr);

private:
	Texture spriteTexture;
	
	sgl::shader bbShader;
	GLuint bbVAO, bbVBO, bbEBO;
	void setupBillboard();
	void destroyBillboard();
};
