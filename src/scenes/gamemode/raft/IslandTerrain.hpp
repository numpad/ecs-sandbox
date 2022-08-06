#pragma once

#include "Assets/Vertex.hpp"
#include <array>
#include <fmt/color.h>
#include <glm/glm.hpp>

static constexpr int EDGE_TABLE[256] = {
	#include "Terrain/EdgeTable.txt"
};
static constexpr int TRIANGLE_TABLE[256][16] = {
	#include "Terrain/TriangleTable.txt"
};

class IslandTerrain {
public:
	
	IslandTerrain(glm::ivec3 size);
	~IslandTerrain();

	void set(const glm::ivec3 pos, const float val);
	float get(const glm::ivec3 pos) const;

	Vertex* polygonize();

private:
	glm::ivec3 m_size;
	glm::vec3 m_scale{1.0f};
	float* m_grid = nullptr;
	float m_surface = 0.0f;
	
	void getCubeCornerOffsets(glm::vec3* positions) const;
	void getCubeSurfaceValues(const size_t index, float* values) const;
	int getCubeIndex(const size_t index, float* values) const;
};

