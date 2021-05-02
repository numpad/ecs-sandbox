#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>

struct CRunningToTarget {
	/* prefer entity over pos */
	entt::entity entity = entt::null;
	glm::vec3 pos;
	
	float force;
	float closeEnough = 1.0f;
	
	bool stopOnceReached = true;
	
	CRunningToTarget(glm::vec3 targetPos, float force, float close = 0.1f, bool stopOnceReached = true) 
		: pos(targetPos), force(force), closeEnough(close), stopOnceReached(stopOnceReached)
	{
	}
	
	
	CRunningToTarget(entt::entity targetEntity, float force, float close = 0.1f, bool stopOnceReached = true) 
		: entity(targetEntity), force(force), closeEnough(close), stopOnceReached(stopOnceReached)
	{
	}
	
	glm::vec3 getTargetPosition(const entt::registry &registry) {
		if (registry.valid(entity) && registry.try_get<CPosition>(entity))
			return registry.get<CPosition>(entity).pos;
		
		return pos;
	}
};
