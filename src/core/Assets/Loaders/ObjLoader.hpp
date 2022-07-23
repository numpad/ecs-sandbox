#pragma once

#include <string>
#include <filesystem>
#include <glm/glm.hpp>
#include "Assets/Mesh.hpp"

namespace fs = std::filesystem;

struct ObjLoaderMaterial {
	std::string name;
	glm::vec3 d;
};

struct ObjLoaderFace {
	int v[3];
	int vn[3];
	int vt[3];
	ObjLoaderMaterial* mtl = nullptr;
};


class ObjLoader {
public:

	static Mesh* load(const fs::path& path_to_obj_file);

private:

};
