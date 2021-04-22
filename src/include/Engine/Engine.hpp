#pragma once

#include "config.hpp"
#include <GL/gl3w.h>

#include <iostream>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <luajit-2.0/lua.hpp>

#include <sgl/sgl_shader.hpp>
#include <sgl/sgl_framebuffer.hpp>
#include <sgl/sgl_texture.hpp>
#include <sgl/sgl_renderbuffer.hpp>
#include <sgl/sgl_attachment.hpp>

#include <Engine/EngineConfig.hpp>

#include <Util/Math3d.hpp>
#include <Util/Font.hpp>
#include <Util/Random.hpp>
#include <Util/Benchmark.hpp>
#include <Util/Blackboard.hpp>

#include <Window/Window.hpp>
#include <Window/Screen.hpp>
#include <Engine/IScene.hpp>
#include <RenderObject/GBuffer.hpp>

class Engine {
public:

	Engine(EngineConfig config = EngineConfig());

	// setup, cleanup
	bool initialize();
	void destroy();
	// game loop
	void run();
	void quit();

	// scene management
	void setActiveScene(IScene *scene);

	// getters
	Window &getWindow() { return m_window; }
	lua_State *getLuaState() { return m_lua; }
	entt::registry &getRegistry() { return m_registry; }

private:
	// engine specific
	EngineConfig m_config;

	// graphics
	Window m_window;
	Screen m_screen;
	sgl::shader m_screenshader;
	GBuffer m_gbuffer;

	// scripting & registry
	entt::registry m_registry;
	lua_State *m_lua = nullptr;

	// application specific
	IScene *m_scene = nullptr;
	IScene *m_next_scene = nullptr;
	void switchScene();

	// imgui helper functions
	void imgui_init(GLFWwindow *window);
	void imgui_prepareframe();
	void imgui_render();
	void imgui_destroy();

	// lua helper functions
	bool luastate_init();
	void luastate_destroy();

	bool init_error(std::string error_details) {
		std::cerr << "\x1b[31m[INI]\x1b[0m " << error_details << std::endl;
		return false;
	}
};
