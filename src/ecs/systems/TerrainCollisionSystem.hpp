#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <ecs/systems/BaseUpdateSystem.hpp>
#include <ecs/components.hpp>

#include <RenderObject/ChunkedWorld.hpp>

class TerrainCollisionSystem : public BaseUpdateSystem {
public:

	TerrainCollisionSystem(entt::registry &registry, std::shared_ptr<ChunkedWorld> chunkedWorld);
	~TerrainCollisionSystem();

	void update() override;

private:
	std::shared_ptr<ChunkedWorld> m_chunkedWorld;
};
