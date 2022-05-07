#include <scenes/testing/SpritePreviewScene.hpp>

bool SpritePreviewScene::onCreate() {
	m_camera = new Camera{glm::vec3(0.0f, 0.0f, 1.0f)};
	m_camera->setTarget(glm::vec3(0.0f));

	m_engine->getConfig().imgui_enabled = true;

	// load shader
	m_imageShader = new sgl::shader("res/glsl/ui/image_vert.glsl", "res/glsl/ui/image_frag.glsl");
	m_previewShader = new sgl::shader("res/glsl/ui/uvimage_vert.glsl", "res/glsl/ui/uvimage_frag.glsl");
	createLogo();

	// load texture
	reload();

	lua_State *L = m_engine->getLuaState();

	return true;
}

void SpritePreviewScene::onDestroy() {
	m_engine->getConfig().imgui_enabled = false;
	delete m_imageShader;
	delete m_previewShader;
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}

void SpritePreviewScene::onUpdate(float dt) {
	using namespace ImGui;
	m_elapsedTime += (double)dt;

	if (BeginMainMenuBar()) {
		if (BeginMenu("Open...")) {
			MenuItem("image.png");
			EndMenu();
		}
		EndMainMenuBar();
	}

	if (Begin("Editor")) {
		
	}
	End();

	static float reloadTimer = 0.0f;
	reloadTimer += dt;
	if (reloadTimer > 2.0f) {
		reloadTimer = 0.0f;
		reload();
		fmt::print("[{}] images and shaders reloaded!\n", m_elapsedTime);
	}

	// switch to next scene
	if (glfwGetKey(m_engine->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		m_engine->setActiveScene(new MainMenuScene());
	}
}

void SpritePreviewScene::onRender() {
	glClearColor(0.24f, 0.58f, 1.0f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float screen_width = m_camera->getScreenWidth();
	float screen_height = m_camera->getScreenHeight();

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 model_source = glm::scale(
		glm::translate(glm::mat4(1.0f), glm::vec3(50.0f, 50.0f, 0.0f)),
		glm::vec3(m_texture->get_width() * 4.0f, m_texture->get_height() * 4.0f, 1.0f));
	m_imageShader->operator[]("uProjection") = glm::ortho(0.0f, screen_width, screen_height, 0.0f); //m_camera->getHudProjection();
	m_imageShader->operator[]("uView") = view;
	m_imageShader->operator[]("uModel") = model_source;
	m_imageShader->operator[]("uImage") = 0;
	m_imageShader->operator[]("uUseImage") = true;


	m_imageShader->use();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glActiveTexture(GL_TEXTURE0);
	m_texture->bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	

	glm::mat4 model_preview = glm::scale(
		glm::translate(glm::mat4(1.0f), glm::vec3(350.0f, 50.0f, 0.0f)),
		glm::vec3(m_texCoords->get_width() * 8.0f, m_texCoords->get_height() * 8.0f, 1.0f));
	m_previewShader->use();
	m_previewShader->operator[]("uProjection") = glm::ortho(0.0f, screen_width, screen_height, 0.0f); //m_camera->getHudProjection();
	m_previewShader->operator[]("uView") = view;
	m_previewShader->operator[]("uModel") = model_preview;
	m_previewShader->operator[]("uSource") = 0;
	m_previewShader->operator[]("uTexCoords") = 1;
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glActiveTexture(GL_TEXTURE0);
	m_texture->bind();
	glActiveTexture(GL_TEXTURE1);
	m_texCoords->bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}


/////////////
// PRIVATE //
/////////////

void SpritePreviewScene::createLogo() {
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	GLfloat vertices[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
	
}

void SpritePreviewScene::reload() {
	delete m_texture;
	delete m_texCoords;
	{
		int width, height, nChannels;
		stbi_set_flip_vertically_on_load(false);
		unsigned char *data = stbi_load("res/images/textures/ground_texture.png", &width, &height, &nChannels, 0);
		if (!data) return;

		m_texture = new sgl::texture();
		m_texture->load(width, height, sgl::texture::internalformat::rgba, data, sgl::texture::format::rgba, sgl::texture::datatype::u8);
		stbi_image_free(data);
	}


	{
		int width, height, nChannels;
		stbi_set_flip_vertically_on_load(false);
		unsigned char *data = stbi_load("res/images/textures/ground_uv.png", &width, &height, &nChannels, 0);
		if (!data) return;

		m_texCoords = new sgl::texture();
		m_texCoords->load(width, height, sgl::texture::internalformat::rgba, data, sgl::texture::format::rgba, sgl::texture::datatype::u8);
		stbi_image_free(data);
	}

	delete m_imageShader;
	delete m_previewShader;
	m_imageShader = new sgl::shader("res/glsl/ui/image_vert.glsl", "res/glsl/ui/image_frag.glsl");
	m_previewShader = new sgl::shader("res/glsl/ui/uvimage_vert.glsl", "res/glsl/ui/uvimage_frag.glsl");
}
