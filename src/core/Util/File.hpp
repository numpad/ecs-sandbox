#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

class File {
public:
	static std::string Read(std::string filename);
	static std::vector<std::string> ListDirectory(const fs::path& dir);

private:
};
