#pragma once

#include <stdlib.h>
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
		char state = m_states.at(m_stateIndex);
		if (m_stateIndex < m_states.size() - 1) {
			m_stateIndex++;
		}

		if (state == '?') {
			if (rand() % 100 < 50)
				state = 's';
			else
				state = '-';

			fmt::print(fmt::emphasis::bold, "random:");
		}

		switch (state) {
		case '.':
			fmt::print(fmt::fg(fmt::terminal_color::yellow), "{} In Progress\n", m_description);
			return State::RUNNING;
		case '+':
		case 's':
			fmt::print(fmt::fg(fmt::terminal_color::green), "{} Complete!\n", m_description);
			return State::SUCCESS;
		default:
			fmt::print(fmt::fg(fmt::terminal_color::red), "{} Failed!\n", m_description);
			return State::FAILURE;
		};
	}

private:
	std::string m_description, m_states;
	size_t m_stateIndex = 0;
};
