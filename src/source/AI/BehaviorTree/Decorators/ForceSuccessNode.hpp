#pragma once
#include <assert.h>

#include "AI/BehaviorTree/INode.hpp"

class ForceSuccessNode : public INode {
protected:
	virtual State tick(entt::registry& registry, const entt::entity& entity) {
		assert(m_children.size() == 1);

		const State childState = m_children[0]->tick(registry, entity);

		if (childState == State::FAILURE) {
			return State::SUCCESS;
		}

		return childState;
	}
};
