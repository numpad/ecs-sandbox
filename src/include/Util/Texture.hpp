#pragma once

#include <string>
#include <GL/gl3w.h>
#include <stb/stb_image.h>

class Texture {
public:
	
	Texture() = default;
	Texture(const Texture &copy) = delete;
	~Texture();
	
	// read only data
	inline int getWidth() { return width; }
	inline int getHeight() { return height; }
	inline int getChannels() { return nChannels; }
	
	// opengl data accessors
	inline GLuint getTexture() const { return texture; }
	inline operator GLuint() const { return getTexture(); };
	
	// loading data
	bool loadMemory(unsigned char *data, int width, int height, int channels = 0);
	bool loadImage(std::string path);
	
	// destroying data
	void destroy();
	
	// using texture
	void bind();
	void unbind();
	
private:
	
	void create();
	
	int width, height, nChannels;
	
	GLuint texture = 0;
	GLint prevBoundTexture = 0;
};

