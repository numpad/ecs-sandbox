#pragma once

#include <GLFW/glfw3.h>
#include <entt/entt.hpp>
#include "AI/BehaviorTree/INode.hpp"

class TimeoutNode : public INode {
public:
	TimeoutNode(double secondsToWait) : m_startTime{-1.0}, m_secondsToWait{secondsToWait} {
	}

protected:
	State tick(entt::registry& registry, const entt::entity& entity) {
		if (m_startTime < 0.0) {
			m_startTime = glfwGetTime();
		} else if (glfwGetTime() - m_startTime >= m_secondsToWait) {
			m_startTime = -1.0;
			return State::SUCCESS;
		}

		return State::RUNNING;
	}

private:
	double m_startTime;
	double m_secondsToWait;
};
