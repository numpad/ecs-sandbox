#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <entt/entt.hpp>
#include <ecs/components.hpp>
#include <Util/Random.hpp>

class RandomJumpSystem {
public:
	
	RandomJumpSystem(float chanceToJump = 0.01f)
		: chanceToJump(chanceToJump), chanceToFlip(chanceToJump)
	{
	}
	
	void update(entt::registry &registry);
private:
	float chanceToJump, chanceToFlip;
	
};
