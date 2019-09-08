#pragma once

#include <stdio.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Grid2D.hpp>
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

#include <Assets/AssetManager.hpp>
#include <Assets/Texture.hpp>
#include <Assets/Mesh.hpp>
#include <Assets/Model.hpp>

class World {
public:
	
	World();
	World(const World &copy) = delete;
	
	~World();
	
	// entitites
	entt::entity getNearestEntity(glm::vec3 pos);
	entt::entity spawnDefaultEntity(glm::vec3 pos);
	
	// gameloop
	void update(glm::vec3 viewPos, glm::vec3 viewDir);
	void draw(glm::vec3 &camPos, glm::mat4 &uView, glm::mat4 &uProjection);
	
	// getters
	inline entt::registry &getRegistry() { return registry; }
	inline AssetManager &getAssetManager() { return assetManager; }
	
	inline entt::entity getPlayer() const {
		if (registry.valid(player))
			return player;
		return entt::null;
	}
	inline entt::entity getCrosshair() const {
		if (registry.valid(worldCrosshair))
			return worldCrosshair;
		return entt::null;
	}
	inline float getGravity() const { return gravitySystem.getGravity(); };
	
private:
	entt::registry registry;
	
	// grid
	Grid2D<Model> tileGrid;
	sgl::shader tileGridShader;
	
	// entities
	entt::entity player = entt::null,
		worldCrosshair = entt::null;
	
	entt::entity spawnPlayer(glm::vec3 pos = glm::vec3(0.0f, 0.2f, 0.0f));
	
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
	
	AssetManager assetManager;
	
};
