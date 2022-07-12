#pragma once
#include <assert.h>

#include "AI/BehaviorTree/INode.hpp"

class RetryNode : public INode {
public:
	RetryNode(int attempts = -1) : m_baseAttempts{attempts}, m_attempts{attempts} {
	}

protected:
	virtual State tick(entt::registry& registry, const entt::entity& entity) {
		assert(m_children.size() == 1);

		const State state = m_children[0]->onTick(registry, entity);
		switch (state) {
		case State::FAILURE:
			if (isRetryLimitReached()) {
				m_attempts = m_baseAttempts;
				return State::FAILURE;
			}

			if (m_attempts != -1) {
				m_attempts--;
			}
			return State::RUNNING;
		case State::SUCCESS:
			m_attempts = m_baseAttempts;
			return State::SUCCESS;
		case State::RUNNING:
			return State::RUNNING;
		};

		return State::RUNNING;
	}

private:
	const int m_baseAttempts;
	int m_attempts;

	bool isRetryLimitReached() const {
		return m_attempts != -1 && m_attempts <= 1;
	}
};
