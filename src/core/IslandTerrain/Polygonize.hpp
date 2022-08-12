#pragma once

#include <glm/glm.hpp>
#include "Util/Math3d.hpp"
#include "IslandTerrain/Terrain.hpp"

static constexpr int EDGE_TABLE[256] = {
	#include "Terrain/EdgeTable.txt"
};
static constexpr int TRIANGLE_TABLE[256][16] = {
	#include "Terrain/TriangleTable.txt"
};


static inline size_t ptoi(const glm::ivec3 p, const glm::ivec3 size) {
	return (p.x) + (p.y * size.x) + (p.z * size.x * size.y);
}

static inline glm::vec3 interpolate(const glm::vec3 p1, const glm::vec3 p2, float v1, float v2, const float surface) {
	return p1 + (surface - v1) * ((p2 - p1) / (v2 - v1));
}

static inline size_t getMaxVertexCount(const Terrain terrain) {
	return terrain.size.x * terrain.size.y * terrain.size.z * 12;
}

static void getCubeCornerOffsets(glm::vec3* positions, const glm::vec3 scale) {
	positions[0] = glm::vec3(   0.0f,     0.0f,     0.0f);
	positions[1] = glm::vec3(scale.x,     0.0f,     0.0f);
	positions[2] = glm::vec3(scale.x,     0.0f,  scale.z);
	positions[3] = glm::vec3(   0.0f,     0.0f,  scale.z);
	positions[4] = glm::vec3(   0.0f,  scale.y,     0.0f);
	positions[5] = glm::vec3(scale.x,  scale.y,     0.0f);
	positions[6] = glm::vec3(scale.x,  scale.y,  scale.z);
	positions[7] = glm::vec3(   0.0f,  scale.y,  scale.z);
}

static void getCubeSurfaceValues(const Terrain terrain, const size_t index, float* values) {
	const size_t dx = 1;
	const size_t dy = terrain.size.x;
	const size_t dz = terrain.size.x * terrain.size.y;
	values[0] = terrain.values[index               ];
	values[1] = terrain.values[index + dx          ];
	values[2] = terrain.values[index + dx + dz     ];
	values[3] = terrain.values[index + dz          ];
	values[4] = terrain.values[index + dy          ];
	values[5] = terrain.values[index + dx + dy     ];
	values[6] = terrain.values[index + dx + dy + dz];
	values[7] = terrain.values[index + dy + dz     ];
}

static int getCubeIndex(const Terrain terrain, const size_t index, float* surfaceValues) {
	int cubeIndex = 0;
	if (surfaceValues[0] < terrain.surface) cubeIndex |=   1;
	if (surfaceValues[1] < terrain.surface) cubeIndex |=   2;
	if (surfaceValues[2] < terrain.surface) cubeIndex |=   4;
	if (surfaceValues[3] < terrain.surface) cubeIndex |=   8;
	if (surfaceValues[4] < terrain.surface) cubeIndex |=  16;
	if (surfaceValues[5] < terrain.surface) cubeIndex |=  32;
	if (surfaceValues[6] < terrain.surface) cubeIndex |=  64;
	if (surfaceValues[7] < terrain.surface) cubeIndex |= 128;
	return cubeIndex;
}

template <typename Vx>
void polygonize(const Terrain terrain, Vx* vertices, size_t& vertices_produced, const glm::vec3 scale) {
	//vertices_produced = getMaxVertexCount(terrain);
	vertices_produced = -1;

	glm::vec3 cornerOffsets[8];
	getCubeCornerOffsets(cornerOffsets, scale);
	
	for (int x = 0; x < terrain.size.x - 1; ++x) {
		for (int y = 0; y < terrain.size.y - 1; ++y) {
			for (int z = 0; z < terrain.size.z - 1; ++z) {
				const glm::ivec3 p(x, y, z);
				const glm::vec3 pf(static_cast<float>(x) * scale.x, static_cast<float>(y) * scale.y, static_cast<float>(z) * scale.z);
				const size_t i = ptoi(p, terrain.size);
				
				float surfaceValues[8];
				getCubeSurfaceValues(terrain, i, surfaceValues);
				const int cubeIndex = getCubeIndex(terrain, i, surfaceValues);
				const int edges = EDGE_TABLE[cubeIndex];
				if (edges == 0) continue;
				
				glm::vec3 positions[12];
				if (edges &    1) positions[ 0] = interpolate(pf + cornerOffsets[0], pf + cornerOffsets[1], surfaceValues[0], surfaceValues[1], terrain.surface);
				if (edges &    2) positions[ 1] = interpolate(pf + cornerOffsets[1], pf + cornerOffsets[2], surfaceValues[1], surfaceValues[2], terrain.surface);
				if (edges &    4) positions[ 2] = interpolate(pf + cornerOffsets[2], pf + cornerOffsets[3], surfaceValues[2], surfaceValues[3], terrain.surface);
				if (edges &    8) positions[ 3] = interpolate(pf + cornerOffsets[3], pf + cornerOffsets[0], surfaceValues[3], surfaceValues[0], terrain.surface);
				if (edges &   16) positions[ 4] = interpolate(pf + cornerOffsets[4], pf + cornerOffsets[5], surfaceValues[4], surfaceValues[5], terrain.surface);
				if (edges &   32) positions[ 5] = interpolate(pf + cornerOffsets[5], pf + cornerOffsets[6], surfaceValues[5], surfaceValues[6], terrain.surface);
				if (edges &   64) positions[ 6] = interpolate(pf + cornerOffsets[6], pf + cornerOffsets[7], surfaceValues[6], surfaceValues[7], terrain.surface);
				if (edges &  128) positions[ 7] = interpolate(pf + cornerOffsets[7], pf + cornerOffsets[4], surfaceValues[7], surfaceValues[4], terrain.surface);
				if (edges &  256) positions[ 8] = interpolate(pf + cornerOffsets[0], pf + cornerOffsets[4], surfaceValues[0], surfaceValues[4], terrain.surface);
				if (edges &  512) positions[ 9] = interpolate(pf + cornerOffsets[1], pf + cornerOffsets[5], surfaceValues[1], surfaceValues[5], terrain.surface);
				if (edges & 1024) positions[10] = interpolate(pf + cornerOffsets[2], pf + cornerOffsets[6], surfaceValues[2], surfaceValues[6], terrain.surface);
				if (edges & 2048) positions[11] = interpolate(pf + cornerOffsets[3], pf + cornerOffsets[7], surfaceValues[3], surfaceValues[7], terrain.surface);

				const int* triangles = TRIANGLE_TABLE[cubeIndex];
				for (int faceIndex = 0; triangles[faceIndex] != -1; faceIndex += 3) {
					const glm::vec3 facePositions[3] = {
						positions[triangles[faceIndex+2]],
						positions[triangles[faceIndex+1]],
						positions[triangles[faceIndex+0]]
					};
					const glm::vec3 normal = m3d::triangleCalcNormal(facePositions);

					for (int j = 0; j < 3; ++j) {
						Vx v;
						v.position = facePositions[j];
						v.normal = normal;
						
						//vertices[i * 12 + faceIndex + j] = v;
						vertices[++vertices_produced] = v;
						assert(vertices_produced <= getMaxVertexCount(terrain));
					}
				}
			}
		}
	}
}


