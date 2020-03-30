#pragma once

#include <thread>
#include <unordered_map>
#include <limits>
#include <glm/glm.hpp>
#include <Terrain/Terrain.hpp>
#include <Util/Math3d.hpp>
#include <Grid2D.hpp>

using namespace glm;

class ChunkedTerrain : public Terrain {
public:
	
	ChunkedTerrain(vec3 chunkSize);
	
	vec3 getChunkSize() const;
	
	bool has(ivec2 coords) const;
	void set(ivec2 coords, Terrain *terrain);
	Terrain *get(ivec2 coords);
	void getChunkBounds(ivec2 coords, vec3 &min, vec3 &max) const;
	Terrain *remove(ivec2 coords);
	
	ivec2 worldPosToChunk(vec3 p) const;
	vec3 worldPosToLocalChunkPos(vec3 p) const;
	
	Grid2D<Terrain> &getChunkGrid() { return this->chunks; }
protected:
	
	float sampleValue(vec3 p) const;
	
private:
	vec3 chunkSize;
	Grid2D<Terrain> chunks;
	
};