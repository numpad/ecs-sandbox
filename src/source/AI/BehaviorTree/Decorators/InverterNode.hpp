#pragma once
#include <assert.h>

#include "AI/BehaviorTree/INode.hpp"

class InverterNode : public INode {
protected:
	virtual State tick(entt::registry& registry, const entt::entity& entity) {
		assert(m_children.size() == 1);

		const State childState = m_children[0]->tick(registry, entity);

		switch (childState) {
		case State::SUCCESS:
			return State::FAILURE;
		case State::FAILURE:
			return State::SUCCESS;
		default:
			return State::RUNNING;
		};
	}
};
