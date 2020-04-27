#pragma once

#include <string>

#include <config.hpp>

#include <sol/sol.hpp>
#include <entt/entt.hpp>

#include <Util/File.hpp>
#include <Util/Benchmark.hpp>

#include <ecs/components.hpp>
#include <glm/glm.hpp>

using namespace glm;

namespace ScriptBinder {
	
	void luaTest();
	
	void loadEntity(std::string);
	
	void registerEngine(sol::state &lua);
	
	// grouped register calls
	
	int registerVectors(sol::state &lua);
	int registerEnTT(sol::state &lua);
	int registerComponents(sol::state &lua);
}
