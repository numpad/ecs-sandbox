#pragma once

#include <unordered_map>

#include <glm/glm.hpp>

#include <Assets/Mesh.hpp>
#include <Terrain/Terrain.hpp>
#include <Terrain/SignedDistTerrain.hpp>
#include <Terrain/ChunkedTerrain.hpp>
#include <Terrain/CubeMarcher.hpp>
#include <Util/Math3d.hpp>

#include <util/sgl_shader.hpp>

using namespace glm;

class ChunkedWorld {
public:
	
	ChunkedWorld(vec3 chunkSize);
	~ChunkedWorld();
	
	void set(ivec2 coords, Terrain &terrain);
	
	void draw(sgl::shader &shader);
	
private:
	CubeMarcher marcher;
	
	ChunkedTerrain chunkedTerrain;
	std::unordered_map<ivec2, Mesh *> chunkMeshes;
	
	void polygonizeChunk(ivec2 coords);
};