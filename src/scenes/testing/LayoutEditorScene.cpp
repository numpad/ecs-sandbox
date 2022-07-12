#include <yoga/YGNode.h>
#include "scenes/testing/LayoutEditorScene.hpp"

bool LayoutEditorScene::onCreate() {
	m_camera = new Camera(glm::vec3(0.0f, 0.0f, 1.0f));
	m_camera->setTarget(glm::vec3(0.0f));

	m_engine->getConfig().imgui_enabled = true;

	updateFiles();
	loadLayout(m_files.at(0));

	lua_State* L = m_engine->getLuaState();
	
	return true;
}

void LayoutEditorScene::onDestroy() {
	m_engine->getConfig().imgui_enabled = false;
}

void LayoutEditorScene::onUpdate(float dt) {
	using namespace ImGui;
	m_elapsedTime += (double)dt;

	if (BeginMainMenuBar()) {
		if (BeginMenu("Open...")) {
			for (std::string& filename : m_files) {
				if (MenuItem(filename.c_str())) {
					loadLayout(filename);
				}
			}

			if (m_ui.getLayout() != nullptr) {
				Separator();
				if (MenuItem("Close")) {
					YGNodeFreeRecursive(m_ui.getLayout());
					// TODO: add cleanup function
					m_ui.setLayout(nullptr);
					m_selectedNode = nullptr;
				}
			}

			EndMenu();
		}
		EndMainMenuBar();
	}

	if (Begin("Editor")) {
		if (m_selectedNode != nullptr) {
			static int direction = YGDirectionInherit;
			const char* flexdirections[] = {"row", "column", "row-reverse", "column-reverse"};
			static const char* flexdirection = flexdirections[0];
			static char flexbasis[256] = {0};
			static float flexgrow = 0.0f;
			static float flexshrink = 1.0f;
			static int flexwrap = YGWrapNoWrap;

			NodeID* nodeId = (NodeID*)YGNodeGetContext(m_selectedNode);
			Text("Node: %s", nodeId->id.c_str());
			if (YGNodeGetChildCount(m_selectedNode) > 0) {
				if (Button("Select 1st child")) {
					m_selectedNode = YGNodeGetChild(m_selectedNode, 0);
				}
			}
			if (BeginTabBar("tabs")) {
				if (BeginTabItem("Flex")) {
					Text("Direction");
					RadioButton("inherit", &direction, YGDirectionInherit);
					SameLine();
					RadioButton("ltr", &direction, YGDirectionLTR);
					SameLine();
					RadioButton("rtl", &direction, YGDirectionRTL);

					Text("Flex Direction");
					if (BeginCombo("##flexdirection", flexdirection)) {
						for (size_t n = 0; n < IM_ARRAYSIZE(flexdirections); ++n) {
							bool is_selected = (flexdirection == flexdirections[n]);
							if (Selectable(flexdirections[n], is_selected)) {
								flexdirection = flexdirections[n];
							}
							if (is_selected) {
								SetItemDefaultFocus();
							}
						}
						EndCombo();
					}

					if (BeginTable("##flexbasistable", 3, ImGuiTableFlags_NoBordersInBody)) {
						TableNextColumn();
						Text("Basis");
						TableNextColumn();
						Text("Grow");
						TableNextColumn();
						Text("Shrink");
						TableNextColumn();

						InputTextWithHint("##flexbasis", "auto", flexbasis, 256);
						TableNextColumn();
						InputFloat("##flexgrow", &flexgrow, 0.0f, 0.0f, "%g");
						TableNextColumn();
						InputFloat("##flexshrink", &flexshrink, 0.0f, 0.0f, "%g");

						EndTable();
					}

					Text("Flex Wrap");
					RadioButton("no wrap", &flexwrap, YGWrapNoWrap);
					SameLine();
					RadioButton("wrap", &flexwrap, YGWrapWrap);
					SameLine();
					RadioButton("wrap-reverse", &flexwrap, YGWrapWrapReverse);

					EndTabItem();
				}
				if (BeginTabItem("Alignment")) {

					EndTabItem();
				}
				if (BeginTabItem("Layout")) {

					EndTabItem();
				}

				Separator();
				if (Button("add child node")) {
					const YGNodeRef node = YGNodeNew();
					YGNodeStyleSetWidth(node, 50);
					YGNodeStyleSetHeight(node, 50);
					const int index = YGNodeGetChildCount(node);
					YGNodeInsertChild(m_selectedNode, node, index);
					// TODO: need to insert userdata
					m_ui.resize(glm::vec2(m_camera->getScreenSize()));
				}
				SameLine();
				if ((m_selectedNode != m_ui.getLayout()) && Button("remove node")) {
					const YGNodeRef node = m_selectedNode;
					m_selectedNode = YGNodeGetParent(m_selectedNode);
					YGNodeFreeRecursive(node);
					// TODO: need to add free function
					m_ui.resize(glm::vec2(m_camera->getScreenSize()));
				}

				EndTabBar();
			}
		} else {
			constexpr const char* text = "Select a node";
			auto window_dimensions = GetWindowSize();
			auto text_dimensions = CalcTextSize(text);

			SetCursorPosX((window_dimensions.x - text_dimensions.x) * 0.5f);
			SetCursorPosY((window_dimensions.y - text_dimensions.y) * 0.5f);
			TextColored(ImVec4(0.75, 0.75, 0.75, 1.0), text);
		}
	}
	End();

	// switch to next scene
	if (glfwGetKey(m_engine->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		m_engine->setActiveScene(new MainMenuScene());
	}
}

void LayoutEditorScene::onRender() {
	glClearColor(0.24f, 0.58f, 1.0f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_ui.draw();
}

/////////////
// PRIVATE //
/////////////

void LayoutEditorScene::updateFiles() {
	const std::string path = "res/scripts/layout/";
	for (const std::filesystem::directory_entry& file : std::filesystem::directory_iterator(path)) {
		const std::string filename = file.path().filename().string();
		m_files.push_back(filename);
	}
}

void LayoutEditorScene::loadLayout(const std::string& filename) {
	lua_State* L = m_engine->getLuaState();
	
	// parse layout file
	if (luaL_dofile(m_engine->getLuaState(), std::string("res/scripts/layout/" + filename).c_str())) {
		const char* err = lua_tostring(L, -1);
		fmt::print("Error loading layout:\n");
		fmt::print(fmt::fg(fmt::terminal_color::red), "{}\n", err);
		lua_pop(L, 1);
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
	YGNode* layout = (YGNode*)lua_touserdata(L, -1);
	lua_pop(L, 1);

	if (m_ui.getLayout() != nullptr) {
		delete m_ui.getLayout();
	}

	m_ui.setLayout(layout);
	m_ui.resize(glm::vec2(m_camera->getScreenSize()));
	m_selectedNode = layout;
}

