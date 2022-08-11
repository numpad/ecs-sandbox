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

	size_t getMaxVertexCount() const;
	void polygonize(Vertex* vertices, size_t& vertices_produced, const glm::vec3 scale = glm::vec3(1.0f));

private:
	glm::ivec3 m_size;
	float* m_grid = nullptr;
	float m_surface = 0.0f;
	
	void getCubeCornerOffsets(glm::vec3* positions, const glm::vec3 scale) const;
	void getCubeSurfaceValues(const size_t index, float* values) const;
	int getCubeIndex(const size_t index, float* values) const;

public:
	const glm::ivec3& size = m_size;
};

