#include "scenes/ui/MainMenuScene.hpp"
#include "scenes/gamemode/raft/MiniRaftScene.hpp"

bool MainMenuScene::onCreate() {
	m_logo = new sgl::texture();

	int width, height, nChannels;
	stbi_set_flip_vertically_on_load(false);
	unsigned char* data = stbi_load("res/images/ui/title.png", &width, &height, &nChannels, 0);
	if (!data)
		return false;

	m_logo->load(width, height, sgl::texture::internalformat::rgba, data, sgl::texture::format::rgba,
	             sgl::texture::datatype::u8);
	stbi_image_free(data);

	return true;
}

void MainMenuScene::onDestroy() {
	delete m_logo;
}

void MainMenuScene::onUpdate(float dt) {
}

void MainMenuScene::onRender() {
	glClearColor(0.3f, 0.5f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	constexpr ImGuiWindowFlags wflags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
	                                    ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground;

	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(100, 0));
	ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - 200, io.DisplaySize.y));
	if (ImGui::Begin("Main Menu", nullptr, wflags)) {
		if (ImGui::BeginTable("main_menu", 3)) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(1);

			ImGui::Image((void*)m_logo->get_texture(), ImVec2(ImGui::GetColumnWidth(), ImGui::GetColumnWidth()));

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(1);

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6, 0.6, 0.6, 1.0));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7, 0.7, 0.7, 1.0));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7, 0.7, 0.7, 1.0));

			ImVec2 button_size(ImGui::GetColumnWidth(), ImGui::GetColumnWidth() * 0.16f);
			if (ImGui::Button("Continue...", button_size)) {
			}
			ImGui::PopStyleColor(3);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(1);
			if (ImGui::Button("New Game", button_size)) {
				switch (m_modeindex % m_modenames.size()) {
				case 0:
					m_engine->setActiveScene(new LayoutEditorScene());
					break;
				case 1:
					m_engine->setActiveScene(new TestGameScene());
					break;
				case 2:
					m_engine->setActiveScene(new MiniRaftScene());
					break;
				case 3:
					m_engine->setActiveScene(new TowerScene());
					break;
				case 4:
					m_engine->setActiveScene(new SpritePreviewScene());
					break;
				};
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			if (ImGui::Button("<", button_size)) {
				--m_modeindex;
			}
			ImGui::TableSetColumnIndex(1);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0, 0.0, 0.0, 0.0));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0, 0.0, 0.0, 0.0));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0, 0.0, 0.0, 0.0));
			if (ImGui::Button(m_modenames[m_modeindex % m_modenames.size()].c_str(), button_size)) {
			}
			ImGui::PopStyleColor(3);
			ImGui::TableSetColumnIndex(2);
			if (ImGui::Button(">", button_size)) {
				++m_modeindex;
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(1);
			if (ImGui::Button("Settings", button_size)) {
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(1);
			if (ImGui::Button("Quit", button_size)) {
				m_engine->quit();
			}

			ImGui::EndTable();
		}
	}
	ImGui::End();
}
