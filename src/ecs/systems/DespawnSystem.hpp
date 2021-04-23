#pragma once

#include <entt/entt.hpp>
#include <ecs/systems/IUpdateSystem.hpp>
#include <ecs/components.hpp>
#include <ecs/events.hpp>

class DespawnSystem : public IUpdateSystem {
public:

	DespawnSystem(entt::registry &registry);
	~DespawnSystem();

	void update(float dt) override;

	void entityKilled(const KillEntityEvent &e);

private:
	float voidHeight = -5.0f;

};
