#pragma once

#include <Assets/Model.hpp>
#include <entt/entt.hpp>
#include <ecs/components.hpp>
#include <Grid2D.hpp>

class GravitySystem {
public:
	
	GravitySystem(float gravity);
	
	void update(entt::registry &registry, const Grid2D<Model> &tileGrid);
	
	inline float getGravity() const { return gravity; }
	
private:
	float gravity;
	
};
