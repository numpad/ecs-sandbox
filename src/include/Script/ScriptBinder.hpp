#pragma once

#include <string>

#include <config.hpp>

#include <angelscript.h>
#include <sol/sol.hpp>

#include <Util/File.hpp>
#include <Util/Benchmark.hpp>

#include <ecs/components.hpp>
#include <glm/glm.hpp>

namespace ScriptBinder {
	
	void angelscriptTest();
	void luaTest();
	
	void registerEngine(asIScriptEngine *engine);
	
	// grouped register calls
	
	int registerComponents(asIScriptEngine *engine);
}
