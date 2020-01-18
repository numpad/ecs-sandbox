#include <RenderObject/ChunkedWorld.hpp>

////////////
// PUBLIC //
////////////

ChunkedWorld::ChunkedWorld(vec3 chunkSize)
	: chunkedTerrain(chunkSize)
{
	marcher.setSampleDetail(.1f);
	
}

ChunkedWorld::~ChunkedWorld() {
	for (auto it : chunkMeshes) {
		it.second->destroy();
		delete it.second;
	}
}

void ChunkedWorld::set(ivec2 coords, Terrain &terrain) {
	chunkedTerrain.set(coords, terrain);
}

void ChunkedWorld::draw(sgl::shader &shader) const {
	for (auto it : chunkMeshes) {
		it.second->draw(shader);
	}
}

void ChunkedWorld::polygonizeChunk(ivec2 coords) {
	auto search = chunkMeshes.find(coords);
	if (search != chunkMeshes.end()) {
		// chunk already polygonized into mesh, delete old
		search->second->destroy();
		delete search->second;
	}
	
	// cubemarch
	vec3 min, max;
	chunkedTerrain.getChunkBounds(coords, min, max);
	marcher.setSampleRange(min, max);
	// build mesh
	auto vertices = marcher.polygonize(chunkedTerrain);
	Mesh *mesh = new Mesh(vertices, false);
	chunkMeshes[coords] = mesh;
}

void ChunkedWorld::polygonizeAllChunks() {
	for (auto it : chunkedTerrain.getChunks()) {
		this->polygonizeChunk(it.first);
	}
}

/////////////
// PRIVATE //
/////////////

