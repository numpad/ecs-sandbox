#pragma once

#include "config.hpp"
#include <GL/gl3w.h>

#include <iostream>

#include <entt/entt.hpp>
#include <fmt/core.h>
#include <fmt/color.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <luajit/lua.hpp>
#include <yoga/Yoga.h>

#include <sgl/sgl_shader.hpp>
#include <sgl/sgl_framebuffer.hpp>
#include <sgl/sgl_texture.hpp>
#include <sgl/sgl_renderbuffer.hpp>
#include <sgl/sgl_attachment.hpp>

#include <Engine/EngineConfig.hpp>
#include <Engine/IScene.hpp>
#include <Graphics/Graphics.hpp>
#include <Graphics/GLState.hpp>

#include <Util/Math3d.hpp>
#include <Util/Font.hpp>
#include <Util/Random.hpp>
#include <Util/Benchmark.hpp>
#include <Util/Blackboard.hpp>

#include <Window/Window.hpp>
#include <Window/Screen.hpp>
#include <RenderObject/GBuffer.hpp>
#include <Script/FFI.hpp>

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
	void setActiveScene(IScene* scene);

	// getters
	lua_State* getLuaState() {
		return m_lua;
	}
	IScene* getScene() {
		return m_scene;
	}

	// singleton
	static Engine* Instance;

private:
	// engine specific
	EngineConfig m_config;

	// graphics
	Graphics m_graphics;
	static inline Window* m_main_window = nullptr;
	Window m_window;
	Screen m_screen;
	sgl::shader m_screenshader;
	GBuffer m_gbuffer;

	// scripting & registry
	lua_State* m_lua = nullptr;
	entt::dispatcher m_dispatcher;

	// application specific
	IScene* m_scene = nullptr;
	IScene* m_next_scene = nullptr;
	void switchScene();

	// imgui helper functions
	void imgui_init(GLFWwindow* window);
	void imgui_prepareframe();
	void imgui_render();
	void imgui_destroy();

	// lua helper functions
	bool luastate_init();
	void luastate_destroy();

public:
	Window& window = m_window;
	GBuffer& gbuffer = m_gbuffer;
	entt::dispatcher& dispatcher = m_dispatcher;
	Graphics& graphics = m_graphics;
	EngineConfig& config = m_config;
};
