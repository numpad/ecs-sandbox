#pragma once

#include <ecs/systems/BaseUpdateSystem.hpp>

#include <entt/entt.hpp>

#include <ecs/events.hpp>

class LogSystem : public BaseUpdateSystem {
public:
	
	LogSystem(entt::registry &registry);
	~LogSystem();
	
	void receive(const LogEvent &event);
	
	void update();
	
private:
	
	
};
