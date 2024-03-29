#pragma once

#include "AI/BehaviorTree/INode.hpp"

class SelectorNode : public INode {
protected:
	virtual State tick(entt::registry& registry, const entt::entity& entity) {
		const auto state = m_children.at(m_currentNode)->onTick(registry, entity);

		switch (state) {
		case State::SUCCESS:
			m_currentNode = 0;
			return State::SUCCESS;
		case State::FAILURE:
			if (m_currentNode == m_children.size() - 1) {
				m_currentNode = 0;
				return State::FAILURE;
			}

			m_currentNode++;
			return State::RUNNING;
		case State::RUNNING:
			return State::RUNNING;
		};
	}

private:
	size_t m_currentNode = 0;
};
