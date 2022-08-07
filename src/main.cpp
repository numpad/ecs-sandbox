#include "config.hpp"
#include <GL/gl3w.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <thread>
#include <string>

#include <stb/stb_image.h>
#include <sgl/sgl_shader.hpp>
#include <sgl/sgl_framebuffer.hpp>
#include <sgl/sgl_texture.hpp>
#include <sgl/sgl_renderbuffer.hpp>
#include <sgl/sgl_attachment.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <entt/entt.hpp>

#include <Engine/Engine.hpp>

#include <Window/Window.hpp>
#include <Window/Screen.hpp>

#include <Assets/Texture.hpp>
#include <Assets/AssetManager.hpp>
#include <ecs/components.hpp>
#include <ecs/systems.hpp>
#include <ecs/events.hpp>
#include <World.hpp>

#include <Terrain/SignedDistTerrain.hpp>
#include <Terrain/ChunkedTerrain.hpp>
#include <Terrain/CubeMarcher.hpp>

#include <RenderObject/Camera.hpp>
#include <RenderObject/ChunkedWorld.hpp>
#include <RenderObject/GBuffer.hpp>

#include <cereal/types/memory.hpp>
#include <cereal/archives/json.hpp>

#include <Util/Math3d.hpp>
#include <Util/Font.hpp>
#include <Util/Random.hpp>
#include <Util/Benchmark.hpp>
#include <Util/Blackboard.hpp>

#include <luajit/lua.hpp>

#include <scenes/cutscenes/splashscreen/SplashScreenScene.hpp>

#include <Debug/ImguiPresets.hpp>

int main(int, char**) {

	Engine engine;

	if (engine.initialize()) {

		engine.run();
	}

	engine.destroy();

	return 0;
}
