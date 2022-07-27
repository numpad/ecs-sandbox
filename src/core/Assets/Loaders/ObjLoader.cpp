#include "Assets/Loaders/ObjLoader.hpp"
#include "Assets/Vertex.hpp"
#include <algorithm>
#include <filesystem>
#include <fmt/color.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <fmt/core.h>

static bool loadMtlLib(std::vector<ObjLoaderMaterial>& materials, const fs::path& path_to_mtl_file) {
	std::ifstream file(path_to_mtl_file);
	if (!file.is_open()) {
		return false;
	}

	for (std::string line; std::getline(file, line);) {
		std::istringstream iss(line);

		std::string keyword;
		iss >> keyword;
		if (keyword == "newmtl") {
			materials.push_back(ObjLoaderMaterial());
			std::string name;
			iss >> name;
			materials.back().name = name;
		} else if (keyword == "Kd") {
			float d[3];
			iss >> d[0] >> d[1] >> d[2];
			materials.back().d = glm::vec3(d[0], d[1], d[2]);
		}
	}

	return true;
}

Mesh* ObjLoader::load(const fs::path& path_to_obj_file) {
	std::ifstream file(path_to_obj_file.string());
	if (!file.is_open()) {
		fmt::print(fmt::fg(fmt::terminal_color::yellow), "[WARN] Could not open '{}'.\n", path_to_obj_file.string());
		return nullptr;
	}

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords;
	std::vector<ObjLoaderFace> faces;
	std::vector<ObjLoaderMaterial> materials;
	materials.push_back(ObjLoaderMaterial());
	materials.back().name = "_default";
	materials.back().d = glm::vec3(1.0f);

	size_t currentMaterial = 0;
	for (std::string line; std::getline(file, line);) {
		if (line.empty() || line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
			continue;
		}

		std::istringstream iss(line);
		std::string keyword;
		iss >> keyword;
		
		if (keyword == "v") {
			float x, y, z;
			iss >> x >> y >> z;
			positions.emplace_back(x, y, z);
		} else if (keyword == "vn") {
			float x, y, z;
			iss >> x >> y >> z;
			normals.emplace_back(x, y, z);
		} else if (keyword == "vt") {
			float u, v;
			iss >> u >> v;
			texcoords.emplace_back(u, v);
		} else if (keyword == "f") {
			std::string fs[3];
			iss >> fs[0] >> fs[1] >> fs[2];

			ObjLoaderFace face;
			for (int i = 0; i < 3; ++i) {
				sscanf(fs[i].c_str(), "%d/%d/%d", &face.v[i], &face.vt[i], &face.vn[i]);
			}
			face.mtl = &materials.at(currentMaterial);
			faces.push_back(face);
		} else if (keyword == "mtllib") {
			std::string name;
			iss >> name;

			fs::path pathToMtl = path_to_obj_file;
			pathToMtl.remove_filename();
			pathToMtl.append(name);
			const bool loaded = loadMtlLib(materials, pathToMtl);
			if (!loaded) {
				return nullptr;
			}
		} else if (keyword == "usemtl") {
			std::string name;
			iss >> name;

			auto result = std::find_if(materials.begin(), materials.end(), [name](const auto& m) {
				return (m.name == name);
			});

			if (result == materials.end()) {
				currentMaterial = 0;
				fmt::print(fmt::fg(fmt::terminal_color::yellow), "[WARN] Material '{}' is not defined in '{}'.\n", name,
				           path_to_obj_file.string());
			} else {
				currentMaterial = std::distance(materials.begin(), result);
			}
		} else {
			fmt::print(fmt::fg(fmt::terminal_color::yellow),
			           "[WARN] Encountered unknown keyword '{}' while parsing '{}'.\n", keyword,
			           path_to_obj_file.string());
		}
	}

	/*
	fmt::print("Positions: {}\n", positions.size());
	fmt::print("Normals: {}\n", normals.size());
	fmt::print("TexCoords: {}\n", texcoords.size());
	fmt::print("Faces: {}\n", faces.size());
	fmt::print("Materials: {}\n", materials.size());
	for (ObjLoaderMaterial& mat : materials) {
	    fmt::print(" - {}\n", mat.name);
	}
	*/

	std::vector<Vertex> vertices;
	vertices.reserve(faces.size() * 3);
	for (const ObjLoaderFace& face : faces) {
		for (int i = 0; i < 3; ++i) {
			Vertex v;
			v.position = positions.at(face.v[i] - 1);
			v.normal = normals.at(face.vn[i] - 1);
			v.texcoords = texcoords.at(face.vt[i] - 1);
			if (face.mtl != nullptr) {
				v.color = face.mtl->d;
			}
			vertices.push_back(v);
		}
	}

	return new Mesh(vertices);
}
