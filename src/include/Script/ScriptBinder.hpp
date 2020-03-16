#pragma once

#include <string>

#include <config.hpp>

#include <angelscript.h>
#include <Util/File.hpp>
#include <Util/Benchmark.hpp>

#include <ecs/components.hpp>
#include <glm/glm.hpp>

namespace ScriptBinder {
	
	void angelscriptTest();
	
	void registerEngine(asIScriptEngine *engine);
	
	// grouped register calls
	
	int registerVectors(asIScriptEngine *engine);
	int registerComponents(asIScriptEngine *engine);
}
