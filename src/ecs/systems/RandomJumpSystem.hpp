#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <entt/entt.hpp>
#include <ecs/components.hpp>
#include <ecs/systems/IUpdateSystem.hpp>
#include <Util/Random.hpp>

class RandomJumpSystem : public IUpdateSystem {
public:
	
	RandomJumpSystem(entt::registry &registry, float chanceToJump = 0.01f)
		: IUpdateSystem(registry), chanceToJump(chanceToJump), chanceToFlip(chanceToJump)
	{
	}
	
	void update(float dt);
	
private:
	float chanceToJump, chanceToFlip;
	
};
