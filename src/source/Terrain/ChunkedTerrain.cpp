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

void ChunkedTerrain::set(ivec2 coords, Terrain &terrain) {
	chunks.insert_or_assign(coords, terrain);
}

void ChunkedTerrain::getChunkBounds(ivec2 coords, vec3 &min, vec3 &max) const {
	min = 2.f * chunkSize * vec3(float(coords.x), 0.f, float(coords.y)) - chunkSize;
	max = 2.f * chunkSize * vec3(float(coords.x), 0.f, float(coords.y)) + chunkSize;
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
	p.x = glm::sign(p.x - chunk.x * (chunkSize.x * 2.f)) * glm::mod(glm::abs(p.x - chunk.x * (chunkSize.x * 2.f)), chunkSize.x + 0.000f);
	p.z = glm::sign(p.z - chunk.y * (chunkSize.z * 2.f)) * glm::mod(glm::abs(p.z - chunk.y * (chunkSize.z * 2.f)), chunkSize.z + 0.000f);
	return p;
}

const std::unordered_map<ivec2, Terrain &> &ChunkedTerrain::getChunks() const {
	return chunks;
}

///////////////
// PROTECTED //
///////////////

float ChunkedTerrain::sampleValue(vec3 p) const {
	ivec2 chunk = worldPosToChunk(p);
	
	auto search = chunks.find(chunk);
	if (search != chunks.end()) {
		vec3 local = worldPosToLocalChunkPos(p);
		return search->second.get(local);
	}
	
	return std::numeric_limits<float>::max();
}

///////////////
//  PRIVATE  //
///////////////

