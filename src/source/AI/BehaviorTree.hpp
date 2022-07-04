#pragma once

#include "AI/BehaviorTree/INode.hpp"

class BehaviorTree {
public:
	BehaviorTree(INode* rootNode) : m_rootNode{rootNode} {
	}

	INode::State tick(entt::registry& registry, const entt::entity& entity) {
		return m_rootNode->tick(registry, entity);
	}

private:
	INode* m_rootNode;
};
