#include <scenes/testing/LayoutEditorScene.hpp>

bool LayoutEditorScene::onCreate() {
	m_camera = new Camera{glm::vec3(0.0f, 0.0f, 1.0f)};
	m_camera->setTarget(glm::vec3(0.0f));

	m_engine->getConfig().imgui_enabled = true;

	// load shader
	m_logoShader = new sgl::shader{};
	m_logoShader->load("res/glsl/ui/image_vert.glsl", sgl::shader::VERTEX);
	m_logoShader->load("res/glsl/ui/image_frag.glsl", sgl::shader::FRAGMENT);
	m_logoShader->compile();
	m_logoShader->link();

	createLogo();
	updateFiles();
	loadLayout("");

	lua_State *L = m_engine->getLuaState();

	return true;
}

void LayoutEditorScene::onDestroy() {
	m_engine->getConfig().imgui_enabled = false;
	delete m_logoShader;
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
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

			if (m_layout != nullptr) {
				Separator();
				if (MenuItem("Close")) {
					YGNodeFreeRecursive(m_layout);
					// TODO: add cleanup function
					m_layout = nullptr;
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
			const char* flexdirections[] = { "row", "column", "row-reverse", "column-reverse" };
			static const char* flexdirection = flexdirections[0];
			static char flexbasis[256] = {0};
			static float flexgrow = 0.0f;
			static float flexshrink = 1.0f;
			static int flexwrap = YGWrapNoWrap;

			if (BeginTabBar("tabs")) {
				if (BeginTabItem("Flex")) {
					Text("Direction");
					RadioButton("inherit", &direction, YGDirectionInherit); SameLine();
					RadioButton("ltr", &direction, YGDirectionLTR); SameLine();
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
					RadioButton("no wrap", &flexwrap, YGWrapNoWrap); SameLine();
					RadioButton("wrap", &flexwrap, YGWrapWrap); SameLine();
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
					YGNodeCalculateLayout(m_layout, m_camera->getScreenWidth(), m_camera->getScreenHeight(), YGDirectionLTR); // TODO: only calculate parent layout
				}
				SameLine();
				if ((m_selectedNode != m_layout) && Button("remove node")) {
					const YGNodeRef node = m_selectedNode;
					m_selectedNode = YGNodeGetParent(m_selectedNode);
					YGNodeFreeRecursive(node);
					// TODO: need to add free function
					YGNodeCalculateLayout(m_layout, m_camera->getScreenWidth(), m_camera->getScreenHeight(), YGDirectionLTR); // TODO: only calculate parent layout
				}

				EndTabBar();
			}
		} else {
			constexpr const char *text = "Select a node";
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

	float width = m_camera->getScreenWidth();
	float height = m_camera->getScreenHeight();
	m_logoShader->operator[]("uProjection") = glm::ortho(0.0f, width, height, 0.0f); //m_camera->getHudProjection();
	m_logoShader->operator[]("uView") = glm::mat4(1.0f);
	m_logoShader->operator[]("uModel") = glm::mat4(1.0f);

	// Yoga wireframe rendering
	static float lastWidth = 0.0f, lastHeight = 0.0f;
	if (m_layout != nullptr && (lastWidth != width || lastHeight != height)) {
		fmt::print("updating layout!\n");
		lastWidth = width;
		lastHeight = height;
		YGNodeCalculateLayout(m_layout, width, height, YGDirectionLTR);
	}

	m_logoShader->use();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	drawLayout(m_layout, glm::mat4(1.0f));
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

void LayoutEditorScene::createLogo() {
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	GLfloat vertices[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
	
}

void LayoutEditorScene::loadLayout(const std::string &filename) {
	lua_State *L = m_engine->getLuaState();

	if (m_layout != nullptr) {
		YGNodeFreeRecursive(m_layout);
		m_layout = nullptr;
		m_selectedNode = nullptr;
	}

	if (luaL_dofile(m_engine->getLuaState(), std::string("res/scripts/layout/" + filename).c_str())) {
		const char *err = lua_tostring(L, -1);
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
	YGNodeRef layout = (YGNode*)lua_touserdata(L, -1);
	lua_pop(L, 1);
	m_layout = layout;

	m_selectedNode = m_layout;

	YGNodeCalculateLayout(m_layout, m_camera->getScreenWidth(), m_camera->getScreenHeight(), YGDirectionLTR);
}

void LayoutEditorScene::drawLayout(YGNodeRef parent, glm::mat4 view, float z) {
	if (parent == nullptr) return;

	float x = YGNodeLayoutGetLeft(parent);
	float y = YGNodeLayoutGetTop(parent);
	float w = YGNodeLayoutGetWidth(parent);
	float h = YGNodeLayoutGetHeight(parent);

	view = glm::translate(view, glm::vec3(x, y, 0.0f));
	glm::mat4 model = glm::scale(
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, z)),
		glm::vec3(w, h, 1.0f));
	m_logoShader->operator[]("uView") = view;
	m_logoShader->operator[]("uModel") = model;
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	for (uint32_t i = 0; i < YGNodeGetChildCount(parent); ++i) {
		YGNodeRef child = YGNodeGetChild(parent, i);
		drawLayout(child, view, z += 0.01f);
	}
}

