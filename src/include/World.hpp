#pragma once

#include <stdio.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Grid3D.hpp>
#include <util/sgl_shader.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <entt/entt.hpp>

#include <components/CPosition.hpp>

#include <RenderObject/Billboard.hpp>

class World {
	
public:
	World();
	World(const World &copy) = delete;
	
	~World();
	
	
	void update();
	void draw(glm::mat4 &uView, glm::mat4 &uProjection);
	
private:
	entt::registry registry;
	
	Billboard billboard;
	
	sgl::shader floorShader;
	GLuint floorVAO, floorVBO, floorEBO;
	void setupFloor();
	void destroyFloor();
	
};
