#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <Assets/Vertex.hpp>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <sgl/sgl_shader.hpp>
#include <Assets/Texture.hpp>

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture *> textures;
	
	// without indices
	Mesh(std::vector<Vertex> vertices, bool hasTexcoords = true);
	Mesh(std::vector<Vertex> vertices, std::vector<Texture *> textures,
		bool hasTexcoords = true);
	// with indices
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices,
		bool hasTexcoords = true);
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices,
		std::vector<Texture *> textures, bool hasTexcoords = true);
	
	Mesh(const Mesh &copy) = delete;
	~Mesh();
	
	void draw(sgl::shader &shader);
	
	void destroy();
	
private:
	GLuint VAO, VBO, EBO;
	bool hasTexcoords, hasIndices;
	
	void setupMesh();
		
};
