#include "scenes/gamemode/raft/IslandTerrain.hpp"
#include "Util/Math3d.hpp"

////////////
// INLINE //
////////////

static inline size_t ptoi(const glm::ivec3 p, const glm::ivec3 size) {
	return (p.x) + (p.y * size.x) + (p.z * size.x * size.y);
}

static inline glm::vec3 interpolate(const glm::vec3 p1, const glm::vec3 p2, float v1, float v2, const float surface) {
	return p1 + (surface - v1) * ((p2 - p1) / (v2 - v1));
}

////////////
// PUBLIC //
////////////

IslandTerrain::IslandTerrain(glm::ivec3 size) : m_size{size} {
	m_grid = new float[size.x * size.y * size.z];

	for (int i = 0; i < size.x * size.y * size.z; ++i) {
		m_grid[i] = rand() % 20 - 10;
	}
}

IslandTerrain::~IslandTerrain() {
	delete[] m_grid;
}

void IslandTerrain::set(const glm::ivec3 pos, const float val) {
	const size_t index = ptoi(pos, m_size);
	m_grid[index] = val;
}

float IslandTerrain::get(const glm::ivec3 pos) const {
	const size_t index = ptoi(pos, m_size);
	return m_grid[index];
}

Vertex* IslandTerrain::polygonize() {
	Vertex* vertices = new Vertex[m_size.x * m_size.y * m_size.z * 8 * 12];
	size_t vertices_produced = -1;
	
	glm::vec3 cornerOffsets[8];
	getCubeCornerOffsets(cornerOffsets);

	for (int x = 0; x < m_size.x - 1; ++x) {
		for (int y = 0; y < m_size.y - 1; ++y) {
			for (int z = 0; z < m_size.z - 1; ++z) {
				const glm::ivec3 p(x, y, z);
				glm::vec3 pf(static_cast<float>(x) * m_scale.x, static_cast<float>(y) * m_scale.y, static_cast<float>(z) * m_scale.z);
				const size_t i = ptoi(p, m_size);
				
				float surfaceValues[8];
				getCubeSurfaceValues(i, surfaceValues);
				const int cubeIndex = getCubeIndex(i, surfaceValues);
				const int edges = EDGE_TABLE[cubeIndex];
				if (edges == 0) continue;
				
				glm::vec3 positions[12];
				if (edges &    1) positions[ 0] = interpolate(pf + cornerOffsets[0], pf + cornerOffsets[1], surfaceValues[0], surfaceValues[1], m_surface);
				if (edges &    2) positions[ 1] = interpolate(pf + cornerOffsets[1], pf + cornerOffsets[2], surfaceValues[1], surfaceValues[2], m_surface);
				if (edges &    4) positions[ 2] = interpolate(pf + cornerOffsets[2], pf + cornerOffsets[3], surfaceValues[2], surfaceValues[3], m_surface);
				if (edges &    8) positions[ 3] = interpolate(pf + cornerOffsets[3], pf + cornerOffsets[0], surfaceValues[3], surfaceValues[0], m_surface);
				if (edges &   16) positions[ 4] = interpolate(pf + cornerOffsets[4], pf + cornerOffsets[5], surfaceValues[4], surfaceValues[5], m_surface);
				if (edges &   32) positions[ 5] = interpolate(pf + cornerOffsets[5], pf + cornerOffsets[6], surfaceValues[5], surfaceValues[6], m_surface);
				if (edges &   64) positions[ 6] = interpolate(pf + cornerOffsets[6], pf + cornerOffsets[7], surfaceValues[6], surfaceValues[7], m_surface);
				if (edges &  128) positions[ 7] = interpolate(pf + cornerOffsets[7], pf + cornerOffsets[4], surfaceValues[7], surfaceValues[4], m_surface);
				if (edges &  256) positions[ 8] = interpolate(pf + cornerOffsets[0], pf + cornerOffsets[4], surfaceValues[0], surfaceValues[4], m_surface);
				if (edges &  512) positions[ 9] = interpolate(pf + cornerOffsets[1], pf + cornerOffsets[5], surfaceValues[1], surfaceValues[5], m_surface);
				if (edges & 1024) positions[10] = interpolate(pf + cornerOffsets[2], pf + cornerOffsets[6], surfaceValues[2], surfaceValues[6], m_surface);
				if (edges & 2048) positions[11] = interpolate(pf + cornerOffsets[3], pf + cornerOffsets[7], surfaceValues[3], surfaceValues[7], m_surface);

				const int* triangles = TRIANGLE_TABLE[cubeIndex];
				for (int i = 0; triangles[i] != -1; i += 3) {
					glm::vec3 facePositions[3] = {positions[triangles[i]], positions[triangles[i+1]], positions[triangles[i+2]]};
					glm::vec3 normal = m3d::triangleCalcNormal(facePositions);

					for (int j = 0; j < 3; ++j) {
						Vertex v;
						v.position = facePositions[0];
						v.normal = normal;
						
						vertices[++vertices_produced] = v;
					}
				}
			}
		}
	}

	return vertices;
}

/////////////
// PRIVATE //
/////////////

void IslandTerrain::getCubeCornerOffsets(glm::vec3* positions) const {
	positions[0] = glm::vec3(     0.0f,       0.0f,       0.0f);
	positions[1] = glm::vec3(m_scale.x,       0.0f,       0.0f);
	positions[2] = glm::vec3(m_scale.x,       0.0f,  m_scale.z);
	positions[3] = glm::vec3(     0.0f,       0.0f,  m_scale.z);
	positions[4] = glm::vec3(     0.0f,  m_scale.y,       0.0f);
	positions[5] = glm::vec3(m_scale.x,  m_scale.y,       0.0f);
	positions[6] = glm::vec3(m_scale.x,  m_scale.y,  m_scale.z);
	positions[7] = glm::vec3(     0.0f,  m_scale.y,  m_scale.z);
}

void IslandTerrain::getCubeSurfaceValues(const size_t index, float* values) const {
	const size_t dx = 1;
	const size_t dy = m_size.x;
	const size_t dz = m_size.x * m_size.y;
	values[0] = m_grid[index               ];
	values[1] = m_grid[index + dx          ];
	values[2] = m_grid[index + dx + dz     ];
	values[3] = m_grid[index + dz          ];
	values[4] = m_grid[index + dy          ];
	values[5] = m_grid[index + dx + dy     ];
	values[6] = m_grid[index + dx + dy + dz];
	values[7] = m_grid[index + dy + dz     ];
}

int IslandTerrain::getCubeIndex(const size_t index, float* surfaceValues) const {
	int cubeIndex = 0;
	if (surfaceValues[0] < m_surface) cubeIndex |=   1;
	if (surfaceValues[1] < m_surface) cubeIndex |=   2;
	if (surfaceValues[2] < m_surface) cubeIndex |=   4;
	if (surfaceValues[3] < m_surface) cubeIndex |=   8;
	if (surfaceValues[4] < m_surface) cubeIndex |=  16;
	if (surfaceValues[5] < m_surface) cubeIndex |=  32;
	if (surfaceValues[6] < m_surface) cubeIndex |=  64;
	if (surfaceValues[7] < m_surface) cubeIndex |= 128;
	return cubeIndex;
}


