#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <ecs/components.hpp>

struct CSpawnPoint {
	glm::vec3 pos;
	entt::entity entity = entt::null;
	
	CSpawnPoint(glm::vec3 pos) : pos(pos)
	{
	}
	
	CSpawnPoint(entt::entity entity) : entity(entity)
	{
	}
	
	glm::vec3 getPosition(entt::registry &registry) {
		if (registry.valid(entity) && registry.try_get<CPosition>(entity))
			return registry.get<CPosition>(entity).pos;
		
		return pos;
	}
	
};
