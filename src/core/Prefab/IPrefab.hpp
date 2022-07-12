#pragma once

#include <entt/entt.hpp>

struct IPrefab {
	virtual entt::entity Instantiate(entt::registry &registry, const entt::entity& entity) const = 0;
};

