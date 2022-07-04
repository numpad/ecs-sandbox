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

	virtual State tick(entt::registry& registry, const entt::entity& entity) = 0;

protected:
	std::vector<INode*> m_children;
};
