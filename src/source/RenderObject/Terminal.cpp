#include "RenderObject/Terminal.hpp"

////////////
// PUBLIC //
////////////

Terminal::Terminal() {
	m_font = new Font("res/fonts/FSmono.ttf", 16);
	m_text = L"Enter password:";

	m_texture = new sgl::texture();
	m_texture->load(m_width, m_height, sgl::texture::internalformat::rgb, nullptr, sgl::texture::format::rgb, sgl::texture::datatype::u8);
	
	m_fbo = new sgl::framebuffer();
	m_fbo->attach(*m_texture, sgl::attachment::color(0));
	m_fbo->targets();
}

Terminal::~Terminal() {
	delete m_font;
	delete m_fbo;
	delete m_texture;
}

void Terminal::update() {
	m_fbo->bind();

	glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = glm::ortho(0.0f, float(m_width), 0.0f, float(m_height));
	m_font->drawString(projection, m_text, 0, 0, 1.0f, glm::vec3(0.2f, 1.0f, 0.3f));

	m_fbo->unbind();
}


/////////////
// PRIVATE //
/////////////

