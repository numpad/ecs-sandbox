#pragma once

#include <glm/glm.hpp>
#include <Util/Random.hpp>

using namespace glm;

// hashing function for ivec2
template <>
struct std::hash<glm::ivec2> {
	size_t operator()(const glm::ivec2& k) const {
		using std::hash;
		return ((hash<int>()(k.x) ^ (hash<int>()(k.y) << 1)) >> 1);
	}
};

// types
namespace m3d {
	
	struct ray {
		vec3 origin;
		vec3 dir;
		
		ray(vec3 o, vec3 d) : origin(o), dir(normalize(d)) {}
	};
	
	struct plane {
		vec3 normal;
		float dist;
		
		plane(vec3 n, float d = 0.0f) : normal(normalize(n)), dist(d) {}
	};
		
}


// util functions
namespace m3d {
	vec3 randomizeVec3(vec3 v, float change);
	
	vec3 mouseToCameraRay(const mat4 &uProjection,
		const mat4 &uView, const vec2 ndcPoint);
	
	// cast a ray onto a plane
	vec3 raycast(ray r, plane p);
	

	
	int solve_ballistic_arc(glm::vec3 proj_pos, float proj_speed, glm::vec3 target, float gravity, glm::vec3 &s0, glm::vec3 &s1);
	 
	float ballistic_range(float speed, float gravity, float initial_height);
	
	bool solve_ballistic_arc_lateral(glm::vec3 proj_pos, float lateral_speed, glm::vec3 target_pos, float max_height, glm::vec3 &fire_velocity, float &gravity);
	
	vec3 triangleCalcNormal(vec3 *triangle);
	
}
