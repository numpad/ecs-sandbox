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
	return glm::distance(vec3(0.0f), p) - sphereRadius;
}

///////////////
//  PRIVATE  //
///////////////

