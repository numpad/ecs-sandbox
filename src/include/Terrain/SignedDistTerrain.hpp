#pragma once

#include <memory>
#include <vector>
#include <limits>
#include <glm/glm.hpp>
#include <Terrain/Terrain.hpp>

using std::unique_ptr;
using namespace glm;

class SignedDistTerrain : public Terrain {
public:
	enum class Op { UNION, DIFF, INTERSECT };
private:
	struct Body {
		vec3 p;
		Body(vec3 p) : p(p) { }
		virtual ~Body() {}
		virtual float distance(vec3 p) = 0;
	};
	struct Sphere : Body {
		float r;
		Sphere(vec3 p, float r) : Body(p), r(r) {}
		float distance(vec3 p) { return glm::length(p) - r; }
	};
	struct Box : Body {
		vec3 b;
		Box(vec3 p, vec3 b) : Body(p), b(b) {}
		float distance(vec3 p) {
			vec3 q = glm::abs(p) - b;
			return glm::length(glm::max(q, 0.0f)) + glm::min(glm::max(q.x, glm::max(q.y, q.z)), 0.0f);
		}
	};
	struct Cylinder : Body {
		float h;
		float r;
		Cylinder(vec3 p, float h, float r) : Body(p), h(h), r(r) {}
		float distance(vec3 p) {
			vec2 d = glm::abs(vec2(glm::length(vec2(p.x, p.z)), p.y)) - vec2(h, r);
			return glm::min(glm::max(d.x, d.y), 0.0f) + glm::length(glm::max(d, 0.0f));
		}
	};
	struct Plane : Body {
		vec3 n;
		float d;
		Plane(vec3 p, vec3 n, float d = 0.f) : Body(p), n(n), d(d) { }
		float distance(vec3 p) {
			return dot(p, -n) - d;
		}
	};
public:
	
	SignedDistTerrain() = default;

	void sphere(vec3 p, float r, Op op = Op::UNION);
	void box(vec3 p, vec3 s, Op op = Op::UNION);
	void cylinder(vec3 p, float h, float r, Op op = Op::UNION);
	void plane(vec3 p, vec3 n, float d = 0.f, Op op = Op::UNION);
	
protected:
	
	float sampleValue(vec3 pos) const;
	
private:
	std::vector<unique_ptr<Body>> bodies;
	std::vector<Op> operations;
};
