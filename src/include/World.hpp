#pragma once

#include <stdio.h>
#include <random>
#include <array>
#include <vector>
#include <memory>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
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
#include <ecs/events.hpp>

#include <Util/Random.hpp>
#include <Util/Math3d.hpp>
#include <RenderObject/Billboard.hpp>

#include <Assets/AssetManager.hpp>
#include <Assets/Texture.hpp>
#include <Assets/Mesh.hpp>
#include <Assets/Model.hpp>

#include <Terrain/SignedDistTerrain.hpp>
#include <RenderObject/ChunkedWorld.hpp>

using namespace glm;

class World {
public:
	
	World();
	World(const World &copy) = delete;
	
	~World();
	
	// entitites
	entt::entity getNearestEntity(vec3 pos);
	entt::entity spawnDefaultEntity(vec3 pos);
	
	// gameloop
	void update(vec3 viewPos, vec3 viewDir);
	void draw(vec3 &camPos, mat4 &uView, mat4 &uProjection);
	
	// getters
	inline entt::registry &getRegistry() { return registry; }
	inline entt::dispatcher &getDispatcher() { return registry.ctx<entt::dispatcher>(); }
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
	
private:
	entt::registry registry;
	
	// grid
	Grid2D<SignedDistTerrain> tileGrid;
	ChunkedWorld chunks;
	sgl::shader chunkShader;
		
	// entities
	entt::entity player = entt::null,
		worldCrosshair = entt::null;
	
	entt::entity spawnPlayer(vec3 pos = vec3(0.0f, 0.2f, 0.0f));
	
	// systems
	std::vector<std::unique_ptr<BaseUpdateSystem>> updateSystems;
	CharacterController charControllerSystem;
	std::unique_ptr<BillboardRenderSystem> billboardRenderSystem;
	
	void loadSystems();
	
	// shader & models
	sgl::shader floorShader;
	void setupFloor();
	void destroyFloor();
	void drawFloor(mat4 &uView, mat4 &uProjection);
	
	AssetManager assetManager;
	
};
