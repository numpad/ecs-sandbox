#pragma once

#include <config.hpp>

#include <string>
#include <map>
#include <limits>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>
#include <GL/gl3w.h>

#include <sgl/sgl_shader.hpp>

using namespace glm;

class Font {
private:
	struct Character {
		GLuint texture;
		ivec2 size, bearing;
		unsigned int advance;
	};
	
public:
	static void Init();
	static void Destroy();
	
	Font(std::string ttf, unsigned int height_px);
	void destroy();
	
	vec2 getSize(std::wstring str, float scale = 1.0f);
	
	void drawString(mat4 uProj, std::wstring str, float x, float y,
		float scale = 1.f, vec3 color = vec3(1.f));
	
private:
	FT_Face face;
	std::map<wchar_t, Character> chars;
	
	GLuint VAO, VBO;
	
	bool loadFontface(std::string ttf, unsigned int height_px);
	void loadChar(wchar_t chr);
	
	static FT_Library ft;
	static sgl::shader *defaultShader;
};
