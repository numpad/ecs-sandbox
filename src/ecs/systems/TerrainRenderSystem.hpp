#pragma once

#include <memory>

#include <entt/entt.hpp>

#include <sgl/sgl_shader.hpp>
#include <Assets/AssetManager.hpp>
#include <Util/Blackboard.hpp>
#include <RenderObject/ChunkedWorld.hpp>
#include <RenderObject/Camera.hpp>
#include <ecs/systems/IRenderSystem.hpp>

class TerrainRenderSystem : public IRenderSystem {
public:

	TerrainRenderSystem(const entt::registry &registry, std::shared_ptr<Camera> camera, AssetManager &assetManager, std::shared_ptr<ChunkedWorld> chunkedWorld);
	~TerrainRenderSystem();

	void draw();

private:
	sgl::shader m_chunkShader;
	AssetManager &m_assetManager;
	std::shared_ptr<ChunkedWorld> m_chunkedWorld;

	void loadShader();

	void updateUniforms();
	void bindTextures();

};
