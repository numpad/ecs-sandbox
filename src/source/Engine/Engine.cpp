#include "Engine/Engine.hpp"

Engine::Engine(EngineConfig config)
 : m_config(config)
{

}

bool Engine::initialize() {
	// initialize scripting
	luastate_init();

	// initialize window
	if (!Window::Init()) return init_error("Failed initializing window.");
	if (!m_window.create(m_config.window_width, m_config.window_height)) return init_error("Failed creating window.");

	// initialize global stuff
	Camera::Init(m_window);
	Font::Init();

	imgui_init(m_window);

	// prepare shaders & GBuffer
	m_screen.initialize();
	m_gbuffer.initialize(m_config.window_width, m_config.window_height);

	m_screenshader.load("res/glsl/2d/screen_vert.glsl", sgl::shader::VERTEX);
	m_screenshader.load("res/glsl/2d/screen_frag.glsl", sgl::shader::FRAGMENT);
	m_screenshader.compile();
	m_screenshader.link();
	Blackboard::write("deferredShader", &m_screenshader);

	m_screenshader["uTexColor"] = 0; // gbuffer attachment 0
	m_screenshader["uTexPosition"] = 1;
	m_screenshader["uTexNormal"] = 2;
	m_screenshader["uTexDepth"] = 3;

	return true;
}

void Engine::destroy() {
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
		// switch to next scene if required
		if (m_next_scene) {
			switchScene();
		}

		// poll events
		glfwPollEvents();
		imgui_prepareframe();

		// calc time
		double msCurrentTime = glfwGetTime();
		static float msPerFrame = 0.0f;
		msFrames++;
		if (msCurrentTime - msLastTime >= 1.0) {
			msPerFrame = 1000.0f / (float)msFrames;
			msFrames = 0;
			msLastTime += 1.0;
		}
		
		// input
		int screenX, screenY;
		glfwGetWindowSize(m_window, &screenX, &screenY);
		
		#if CFG_IMGUI_ENABLED
			static int settings_attachment = 0;
			//imguiRenderMenuBar(m_window, world, crosspos, topdown, camera, msPerFrame, settings_attachment);
		#endif
		
		// TODO: find a better way to resize fbo attachments
		m_gbuffer.resize(screenX, screenY);
		
		// rendering
		m_gbuffer.bind();
		
		glClearColor(.231f, .275f, .302f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// TODO: dont hardcode this, also referenced in DecalRenderSystem::draw().
		//       the gbuffer should be easily available in all IRenderSystems
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, *m_gbuffer.m_position);

		// actual rendering
		if (m_scene) {
			m_scene->onUpdate(1.f / 60.f); // TODO(chris): timestep
			m_scene->onRender();
		}

		m_gbuffer.unbind();

		// render framebuffer to screen
		#if CFG_IMGUI_ENABLED
			m_screenshader["uTexChoiceActive"] = (settings_attachment != 0);
			GLint display_texture = *m_gbuffer.m_color;
			if (settings_attachment != 0) {
				glGetNamedFramebufferAttachmentParameteriv(*m_gbuffer.m_fbo, settings_attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &display_texture);
			}
			m_screenshader["uTexColor"] = 0;
			m_screenshader["uTexPosition"] = 1;
			m_screenshader["uTexNormal"] = 2;
			m_screenshader["uTexDepth"] = 3;
		#else
			const GLint display_texture = *m_gbuffer.m_color;
		#endif
		m_screenshader["uTime"] = (float)glfwGetTime();

		GLint pmode;
		glGetIntegerv(GL_POLYGON_MODE, &pmode);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		m_screenshader.use();
		glDisable(GL_DEPTH_TEST);
		m_screen.bind();
		
		m_gbuffer.bind_textures();
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, display_texture);
		
		m_screen.draw();
		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, pmode);
		
		// present rendered
		imgui_render();
		glfwSwapBuffers(m_window);
	}
}

void Engine::setActiveScene(IScene *scene) {
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
	if (m_scene) m_scene->m_engine = this;
	
	// try and create new scene
	if (m_scene) {
		if (!m_scene->onCreate()) {
			// scene failed to initialize, destroy it and shutdown.
			m_scene->onDestroy();
			delete m_scene;
			m_scene = nullptr;
			init_error("Scene initialization failed, running dry!");
			quit();
		}
	}
}

void Engine::imgui_init(GLFWwindow *window) {
	#if CFG_IMGUI_ENABLED
		// init imgui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ImGui::StyleColorsLight();
		// imgui glfw init
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 450");
		printf("[INIT] dear imgui %s\n", ImGui::GetVersion());
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

	if (!m_lua) return init_error("Failed initializing Lua state.");

	// open stdlib, add modules path
	luaL_openlibs(m_lua);
	luaL_dostring(m_lua, "package.path = package.path .. ';res/scripts/modules/?.lua'");

	// register reference to engine
	lua_pushlightuserdata(m_lua, this);
	lua_setglobal(m_lua, "_Engine");

	return true;
}

void Engine::luastate_destroy() {
	lua_close(m_lua);
}
