#pragma once

#include <Assets/Model.hpp>
#include <entt/entt.hpp>
#include <ecs/components.hpp>
#include <ecs/systems/BaseUpdateSystem.hpp>
#include <Grid2D.hpp>

class GravitySystem : public BaseUpdateSystem {
public:
	
	GravitySystem(float gravity, Grid2D<Model> &tileGrid);
	
	void update(entt::registry &registry);
	
	inline float getGravity() const { return gravity; }
	
private:
	float gravity;
	Grid2D<Model> &tileGrid;
	
};
