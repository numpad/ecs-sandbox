#pragma once

#include "Prefab/IPrefab.hpp"

namespace Prefab {

struct Enemy : public IPrefab {
	entt::entity Instantiate(entt::registry& registry, const entt::entity& entity) const {

		return entity;
	}
};

} // namespace Prefab
