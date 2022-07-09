#pragma once

#include "AI/BehaviorTree/INode.hpp"
#include "ecs/components/CPosition.hpp"

class IsNearEntity : public INode {
public:
	IsNearEntity(entt::entity target, float distance) : m_target{target}, m_maxDistance{distance} {
	}

protected:
	State tick(entt::registry& registry, const entt::entity& entity) {
		CPosition* pos = registry.try_get<CPosition>(entity);
		CPosition* targetPos = registry.try_get<CPosition>(m_target);

		if (pos && targetPos && glm::distance(pos->pos, targetPos->pos) <= m_maxDistance) {
			return State::SUCCESS;
		}

		return State::FAILURE;
	}

private:
	entt::entity m_target;
	float m_maxDistance;
};
