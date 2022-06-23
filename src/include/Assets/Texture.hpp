#pragma once

#include <config.hpp>
#include <string>
#include <GL/gl3w.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>

class Texture {
public:
	enum class WrapMode {
		REPEAT = GL_REPEAT,
		MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
		CLAMP = GL_CLAMP_TO_EDGE,
		BORDER = GL_CLAMP_TO_BORDER
	};

	enum class UsageType {
		DIFFUSE,
		SPECULAR,
		NORMAL
	};

	// construction flags
	enum Flags {
		NONE = 1 << 0,
		GEN_MIPMAPS = 1 << 1,
		NO_VERTICAL_FLIP = 1 << 2
	};

	enum Flip {
		ORIGINAL = 1 << 0,
		HORIZONTAL = 1 << 1,
		VERTICAL = 1 << 2
	};

public:
	Texture(Texture::Flags flags = Flags::NONE);
	Texture(const Texture& copy);
	virtual ~Texture();

	// read only data
	inline int getWidth() const {
		return width;
	}
	inline int getHeight() const {
		return height;
	}
	inline int getChannels() const {
		return nChannels;
	}

	// subrect
	glm::vec4 getSubRect() const;
	virtual void resetSubRect();
	void flipSubRect(Flip flip);

	// convenience
	inline float getAspectRatio() const {
		return float(height) / float(width);
	}
	glm::vec2 getNormalizedPixelSize() const;

	// opengl data accessors
	inline GLuint getTexture() const {
		return texture;
	}
	inline operator GLuint() const {
		return getTexture();
	};

	// loading data
	bool loadMemory(unsigned char* data, int width, int height, int channels = 0);
	bool loadImage(std::string path);
	bool loadTexture(const Texture& texture);

	// destroying data
	void destroy();

	// using texture
	void bind();
	void unbind();

	// configuring data
	void setWrapMode(WrapMode s, WrapMode t);
	void setWrapMode(WrapMode s_and_t);
	void setBorderColor(float r, float g, float b, float a = 1.0f);
	// TODO: min+mag filter

	// usage type
	void setUsageType(UsageType type);
	UsageType getUsageType() const;
	std::string getUsageString() const;

private:
	bool isFlagSet(Flags flag);

	void create();

	int width, height, nChannels;
	Flags flags = Flags::NONE;

	UsageType usageType = UsageType::DIFFUSE;

	GLuint texture = 0;
	GLint prevBoundTexture = 0;

protected:
	glm::vec4 subrect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // xy = offset, zw = scale
};
