#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>

struct CRunningToTarget {
	/* prefer entity over pos */
	entt::entity entity = entt::null;
	glm::vec3 pos;
	
	float force;
	float closeEnough;
	
	CRunningToTarget(glm::vec3 targetPos, float force, float close = 0.1f) 
		: pos(targetPos), force(force), closeEnough(close)
	{
	}
	
	
	CRunningToTarget(entt::entity targetEntity, float force, float close = 0.065f) 
		: entity(targetEntity), force(force), closeEnough(close)
	{
	}
	
	glm::vec3 getTargetPosition(const entt::registry &registry) {
		if (registry.valid(entity) && registry.has<CPosition>(entity))
			return registry.get<CPosition>(entity).pos;
		
		return pos;
	}
};
