#pragma once

#include <vector>
#include <entt/entt.hpp>

class INode {
public:
	enum class State {
		SUCCESS,
		FAILURE,
		RUNNING
	};

	void addChild(INode* child) {
		m_children.push_back(child);
	}

	State onTick(entt::registry& registry, const entt::entity& entity) {
		m_state = tick(registry, entity);
		return m_state;
	}

	State getState() const {
		return m_state;
	}

protected:
	State m_state = State::RUNNING;
	std::vector<INode*> m_children;

	virtual State tick(entt::registry& registry, const entt::entity& entity) = 0;
};
