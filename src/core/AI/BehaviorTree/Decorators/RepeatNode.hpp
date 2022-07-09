#pragma once
#include <assert.h>

#include "AI/BehaviorTree/INode.hpp"

class RepeatNode : public INode {
public:
	RepeatNode(int attempts = -1) : m_baseAttempts{attempts}, m_attempts{attempts} {
	}

protected:
	virtual State tick(entt::registry& registry, const entt::entity& entity) {
		assert(m_children.size() == 1);

		const State state = m_children[0]->onTick(registry, entity);
		switch (state) {
		case State::SUCCESS:
			if (isRepeatLimitReached()) {
				m_attempts = m_baseAttempts;
				return State::SUCCESS;
			}

			if (m_attempts != -1) {
				m_attempts--;
			}
			return State::RUNNING;
		case State::FAILURE:
			m_attempts = m_baseAttempts;
			return State::FAILURE;
		case State::RUNNING:
			return State::RUNNING;
		};

		return State::RUNNING;
	}

private:
	const int m_baseAttempts;
	int m_attempts;

	bool isRepeatLimitReached() const {
		return m_attempts != -1 && m_attempts <= 1;
	}
};
