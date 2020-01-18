#pragma once

#include <thread>
#include <unordered_map>
#include <limits>
#include <glm/glm.hpp>
#include <Terrain/Terrain.hpp>
#include <Util/Math3d.hpp>

using namespace glm;

class ChunkedTerrain : public Terrain {
public:
	
	ChunkedTerrain(vec3 chunkSize);
	
	vec3 getChunkSize() const;
	
	void set(ivec2 coords, Terrain &terrain);
	void getChunkBounds(ivec2 coords, vec3 &min, vec3 &max) const;
	
	ivec2 worldPosToChunk(vec3 p) const;
	vec3 worldPosToLocalChunkPos(vec3 p) const;
	
	const std::unordered_map<ivec2, Terrain &> &getChunks() const;
	
protected:
	
	float sampleValue(vec3 p) const;
	
private:
	vec3 chunkSize;
	std::unordered_map<ivec2, Terrain &> chunks;
	
};