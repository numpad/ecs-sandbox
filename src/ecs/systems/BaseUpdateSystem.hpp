#pragma once

#include <entt/entt.hpp>

class BaseUpdateSystem {
public:
	
	BaseUpdateSystem(entt::registry &registry) : registry(registry) {}
	
	virtual ~BaseUpdateSystem() { }
	virtual void update() = 0;

protected:
	entt::registry &registry;
	
};
