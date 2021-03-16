#include <Script/FFI.hpp>


extern "C" {

	// Map generation / Terrain
	SignedDistTerrain* FFI_signeddistterrain_new() {
		return new SignedDistTerrain();
	}
	void FFI_signeddistterrain_plane(SignedDistTerrain *terrain, glm::vec3 p, glm::vec3 n, float h) {
		terrain->plane(p, n, h);
	}
	void FFI_signeddistterrain_sphere(SignedDistTerrain *terrain, glm::vec3 pos, float r, bool add_or_sub) {
		terrain->sphere(pos, r, add_or_sub ? SignedDistTerrain::Op::UNION : SignedDistTerrain::Op::DIFF);
	}
	void FFI_signeddistterrain_box(SignedDistTerrain *terrain, glm::vec3 pos, glm::vec3 size, bool add_or_sub) {
		terrain->box(pos, size, add_or_sub ? SignedDistTerrain::Op::UNION : SignedDistTerrain::Op::DIFF);
	}
	void FFI_signeddistterrain_cylinder(SignedDistTerrain *terrain, glm::vec3 pos, float h, float r, bool add_or_sub) {
		terrain->cylinder(pos, h, r, add_or_sub ? SignedDistTerrain::Op::UNION : SignedDistTerrain::Op::DIFF);
	}
	void FFI_chunkedworld_set(ChunkedWorld *world, void *grid, int x, int y, SignedDistTerrain *terrain) {
		world->set(ivec2(x, y), terrain);
		((Grid2D<SignedDistTerrain> *)grid)->set(x, y, (SignedDistTerrain *)1);
	}

	float FFI_chunkedTerrain_raycast(ChunkedTerrain *terrain, glm::vec3 p, glm::vec3 d, float max_len) {
		return terrain->raycastd(p, d, max_len);
	}

	float FFI_chunkedTerrain_sampleValue(ChunkedTerrain *terrain, glm::vec3 p) {
		return terrain->sampleValueAt(p);
	}

	// Math / Vec3
	glm::vec3 FFI_vec3_add(glm::vec3 a, glm::vec3 b) {
		return a + b;
	}
	glm::vec3 FFI_vec3_sub(glm::vec3 a, glm::vec3 b) {
		return a - b;
	}
	glm::vec3 FFI_vec3_mul(glm::vec3 a, glm::vec3 b) {
		return a * b;
	}
	glm::vec3 FFI_vec3_smul(glm::vec3 a, float s) {
		return a * s;
	}
	glm::vec3 FFI_vec3_div(glm::vec3 a, glm::vec3 b) {
		return a / b;
	}
	glm::vec3 FFI_vec3_sdiv(glm::vec3 a, float s) {
		return a / s;
	}
	float FFI_vec3_dot(glm::vec3 a, glm::vec3 b) {
		return glm::dot(a, b);
	}
	glm::vec3 FFI_vec3_cross(glm::vec3 a, glm::vec3 b) {
		return glm::cross(a, b);
	}
	float FFI_vec3_len(glm::vec3 a) {
		return glm::length(a);
	}
	glm::vec3 FFI_vec3_normalize(glm::vec3 a) {
		return glm::normalize(a);
	}
	
}

