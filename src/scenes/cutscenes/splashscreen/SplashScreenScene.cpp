#include <scenes/cutscenes/splashscreen/SplashScreenScene.hpp>

bool SplashScreenScene::onCreate() {
	m_camera = new Camera{glm::vec3(0.0f, 0.0f, -1.0f)};
	m_camera->setTarget(glm::vec3(0.0f));

	// load texture
	m_logoTexture = new sgl::texture{};
	int width, height, nChannels;
	stbi_set_flip_vertically_on_load(false);
	unsigned char *data = stbi_load("res/images/ui/splash_logo.png", &width, &height, &nChannels, 0);
	if (!data) return false;
	m_logoTexture->load(width, height, sgl::texture::internalformat::rgba, data, sgl::texture::format::rgba, sgl::texture::datatype::u8);
	stbi_image_free(data);

	// load shader
	m_logoShader = new sgl::shader{};
	m_logoShader->load("res/glsl/ui/image_vert.glsl", sgl::shader::VERTEX);
	m_logoShader->load("res/glsl/ui/image_frag.glsl", sgl::shader::FRAGMENT);
	m_logoShader->compile();
	m_logoShader->link();

	createLogo();

	return true;
}

void SplashScreenScene::onDestroy() {
	delete m_logoTexture;
	delete m_logoShader;
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}

void SplashScreenScene::onUpdate(float dt) {
	m_elapsedTime += (double)dt;

	// switch to next scene
	if (m_elapsedTime > 2.5f) {
		m_engine->setActiveScene(new MainMenuScene{});
	}
}

void SplashScreenScene::onRender() {
	glClearColor(0.24f, 0.58f, 1.0f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float width = m_camera->getScreenWidth();
	float height = m_camera->getScreenHeight();
	m_logoShader->operator[]("uProjection") = m_camera->getHudProjection();
	m_logoShader->operator[]("uView") = glm::mat4(1.0f);
	auto model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(3.0f, 3.0f, 0.0f));
	model = glm::scale(model, glm::vec3(300.0f, 150.0f, 1.0f));
	m_logoShader->operator[]("uModel") = model;

	m_logoShader->use();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glDrawArrays(GL_TRIANGLES, 0, 24);
}

void SplashScreenScene::createLogo() {
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	GLfloat vertices[] = {
		0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
	
}