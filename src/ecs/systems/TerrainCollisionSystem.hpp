#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <ecs/systems/IUpdateSystem.hpp>
#include <ecs/components.hpp>

#include <RenderObject/ChunkedWorld.hpp>

class TerrainCollisionSystem : public IUpdateSystem {
public:
	TerrainCollisionSystem(entt::registry& registry, std::shared_ptr<ChunkedWorld> chunkedWorld);
	~TerrainCollisionSystem();

	void update(float dt) override;

private:
	std::shared_ptr<ChunkedWorld> m_chunkedWorld;
};
