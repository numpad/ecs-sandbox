#include <Util/Math3d.hpp>

using namespace glm;

namespace m3d {

	vec3 randomizeVec3(vec3 v, float change) {
		static Random rand(-1.0f, 1.0f);
		return v + vec3(rand(), rand(), rand()) * change;
	}
	
	vec3 mouseToCameraRay(const mat4 &uProjection,
		const mat4 &uView, const vec2 ndcPoint)
	{
		using namespace glm;
		
		vec4 rayClip = vec4(ndcPoint.x, -ndcPoint.y, -1.0f, 1.0f);
		
		vec4 rayView = inverse(uProjection) * rayClip;
		rayView = vec4(rayView.x, rayView.y, -1.0f, 0.0f);
		
		vec3 rayWorld = vec3(inverse(uView) * rayView);
		rayWorld = normalize(rayWorld);
		
		return rayWorld;
	}
	
	vec3 raycast(ray ray, plane plane) {
		float t = -(dot(ray.origin, plane.normal) + plane.dist) / dot(ray.dir, plane.normal);
		return ray.origin + ray.dir * t;
	}
}
