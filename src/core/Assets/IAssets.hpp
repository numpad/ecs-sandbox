#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
namespace fs = std::filesystem;

#include <fmt/core.h>
#include <fmt/color.h>
#include "Assets/Texture.hpp"

class IAssets {
public:
	IAssets(const fs::path& rootDir) : m_rootDir{rootDir} {
	}

	template <typename T>
	T* load(const fs::path& relativePath) = delete;

	template <>
	Texture* load<Texture>(const fs::path& relativePath) {
		auto texture = m_textures.find(relativePath);
		// return asset if it exists
		if (texture != m_textures.end())
			return texture->second;

		// …otherwise load it
		fs::path p = m_rootDir;
		p += relativePath;
		if (!fs::exists(p)) {
			fmt::print(fmt::fg(fmt::terminal_color::yellow), "[WRN] the file '{}' does not exist!\n", "TODO");
			return nullptr;
		}

		return new Texture();
	}

	template <typename T>
	void unload(const fs::path& relativePath = "") = delete;

	template <>
	void unload<Texture>(const fs::path& relativePath) {
		// if relativePath is not set, remove all
		if (relativePath.empty()) {
			for (const auto& [path, texture] : m_textures) {
				unload<Texture>(path);
			}

			return;
		}
	}

private:
	const fs::path& m_rootDir;

	std::unordered_map<fs::path, Texture*> m_textures;
};
