#include <Script/ScriptBinder.hpp>

namespace ScriptBinder {
		
	void loadEntity(std::string name) {
		std::string path = "res/scripts/entitydefs/" + name;
		
		sol::state loader;
		loader.open_libraries();
		registerEngine(loader);
		
		entt::registry reg;
		loader["registry"] = std::ref(reg);
		
		loader.script_file(path);
		std::string ename = loader["entity"]["name"];
		printf("loading '%s'\n", ename.c_str());
		
		sol::table components = loader["entity"]["components"];
		for (const auto &kv : components) {
			printf("%s\n", kv.first.as<std::string>().c_str());
		}
		
		// testing assigning
		auto p = loader["p"];
		
		// assign it by script (that sucks)
		//p["assign"](p, reg, e);
		
		
		reg.view<CPosition>().each([](auto entity, auto pos) {
			printf("#\n# GOT ENTITY WITH CPosition!\n#\n");
		});
	}
	
	void luaTest() {
		sol::state lua;
		lua.open_libraries();
		
		registerEngine(lua);
		
		loadEntity("bat.lua");
		
	}
	
	void registerEngine(sol::state &lua) {
		registerVectors(lua);
		registerEnTT(lua);
		registerComponents(lua);
	}
	
	int registerVectors(sol::state &lua) {
		using namespace glm;
		
		// vec3
		lua.new_usertype<vec3>("vec3",
			sol::constructors<vec3(), vec3(float), vec3(float, float, float)>(),
			"x", &vec3::x,
			"y", &vec3::y,
			"z", &vec3::z,
			// functions
			"length",    [](const vec3 self)                   { return length(self); },
			"normalize", [](const vec3 self)                   { return normalize(self); },
			"cross",     [](const vec3 self, const vec3 other) { return cross(self, other); },
			"distance",  [](const vec3 self, const vec3 other) { return distance(self, other); },
			"dot",       [](const vec3 self, const vec3 other) { return dot(self, other); },
			"reflect",   [](const vec3 self, const vec3 n)     { return reflect(self, n); },
			// operators
			sol::meta_function::addition, sol::overload(
				[](const vec3 self, const float v)       { return self + v; },
				[](const vec3 self, const vec3 other)    { return self + other; }),
			sol::meta_function::subtraction, sol::overload(
				[](const vec3 self, const float v)       { return self - v; },
				[](const vec3 self, const vec3 other)    { return self - other; }),
			sol::meta_function::multiplication, sol::overload(
				[](const vec3 self, const float v)       { return self * v; },
				[](const vec3 self, const vec3 other)    { return self * other; }),
			sol::meta_function::division, sol::overload(
				[](const vec3 self, const float v)       { return self / v; },
				[](const vec3 self, const vec3 other)    { return self / other; })
			);
		
		// vec2
		lua.new_usertype<vec2>("vec2",
			sol::constructors<vec2(), vec2(float), vec2(float, float)>(),
			"x", &vec2::x,
			"y", &vec2::y,
			// functions
			"length",    [](const vec2 self)                   { return length(self); },
			"normalize", [](const vec2 self)                   { return normalize(self); },
			"distance",  [](const vec2 self, const vec2 other) { return distance(self, other); },
			"dot",       [](const vec2 self, const vec2 other) { return dot(self, other); },
			"reflect",   [](const vec2 self, const vec2 n)     { return reflect(self, n); },
			// operators
			sol::meta_function::addition, sol::overload(
				[](const vec2 self, const float v)    { return self + v; },
				[](const vec2 self, const vec2 other) { return self + other; }),
			sol::meta_function::subtraction, sol::overload(
				[](const vec2 self, const float v)    { return self - v; },
				[](const vec2 self, const vec2 other) { return self - other; }),
			sol::meta_function::multiplication, sol::overload(
				[](const vec2 self, const float v)    { return self * v; },
				[](const vec2 self, const vec2 other) { return self * other; }),
			sol::meta_function::division, sol::overload(
				[](const vec2 self, const float v)    { return self / v; },
				[](const vec2 self, const vec2 other) { return self / other; })
			);
		
		return 0;
	}
	
	int registerEnTT(sol::state &lua) {
		lua.new_usertype<entt::registry>("Registry",
			"create", [](entt::registry &self) { return self.create(); });
		lua.new_usertype<entt::entity>("Entity");
		
		return 0;
	}
	
	int registerComponents(sol::state &lua) {
		using namespace glm;
		
		// CPosition
		lua.new_usertype<CPosition>("CPosition",
			sol::constructors<CPosition(float, float, float), CPosition(vec3)>(),
			"assign", [](const CPosition self, entt::registry &r, entt::entity e) { r.emplace<CPosition>(e, self); },
			"pos", &CPosition::pos);
		// CVelocity
		lua.new_usertype<CVelocity>("CVelocity",
			sol::constructors<CVelocity(), CVelocity(float, float, float), CVelocity(vec3)>(),
			"assign", [](const CVelocity self, entt::registry &r, entt::entity e) { r.emplace<CVelocity>(e, self); },
			"vel",     &CVelocity::vel,
			"acc",     &CVelocity::acc,
			"maxvel", &CVelocity::maxvel);
		
		return 0;
	}
}
