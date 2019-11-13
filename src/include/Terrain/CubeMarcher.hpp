#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <Terrain/Terrain.hpp>

using namespace glm;

class CubeMarcher {
public:
	struct Cell {
		vec3 points[8];
		float values[8];
	};
	
	CubeMarcher(const Terrain &terrain);
	
	std::vector<vec3> polygonize();
	
private:
	float stepscale = 0.2f;
	const Terrain &terrain;
	float isolevel = 0.0f;
	
	CubeMarcher::Cell getCell(vec3 pos);
	
	// http://paulbourke.net/geometry/polygonise/
	int polygonizeCube(vec3 cellStart, std::vector<vec3> &triangleVertices);
	bool getEdges(Cell start, int *cubeindex, int *edges);
	vec3 interpolate(vec3 p1, vec3 p2, float valp1, float valp2);
	
	static constexpr int EDGE_TABLE[256] = {
		#include <Terrain/EdgeTable.txt>
	};
	static constexpr int TRIANGLE_TABLE[256][16] = {
		#include <Terrain/TriangleTable.txt>
	};
	
};

