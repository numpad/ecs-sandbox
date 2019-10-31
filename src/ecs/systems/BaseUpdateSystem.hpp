#pragma once

#include <entt/entt.hpp>

class BaseUpdateSystem {

public:
	
	virtual void update(entt::registry &registry) = 0;
	
};
