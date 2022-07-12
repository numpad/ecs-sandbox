#pragma once

#include <Terrain/SignedDistTerrain.hpp>
#include <entt/entt.hpp>
#include <ecs/components.hpp>
#include <ecs/events.hpp>
#include <ecs/systems/IUpdateSystem.hpp>
#include <Grid2D.hpp>

class GravitySystem : public IUpdateSystem {
public:
	GravitySystem(entt::registry& registry, float gravity);

	void update(float dt);

	inline float getGravity() const {
		return gravity;
	}

private:
	float gravity;
};
