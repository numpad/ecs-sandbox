#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <entt/entt.hpp>
#include <ecs/components.hpp>
#include <ecs/systems/BaseUpdateSystem.hpp>
#include <Util/Random.hpp>

class RandomJumpSystem : public BaseUpdateSystem {
public:
	
	RandomJumpSystem(entt::registry &registry, float chanceToJump = 0.01f)
		: BaseUpdateSystem(registry), chanceToJump(chanceToJump), chanceToFlip(chanceToJump)
	{
	}
	
	void update();
	
private:
	float chanceToJump, chanceToFlip;
	
};
