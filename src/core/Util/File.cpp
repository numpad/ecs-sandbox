#include <Util/File.hpp>

std::string File::Read(std::string filename) {
	std::string buffer;

	std::ifstream is(filename);
	is.seekg(0, std::ios::end);
	buffer.resize(is.tellg());
	is.seekg(0);
	is.read(buffer.data(), buffer.size());

	return buffer;
}

std::vector<std::string> File::ListDirectory(const fs::path& dir) {
	std::vector<std::string> files;

	for (const std::filesystem::directory_entry& file : std::filesystem::directory_iterator(dir)) {
		const std::string filename = file.path().filename().string();
		files.push_back(filename);
	}

	return files;
}
