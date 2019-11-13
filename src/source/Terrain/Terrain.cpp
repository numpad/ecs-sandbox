#include <Terrain/Terrain.hpp>

using namespace glm;


///////////////
//  PUBLIC   //
///////////////

Terrain::Terrain() {
	
}

///////////////
// PROTECTED //
///////////////

float Terrain::sampleValue(vec3 p) const {
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
	return d;
}

///////////////
//  PRIVATE  //
///////////////

