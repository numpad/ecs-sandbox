#include <Assets/Texture.hpp>

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


Texture::Texture(Texture::Flags flags)
	: flags(flags)
{
	
}

Texture::Texture(const Texture &copy) {
	width     = copy.width;
	height    = copy.height;
	nChannels = copy.nChannels;
	flags     = copy.flags;
	usageType = copy.usageType;
	texture   = copy.texture;
	prevBoundTexture = copy.prevBoundTexture;
	subrect   = copy.subrect;
}

Texture::~Texture() {
}

// subrect

glm::vec4 Texture::getSubRect() const {
	return subrect;
}

void Texture::resetSubRect() {
	subrect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
}

void Texture::flipSubRect(Texture::Flip flip) {
	if (flip & Texture::Flip::HORIZONTAL) {
		subrect.x += subrect.z;
		subrect.z *= -1.0f;
	}
	if (flip & Texture::Flip::VERTICAL) {
		subrect.y += subrect.w;
		subrect.w *= -1.0f;
	}
}

// convenience

glm::vec2 Texture::getNormalizedPixelSize() const {
	return glm::vec2(1.0f / (float)getWidth(), 1.0f / (float)getHeight());
}

// loading data

bool Texture::loadImage(std::string path) {
	bool flip_y = !isFlagSet(Texture::Flags::NO_VERTICAL_FLIP);
	stbi_set_flip_vertically_on_load(flip_y);
	
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
	
	if (this->isFlagSet(Texture::Flags::GEN_MIPMAPS)) {
		glGenerateMipmap(GL_TEXTURE_2D);
		#if CFG_DEBUG
			printf("[LOG] Texture: GEN_MIPMAPS flag is set! Generating...\n");
		#endif
	}
	this->unbind();
	
	return true;
}

bool Texture::loadTexture(const Texture &texture) {
	this->width = texture.width;
	this->height = texture.height;
	this->nChannels = texture.nChannels;
	this->flags = texture.flags;
	this->usageType = texture.usageType;
	this->texture = texture.texture;
	this->subrect = texture.subrect;
	
	return true;
}

// destroying data

void Texture::destroy() {
	#if CFG_DEBUG
		printf("[LOG] Texture: deleted texture @%u\n", texture);
	#endif
	
	glDeleteTextures(1, &texture);
	width = height = nChannels = 0;
}


// using texture

void Texture::bind() {
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevBoundTexture);
	glBindTexture(GL_TEXTURE_2D, texture);
}

void Texture::unbind() {
	glBindTexture(GL_TEXTURE_2D, prevBoundTexture);
}

// configuring

void Texture::setWrapMode(Texture::WrapMode s, Texture::WrapMode t) {
	this->bind();
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<int>(s));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<int>(t));
	
	this->unbind();
}

void Texture::setWrapMode(Texture::WrapMode s_and_t) {
	this->setWrapMode(s_and_t, s_and_t);
}

void Texture::setBorderColor(float r, float g, float b, float a) {
	this->bind();
	
	float borderColor[] = {r, g, b, a};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	
	this->unbind();
}

// usage type
void Texture::setUsageType(Texture::UsageType type) {
	this->usageType = type;
}

Texture::UsageType Texture::getUsageType() const {
	return usageType;
}

std::string Texture::getUsageString() const {
	switch (usageType) {
		case Texture::UsageType::DIFFUSE: return "Diffuse";
		case Texture::UsageType::SPECULAR: return "Specular";
		case Texture::UsageType::NORMAL: return "Normal";
		default: break;
	};
	
	return "Diffuse";
}

/////////////
// PRIVATE //
/////////////

bool Texture::isFlagSet(Flags flag) {
	return this->flags & flag;
}

void Texture::create() {
	glGenTextures(1, &texture);
	
	#if CFG_DEBUG
		printf("[LOG] Texture: created @%u\n", texture);
	#endif
	
	this->bind();
	
	// float borderColor[] = {1.0f, 1.0f, 1.0f, 0.0f};
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	this->setWrapMode(Texture::WrapMode::CLAMP);
	
	GLint minfilter = GL_NEAREST;
	if (this->isFlagSet(Texture::Flags::GEN_MIPMAPS))
		minfilter = GL_NEAREST_MIPMAP_LINEAR;
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minfilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	this->unbind();
}
