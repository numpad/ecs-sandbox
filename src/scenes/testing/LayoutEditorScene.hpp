#pragma once

#include <vector>
#include <string>
#include <filesystem>

#include <yoga/Yoga.h>
#include <sgl/sgl_texture.hpp>

#include <Engine/Engine.hpp>
#include <Engine/IScene.hpp>
#include <RenderObject/Camera.hpp>
#include <scenes/ui/MainMenuScene.hpp>

class LayoutEditorScene : public IScene {
public:

	bool onCreate() override;
	void onDestroy() override;
	void onUpdate(float dt) override;
	void onRender() override;

private:
	Camera *m_camera = nullptr;
	double m_elapsedTime = 0.0;

	sgl::shader *m_logoShader;
	std::vector<std::string> m_files;
	
	YGNodeRef m_layout = nullptr;
	YGNodeRef m_selectedNode = nullptr;
	void updateFiles();
	void loadLayout(const std::string &filename);
	void drawLayout(YGNodeRef parent, glm::mat4 view, float z = -0.9f);

	GLuint m_vao, m_vbo;
	void createLogo();


};
