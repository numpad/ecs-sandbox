#include <Terrain/ChunkedTerrain.hpp>

using namespace glm;

//////////////
//  PUBLIC  //
//////////////

ChunkedTerrain::ChunkedTerrain(vec3 chunkSize)
	: chunkSize(chunkSize)
{
	
}

vec3 ChunkedTerrain::getChunkSize() const {
	return chunkSize;
}

bool ChunkedTerrain::has(ivec2 coords) const {
	return chunks.at(coords) != nullptr;
}

void ChunkedTerrain::set(ivec2 coords, Terrain *terrain) {
	chunks.set(coords, terrain);
}

Terrain *ChunkedTerrain::get(ivec2 coords) {
	return chunks.at(coords);
}

void ChunkedTerrain::getChunkBounds(ivec2 coords, vec3 &min, vec3 &max) const {
	min = 2.f * chunkSize * vec3(float(coords.x), 0.f, float(coords.y)) - chunkSize;
	max = 2.f * chunkSize * vec3(float(coords.x), 0.f, float(coords.y)) + chunkSize;
}

Terrain *ChunkedTerrain::remove(ivec2 coords) {
	return chunks.remove(coords);
}

ivec2 ChunkedTerrain::worldPosToChunk(vec3 p) const {
	ivec2 cp;
	cp.x = glm::sign(p.x) * int(int(abs(p.x) + chunkSize.x) / (chunkSize.x * 2.f));
	cp.y = glm::sign(p.z) * int(int(abs(p.z) + chunkSize.z) / (chunkSize.z * 2.f));
	return cp;
}

vec3 ChunkedTerrain::worldPosToLocalChunkPos(vec3 p) const {
	vec2 chunk = vec2(worldPosToChunk(p));
	vec3 signs = glm::sign(p);
	
	// 0.0001f actually
	p.x = glm::sign(p.x - chunk.x * (chunkSize.x * 2.f)) * glm::mod(glm::abs(p.x - chunk.x * (chunkSize.x * 2.f)), chunkSize.x + 0.0001f);
	p.z = glm::sign(p.z - chunk.y * (chunkSize.z * 2.f)) * glm::mod(glm::abs(p.z - chunk.y * (chunkSize.z * 2.f)), chunkSize.z + 0.0001f);
	return p;
}

float ChunkedTerrain::raycastd(vec3 origin, vec3 dir, float max_length) {
	// if `dir` is not set, just test the `origin`
	if (glm::length(dir) == 0.f) max_length = 0.f;
	else dir = glm::normalize(dir);

	float nearest; // distance to the nearest point of the terrain (result of SDF)
	float dist; // distance we traveled on the ray
	int iteration = 0;
	constexpr int max_iterations = 100;
	for (dist = 0.f; dist <= max_length && iteration < max_iterations; dist += nearest) {
		vec3 p = origin + dir * dist;
		nearest = sampleValue(p);
		if (nearest <= 0.f) {
			return dist;
		}
		// prevent infinite loop
		if (max_length == 0.f) return -1.f;
		iteration++;
	}

	// if we didn't exactly reach `max_length`, test it too.
	if (dist < max_length) {
		if (sampleValue(origin + dir * max_length) <= 0.f) return max_length;
	}
	return -1.0f;
}

///////////////
// PROTECTED //
///////////////

float ChunkedTerrain::sampleValue(vec3 p) const {
	ivec2 chunk = worldPosToChunk(p);
	
	Terrain *search = chunks.at(chunk);
	if (search != nullptr) {
		vec3 local = worldPosToLocalChunkPos(p);
		return search->get(local);
	}
	
	return std::numeric_limits<float>::max();
}

///////////////
//  PRIVATE  //
///////////////

