#pragma once

#include "Engine/IScene.hpp"
#include "ecs/events/MouseButtonEvent.hpp"
#include "ecs/systems/IRenderSystem.hpp"
#include "ecs/systems/IUpdateSystem.hpp"
#include "scenes/gamemode/raft/IslandTerrain.hpp"
#include "scenes/gamemode/raft/RaftSet.hpp"
#include "scenes/gamemode/raft/OceanPlane.hpp"
#include <glm/glm.hpp>

class MiniRaftScene : public IScene {
public:
	bool onCreate() override;
	void onDestroy() override;
	void onUpdate(float dt) override;
	void onRender() override;

private:
	AssetManager m_assetmanager;
	entt::registry m_registry;
	std::shared_ptr<Camera> m_camera;

	std::vector<IUpdateSystem*> m_updatesystems;
	std::vector<IRenderSystem*> m_rendersystems;

	RaftSet m_raft;
	OceanPlane m_waterplane;
	IslandTerrain m_island = glm::ivec3(32);

	entt::entity m_spawnerBox = entt::null;

	void onMouseButtonInput(const MouseButtonEvent& event);
};
