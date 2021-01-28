#pragma once

#include <Terrain/SignedDistTerrain.hpp>
#include <entt/entt.hpp>
#include <ecs/components.hpp>
#include <ecs/events.hpp>
#include <ecs/systems/BaseUpdateSystem.hpp>
#include <Grid2D.hpp>

class GravitySystem : public BaseUpdateSystem {
public:
	
	GravitySystem(entt::registry &registry, float gravity);
	
	void update();
	
	inline float getGravity() const { return gravity; }
	
private:
	float gravity;
		
};
