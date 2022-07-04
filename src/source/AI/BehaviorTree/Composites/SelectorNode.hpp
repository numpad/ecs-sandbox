#pragma once

#include "AI/BehaviorTree/INode.hpp"

class SelectorNode : public INode {
protected:
	virtual State tick(entt::registry& registry, const entt::entity& entity) {
		// iterate all children sequentially
		for (INode* child : m_children) {
			const auto state = child->tick(registry, entity);
			// if a child succeeds, continue with the next one
			if (state == State::FAILURE) {
				continue;
			}

			return state;
		}

		return State::FAILURE;
	}
};
