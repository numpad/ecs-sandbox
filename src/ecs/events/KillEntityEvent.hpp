#pragma once

#include <string>
#include <entt/entt.hpp>

struct KillEntityEvent {
	entt::entity which;
	std::string how;
};
