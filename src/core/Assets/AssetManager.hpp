#pragma once

#include <config.hpp>

#include <string>
#include <tuple>
#include <map>
#include <unordered_map>
#include <vector>

#include <dr/dr_wav.h>
#include <sgl/sgl_audio.hpp>

#include <Assets/Texture.hpp>
#include <Assets/TiledTexture.hpp>
#include <Assets/Model.hpp>
#include <Assets/Mesh.hpp>

class AssetManager {
public:
	AssetManager() = default;
	AssetManager(const AssetManager& copy) = delete;
	~AssetManager();

	// preparing data
	bool preloadTexture(std::string path, Texture::Flags flags = Texture::Flags::NONE,
	                    Texture::WrapMode wrap_s = Texture::WrapMode::CLAMP,
	                    Texture::WrapMode wrap_t = Texture::WrapMode::CLAMP);

	// retrieving data
	Texture* getTexture(std::string path);
	TiledTexture* getTiledTexture(std::string path, unsigned int tiles_w, unsigned int tiles_h, int tile_x = 0,
	                              int tile_y = 0);

	Mesh* getMesh(std::string path);
	Model* getModel(std::string path);

	sgl::audio* getAudio(std::string path);

private:
	std::string base_path;

	std::unordered_map<std::string, Texture*> textures;
	std::map<std::tuple<std::string, unsigned int, unsigned int, int, int>, TiledTexture*> tiledtextures;
	std::unordered_map<std::string, Model*> models;
	std::unordered_map<std::string, Mesh*> meshes;
	std::unordered_map<std::string, sgl::audio*> audios;

	// loading data
	bool loadTexture(std::string path, Texture::Flags flags);
	bool loadTiledTexture(std::string path, unsigned int tiles_w, unsigned int tiles_h, int tile_x = 0, int tile_y = 0,
	                      Texture::Flags flags = Texture::Flags::NONE);
	bool loadMesh(std::string path);
	bool loadModel(std::string path);
	bool loadAudio(std::string path);

	// load different formats
	bool loadAudioWAV(std::string path);
};
