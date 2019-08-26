#include <Assets/AssetManager.hpp>

AssetManager::AssetManager(std::string base_path)
	: base_path(base_path)
{
	
}

AssetManager::~AssetManager() {
	
	int freedTextures = 0;
	for (auto iter : textures) {
		++freedTextures;
		delete iter.second;
	}

#if CFG_DEBUG
	printf("[LOG] AssetManager: freed %d textures.", freedTextures);
#endif
}


bool AssetManager::loadTexture(std::string path) {
	auto texture = textures.find(path);
	
	// texture already loaded
	// todo: reload from path
	if (texture != textures.end()) return false;
	
	Texture *tex = new Texture();
	bool is_loaded = tex->loadImage(base_path + path);
	
	if (is_loaded) {
		textures.insert({path, tex});
		return true;
	}
	
	return false;
}

Texture *AssetManager::getTexture(std::string path) {
	auto texture = textures.find(path);
	if (texture == textures.end()) {
		bool is_loaded = loadTexture(path);
		if (!is_loaded) return nullptr;
	}
	
	return textures.at(path);
}
