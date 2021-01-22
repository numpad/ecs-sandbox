#include <Util/Font.hpp>

////////////
// STATIC //
////////////

FT_Library Font::ft = nullptr;
sgl::shader* Font::defaultShader = nullptr;

void Font::Init() {
	if (Font::ft != nullptr) {
		#ifdef CFG_DEBUG
			printf("[ERR] FreeType: already initialized!\n");
		#endif
		return;
	}
	
	if (FT_Init_FreeType(&Font::ft)) {
		printf("[ERR] FreeType: could not be initialized.\n");
		return;
	}
	
	// load default shader
	Font::defaultShader = new sgl::shader;
	Font::defaultShader->load("res/glsl/2d/font_vert.glsl", sgl::shader::VERTEX);
	Font::defaultShader->load("res/glsl/2d/font_frag.glsl", sgl::shader::FRAGMENT);
	Font::defaultShader->compile(sgl::shader::VERTEX);
	Font::defaultShader->compile(sgl::shader::FRAGMENT);
	Font::defaultShader->link();
}

void Font::Destroy() {
	delete Font::defaultShader;
	FT_Done_FreeType(Font::ft);
}

////////////
// PUBLIC //
////////////

Font::Font(std::string ttf, unsigned int height_px) {
	loadFontface(ttf, height_px);
	
	// load ascii
	GLint prevAlignment;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlignment);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (unsigned long i = 0; i < 128; ++i)
		loadChar(i);
	std::wstring extra_chars = L"äöüÄÖÜ§ß²³€";
	for (wchar_t c : extra_chars)
		loadChar(c);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlignment);
	
	// init vao, vbo
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Font::destroy() {
	FT_Done_Face(face);
	for (auto c : chars) {
		glDeleteTextures(1, &c.second.texture);
	}
}

vec2 Font::getSize(std::wstring str, float scale) {
	vec2 size(0.f, std::numeric_limits<float>::min());
	
	float last_width;
	for (wchar_t c : str) {
		if (chars.count(c) == 0) continue;
		Character ch = chars[c];
		float w = ch.size.x * scale;
		float h = ch.size.y * scale;
		
		last_width = w;
		size.x += (ch.advance >> 6) * scale;
		if (h > size.y) size.y = h;
	}
	size.x += last_width;
	
	return size;
}

void Font::drawString(mat4 uProj, std::wstring str, float x, float y, float scale, vec3 color) {
	// TODO: restore previous blend func
	GLint blendEnabled;
	glGetIntegerv(GL_BLEND, &blendEnabled);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Font::defaultShader->use();
	(*Font::defaultShader)["uTextColor"] = color;
	(*Font::defaultShader)["uTexture"] = 0;
	(*Font::defaultShader)["uProjection"] = uProj;
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);
	
	GLint depthTestEnabled;
	glGetIntegerv(GL_DEPTH_TEST, &depthTestEnabled);
	glDisable(GL_DEPTH_TEST);
	
	for (wchar_t c : str) {
		if (chars.count(c) == 0) continue;
		Character ch = chars[c];
		
		float xpos = x + ch.bearing.x * scale;
		float ypos = y - (ch.size.y - ch.bearing.y) * scale;
		float w = ch.size.x * scale;
		float h = ch.size.y * scale;
		GLfloat vertices[24] = {
			xpos    , ypos + h, 0.f, 0.f,
			xpos    , ypos    , 0.f, 1.f,
			xpos + w, ypos    , 1.f, 1.f,
			xpos    , ypos + h, 0.f, 0.f,
			xpos + w, ypos,     1.f, 1.f,
			xpos + w, ypos + h, 1.f, 0.f
		};
		
		glBindTexture(GL_TEXTURE_2D, ch.texture);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 24, vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (ch.advance >> 6) * scale;
	}
	
	// is this necessarry?
	if (depthTestEnabled) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
	if (blendEnabled) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
	
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/////////////
// PRIVATE //
/////////////

bool Font::loadFontface(std::string ttf, unsigned int height_px) {
	if (FT_New_Face(Font::ft, ttf.c_str(), 0, &face)) {
		printf("[ERR] FreeType: could not load face '%s'.\n", ttf.c_str());
		return false;
	}
	
	FT_Set_Pixel_Sizes(face, 0, height_px);
	return true;
}

// expects GL_UNPACK_ALIGNMENT to be 1
void Font::loadChar(wchar_t chr) {
	if (chars.count(chr) > 0) return; // already loaded
	
	if (FT_Load_Char(face, chr, FT_LOAD_RENDER)) {
		printf("[ERR] FreeType: cannot load char %d\n", chr);
		return;
	}
	
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0, GL_RED,
		face->glyph->bitmap.width,
		face->glyph->bitmap.rows,
		0, GL_RED,
		GL_UNSIGNED_BYTE,
		face->glyph->bitmap.buffer);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	Font::Character c;
	c.texture = texture;
	c.size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
	c.bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
	c.advance = face->glyph->advance.x;
	
	chars.insert(std::make_pair(chr, c));
}
