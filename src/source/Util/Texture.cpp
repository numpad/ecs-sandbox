#include <Util/Texture.hpp>

static GLenum channelsToEnum(int channels) {
	switch (channels) {
		case 1: return GL_RED;
		case 2: return GL_RG;
		case 3: return GL_RGB;
		case 4: return GL_RGBA;
		default: break;	
	};
	return GL_RGBA;
}

Texture::~Texture() {
	this->destroy();
}

bool Texture::loadImage(std::string path) {
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nChannels, 0);
	if (!data) return false;
	
	bool result = loadMemory(data, width, height, nChannels);
	
	stbi_image_free(data);
	return result;
}

bool Texture::loadMemory(unsigned char *data, int width, int height, int channels) {
	this->create();
	
	this->bind();
	glTexImage2D(GL_TEXTURE_2D, 0, channelsToEnum(channels), width, height, 0,
		channelsToEnum(channels), GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	this->unbind();
	
	return true;
}

void Texture::bind() {
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevBoundTexture);
	glBindTexture(GL_TEXTURE_2D, texture);
}

void Texture::unbind() {
	glBindTexture(GL_TEXTURE_2D, prevBoundTexture);
}

void Texture::destroy() {
	glDeleteTextures(1, &texture);
}

/////////////
// PRIVATE //
/////////////

void Texture::create() {
	glGenTextures(1, &texture);
	
	this->bind();
	
	// float borderColor[] = {1.0f, 1.0f, 1.0f, 0.0f};
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	this->unbind();
}
