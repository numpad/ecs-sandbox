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
