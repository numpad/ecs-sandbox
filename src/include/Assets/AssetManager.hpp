#pragma once

#include <config.hpp>
#include <string>
#include <unordered_map>

#include <Util/Texture.hpp>

class AssetManager {
public:
	
	AssetManager(std::string base_path);
	AssetManager(const AssetManager &copy) = delete;
	~AssetManager();
	
	bool loadTexture(std::string path);
	
	Texture *getTexture(std::string path);
	
private:
	std::string base_path;
	
	std::unordered_map<std::string, Texture *> textures;
	
};
