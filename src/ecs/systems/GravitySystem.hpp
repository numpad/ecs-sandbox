#pragma once

#include <Terrain/SignedDistTerrain.hpp>
#include <entt/entt.hpp>
#include <ecs/components.hpp>
#include <ecs/events.hpp>
#include <ecs/systems/BaseUpdateSystem.hpp>
#include <Grid2D.hpp>

class GravitySystem : public BaseUpdateSystem {
public:
	
	GravitySystem(entt::registry &registry, float gravity, Grid2D<SignedDistTerrain> &tileGrid);
	
	void update();
	
	// TODO: put event listener in other system
	void entityKilled(const KillEntityEvent &e);
	
	inline float getGravity() const { return gravity; }
	
private:
	float gravity;
	// below which height are entities killed
	float voidHeight = -5.f;
	
	Grid2D<SignedDistTerrain> &tileGrid;
	
};
