#include "Engine/Engine.hpp"
#include "UI/ImageWidget.hpp"
#include "ecs/events/TextInputEvent.hpp"
#include "scenes/cutscenes/splashscreen/SplashScreenScene.hpp"

////////////
// PUBLIC //
////////////

bool SplashScreenScene::onCreate() {
	Engine::Instance->dispatcher.sink<TextInputEvent>().connect<&SplashScreenScene::onKeyInput>(this);

	m_camera = new Camera(glm::vec3(0.0f, 0.0f, 1.0f));
	m_camera->setTarget(glm::vec3(0.0f));

	lua_State* L = Engine::Instance->getLuaState();

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
	YGNodeRef layout = (YGNodeRef)lua_touserdata(L, -1);
	lua_pop(L, 1);

	// load textures
	m_logoTexture = new sgl::texture();
	m_profileTexture = new sgl::texture();
	m_logoTexture->load_file("res/images/ui/engine_logo.png");
	m_profileTexture->load_file("res/images/ui/profile_picture.png");
	m_logoTexture->set_filter(sgl::texture::filter::nearest);
	m_profileTexture->set_filter(sgl::texture::filter::linear);

	// create layout
	ImageWidget* logoWidget = new ImageWidget(m_logoTexture->get_texture());
	ImageWidget* numpadWidget = new ImageWidget(m_profileTexture->get_texture());

	m_ui.setLayout(layout);
	m_ui.setWidget("logo", logoWidget);
	m_ui.setWidget("profile", numpadWidget);

	return true;
}

void SplashScreenScene::onDestroy() {
	Engine::Instance->dispatcher.sink<TextInputEvent>().disconnect(this);

	delete m_logoTexture;
	delete m_profileTexture;
	m_ui.deleteWidgets();
}

void SplashScreenScene::onUpdate(float dt) {
	m_elapsedTime += (double)dt;

	// switch to next scene
	if (m_elapsedTime > 4.0f) {
		Engine::Instance->setActiveScene(new MainMenuScene{});
	}
}

/////////////
// PRIVATE //
/////////////

void SplashScreenScene::onKeyInput(const TextInputEvent& event) {
	Engine::Instance->setActiveScene(new MainMenuScene());
}

void SplashScreenScene::onRender() {
	glClearColor(0.35f, 0.15f, 0.24f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float width = m_camera->getScreenWidth();
	float height = m_camera->getScreenHeight();
	m_ui.resize(glm::vec2(width, height));
	m_ui.draw();
}
