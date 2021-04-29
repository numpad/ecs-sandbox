#pragma once

#include <entt/entt.hpp>

#include <ecs/systems/IUpdateSystem.hpp>
#include <ecs/components.hpp>

class EntityDeleteSystem : public IUpdateSystem {
public:
	
	EntityDeleteSystem(entt::registry &registry);
	~EntityDeleteSystem();

	void update(float dt) override;

private:
	
};
