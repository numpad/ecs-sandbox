#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <ecs/components.hpp>
#include <ecs/systems/IUpdateSystem.hpp>

class PositionUpdateSystem : public IUpdateSystem {
public:
	PositionUpdateSystem(entt::registry& registry);

	void update(float dt);

private:
};
