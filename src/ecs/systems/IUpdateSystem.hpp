#pragma once

#include <entt/entt.hpp>

class IUpdateSystem {
public:
	
	IUpdateSystem(entt::registry &registry) : registry(registry) {}
	
	virtual ~IUpdateSystem() { }
	virtual void update(float dt) = 0;

protected:
	entt::registry &registry;
	
};
