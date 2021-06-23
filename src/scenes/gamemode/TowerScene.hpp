#pragma once

#include <vector>
#include <algorithm>
#include <memory>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include <Engine/IScene.hpp>
#include <ecs/systems.hpp>
#include <scenes/ui/MainMenuScene.hpp>
#include <RenderObject/Camera.hpp>
#include <Assets/AssetManager.hpp>
#include <Debug/ImguiPresets.hpp>
#include <ImprovedTerrain/DynamicTerrain.hpp>
#include <ImprovedTerrain/ISignedDistanceBody.hpp>
#include <ImprovedTerrain/Bodies/SphereBody.hpp>
#include <Util/Random.hpp>

class TowerScene : public IScene {
public:

	bool onCreate() override;
	void onDestroy() override;
	void onUpdate(float dt) override;
	void onRender() override;

	entt::registry m_registry;
	AssetManager m_assetmanager;
	DynamicTerrain m_terrain;

private:
	std::shared_ptr<Camera> m_camera;
	sgl::shader m_chunkshader;

	std::vector<entt::entity> m_players;

	// systems
	std::vector<std::shared_ptr<IUpdateSystem>> m_updatesystems;
	std::vector<std::shared_ptr<IRenderSystem>> m_rendersystems;
	std::unique_ptr<LogSystem> m_logsystem;
	void loadSystems();

	void loadTerrainShader();
	void updateTerrainShader();

	// event listener
	void onEntityKilled(const KillEntityEvent &event);
	void onBombExplodes(const ExplosionEvent &event);
	void onMouseButtonInput(const MouseButtonEvent &event);
};
