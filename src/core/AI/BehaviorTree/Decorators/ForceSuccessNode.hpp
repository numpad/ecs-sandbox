#pragma once
#include <assert.h>

#include "AI/BehaviorTree/INode.hpp"

class ForceSuccessNode : public INode {
protected:
	virtual State tick(entt::registry& registry, const entt::entity& entity) {
		assert(m_children.size() == 1);

		const State state = m_children[0]->onTick(registry, entity);
		if (state == State::RUNNING) {
			return State::RUNNING;
		}

		return State::SUCCESS;
	}
};
