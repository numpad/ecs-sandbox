#include <yoga/YGNode.h>
#include "scenes/testing/LayoutEditorScene.hpp"
#include "core/UI/DebugWidget.hpp"
#include "core/Util/File.hpp"

bool LayoutEditorScene::onCreate() {
	m_camera = new Camera(glm::vec3(0.0f, 0.0f, 1.0f));
	m_camera->setTarget(glm::vec3(0.0f));

	m_files = File::ListDirectory("res/scripts/layout/");

	return true;
}

void LayoutEditorScene::onDestroy() {
	m_ui.deleteWidgets();
}

void LayoutEditorScene::onUpdate(float dt) {
	// imgui variables
	static bool autoReload = true;
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Open...")) {
			for (std::string& filename : m_files) {
				if (ImGui::MenuItem(filename.c_str())) {
					loadLayout(filename);
				}
			}
			ImGui::Separator();

			if (ImGui::MenuItem("Refresh")) {
				m_files = File::ListDirectory("res/scripts/layout/");
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Settings")) {
			ImGui::Checkbox("Auto Reload", &autoReload);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	
	// reload file every 2 seconds
	static float reloadTimeout = 0.0f;
	reloadTimeout += dt;
	if (autoReload && !m_currentFile.empty() && reloadTimeout > 2.0f) {
		loadLayout(m_currentFile);
		reloadTimeout = 0.0f;
	}

	// close editor
	if (glfwGetKey(Engine::Instance->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		Engine::Instance->setActiveScene(new MainMenuScene());
	}
}

void LayoutEditorScene::onRender() {
	glClearColor(0.24f, 0.58f, 1.0f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_ui.resize(glm::vec2(Engine::Instance->window.getSize()));
	m_ui.draw();
}

/////////////
// PRIVATE //
/////////////

void LayoutEditorScene::loadLayout(const std::string& filename) {
	m_currentFile = filename;

	// parse layout file
	lua_State* L = Engine::Instance->getLuaState();
	if (luaL_dofile(Engine::Instance->getLuaState(), std::string("res/scripts/layout/" + m_currentFile).c_str())) {
		const char* err = lua_tostring(L, -1);
		fmt::print("Error loading layout:\n");
		fmt::print(fmt::fg(fmt::terminal_color::red), "{}\n", err);
		return;
	}

	// check if return value is ok
	if (!lua_istable(L, -1)) {
		fmt::print("Error loading layout!\n");
		fmt::print(fmt::fg(fmt::terminal_color::red), "Return value is not of type 'Yoga'...\n");
		return;
	}

	// get layout from script
	lua_getfield(L, -1, "super");
	YGNodeRef layout = (YGNodeRef)lua_touserdata(L, -1);
	lua_pop(L, 1);

	if (m_ui.getLayout() != nullptr) {
		delete m_ui.getLayout();
	}

	m_ui.setLayout(layout);
	m_ui.setWidget("*", new DebugWidget());
	m_ui.resize(glm::vec2(Engine::Instance->window.getSize()));
	m_selectedNode = layout;
}
