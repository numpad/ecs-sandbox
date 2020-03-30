#include <ecs/systems/LogSystem.hpp>

LogSystem::LogSystem(entt::registry &registry)
	: BaseUpdateSystem(registry) {
	
	registry.ctx<entt::dispatcher>().sink<LogEvent>().connect<&LogSystem::receive>(*this);
}

LogSystem::~LogSystem() {
	registry.ctx<entt::dispatcher>().sink<LogEvent>().disconnect<&LogSystem::receive>(*this);
}


void LogSystem::receive(const LogEvent &event) {
	printf("log> %s\n", event.text.c_str());
}

void LogSystem::update() {
	
}
