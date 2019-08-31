#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <Assets/Vertex.hpp>
#include <GL/gl3w.h>
#include <util/sgl_shader.hpp>
#include <Assets/Texture.hpp>

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture *> textures;
	
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices,
		std::vector<Texture *> textures);
	
	Mesh(const Mesh &copy) = delete;
	~Mesh();
	
	void draw(sgl::shader &shader);
	
	void destroy();
	
private:
	GLuint VAO, VBO, EBO;
	
	void setupMesh();
		
};
