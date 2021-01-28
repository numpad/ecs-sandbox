#pragma once

#include <glm/glm.hpp>
#include <Terrain/SignedDistTerrain.hpp>
#include <RenderObject/ChunkedWorld.hpp>

extern "C" {

	// Map generation / Terrain
	SignedDistTerrain* FFI_signeddistterrain_new();
	void FFI_signeddistterrain_plane(SignedDistTerrain *terrain, glm::vec3 p, glm::vec3 n, float h);
	void FFI_signeddistterrain_sphere(SignedDistTerrain *terrain, glm::vec3 pos, float r, bool add_or_sub);
	void FFI_signeddistterrain_box(SignedDistTerrain *terrain, glm::vec3 pos, glm::vec3 size, bool add_or_sub);
	void FFI_signeddistterrain_cylinder(SignedDistTerrain *terrain, glm::vec3 pos, float h, float r, bool add_or_sub);
	// TODO: fix void pointer to grid instead of Grid2D<SignedDistTerrain>
	void FFI_chunkedworld_set(ChunkedWorld *world, void *grid, int x, int y, SignedDistTerrain *terrain);
	float FFI_chunkedTerrain_raycast(ChunkedTerrain *terrain, glm::vec3 p, glm::vec3 d, float max_len);
	float FFI_chunkedTerrain_sampleValue(ChunkedTerrain *terrain, glm::vec3 p);

	// Math / Vec3
	glm::vec3 FFI_vec3_add(glm::vec3 a, glm::vec3 b);
	glm::vec3 FFI_vec3_sub(glm::vec3 a, glm::vec3 b);
	glm::vec3 FFI_vec3_mul(glm::vec3 a, glm::vec3 b);
	glm::vec3 FFI_vec3_smul(glm::vec3 a, float s);
	glm::vec3 FFI_vec3_div(glm::vec3 a, glm::vec3 b);
	glm::vec3 FFI_vec3_sdiv(glm::vec3 a, float s);
	float FFI_vec3_dot(glm::vec3 a, glm::vec3 b);
	glm::vec3 FFI_vec3_cross(glm::vec3 a, glm::vec3 b);
	float FFI_vec3_len(glm::vec3 a);
	glm::vec3 FFI_vec3_normalize(glm::vec3 a);
	
}
