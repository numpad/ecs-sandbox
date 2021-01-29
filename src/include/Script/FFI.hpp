#pragma once

#include <glm/glm.hpp>
#include <Terrain/SignedDistTerrain.hpp>
#include <RenderObject/ChunkedWorld.hpp>

extern "C" {

	SignedDistTerrain* FFI_signeddistterrain_new() {
		return new SignedDistTerrain();
	}
	void FFI_signeddistterrain_plane(SignedDistTerrain *terrain, float h) {
		terrain->plane(vec3(0.f), vec3(0.f, 1.f, 0.f), h);
	}
	void FFI_signeddistterrain_sphere(SignedDistTerrain *terrain, float x, float y, float z, float r, bool add_or_sub) {
		terrain->sphere(vec3(x, y, z), r, add_or_sub ? SignedDistTerrain::Op::UNION : SignedDistTerrain::Op::DIFF);
	}
	void FFI_signeddistterrain_box(SignedDistTerrain *terrain, float x, float y, float z, float a, float b, float c, bool add_or_sub) {
		terrain->box(vec3(x, y, z), vec3(a, b, c), add_or_sub ? SignedDistTerrain::Op::UNION : SignedDistTerrain::Op::DIFF);
	}
	void FFI_chunkedworld_set(ChunkedWorld *world, void *grid, int x, int y, SignedDistTerrain *terrain) {
		world->set(ivec2(x, y), terrain);
		((Grid2D<SignedDistTerrain> *)grid)->set(x, y, (SignedDistTerrain *)1);
	}

}
