#pragma once

#include <memory>
#include <vector>
#include <tuple>
#include <glm/glm.hpp>
#include <Terrain/Terrain.hpp>

using std::unique_ptr;
using namespace glm;

class SignedDistTerrain : public Terrain {
	enum class Op { ADD, SUB };
	struct Body {
		virtual float distance(vec3 sampleP) = 0;
	};
	struct Sphere : Body {
		vec3 p;
		float r;
		Sphere(vec3 p, float r) : p(p), r(r) {};
		float distance (vec3 sampleP) { return glm::distance(p, sampleP) - r; }
	};
public:
	
	void addSphere(vec3 p, float r);
	void subSphere(vec3 p, float r);
	
protected:
	
	float sampleValue(vec3 pos) const;
	
private:
	std::vector<unique_ptr<Body>> bodies;
	std::vector<Op> operations;
};
