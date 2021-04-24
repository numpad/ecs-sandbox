#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <Assets/Vertex.hpp>
#include <Terrain/Terrain.hpp>
#include <ImprovedTerrain/ISignedDistanceFunction.hpp>
#include <ImprovedTerrain/ISignedDistanceBody.hpp>
#include <Util/Math3d.hpp>

using namespace glm;

class CubeMarcher {
public:
	struct Cell {
		vec3 points[8];
		float values[8];
	};
	
	CubeMarcher();
	
	void setSampleRange(float cube);
	void setSampleRange(vec3 min, vec3 max);
	void setSampleDetail(float marchingCubeSize);
	
	std::vector<Vertex> polygonize(const Terrain &terrain);
	std::vector<Vertex> polygonize(const ISignedDistanceFunction &sdf);
	std::vector<Vertex> polygonize(const ISignedDistanceBody &sdb, const float stepscale);
	
private:
	vec3 sampleRangeMin = vec3(-1.f);
	vec3 sampleRangeMax = vec3(1.f);
	float stepscale = 0.2f; // size of cubes to sample, smaller = finer
	float isolevel = 0.0f;
	
	// http://paulbourke.net/geometry/polygonise/
	CubeMarcher::Cell getCell(const Terrain &terrain, vec3 pos);
	CubeMarcher::Cell getCell(const ISignedDistanceFunction &sdf, vec3 pos);
	int polygonizeCube(const Terrain &terrain, vec3 cellStart, std::vector<Vertex> &triangleVertices);
	int polygonizeCube(const ISignedDistanceFunction &sdf, vec3 cellStart, std::vector<Vertex> &triangleVertices);
	bool getEdges(Cell start, int *cubeindex, int *edges);
	vec3 interpolate(vec3 p1, vec3 p2, float valp1, float valp2);
	
	static constexpr int EDGE_TABLE[256] = {
		#include <Terrain/EdgeTable.txt>
	};
	static constexpr int TRIANGLE_TABLE[256][16] = {
		#include <Terrain/TriangleTable.txt>
	};
	
};

