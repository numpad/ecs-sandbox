#pragma once

#include <entt/entt.hpp>

class BaseUpdateSystem {
public:
	
	virtual ~BaseUpdateSystem() { }
	virtual void update(entt::registry &registry) = 0;
	
};
