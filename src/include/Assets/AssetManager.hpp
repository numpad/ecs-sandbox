#pragma once

#include <config.hpp>

#include <string>
#include <unordered_map>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Assets/Texture.hpp>
#include <Assets/Model.hpp>
#include <Assets/Mesh.hpp>

class AssetManager {
public:
	
	AssetManager() = default;
	AssetManager(const AssetManager &copy) = delete;
	~AssetManager();
	
	// preparing data
	bool preloadTexture(std::string path,
		Texture::Flags flags = Texture::Flags::NONE,
		Texture::WrapMode wrap_s = Texture::WrapMode::CLAMP,
		Texture::WrapMode wrap_t = Texture::WrapMode::CLAMP);
	
	// retrieving data
	Texture *getTexture(std::string path);
	Model *getModel(std::string path);
	
	
private:
	std::string base_path;
		
	std::unordered_map<std::string, Texture *> textures;
	std::unordered_map<std::string, Model *> models;
	std::vector<Mesh *> meshes;
	
	// loading data
	bool loadTexture(std::string path, Texture::Flags flags);
	bool loadModel(std::string path);
	
	Mesh *collectMesh(Mesh *m);
	
	void collectMeshes(Model &model, aiNode *node, const aiScene *scene);
	Mesh *convertAndCollectMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture *> convertAndCollectMaterialTextures(
		aiMaterial *mat, aiTextureType type, Texture::UsageType usage);
};
