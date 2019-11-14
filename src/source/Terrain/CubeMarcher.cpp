#include <Terrain/CubeMarcher.hpp>

using namespace glm;

CubeMarcher::CubeMarcher(const Terrain &terrain)
	: terrain(terrain)
{
	
}

void CubeMarcher::setSampleRange(float cube) {
	setSampleRange(vec3(-cube), vec3(cube));
}

void CubeMarcher::setSampleRange(vec3 min, vec3 max) {
	sampleRangeMin = min;
	sampleRangeMax = max;
}

void CubeMarcher::setSampleDetail(float marchingCubeSize) {
	stepscale = glm::abs(marchingCubeSize);
}

std::vector<vec3> CubeMarcher::polygonize() {
	std::vector<vec3> triangleVertices;
	
	// pixel perfect sample range takes marching cube size into account
	vec3 ppmin = sampleRangeMin + stepscale * 0.5f;
	vec3 ppmax = sampleRangeMax - stepscale * 0.5f;
	
	vec3 min = (ppmin) / stepscale,
	     max = (ppmax) / stepscale;
	for (float z = min.z; z <= max.z; ++z) {
		for (float y = min.y; y <= max.y; ++y) {
			for (float x = min.x; x <= max.x; ++x) {
				// sample cube around point
				vec3 sampleP = vec3(x, y, z) - 0.5f;
				polygonizeCube(sampleP * stepscale, triangleVertices);
			}
		}
	}
	
	return triangleVertices;
}


///////////////
//  PRIVATE  //
///////////////

CubeMarcher::Cell CubeMarcher::getCell(vec3 pos) {
	CubeMarcher::Cell cell;
	
	cell.points[0] = pos + vec3(0.f, 0.f, 0.f) * stepscale;
	cell.points[1] = pos + vec3(1.f, 0.f, 0.f) * stepscale;
	cell.points[2] = pos + vec3(1.f, 0.f, 1.f) * stepscale;
	cell.points[3] = pos + vec3(0.f, 0.f, 1.f) * stepscale;
	cell.points[4] = pos + vec3(0.f, 1.f, 0.f) * stepscale;
	cell.points[5] = pos + vec3(1.f, 1.f, 0.f) * stepscale;
	cell.points[6] = pos + vec3(1.f, 1.f, 1.f) * stepscale;
	cell.points[7] = pos + vec3(0.f, 1.f, 1.f) * stepscale;
	
	cell.values[0] = terrain.get(cell.points[0]);
	cell.values[1] = terrain.get(cell.points[1]);
	cell.values[2] = terrain.get(cell.points[2]);
	cell.values[3] = terrain.get(cell.points[3]);
	cell.values[4] = terrain.get(cell.points[4]);
	cell.values[5] = terrain.get(cell.points[5]);
	cell.values[6] = terrain.get(cell.points[6]);
	cell.values[7] = terrain.get(cell.points[7]);
	
	return cell;
}

int CubeMarcher::polygonizeCube(vec3 cellStart, std::vector<vec3> &triangleVertices) {
	CubeMarcher::Cell cell = getCell(cellStart);
	
	int cubeindex, edges;
	if (!getEdges(cell, &cubeindex, &edges)) return 0; // Cube/Cell is fully in/outside the surface
	
	constexpr int MAX_VERTICES = 12;
	vec3 vertices[MAX_VERTICES];
	
	if (edges &    1) vertices[ 0] = interpolate(cell.points[0], cell.points[1], cell.values[0], cell.values[1]);
	if (edges &    2) vertices[ 1] = interpolate(cell.points[1], cell.points[2], cell.values[1], cell.values[2]);
	if (edges &    4) vertices[ 2] = interpolate(cell.points[2], cell.points[3], cell.values[2], cell.values[3]);
	if (edges &    8) vertices[ 3] = interpolate(cell.points[3], cell.points[0], cell.values[3], cell.values[0]);
	if (edges &   16) vertices[ 4] = interpolate(cell.points[4], cell.points[5], cell.values[4], cell.values[5]);
	if (edges &   32) vertices[ 5] = interpolate(cell.points[5], cell.points[6], cell.values[5], cell.values[6]);
	if (edges &   64) vertices[ 6] = interpolate(cell.points[6], cell.points[7], cell.values[6], cell.values[7]);
	if (edges &  128) vertices[ 7] = interpolate(cell.points[7], cell.points[4], cell.values[7], cell.values[4]);
	if (edges &  256) vertices[ 8] = interpolate(cell.points[0], cell.points[4], cell.values[0], cell.values[4]);
	if (edges &  512) vertices[ 9] = interpolate(cell.points[1], cell.points[5], cell.values[1], cell.values[5]);
	if (edges & 1024) vertices[10] = interpolate(cell.points[2], cell.points[6], cell.values[2], cell.values[6]);
	if (edges & 2048) vertices[11] = interpolate(cell.points[3], cell.points[7], cell.values[3], cell.values[7]);
	
	int tricount = 0;
	for (int i = 0; TRIANGLE_TABLE[cubeindex][i] != -1; i += 3, ++tricount) {
		 triangleVertices.push_back(vertices[TRIANGLE_TABLE[cubeindex][i    ]]);
		 triangleVertices.push_back(vertices[TRIANGLE_TABLE[cubeindex][i + 1]]);
		 triangleVertices.push_back(vertices[TRIANGLE_TABLE[cubeindex][i + 2]]);
		 ++tricount;
	}
	
	return tricount;
}

bool CubeMarcher::getEdges(Cell cell, int *cubeindex, int *edges) {
	/*
	 * Determine the index into the edge table which
	 * tells us which vertices are inside of the surface
	 */
	*cubeindex = 0;
	if (cell.values[0] < isolevel) *cubeindex |=   1;
	if (cell.values[1] < isolevel) *cubeindex |=   2;
	if (cell.values[2] < isolevel) *cubeindex |=   4;
	if (cell.values[3] < isolevel) *cubeindex |=   8;
	if (cell.values[4] < isolevel) *cubeindex |=  16;
	if (cell.values[5] < isolevel) *cubeindex |=  32;
	if (cell.values[6] < isolevel) *cubeindex |=  64;
	if (cell.values[7] < isolevel) *cubeindex |= 128;
	
	/* Cube is entirely in/out of the surface */
	if (CubeMarcher::EDGE_TABLE[*cubeindex] == 0)
		return false;
	
	*edges = CubeMarcher::EDGE_TABLE[*cubeindex];
	return true;
}

bool vec4LessThan(const vec4 &left, const vec4 &right) {
	if (left.x < right.x)		return true;
	else if (left.x > right.x)	return false;
	if (left.y < right.y)		return true;
	else if (left.y > right.y)	return false;
	if (left.z < right.z)		return true;
	else if (left.z > right.z)	return false;
	return false;
}

vec3 LinearInterp(vec4 p1, vec4 p2, float value) {
	if (vec4LessThan(p2, p1)) {
		vec4 temp = p1;
		p1 = p2;
		p2 = temp;    
	}

	vec3 p;
	if (glm::abs(p1.w - p2.w) > 0.00001)
		p = vec3(p1) + (vec3(p2) - vec3(p1)) / (p2.w - p1.w) * (value - p1.w);
	else 
		p = vec3(p1 + (p2 - p1) * 0.5f);
	return p;
}


vec3 CubeMarcher::interpolate(vec3 p1, vec3 p2, float v1, float v2) {
	// no interpolation:
	return LinearInterp(vec4(p1, v1), vec4(p2, v2), isolevel);
	//vec3 phd = (p2 - p1) * .5f;
	//return p1 + phd;
	
	constexpr float EPSILON = 0.000001f;
	vec3 p;
	
	if (glm::abs(isolevel - v1) < EPSILON)
		return p1;
	if (glm::abs(isolevel - v2) < EPSILON)
		return p2;
	if (glm::abs(v1 - v2) < EPSILON)
		return p1;
	
	float mu = (isolevel - v1) / (v2 - v1);
	p.x = p1.x + mu * (p2.x - p1.x);
	p.y = p1.y + mu * (p2.y - p1.y);
	p.z = p1.z + mu * (p2.z - p1.z);

	return p;
}
