#pragma once

#include <config.hpp>

#include <string>
#include <tuple>
#include <map>
#include <unordered_map>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Assets/Texture.hpp>
#include <Assets/TiledTexture.hpp>
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
	TiledTexture *getTiledTexture(std::string path,
		unsigned int tiles_w, unsigned int tiles_h, int tile_x = 0, int tile_y = 0);
	
	Model *getModel(std::string path);
	
	
private:
	std::string base_path;
		
	std::unordered_map<std::string, Texture *> textures;
	std::map<std::tuple<std::string, unsigned int, unsigned int, int, int>, TiledTexture *> tiledtextures;
	std::unordered_map<std::string, Model *> models;
	std::vector<Mesh *> meshes;
	
	// loading data
	bool loadTexture(std::string path, Texture::Flags flags);
	bool loadTiledTexture(std::string path,
		unsigned int tiles_w, unsigned int tiles_h, int tile_x = 0, int tile_y = 0,
		Texture::Flags flags = Texture::Flags::NONE);
	bool loadModel(std::string path);
	
	Mesh *collectMesh(Mesh *m);
	
	void collectMeshes(Model &model, aiNode *node, const aiScene *scene);
	Mesh *convertAndCollectMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture *> convertAndCollectMaterialTextures(
		aiMaterial *mat, aiTextureType type, Texture::UsageType usage);
};
