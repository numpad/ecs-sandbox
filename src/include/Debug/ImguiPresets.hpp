#pragma once

#include <imgui/imgui.h>
#include <entt/entt.hpp>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <fmt/core.h>
#include <fmt/color.h>

#include <Util/Random.hpp>
#include <Util/Blackboard.hpp>
#include <World.hpp>
#include <Engine/Engine.hpp>

void imguiEntityEdit(entt::registry &registry, entt::entity entity);
void imguiEntityEditor(entt::registry &registry, bool pickingactive, glm::vec3 crosspos);
void imguiRenderMenuBar(Engine *engine, entt::registry &registry, glm::vec3 &crosspos, float msPerFrame);
void imguiLuaJitConsole(lua_State *L);

void imguiGamepadInfo();
