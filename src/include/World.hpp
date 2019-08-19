#pragma once

#include <stdio.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Grid3D.hpp>
#include <util/sgl_shader.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <entt/entt.hpp>

#include <ecs/components.hpp>
#include <ecs/systems.hpp>

#include <Util/Random.hpp>
#include <RenderObject/Billboard.hpp>

#include <Util/Texture.hpp>

class World {
	
public:
	World();
	World(const World &copy) = delete;
	
	~World();
	
	entt::entity getNearestEntity(glm::vec3 pos);
	
	entt::entity spawnDefaultEntity(glm::vec3 pos);
	
	void update();
	void draw(glm::mat4 &uView, glm::mat4 &uProjection);
	
	inline entt::registry &getRegistry() { return registry; };
	
private:
	entt::registry registry;
	
	BillboardRenderSystem billboardSystem;
	GravitySystem gravitySystem = GravitySystem(0.000981f);
		
	sgl::shader floorShader;
	GLuint floorVAO, floorVBO, floorEBO;
	void setupFloor();
	void destroyFloor();
	void drawFloor(glm::mat4 &uView, glm::mat4 &uProjection);
	
};
