#pragma once

#include <stdio.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Grid3D.hpp>
#include <util/sgl_shader.hpp>

class World {
	
public:
	World();
	~World();
	
	void draw();
	
private:
	unsigned int width, height, depth;
	Grid3D<int> grid;
	
	sgl::shader floorShader;
	GLuint floorVAO, floorVBO, floorEBO;
	void setupFloor();
	void destroyFloor();
	
};
