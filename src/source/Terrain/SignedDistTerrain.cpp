#include <Terrain/SignedDistTerrain.hpp>

using namespace glm;

///////////////
//  PUBLIC   //
///////////////

void SignedDistTerrain::addSphere(vec3 p, float r) {
	bodies.emplace_back(new Sphere(p, r));
	operations.push_back(Op::ADD);
}

void SignedDistTerrain::subSphere(vec3 p, float r) {
	bodies.emplace_back(new Sphere(p, r));
	operations.push_back(Op::SUB);
}

///////////////
// PROTECTED //
///////////////

float SignedDistTerrain::sampleValue(vec3 p) const {
	float d = p.y;
	
	int i = 0;
	for (auto &b : bodies) {
		Op op = operations[i++];
		float newdist = b->distance(p);
		switch (op) {
			case Op::ADD: d = glm::min(d, newdist); break;
			case Op::SUB: d = glm::max(d, -newdist); break;
			
		};
	}
	/*
	constexpr float sphereRadius = 1.0f;
	float f1 = p.y;
	float s1 = glm::distance(vec3(0.0f), p) - sphereRadius;
	float s2 = glm::distance(vec3(1.0f, 0.0f, 0.0f), p) - sphereRadius * 0.5f;
	float s3 = glm::distance(vec3(0.0f, 0.0f, 0.0f), p) - sphereRadius * 0.7f;
	float s4 = glm::distance(vec3(-1.0f, 0.0f, 0.0f), p) - sphereRadius * 0.5f;
	float s5 = glm::distance(vec3(0.0f, 0.8f, 0.0f), p) - sphereRadius * 0.5f;
	float d = glm::max(s1, -s2);
	d = glm::max(d, -s3);
	d = glm::max(d, -s4);
	d = glm::min(d, s5);
	d = glm::min(d, f1);
	*/
	return d;
}

///////////////
//  PRIVATE  //
///////////////

