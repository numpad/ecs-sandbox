#include "Engine/Engine.hpp"

Engine* Engine::Instance = nullptr;

static void callback_framebuffer_resized(GLFWwindow* window, int width, int height) {
	Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));

	// resize viewport, gbuffer and cameras
	glViewport(0, 0, width, height);
	engine->getGBuffer().resize(width, height);
	for (Camera* cam : Camera::CAMERAS) {
		if (cam->windowAspectLocked) {
			cam->setScreenSize(width, height);
			cam->windowAspectLocked = true;
		}
	}

	engine->getDispatcher().enqueue<WindowResizeEvent>(width, height);
}

static void callback_window_close(GLFWwindow* window) {
	Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));

	engine->getDispatcher().enqueue<WindowCloseEvent>();
}

static void callback_joystick_connected(int joystick_id, int connected) {
	Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(Engine::getMainWindow()));

	engine->getDispatcher().enqueue<GamepadConnectedEvent>(joystick_id, connected == GLFW_CONNECTED);
}

static void callback_key_pressed(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(Engine::getMainWindow()));

	// toggle debug ui
	if (key == GLFW_KEY_I && action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL)) {
		engine->getConfig().imgui_enabled = !engine->getConfig().imgui_enabled;
	}

	engine->getDispatcher().enqueue<KeyEvent>(key, scancode, action, mods);
}

static void callback_mouse_input(GLFWwindow* window, int button, int action, int mods) {
	Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
	double mx, my;
	glfwGetCursorPos(window, &mx, &my);

	engine->getDispatcher().enqueue<MouseButtonEvent>(button, action == GLFW_PRESS, mods,
	                                                  glm::vec2(float(mx), float(my)));
}

////////////
// PUBLIC //
////////////

Engine::Engine(EngineConfig config) : m_config(config) {
	if (Engine::Instance != nullptr) {
		fmt::print(fmt::emphasis::bold | fmt::fg(fmt::terminal_color::red), "Engine already initialized!\n");
		return;
	}

	Engine::Instance = this;
}

bool Engine::initialize() {
#if CFG_DEBUG
	const char* cfg_debug_state = "DEBUG";
#else
	const char* cfg_debug_state = "Release build";
#endif
	fmt::print(fmt::emphasis::bold, "{} v{}.{} ({}) [{}]\n", CFG_PROJECT_NAME, CFG_VERSION_MAJOR, CFG_VERSION_MINOR,
	           cfg_debug_state, CFG_CMAKE_BUILD_TYPE);

	// initialize scripting
	if (luastate_init()) {
		lua_getglobal(m_lua, "_VERSION");
		const char* luaversion = lua_tostring(m_lua, -1);
		fmt::print(fmt::fg(fmt::terminal_color::green), "{}", luaversion);
		lua_pop(m_lua, 1);
		// check if LuaJIT is available
		lua_getglobal(m_lua, "jit");
		if (lua_istable(m_lua, -1)) {
			lua_getfield(m_lua, -1, "version");
			const char* jitversion = lua_tostring(m_lua, -1);
			fmt::print(fmt::fg(fmt::terminal_color::green), " ({})", jitversion);
			lua_pop(m_lua, 1);
		}
		fmt::print("\n");
	} else {
		fmt::print(fmt::fg(fmt::terminal_color::red), "Failed initializing Lua\n");
		return false;
	}

	// initialize window
	if (!Window::Init()) {
		fmt::print(fmt::fg(fmt::terminal_color::red), "glfwInit() failed\n");
		return false;
	}
	fmt::print(fmt::fg(fmt::terminal_color::green), "GLFW {}\n", glfwGetVersionString());

	if (!m_window.create(m_config.window_width, m_config.window_height)) {
		fmt::print(fmt::fg(fmt::terminal_color::red), "Error creating window\n");
		return false;
	}

	glfwSetWindowUserPointer(m_window, this);
	// connect callbacks
	glfwSetFramebufferSizeCallback(m_window, callback_framebuffer_resized);
	glfwSetWindowCloseCallback(m_window, callback_window_close);
	glfwSetJoystickCallback(callback_joystick_connected);
	glfwSetKeyCallback(m_window, callback_key_pressed);
	glfwSetMouseButtonCallback(m_window, callback_mouse_input);

	// set global main window
	if (Engine::m_main_window == nullptr) {
		Engine::m_main_window = &m_window;
	}

	// initialize global stuff
	Camera::Init(m_window);
	Font::Init();

	imgui_init(m_window);

	// initialize members
	m_graphics.initialize();

	// prepare shaders & GBuffer
	m_screen.initialize();
	m_gbuffer.initialize(m_config.window_width, m_config.window_height);

	m_screenshader.load("res/glsl/2d/screen_vert.glsl", sgl::shader::VERTEX);
	m_screenshader.load("res/glsl/2d/screen_frag.glsl", sgl::shader::FRAGMENT);
	m_screenshader.compile();
	m_screenshader.link();
	Blackboard::write("deferredShader", &m_screenshader);

	// TODO: register this somewhere appropriate
	lua_register(m_lua, "LUA_YGNodeNew", LUA_YGNodeNew);
	if (luaL_dofile(m_lua, "res/scripts/engine/init.lua")) {
		const char* luaerror = lua_tostring(m_lua, -1);
		fmt::print(fmt::fg(fmt::terminal_color::red) | fmt::emphasis::bold, "Lua Error:\n");
		fmt::print(fmt::fg(fmt::terminal_color::red), "{}\n", luaerror);
		lua_pop(m_lua, 1);

		return false;
	}

	return true;
}

void Engine::destroy() {
	fmt::print(fmt::fg(fmt::terminal_color::white) | fmt::emphasis::bold, "Destroying engine...\n");
	// cleanup user
	setActiveScene(nullptr);

	// close lua state
	luastate_destroy();

	// cleanup engine
	m_screen.destroy();
	m_gbuffer.destroy();
	m_window.destroy();
	imgui_destroy();

	// cleanup global
	Font::Destroy();
	Window::Destroy();
}

void Engine::run() {
	double msLastTime = glfwGetTime();
	int msFrames = 0;
	while (!glfwWindowShouldClose(m_window)) {
		// calc time
		double msCurrentTime = glfwGetTime();
		static float msPerFrame = 0.0f;
		msFrames++;
		if (msCurrentTime - msLastTime >= 1.0) {
			msPerFrame = 1000.0f / (float)msFrames;
			msFrames = 0;
			msLastTime += 1.0;
		}

		// switch to next scene if required
		if (m_next_scene) {
			switchScene();
		}

		// poll events
		glfwPollEvents();
		// emit all events enqueued
		m_dispatcher.update();

		imgui_prepareframe();

		// rendering
		m_gbuffer.bind();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render the scene
		if (m_scene) {
			m_scene->onUpdate(1.f / 60.f); // TODO(chris): timestep
			m_scene->onRender();
		}

		m_gbuffer.unbind();

// render framebuffer to screen
#if CFG_IMGUI_ENABLED
		if (ImGui::Begin("GBuffer")) {
			const sgl::texture* preview_gbuffer_textures[] = {m_gbuffer.m_color, m_gbuffer.m_position,
			                                                  m_gbuffer.m_normal, m_gbuffer.m_depth};
			int iteration = 0;
			for (const sgl::texture* preview_gbuffer : preview_gbuffer_textures) {
				const float wwidth = ImGui::GetWindowWidth() * 0.47f; // meh
				const float aspect = preview_gbuffer->get_height() / float(preview_gbuffer->get_width());

				ImGui::Image((void*)preview_gbuffer->get_texture(), ImVec2(wwidth, wwidth * aspect), ImVec2(0.0f, 1.0f),
				             ImVec2(1.0f, 0.0f));

				if (iteration++ % 2 == 0) {
					ImGui::SameLine();
				}
			}
		}
		ImGui::End();
#endif

		// TODO: only need to reset these after reloading
		m_screenshader["uTime"] = (float)glfwGetTime();

		GLint pmode;
		glGetIntegerv(GL_POLYGON_MODE, &pmode);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		GLState deferredState;
		// https://www.khronos.org/opengl/wiki/Common_Mistakes#:~:text=In%20some%20cases%2C%20you%20might%20want%20to%20disable%20depth%20testing%20and%20still%20allow%20the%20depth%20buffer%20updated%20while%20you%20are%20rendering%20your%20objects
		deferredState.depth_test = false;
		deferredState.depth_write = true;
		m_graphics.setState(deferredState);

		m_screenshader.use();
		m_screen.bind();
		m_gbuffer.bind_textures(m_screenshader);

		m_screen.draw();
		glPolygonMode(GL_FRONT_AND_BACK, pmode);

		// draw imgui (if enabled)
		if (m_config.imgui_enabled)
			imgui_render();

		// present rendered
		glfwSwapBuffers(m_window);
	}
}

void Engine::setActiveScene(IScene* scene) {
	m_next_scene = scene;

	if (scene == nullptr) {
		switchScene();
	}
}

void Engine::quit() {
	glfwSetWindowShouldClose(m_window, true);
}

/////////////
// PRIVATE //
/////////////

void Engine::switchScene() {
	// cleanup previous scene
	if (m_scene) {
		m_scene->onDestroy();
		delete m_scene;
	}

	// set active scene (or null)
	m_scene = m_next_scene;
	m_next_scene = nullptr;
	if (m_scene)
		m_scene->m_engine = this;

	// try and create new scene
	if (m_scene) {
		if (!m_scene->onCreate()) {
			// scene failed to initialize, destroy it and shutdown.
			m_scene->onDestroy();
			delete m_scene;
			m_scene = nullptr;
		}
	}

	// no scene = nothing to do
	if (!m_scene && !m_next_scene) {
		fmt::print(fmt::fg(fmt::terminal_color::red) | fmt::emphasis::bold,
		           "Scene initialization failed, running dry...\n");
		quit();
	}
}

void Engine::imgui_init(GLFWwindow* window) {
#if CFG_IMGUI_ENABLED
	// init imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsLight();
	// imgui glfw init
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 450");
	fmt::print(fmt::fg(fmt::terminal_color::green), "dear imgui {}\n", ImGui::GetVersion());
#endif
}
void Engine::imgui_prepareframe() {
#if CFG_IMGUI_ENABLED
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
#endif
}
void Engine::imgui_render() {
#if CFG_IMGUI_ENABLED
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
}
void Engine::imgui_destroy() {
#if CFG_IMGUI_ENABLED
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
#endif
}

bool Engine::luastate_init() {
	m_lua = luaL_newstate();

	if (!m_lua) {
		return false;
	}

	// open stdlib, add modules path
	luaL_openlibs(m_lua);
	luaL_dostring(m_lua, "package.path = package.path .. ';res/scripts/modules/?.lua;res/scripts/?.lua'");

	// register reference to engine
	lua_pushlightuserdata(m_lua, this);
	lua_setglobal(m_lua, "_Engine");

	return true;
}

void Engine::luastate_destroy() {
	lua_close(m_lua);
}
