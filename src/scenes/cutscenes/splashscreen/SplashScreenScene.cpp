#include <scenes/cutscenes/splashscreen/SplashScreenScene.hpp>

////////////
// PUBLIC //
////////////

bool SplashScreenScene::onCreate() {
	m_engine->getDispatcher().sink<KeyEvent>().connect<&SplashScreenScene::onKeyInput>(this);

	m_camera = new Camera{glm::vec3(0.0f, 0.0f, 1.0f)};
	m_camera->setTarget(glm::vec3(0.0f));

	// load texture
	m_logoTexture = new sgl::texture{};
	int width, height, nChannels;
	stbi_set_flip_vertically_on_load(false);
	unsigned char* data = stbi_load("res/images/ui/placeholder.png", &width, &height, &nChannels, 0);
	if (!data)
		return false;
	m_logoTexture->load(width, height, sgl::texture::internalformat::rgba, data, sgl::texture::format::rgb,
	                    sgl::texture::datatype::u8);
	stbi_image_free(data);

	lua_State* L = m_engine->getLuaState();

	if (luaL_dofile(L, "res/scripts/layout/splash.lua")) {
		const char* err = lua_tostring(L, -1);
		fmt::print("Error loading layout:\n");
		fmt::print(fmt::fg(fmt::terminal_color::red), "{}\n", err);
		lua_pop(L, 1);
		return false;
	}

	// check if return value is ok
	if (!lua_istable(L, -1)) {
		fmt::print("Error loading layout!\n");
		fmt::print(fmt::fg(fmt::terminal_color::red), "Return value is not of type 'Yoga'...\n");
		return false;
	}

	// get layout from script
	lua_getfield(L, -1, "super");
	YGNodeRef layout = (YGNode*)lua_touserdata(L, -1);
	lua_pop(L, 1);
	m_layout = layout;

	// create layout
	
	ImageWidget* logoWidget = new ImageWidget(m_logoTexture->get_texture());

	m_ui.setLayout(layout);
	m_ui.setWidget("logo", logoWidget);

	return true;
}

void SplashScreenScene::onDestroy() {
	m_engine->getDispatcher().sink<KeyEvent>().disconnect(this);

	delete m_logoTexture;
}

void SplashScreenScene::onUpdate(float dt) {
	m_elapsedTime += (double)dt;

	// switch to next scene
	if (m_elapsedTime > 3.f) {
		m_engine->setActiveScene(new MainMenuScene{});
	}
}

/////////////
// PRIVATE //
/////////////

void SplashScreenScene::onKeyInput(const KeyEvent& event) {
	m_engine->setActiveScene(new MainMenuScene{});
}

/*
void SplashScreenScene::drawLayout(YGNodeRef parent, glm::mat4 view, float z) {
	float x = YGNodeLayoutGetLeft(parent);
	float y = YGNodeLayoutGetTop(parent);
	float w = YGNodeLayoutGetWidth(parent);
	float h = YGNodeLayoutGetHeight(parent);

	view = glm::translate(view, glm::vec3(x, y, 0.0f));
	glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, z)), glm::vec3(w, h, 1.0f));
	m_logoShader->uniform("uView") = view;
	m_logoShader->uniform("uModel") = model;
	glDrawArrays(GL_TRIANGLES, 0, 6);

	for (uint32_t i = 0; i < YGNodeGetChildCount(parent); ++i) {
		YGNodeRef child = YGNodeGetChild(parent, i);
		drawLayout(child, view, z += 0.01f);
	}
}
*/

void SplashScreenScene::onRender() {
	glClearColor(0.24f, 0.58f, 1.0f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float width = m_camera->getScreenWidth();
	float height = m_camera->getScreenHeight();

	// Yoga wireframe rendering
	static float lastWidth = 0.0f, lastHeight = 0.0f;
	if (lastWidth != width || lastHeight != height) {
		lastWidth = width;
		lastHeight = height;
		YGNodeCalculateLayout(m_layout, width, height, YGDirectionLTR);
		fmt::print("layout has been updated!\n");
	}

	m_ui.draw();
}
