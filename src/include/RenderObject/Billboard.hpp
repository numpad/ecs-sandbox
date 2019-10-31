#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <GL/gl3w.h>
#include <util/sgl_shader.hpp>
#include <Assets/Texture.hpp>

class Billboard {
public:
	
	Billboard();
	Billboard(const Billboard &copy) = delete;
	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
	~Billboard();
	
	static glm::mat4 calcModelMatrix(glm::mat4 &uView, glm::vec3 pos, glm::vec2 size);

	inline GLuint getVAO() { return bbVAO; }
	inline GLuint getVBO() { return bbVBO; }
	inline GLuint getEBO() { return bbEBO; }
	
private:
		
	void draw(Texture *texture, glm::mat4 &uView, glm::mat4 &uProjection,
		glm::vec3 pos, glm::vec2 size, glm::vec3 color);
	
	sgl::shader bbShader;
	GLuint bbVAO, bbVBO, bbEBO;
	void setupBillboard();
	void destroyBillboard();
};
