#pragma once

#include <entt/entt.hpp>
#include "AI/BehaviorTree/INode.hpp"

class LambdaActionNode : public INode {
public:
	LambdaActionNode(std::function<State(entt::registry&, const entt::entity& entity)> fn) : m_function{fn} {
	}

protected:
	State tick(entt::registry& registry, const entt::entity& entity) {
		return m_function(registry, entity);
	}

private:
	std::function<State(entt::registry&, const entt::entity& entity)> m_function;
};
