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
#include <Util/Math3d.hpp>
#include <RenderObject/Billboard.hpp>

#include <Util/Texture.hpp>

class World {
public:
	
	World();
	World(const World &copy) = delete;
	
	~World();
	
	entt::entity getNearestEntity(glm::vec3 pos);
	
	entt::entity spawnDefaultEntity(glm::vec3 pos);
	
	void update(glm::vec3 viewPos, glm::vec3 viewDir);
	void draw(glm::vec3 &camPos, glm::mat4 &uView, glm::mat4 &uProjection);
	
	inline entt::registry &getRegistry() { return registry; };
	
private:
	entt::registry registry;
	
	// entities
	entt::entity player = entt::null;
	entt::entity spawnEntity(entt::registry &registry, glm::vec3 pos);
	
	// systems
	GravitySystem gravitySystem = GravitySystem(0.000981f);
	RandomJumpSystem popcorn = RandomJumpSystem(0.003f);
	WayfindSystem wayfindSystem;
	CharacterController charControllerSystem;
	PressAwaySystem pressawaySystem;
	PositionUpdateSystem posUpdate;
	//BillboardLookAtCameraSystem billboardOrientation;
	BillboardRenderSystem billboardSystem;
	
	sgl::shader floorShader;
	GLuint floorVAO, floorVBO, floorEBO;
	void setupFloor();
	void destroyFloor();
	void drawFloor(glm::mat4 &uView, glm::mat4 &uProjection);
	
};
