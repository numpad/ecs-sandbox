#pragma once

#include <glm/glm.hpp>
#include <Util/Random.hpp>

using namespace glm;

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
	
	vec3 raycast(ray r, plane p);
}
