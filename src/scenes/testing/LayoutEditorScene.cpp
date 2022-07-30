#include <yoga/YGNode.h>
#include "scenes/testing/LayoutEditorScene.hpp"

bool LayoutEditorScene::onCreate() {
	m_camera = new Camera(glm::vec3(0.0f, 0.0f, 1.0f));
	m_camera->setTarget(glm::vec3(0.0f));

	updateFiles();
	loadLayout(m_files.at(0));

	lua_State* L = Engine::Instance->getLuaState();

	return true;
}

void LayoutEditorScene::onDestroy() {
}

void LayoutEditorScene::onUpdate(float dt) {
	m_elapsedTime += (double)dt;

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Open...")) {
			for (std::string& filename : m_files) {
				if (ImGui::MenuItem(filename.c_str())) {
					loadLayout(filename);
				}
			}

			if (m_ui.getLayout() != nullptr) {
				ImGui::Separator();
				if (ImGui::MenuItem("Close")) {
					YGNodeFreeRecursive(m_ui.getLayout());
					// TODO: add cleanup function
					m_ui.setLayout(nullptr);
					m_selectedNode = nullptr;
				}
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (ImGui::Begin("Editor")) {
		if (m_selectedNode != nullptr) {
			static int direction = YGDirectionInherit;
			const char* flexdirections[] = {"row", "column", "row-reverse", "column-reverse"};
			static const char* flexdirection = flexdirections[0];
			static char flexbasis[256] = {0};
			static float flexgrow = 0.0f;
			static float flexshrink = 1.0f;
			static int flexwrap = YGWrapNoWrap;

			NodeID* nodeId = (NodeID*)YGNodeGetContext(m_selectedNode);
			ImGui::Text("Node: %s", nodeId->id.c_str());
			if (YGNodeGetChildCount(m_selectedNode) > 0) {
				if (ImGui::Button("Select 1st child")) {
					m_selectedNode = YGNodeGetChild(m_selectedNode, 0);
				}
			}
			if (ImGui::BeginTabBar("tabs")) {
				if (ImGui::BeginTabItem("Flex")) {
					ImGui::Text("Direction");
					ImGui::RadioButton("inherit", &direction, YGDirectionInherit);
					ImGui::SameLine();
					ImGui::RadioButton("ltr", &direction, YGDirectionLTR);
					ImGui::SameLine();
					ImGui::RadioButton("rtl", &direction, YGDirectionRTL);

					ImGui::Text("Flex Direction");
					if (ImGui::BeginCombo("##flexdirection", flexdirection)) {
						for (size_t n = 0; n < IM_ARRAYSIZE(flexdirections); ++n) {
							bool is_selected = (flexdirection == flexdirections[n]);
							if (ImGui::Selectable(flexdirections[n], is_selected)) {
								flexdirection = flexdirections[n];
							}
							if (is_selected) {
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}

					if (ImGui::BeginTable("##flexbasistable", 3, ImGuiTableFlags_NoBordersInBody)) {
						ImGui::TableNextColumn();
						ImGui::Text("Basis");
						ImGui::TableNextColumn();
						ImGui::Text("Grow");
						ImGui::TableNextColumn();
						ImGui::Text("Shrink");
						ImGui::TableNextColumn();

						ImGui::InputTextWithHint("##flexbasis", "auto", flexbasis, 256);
						ImGui::TableNextColumn();
						ImGui::InputFloat("##flexgrow", &flexgrow, 0.0f, 0.0f, "%g");
						ImGui::TableNextColumn();
						ImGui::InputFloat("##flexshrink", &flexshrink, 0.0f, 0.0f, "%g");

						ImGui::EndTable();
					}

					ImGui::Text("Flex Wrap");
					ImGui::RadioButton("no wrap", &flexwrap, YGWrapNoWrap);
					ImGui::SameLine();
					ImGui::RadioButton("wrap", &flexwrap, YGWrapWrap);
					ImGui::SameLine();
					ImGui::RadioButton("wrap-reverse", &flexwrap, YGWrapWrapReverse);

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Alignment")) {

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Layout")) {

					ImGui::EndTabItem();
				}

				ImGui::Separator();
				if (ImGui::Button("add child node")) {
					const YGNodeRef node = YGNodeNew();
					YGNodeStyleSetWidth(node, 50);
					YGNodeStyleSetHeight(node, 50);
					const int index = YGNodeGetChildCount(node);
					YGNodeInsertChild(m_selectedNode, node, index);
					// TODO: need to insert userdata
					m_ui.resize(glm::vec2(m_camera->getScreenSize()));
				}
				ImGui::SameLine();
				if ((m_selectedNode != m_ui.getLayout()) && ImGui::Button("remove node")) {
					const YGNodeRef node = m_selectedNode;
					m_selectedNode = YGNodeGetParent(m_selectedNode);
					YGNodeFreeRecursive(node);
					// TODO: need to add free function
					m_ui.resize(glm::vec2(m_camera->getScreenSize()));
				}

				ImGui::EndTabBar();
			}
		} else {
			constexpr const char* text = "Select a node";
			auto window_dimensions = ImGui::GetWindowSize();
			auto text_dimensions = ImGui::CalcTextSize(text);

			ImGui::SetCursorPosX((window_dimensions.x - text_dimensions.x) * 0.5f);
			ImGui::SetCursorPosY((window_dimensions.y - text_dimensions.y) * 0.5f);
			ImGui::TextColored(ImVec4(0.75, 0.75, 0.75, 1.0), text);
		}
	}
	ImGui::End();

	// switch to next scene
	if (glfwGetKey(Engine::Instance->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		Engine::Instance->setActiveScene(new MainMenuScene());
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
	lua_State* L = Engine::Instance->getLuaState();

	// parse layout file
	if (luaL_dofile(Engine::Instance->getLuaState(), std::string("res/scripts/layout/" + filename).c_str())) {
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
	YGNodeRef layout = (YGNodeRef)lua_touserdata(L, -1);
	lua_pop(L, 1);

	if (m_ui.getLayout() != nullptr) {
		delete m_ui.getLayout();
	}

	m_ui.setLayout(layout);
	m_ui.resize(glm::vec2(m_camera->getScreenSize()));
	m_selectedNode = layout;
}
