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
	
	IslandTerrain(glm::ivec3 size) : m_size{size} {
		m_grid = new int[size.x * size.y * size.z];

		for (int i = 0; i < size.x * size.y * size.z; ++i) {
			m_grid[i] = -10;
		}
	}

	~IslandTerrain() {
		delete[] m_grid;
	}

	void set(const glm::ivec3 pos, const int val) {
		const size_t index = (pos.x) + (pos.y * m_size.x) + (pos.z * m_size.x * m_size.y);
		m_grid[index] = val;
	}

	int get(const glm::ivec3 pos) const {
		const size_t index = (pos.x) + (pos.y * m_size.x) + (pos.z * m_size.x * m_size.y);
		return m_grid[index];
	}

	Vertex* polygonize() {
		Vertex* vertices = new Vertex[m_size.x * m_size.y * m_size.z];
		
		return vertices;
	}

private:
	glm::ivec3 m_size;
	int* m_grid = nullptr;
	int m_surface = 0;

	int getCubeIndex(const size_t index) const {
		int cubeIndex = 0;
		const size_t dx = 1;
		const size_t dy = m_size.x;
		const size_t dz = m_size.x * m_size.y;
		if (m_grid[index               ] > m_surface) cubeIndex |= 1;
		if (m_grid[index + dx          ] > m_surface) cubeIndex |= 2;
		if (m_grid[index + dx + dz     ] > m_surface) cubeIndex |= 4;
		if (m_grid[index + dz          ] > m_surface) cubeIndex |= 8;
		if (m_grid[index + dy          ] > m_surface) cubeIndex |= 16;
		if (m_grid[index + dx + dy     ] > m_surface) cubeIndex |= 32;
		if (m_grid[index + dx + dy + dz] > m_surface) cubeIndex |= 64;
		if (m_grid[index + dy + dz     ] > m_surface) cubeIndex |= 128;
		return cubeIndex;
	}
};

