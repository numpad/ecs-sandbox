#include <Terrain/SignedDistTerrain.hpp>

using namespace glm;

///////////////
//  PUBLIC   //
///////////////

void SignedDistTerrain::sphere(vec3 p, float r, SignedDistTerrain::Op op) {
	bodies.emplace_back(new Sphere(p, r));
	operations.push_back(op);
}

void SignedDistTerrain::box(vec3 p, vec3 b, SignedDistTerrain::Op op) {
	bodies.emplace_back(new Box(p, b));
	operations.push_back(op);
}

void SignedDistTerrain::cylinder(vec3 p, float h, float r, SignedDistTerrain::Op op) {
	bodies.emplace_back(new Cylinder(p, h, r));
	operations.push_back(op);
}

void SignedDistTerrain::plane(vec3 p, vec3 n, float d, Op op) {
	bodies.emplace_back(new Plane(p, n, d));
	operations.push_back(op);
}

///////////////
// PROTECTED //
///////////////

static inline float sdUnion(float a, float b) { return glm::min(a, b); }
static inline float sdDiff(float a, float b) { return glm::max(a, -b); }
static inline float sdIntersect(float a, float b) { return glm::max(a, b); }
static inline float sdUnionSmooth( float d1, float d2, float k ) {
	float h = clamp( 0.5f + 0.5f*(d2-d1)/k, 0.0f, 1.0f );
	return mix( d2, d1, h ) - k*h*(1.0f-h); }

float SignedDistTerrain::sampleValue(vec3 p) const {
	float d = 9999999.0f;
	
	int i = 0;
	for (auto &b : bodies) {
		Op op = operations[i++];
		// "b->p - p": transform sampled p according to position of b
		float newdist = b->distance(b->p - p);
		switch (op) {
			case Op::UNION: d = sdUnionSmooth(d, newdist, 0.01f); break;
			case Op::DIFF: d = sdDiff(d, newdist); break;
			case Op::INTERSECT: d = sdIntersect(d, newdist); break;
		};
	}
	
	return d;
}

///////////////
//  PRIVATE  //
///////////////

