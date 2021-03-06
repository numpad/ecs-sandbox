#pragma once

#include <cstdio>
#include <random>
#include <array>
#include <vector>
#include <memory>
#include <iostream>
#include <functional>
#include <limits>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <entt/entt.hpp>
#include <sgl/sgl_shader.hpp>

#include <Grid2D.hpp>

#include <ecs/components.hpp>
#include <ecs/systems.hpp>
#include <ecs/events.hpp>

#include <Util/Random.hpp>
#include <Util/Math3d.hpp>
#include <Util/Blackboard.hpp>
#include <Util/Benchmark.hpp>

#include <Assets/AssetManager.hpp>
#include <Assets/Texture.hpp>
#include <Assets/Mesh.hpp>
#include <Assets/Model.hpp>

#include <Terrain/SignedDistTerrain.hpp>

#include <RenderObject/ChunkedWorld.hpp>
#include <RenderObject/Billboard.hpp>
#include <RenderObject/Camera.hpp>

using namespace glm;

class World {
public:
	
	World(GLFWwindow *window, std::shared_ptr<Camera> camera);
	World(const World &copy) = delete;
	
	~World();
	void destroy();
	
	// entitites
	entt::entity getNearestEntity(vec3 pos);
	entt::entity spawnDefaultEntity(vec3 pos);
	void resetEntities();
	
	// generate
	void load();
	
	// gameloop
	void update();
	void draw();
	
	// setters
	void setCamera(std::shared_ptr<Camera> camera);
	
	// getters
	inline entt::registry &getRegistry() { return registry; }
	inline entt::dispatcher &getDispatcher() { return registry.ctx<entt::dispatcher>(); }
	inline AssetManager &getAssetManager() { return assetManager; }
	inline const std::shared_ptr<Camera> getCamera() const { return camera; }
	inline bool is_loaded()  const { return loaded; }
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
	std::shared_ptr<Camera> camera;
	GLFWwindow *m_window;

	// loading
	bool loaded = false; // is the terrain etc. loaded?
	
	// grid
	Grid2D<SignedDistTerrain> tileGrid;
	std::shared_ptr<ChunkedWorld> chunkedWorld;
		
	// entities
	entt::entity player = entt::null,
		worldCrosshair = entt::null;
	
	entt::entity spawnPlayer(vec3 pos = vec3(0.0f, 0.2f, 0.0f));
	
	// systems
	std::vector<std::shared_ptr<BaseUpdateSystem>> updateSystems;
	std::vector<std::shared_ptr<BaseRenderSystem>> renderSystems;
	std::unique_ptr<LogSystem> logEventSystem;
	
	void loadSystems();
	
	// shader & models
	sgl::shader floorShader;
	void setupFloor();
	void destroyFloor();
	
	AssetManager assetManager;
	
};
