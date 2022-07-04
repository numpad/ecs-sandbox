#pragma once
#include <assert.h>

#include "AI/BehaviorTree/INode.hpp"

class RetryNode : public INode {
public:
	RetryNode(int attempts = -1) : m_attempts{attempts} {
	}

protected:
	virtual State tick(entt::registry& registry, const entt::entity& entity) {
		assert(m_children.size() == 1);

		const State childState = m_children[0]->tick(registry, entity);
		if (childState == State::FAILURE && !isRetryLimitReached()) {
			if (m_attempts != -1) {
				m_attempts--;
			}
			return State::RUNNING;
		}

		return childState;
	}

private:
	int m_attempts;

	bool isRetryLimitReached() const {
		return m_attempts != -1 && m_attempts <= 1;
	}
};
