#include <RenderObject/ChunkedWorld.hpp>

////////////
// PUBLIC //
////////////

ChunkedWorld::ChunkedWorld(vec3 chunkSize) : chunkedTerrain(chunkSize) {
	marcher.setSampleDetail(.1f);
}

bool ChunkedWorld::hasChunkAtPos(vec3 pos) const {
	ivec2 p = chunkedTerrain.worldPosToChunk(pos);
	return chunkedTerrain.has(p);
}

void ChunkedWorld::set(ivec2 coords, Terrain* terrain) {
	this->remove(coords, terrain);
	chunkedTerrain.set(coords, terrain);
}

void ChunkedWorld::remove(ivec2 coords, Terrain* not_if_same) {
	Mesh* m = chunkMeshes.remove(coords);
	if (m) {
		delete m;
	}
	Terrain* t = chunkedTerrain.get(coords);
	// remove and delete Terrain if not the same
	if (t != not_if_same) {
		t = chunkedTerrain.remove(coords);
		if (t) {
			delete t;
		}
	}
}

void ChunkedWorld::update(ivec2 coords, Terrain* terrain) {
	set(coords, terrain);
	polygonizeChunk(coords);

	// update surrounding chunks
	polygonizeChunk(coords + ivec2(1, 0));
	polygonizeChunk(coords + ivec2(0, 1));
	polygonizeChunk(coords + ivec2(-1, 0));
	polygonizeChunk(coords + ivec2(0, -1));
}

void ChunkedWorld::draw(sgl::shader& shader) {
	chunkMeshes.each([&shader](int, int, Mesh* t) { t->draw(shader); });
}

void ChunkedWorld::polygonizeChunk(ivec2 coords) {
	auto search = chunkMeshes.at(coords);
	if (search != nullptr) {
		// chunk already polygonized into mesh, delete old
		search->destroy();
		delete search;
	}

	// cubemarch
	vec3 min, max;
	chunkedTerrain.getChunkBounds(coords, min, max);
	marcher.setSampleRange(min, max);
	// build mesh
	auto vertices = marcher.polygonize(chunkedTerrain);
	Mesh* mesh = new Mesh(vertices, false);
	chunkMeshes.set(coords, mesh);
}

void ChunkedWorld::polygonizeAllChunks() {
	printf(" --- NOT IMPLEMENTED ---\n");
	// for (auto it : chunkedTerrain.getChunks()) {
	//	this->polygonizeChunk(it.first);
	// }
}

void ChunkedWorld::destroy() {
	// destroy all generated meshes
	chunkMeshes.each([](int, int, Mesh* m) {
		m->destroy();
		delete m;
	});
	// destroy all chunks
	chunkedTerrain.getChunkGrid().each([this](int x, int y, Terrain*) {
		Terrain* t = this->chunkedTerrain.getChunkGrid().at(ivec2(x, y));
		delete t;
	});
	// remove pointers to chunks
	chunkedTerrain.getChunkGrid().clear();
}

/////////////
// PRIVATE //
/////////////
