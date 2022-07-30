#include "Assets/Loaders/ObjLoader.hpp"
#include <Assets/AssetManager.hpp>
#include <fmt/color.h>
#include <fstream>
#include <sstream>
#include <stdlib.h>

AssetManager::~AssetManager() {

	int freedTextures = 0;
	for (auto iter : textures) {
		++freedTextures;
		iter.second->destroy();
		delete iter.second;
		iter.second = nullptr;
	}

	int freedTiledTextures = 0;
	for (auto iter : tiledtextures) {
		++freedTiledTextures;
		// no `iter.second->destroy();` since we destroyed it already as Texture*
		delete iter.second;
		iter.second = nullptr;
	}

	int freedModels = 0;
	for (auto iter : models) {
		++freedModels;
		delete iter.second;
		iter.second = nullptr;
	}

	int freedMeshes = 0;
	for (auto iter : meshes) {
		++freedMeshes;
		delete iter.second;
		iter.second = nullptr;
	}

	int freedAudios = 0;
	for (auto iter : audios) {
		++freedAudios;
		delete iter.second;
		iter.second = nullptr;
	}

#ifndef NDEBUG
	printf("[LOG] AssetManager: freed %d textures.\n", freedTextures);
	printf("[LOG] AssetManager: freed %d tiledtextures.\n", freedTiledTextures);
	printf("[LOG] AssetManager: freed %d models.\n", freedModels);
	printf("[LOG] AssetManager: freed %d meshes.\n", freedMeshes);
	printf("[LOG] AssetManager: freed %d audios.\n", freedAudios);
#endif
}

// texture loading

bool AssetManager::preloadTexture(std::string path, Texture::Flags flags, Texture::WrapMode wrap_s,
                                  Texture::WrapMode wrap_t) {
	bool loaded = this->loadTexture(path, flags);
	if (!loaded)
		return false;

	getTexture(path)->setWrapMode(wrap_s, wrap_t);

	return true;
}

bool AssetManager::loadTexture(std::string path, Texture::Flags flags) {
	auto texture = textures.find(path);

	// texture already loaded
	// todo: reload from path
	// todo: check if this texture is already loaded with the same flags,
	//       if not: create new Texture*
	if (texture != textures.end()) {
#ifndef NDEBUG
		printf("[LOG] AssetManager: texture already loaded... skipping '%s'\n", (path).c_str());
#endif
		return false;
	}

	Texture* tex = new Texture(flags);
	bool is_loaded = tex->loadImage(path);

	if (is_loaded) {
#ifndef NDEBUG
		printf("[LOG] AssetManager: loaded texture '%s'\n", (path).c_str());
#endif

		textures.insert({path, tex});
		return true;
	} else {
#ifndef NDEBUG
		printf("[ERR] AssetManager: failed loading texture '%s'\n", (path).c_str());
#endif
	}

	return false;
}

Texture* AssetManager::getTexture(std::string path) {
	auto texture = textures.find(path);
	if (texture == textures.end()) {
		bool is_loaded = loadTexture(path, Texture::Flags::NONE);
		if (!is_loaded) {
#ifndef NDEBUG
			printf("[WARN] AssetManager: getTexture(path) could not load:\n  path = \"%s\"!\n", path.c_str());
#endif
			return nullptr;
		}
	}

	return textures.at(path);
}

TiledTexture* AssetManager::getTiledTexture(std::string path, unsigned int tiles_w, unsigned int tiles_h, int tile_x,
                                            int tile_y) {
	auto tile = tiledtextures.find(std::make_tuple(path, tiles_w, tiles_h, tile_x, tile_y));
	if (tile == tiledtextures.end()) {
		bool is_loaded = loadTiledTexture(path, tiles_w, tiles_h, tile_x, tile_y, Texture::Flags::NONE);
		if (!is_loaded) {
#ifndef NDEBUG
			printf("[WARN] AssetManager: getTiledTexture(path) could not load:\n  path = \"%s\"!\n", path.c_str());
#endif
			return nullptr;
		}
	}

	return tiledtextures.at(std::make_tuple(path, tiles_w, tiles_h, tile_x, tile_y));
}

bool AssetManager::loadTiledTexture(std::string path, unsigned int tiles_w, unsigned int tiles_h, int tile_x,
                                    int tile_y, Texture::Flags flags) {
	Texture* texture = getTexture(path);
	if (texture == nullptr) {
#ifndef NDEBUG
		printf("[WARN] AssetManager: loadTiledTexture(path) failed\n");
#endif
		return false;
	}

	TiledTexture* tt = new TiledTexture(tiles_w, tiles_h, flags);
	if (!tt->loadTexture(*texture)) {
#ifndef NDEBUG
		printf("[WARN] AssetManager: loadTiledTexture(path) TiledTexture::loadTexture(const Texture &copy) failed!\n");
#endif
		return false;
	}
	tt->setTile(tile_x, tile_y);

	tiledtextures.insert({std::make_tuple(path, tiles_w, tiles_h, tile_x, tile_y), tt});
	return true;
}

// model loading
Mesh* AssetManager::getMesh(std::string path) {
	auto mesh = meshes.find(path);
	if (mesh == meshes.end()) {
		bool is_loaded = loadMesh(path);
		if (!is_loaded) {
#ifndef NDEBUG
			printf("[WARN] AssetManager: getMesh(path) could not load:\n  path = \"%s\"!\n", path.c_str());
#endif
			return nullptr;
		}
	}

	return meshes.at(path);
}

bool AssetManager::loadMesh(std::string path) {
	Mesh* loadedMesh = ObjLoader::load(path);
	if (loadedMesh == nullptr) {
		return false;
	}

	meshes[path] = loadedMesh;
	return true;
}

bool AssetManager::loadModel(std::string path) {
}

// audio loading

bool AssetManager::loadAudio(std::string path) {
	// TODO: check for filetype and decide which loader to use
	return loadAudioWAV(path);
}

sgl::audio* AssetManager::getAudio(std::string path) {
	auto audio = audios.find(path);
	if (audio == audios.end()) {
		bool is_loaded = loadAudio(path);
		if (!is_loaded) {
#ifndef NDEBUG
			printf("[WARN] AssetManager: getAudio(path) could not load:\n  path = \"%s\"!\n", path.c_str());
#endif
			return nullptr;
		}
	}

	return audios.at(path);
}

bool AssetManager::loadAudioWAV(std::string path) {
	unsigned int channels;
	unsigned int sampleRate;
	drwav_uint64 totalPCMFrameCount;
	short* pSampleData =
	    drwav_open_file_and_read_pcm_frames_s16(path.c_str(), &channels, &sampleRate, &totalPCMFrameCount, nullptr);
	if (pSampleData == nullptr) {
		std::cout << "[ERR] DRWAV: Could not read file '" << path << "'." << std::endl;
		return false;
	}

	// TODO: add support for 8 bit audio files.
	sgl::audio::format audio_fmt;
	switch (channels) {
	case 1:
		audio_fmt = sgl::audio::format::mono16;
		break;
	case 2:
		audio_fmt = sgl::audio::format::stereo16;
		break;
	};

	sgl::audio* audio = new sgl::audio;
	audio->load(audio_fmt, sampleRate, totalPCMFrameCount * channels * sizeof(short), pSampleData);
	drwav_free(pSampleData, nullptr);

	audios[path] = audio;
	return true;
}
