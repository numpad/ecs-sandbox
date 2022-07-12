#include <ecs/systems/LogSystem.hpp>

LogSystem::LogSystem(entt::registry& registry) : IUpdateSystem(registry) {

	registry.ctx<entt::dispatcher>().sink<LogEvent>().connect<&LogSystem::receive>(*this);
}

LogSystem::~LogSystem() {
	registry.ctx<entt::dispatcher>().sink<LogEvent>().disconnect<&LogSystem::receive>(*this);
}

void LogSystem::setLogger(std::function<void(const LogEvent&)> logFunc) {
	this->logFunc = logFunc;
}

void LogSystem::receive(const LogEvent& event) {
	this->logFunc(event);
}

void LogSystem::update(float dt) {
}
