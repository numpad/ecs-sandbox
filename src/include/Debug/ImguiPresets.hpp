#pragma once

#include <imgui/imgui.h>
#include <entt/entt.hpp>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <Util/Random.hpp>
#include <Util/Blackboard.hpp>
#include <World.hpp>

void imguiEntityEdit(entt::registry &registry, entt::entity entity);
void imguiEntityEditor(World &world, bool pickingactive, glm::vec3 crosspos);
void imguiRenderMenuBar(GLFWwindow *window, World &world, glm::vec3 &crosspos, std::shared_ptr<Camera> topdown, std::shared_ptr<Camera> camera, float &msPerFrame, int &settings_attachment);
void imguiLuaJitConsole(lua_State *L);
