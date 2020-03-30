#pragma once

#include <unordered_map>

#include <glm/glm.hpp>

#include <Assets/Mesh.hpp>
#include <Terrain/Terrain.hpp>
#include <Terrain/SignedDistTerrain.hpp>
#include <Terrain/ChunkedTerrain.hpp>
#include <Terrain/CubeMarcher.hpp>
#include <Util/Math3d.hpp>
#include <Grid2D.hpp>
#include <util/sgl_shader.hpp>

using namespace glm;

class ChunkedWorld {
public:
	
	ChunkedWorld(vec3 chunkSize);
	
	bool hasChunkAtPos(vec3 pos) const;
	void set(ivec2 coords, Terrain *terrain);
	
	void remove(ivec2 coords, Terrain *not_if_same = nullptr);
	
	void update(ivec2 coords, Terrain *terrain);
	
	void polygonizeChunk(ivec2 coords);
	void polygonizeAllChunks();
	
	void destroy();
	
	void draw(sgl::shader &shader);
	
	ChunkedTerrain &getTerrain() { return chunkedTerrain; };
	
private:
	CubeMarcher marcher;
	
	ChunkedTerrain chunkedTerrain;
	Grid2D<Mesh> chunkMeshes;
	
};