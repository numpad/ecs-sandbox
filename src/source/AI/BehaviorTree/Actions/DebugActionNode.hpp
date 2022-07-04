#pragma once

#include <string>
#include <fmt/core.h>
#include <fmt/color.h>
#include "AI/BehaviorTree/INode.hpp"

class DebugActionNode : public INode {
public:
	DebugActionNode(const std::string& action, const std::string& states = "s")
	    : m_description{action}, m_states{states} {
	}

protected:
	State tick(entt::registry& registry, const entt::entity& entity) {
		fmt::print(fmt::emphasis::bold, "{}: ", m_description);
		const char state = m_states.at(m_stateIndex);
		if (m_stateIndex < m_states.size() - 1) {
			m_stateIndex++;
		}

		switch (state) {
		case '.':
			fmt::print(fmt::fg(fmt::terminal_color::yellow), "running...\n");
			return State::RUNNING;
		case '+':
		case 's':
			fmt::print(fmt::fg(fmt::terminal_color::green), "success!\n");
			return State::SUCCESS;
		default:
			fmt::print(fmt::fg(fmt::terminal_color::red), "fail!\n");
			return State::FAILURE;
		};
	}

private:
	std::string m_description, m_states;
	size_t m_stateIndex = 0;
};
