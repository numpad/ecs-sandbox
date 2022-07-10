#include "ecs/events/TextInputEvent.hpp"
#include "scenes/cutscenes/splashscreen/SplashScreenScene.hpp"

////////////
// PUBLIC //
////////////

bool SplashScreenScene::onCreate() {
	Engine::Instance->getConfig().imgui_enabled = false;
	m_engine->getDispatcher().sink<TextInputEvent>().connect<&SplashScreenScene::onKeyInput>(this);

	m_camera = new Camera(glm::vec3(0.0f, 0.0f, 1.0f));
	m_camera->setTarget(glm::vec3(0.0f));

	// load textures
	// TODO: improve boilerplate
	{
		m_logoTexture = new sgl::texture{};
		int width, height, nChannels;
		stbi_set_flip_vertically_on_load(false);
		unsigned char* data = stbi_load("res/images/ui/engine_logo.png", &width, &height, &nChannels, 0);
		if (!data)
			return false;
		m_logoTexture->load(width, height, sgl::texture::internalformat::rgba, data, sgl::texture::format::rgba,
							sgl::texture::datatype::u8);
		
		stbi_image_free(data);
	}
	{
		m_profileTexture = new sgl::texture{};
		int width, height, nChannels;
		stbi_set_flip_vertically_on_load(false);
		unsigned char* data = stbi_load("res/images/ui/profile_picture.png", &width, &height, &nChannels, 0);
		if (!data)
			return false;
		m_profileTexture->load(width, height, sgl::texture::internalformat::rgba, data, sgl::texture::format::rgba,
							sgl::texture::datatype::u8);
		
		stbi_image_free(data);
	}

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
	m_logoTexture->set_filter(sgl::texture::filter::nearest);
	m_profileTexture->set_filter(sgl::texture::filter::linear);
	ImageWidget* logoWidget = new ImageWidget(m_logoTexture->get_texture());
	ImageWidget* numpadWidget = new ImageWidget(m_profileTexture->get_texture());

	m_ui.setLayout(layout);
	m_ui.setWidget("logo", logoWidget);
	m_ui.setWidget("profile", numpadWidget);

	return true;
}

void SplashScreenScene::onDestroy() {
	m_engine->getDispatcher().sink<TextInputEvent>().disconnect(this);

	delete m_logoTexture;
	delete m_profileTexture;
	m_ui.deleteWidgets();
}

void SplashScreenScene::onUpdate(float dt) {
	m_elapsedTime += (double)dt;

	// switch to next scene
	if (m_elapsedTime > 4.0f) {
		m_engine->setActiveScene(new MainMenuScene{});
	}
}

/////////////
// PRIVATE //
/////////////

void SplashScreenScene::onKeyInput(const TextInputEvent& event) {
	m_engine->setActiveScene(new MainMenuScene());
}

void SplashScreenScene::onRender() {
	glClearColor(0.35f, 0.15f, 0.24f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	float width = m_camera->getScreenWidth();
	float height = m_camera->getScreenHeight();
	m_ui.resize(glm::vec2(width, height));
	m_ui.draw();
}
