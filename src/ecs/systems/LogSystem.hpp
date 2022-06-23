#pragma once

#include <functional>

#include <entt/entt.hpp>

#include <ecs/systems/IUpdateSystem.hpp>
#include <ecs/events.hpp>

class LogSystem : public IUpdateSystem {
public:
	LogSystem(entt::registry& registry);
	~LogSystem();

	void setLogger(std::function<void(const LogEvent&)> logFunc);

	void receive(const LogEvent& event);

	void update(float dt);

private:
	std::function<void(const LogEvent&)> logFunc = [](const LogEvent& e) { printf("log> %s\n", e.text.c_str()); };
};
